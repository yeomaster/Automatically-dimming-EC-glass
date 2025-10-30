#include <ArduinoBLE.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_MCP4725.h>

BH1750 lightMeter;
Adafruit_MCP4725 dac;

BLEDevice outsidePeripheral;
BLECharacteristic outsideLuxChar;

BLEService phoneService("180A");
BLECharacteristic dummyChar("2A57", BLERead, 1);

BLEService glassStatusService("19B10002-E8F2-537E-4F6C-D104768A1214");
BLECharacteristic statusChar("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 64);
BLECharacteristic controlChar("2A58", BLEWrite, 4);
BLECharacteristic targetReadChar("2A59", BLERead, 4);

float outsideLux = 0.0;
float insideLux = 0.0;
int measuredTransmission = 0;
int targetTransmission = 65; // as integer percent
float factor = 0.0;

bool phoneConnected = false;
float lastAppliedVoltage = -1.0;
bool overrideMode = false;

unsigned long lastCheckTime = 0;

void broadcastStatus(const String& msg) {
  Serial.println(msg);
  statusChar.writeValue(msg.c_str());
}

void updateTargetPercentFloat() {
  float targetPercentFloat = (float)targetTransmission;
  targetReadChar.writeValue((uint8_t*)&targetPercentFloat, sizeof(float));
}

void applyVoltage(float volts) {
  if (volts == lastAppliedVoltage) return;
  int dacValue = (int)((volts / 3.3) * 4095.0);
  dacValue = constrain(dacValue, 0, 4095);
  dac.setVoltage(dacValue, false);
  broadcastStatus("[DAC] Set input voltage to: " + String(volts, 2) + " V");
  lastAppliedVoltage = volts;
}

void setup() {
  Serial.begin(9600);
  while (!Serial); // comment this out if we are using power supply

  Wire.begin();
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("\u26a0\ufe0f Inside BH1750 not found!");
    while (1);
  }

  if (!dac.begin(0x60)) {
    Serial.println("\u26a0\ufe0f DAC not found!");
    while (1);
  }

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("GlassControl");
  phoneService.addCharacteristic(dummyChar);
  BLE.addService(phoneService);
  glassStatusService.addCharacteristic(statusChar);
  glassStatusService.addCharacteristic(controlChar);
  glassStatusService.addCharacteristic(targetReadChar);
  BLE.addService(glassStatusService);

  updateTargetPercentFloat();
  BLE.advertise();
  BLE.scan();

  Serial.println("Scanning for OutsideLuxSensor...");
  lastCheckTime = millis();
}

void loop() {
  phoneConnected = BLE.central() && BLE.central().connected();

  if (controlChar.written()) {
    uint8_t buffer[20];
    int len = controlChar.valueLength();
    controlChar.readValue(buffer, len);

    if (len == 14 && memcmp(buffer, "FORCE_NEGATIVE", 14) == 0) {
      overrideMode = true;
      applyVoltage(2.2);  // hold at 2.2V
      broadcastStatus("[OVERRIDE] FORCE_NEGATIVE active (2.2V)");
      return;
    }

    float newTargetPercent;
    memcpy(&newTargetPercent, buffer, sizeof(float));
    targetTransmission = (int)newTargetPercent;
    overrideMode = false;
    updateTargetPercentFloat();
    broadcastStatus("[UPDATE] targetTransmission set to: " + String(targetTransmission) + "%");
  }

  if (!outsidePeripheral && BLE.available()) {
    BLEDevice found = BLE.available();
    if (found.localName() != "OutsideLuxSensor") return;
    BLE.stopScan();
    outsidePeripheral = found;
    if (!outsidePeripheral.connect()) {
      broadcastStatus("Failed to connect to Outside.");
      outsidePeripheral = BLEDevice();
      BLE.scan();
      return;
    }

    if (!outsidePeripheral.discoverAttributes()) {
      broadcastStatus("Failed to discover Outside attributes.");
      outsidePeripheral.disconnect();
      outsidePeripheral = BLEDevice();
      BLE.scan();
      return;
    }

    BLEService luxService = outsidePeripheral.service("19B10000-E8F2-537E-4F6C-D104768A1214");
    if (!luxService) {
      broadcastStatus("Lux service not found.");
      outsidePeripheral.disconnect();
      outsidePeripheral = BLEDevice();
      BLE.scan();
      return;
    }

    outsideLuxChar = luxService.characteristic("19B10001-E8F2-537E-4F6C-D104768A1214");
    if (!outsideLuxChar) {
      broadcastStatus("Lux characteristic not found.");
      outsidePeripheral.disconnect();
      outsidePeripheral = BLEDevice();
      BLE.scan();
      return;
    }

    broadcastStatus("Connected to Outside.");
  }

  if (outsidePeripheral && !outsidePeripheral.connected()) {
    broadcastStatus("Lost connection to Outside. Rescanning...");
    outsidePeripheral = BLEDevice();
    BLE.scan();
    return;
  }

  if (overrideMode) {
    // Hold 2.2V output during override
    applyVoltage(2.2);
    delay(100);
    return;
  }

  if (millis() - lastCheckTime >= 2000) {
    insideLux = lightMeter.readLightLevel();
    if (outsidePeripheral.connected()) {
      uint8_t buffer[4];
      if (outsideLuxChar.readValue(buffer, 4)) {
        memcpy(&outsideLux, buffer, sizeof(float));
      } else {
        broadcastStatus("Failed to read Outside lux.");
        return;
      }
    }

    measuredTransmission = (int)((insideLux / outsideLux) * 100.0);
    float scale = 0.0667;
    factor = (- (float)measuredTransmission * scale) + ((float)targetTransmission * scale);

    String status = "Inside: " + String(insideLux, 2) +
                    " | Outside: " + String(outsideLux, 2) +
                    " | Measured: " + String(measuredTransmission) + "%" +
                    " | Target: " + String(targetTransmission) + "%" +
                    " | Factor: " + String(factor, 1);
    if (phoneConnected) {
      status += " | Phone connected.";
    }
    broadcastStatus(status);

    if (factor > 0.02) {
      applyVoltage(2.2);
    } else if (factor < -0.03) {
      applyVoltage(0.7);
    } else {
      if (lastAppliedVoltage == 0.7) {
        applyVoltage(1.2);
      } else {
        applyVoltage(lastAppliedVoltage);
      }
    }

    lastCheckTime = millis();
  }

  delay(100);
}
