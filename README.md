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
<img width="700" height="400" alt="image" src="https://github.com/user-attachments/assets/1a388453-ac0e-462a-9afc-e91014c2f1cc" />


- Outside Arduino 33 BLE Nano is connected to a light sensor and a Rx coil, this will be placed 'oustide' i.e, made to be placed in the environment as a means to get outside lux readings
- Inside Arduino board is connected to a MCP4725 I2C DAC, a ICL7660SCPAZ inverter and EC glass. This arduino board will control most of the functions/calculations as well as connect to the user's phone if available

  **Main Component: EC glass**
  <img width="700" height="400" alt="image" src="https://github.com/user-attachments/assets/d0ce87af-ea5e-4427-bc9d-85a16970499b" />
- When voltage is applied across the coatings, ions travel from one layer to another layer, prompting a reversible solid-state change that causes the coating to tint and absorb light. In other words, the glass gets darker.
- To have the opposite effect, ie, make the glass transparent again, reverse voltage( in our case 0.7V) can be applied for max 30 seconds


**How it works:**
- Luminosity reading is detected from the lux sensor connected to the Outside arduino board that is powered by a RX coil, this oustide
- This luminosity reading is sent to the Inside Arduino board via Bluetooth connection
- Another luminosity reading is taken by the lux sensor connected to the Inside board
- The Inside Arduino board calculated the total difference in luminosty between outside and inside, if too much lux is detected from the inside, ie the threshold is overcome, positive voltage is applied to the EC glass, darkening it within a minute
- if too little lux is detected from inside, negative voltage is applied on the EC glass, allowing the glass to return to its transparent state

**Formula used to calculate the luminosity**
<img width="317" height="107" alt="image" src="https://github.com/user-attachments/assets/fd3c8182-f97a-4365-8913-bc374f2efdd3" />

**Factor = 0.0667 ×(𝑡𝑎𝑟𝑔𝑒𝑡 𝑇𝑟𝑎𝑛𝑚𝑖𝑠𝑠𝑖𝑜𝑛 − 𝑚𝑒𝑎𝑠𝑢𝑟𝑒𝑑 𝑇𝑟𝑎𝑛𝑠𝑚𝑖𝑠𝑠𝑖𝑜𝑛)**
 - target transmission = amount of lux difference(i.e. transitted) between the inside and and outside lux
 - measured tranmission = the actual amount of transmission being detected

**graphs showing the luminosty increasing and decreasing due to EC glass darkening and brightening respectively:**
<img width="700" height="400" alt="image" src="https://github.com/user-attachments/assets/3cacba50-f592-4506-b743-231e00bb218d" />
<img width="700" height="400" alt="image" src="https://github.com/user-attachments/assets/306e2736-8039-4b54-a7ec-3f5cdfc5139c" />


**Video example:**
https://youtu.be/hRJkmoS2Szk



