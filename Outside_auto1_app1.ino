#include <ArduinoBLE.h>
#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

BLEService luxService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLECharacteristic luxCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 4);  // 4 bytes for float

void setup() {
  Serial.begin(9600);
  while (!Serial);  // // comment this out if we are using power supply

  Wire.begin();
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("⚠️ BH1750 not found!");
    while (1);
  }

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("OutsideLuxSensor");
  BLE.setAdvertisedService(luxService);

  luxService.addCharacteristic(luxCharacteristic);
  BLE.addService(luxService);

  delay(1000);  // Allow time for BLE to initialize fully

  BLE.advertise();
  Serial.println("Outside Nano is now advertising...");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) {
      float lux = lightMeter.readLightLevel();

      uint8_t buffer[4];
      memcpy(buffer, &lux, sizeof(float));
      luxCharacteristic.writeValue(buffer, 4);

      Serial.print("Sending lux: ");
      Serial.println(lux);

      delay(1000);
    }

    Serial.println("Disconnected from central");
  }
}
