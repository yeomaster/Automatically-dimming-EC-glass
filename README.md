**Auto dimming EC Glass**

This project was done as part of my finaly year, Capstone Design Project.

**Aim:**
- Create a system that automatically darkens or undarkens the glass depending on the luminosity setting the user sets
- The luminosity setting can be either automatically set via the system or manually inputed by the user
- use **WIRELESS POWERING** to reduce the amount of wires needed for the system to function AND to make installation of system easier



**Equipment used:**
- Electrochromic (EC) glass                - Glass that can darken in colour if voltage is applied
- Arduino 33 BLE x2                        - Used as the main board of the systems, one acted as the slave(dubbed 'outside' for our purpose) while another acted as the main board/ master (dubbed 'Inside'), both communicated via inbuilt Bluetooth
- Wireless Tranmitter(Tx) and Receiver(Rx) - For wireless tranmission/charging
- BH1750 x2                                - Light lux sensor
- MCP4725 I2C DAC                          - Converts digital signal/voltage to analog, used to control how much voltage is being sent to the EC glass
- ICL7660SCPAZ                             - Inverted, used to convert 0V ~ 3.0V Voltage to 1.5V ~ -0.8V

**Circuit Design/layout**
- Outside Arduino 33 BLE Nano is connected to a light sensor and a Rx coil
- Inside Arduino board is connected to a MCP4725 I2C DAC that converts the 


**How it works:**

- Luminosity reading is detected from the lux sensor 
