**Auto dimming EC Glass**

This project was done as part of my finaly year, Capstone Design Project.

**Aim:**
- Create a system that automatically darkens or undarkens the glass  depending on the luminosity setting the user sets
- The luminosity setting can be either automatically set via the system or manually inputed by the user

**Equipment used:**
- Electrochromic (EC) glass                - Glass that can darken in colour if voltage is applied
- Arduino 33 BLE x2                        - Used as the main board of the systems, one acted as the slave(dubbed 'outside' for our purpose) while another acted as the main board/ master (dubbed 'Inside'), both communicated via inbuilt Bluetooth
- Wireless Tranmitter and Receiver + coils - For wireless tranmission/charging
- BH1750 x2                                - Light lux sensor
- MCP4725 I2C DAC                          - Converts digital signal/voltage to analog, used to control how much voltage is being sent to the EC glass
- ICL7660SCPAZ                             - Inverted, used to convert 0V ~ 3.0V Voltage to 1.5V ~ -0.8V

**How it works:**

