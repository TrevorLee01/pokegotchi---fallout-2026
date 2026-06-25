# Pokegotchi
tamagotchi project by Trevor Lee

ONSHAPE LINK: https://cad.onshape.com/documents/4788cfc2c41ccbd3b3bb658b/w/c8008663c56e0ba217e7b347/e/ed1d7d4fd3640974190b7d9b?renderMode=0&uiState=6a3ac660f6d661f4c890654a

BOM INCLUDED IN THE FILES! "BOM.csv", bottom of this README, or: https://docs.google.com/spreadsheets/d/1ZSV7nk1klidvS7acBAQEKFNUl_y9adzhBfaWMznL6cg/edit?gid=0#gid=0

______________

<img width="427" height="565" alt="image" src="https://github.com/user-attachments/assets/865a6ed3-4dcc-43af-b470-ce85d72b7704" />

# What is a Pokegotchi?

This pokegotchi comes with 3 whole pokemons
- Snorlax
- Solosis
- Eevee
However, Eevee comes with 5 different types of Eeveelution depending on the exact environment (a secret). Play to find out which eeveelution you end up getting!

The possible 5 includes:
- umbreon
- leafeon
- slyveon
- glaceon
- espeon

____
# Features

The unique features of this tamagotchi are
- internal clock ( to tell between day and night)
- LDR (to tell how dark it is)

(and a completely different ESP32 :D)

____
# Why did I make it?

Through my entire childhood, I've always been into some type of media related to Pokemon. Despite being it a common hobby or interest, that never discouraged me to discontinue the love I have for Pokemon. 
I think it's such a fundamental piece to my development and I want to physically manifest the fondness by utilizing the hardware opportunity given by fallout. 

____
# Pictures‼️

Here is the pcb.

<img width="1376" height="1164" alt="Screenshot 2026-06-23 234951" src="https://github.com/user-attachments/assets/dbe9ceb8-e015-4b67-b0c9-d66373ea6513" />

<img width="1464" height="1286" alt="Screenshot 2026-06-23 235008" src="https://github.com/user-attachments/assets/db260a48-8a27-4d42-9dc1-277e91ec6009" />

<img width="1258" height="1198" alt="image" src="https://github.com/user-attachments/assets/e147ee8c-a3df-4164-be35-e7b19bc4c39f" />

Here is the CAD.

<img width="1232" height="938" alt="image" src="https://github.com/user-attachments/assets/6f25dcf4-2328-4d60-a6dc-3a127af2aafa" />


Here is the render.

<img width="1554" height="1270" alt="image" src="https://github.com/user-attachments/assets/c0d60905-4331-4f02-bf18-5b1aa977f7fa" />

____
# How It Works ╰(°▽°)╯

Turn the Pokegotchi on and select one out of three Pokémon that you want to take care of: Snorlax, Solosis, or Eevee.

The built-in real-time clock (RTC) will help you keep track of the current time, enabling the device to know whether it is day or night. In addition, the light-dependent resistor (LDR) will detect how bright the surrounding environment is and make your Pokémon respond accordingly.

Should you pick Eevee, it will evolve into one of five unique Eeveelutions, depending both on your in-game actions and some hidden environmental factors.

Use the buttons on the Pokegotchi to navigate through the menus.

____
# How to Build Your Own Pokegotchi :D

1. Print the Case: Download the .step files from the repository and use a 3D printer to print the front case and back case.

2. Solder the Components: Solder all the components to the custom PCB, including the ESP32, RTC IC, light sensor (LDR), display, buttons, and other hardware as specified in the bill of materials.

3. Place the Hardware: Install the PCB inside the printed case using 2 M3x10 screws, and then secure the top of the case using 3 M3x5 screws. Don't forget to place the battery inside its pocket, and connect it to the pinheader.

4. Upload the Firmware: Download the .ino file from the repository and flash Pokegotchi firmware onto the ESP32.

5. Get Ready for an Exciting Journey: Turn on the device, select a Pokémon, and start training your Pokegotchi!

____
# BOM

**Total Cost: $44.40**

| Item | Individual Cost | Quantity | Cost | Link | Additional Notes |
|------|----------------|----------|------|------|------------------|
| ESP32-S3-WROOM-1-N16R8 MCU | $6.76 | 1 | $6.76 | https://www.digikey.com/en/products/detail/espressif-systems/ESP32-S3-WROOM-1-N16R8/16162642 | |
| TYPE-C-31-M-12 USB-C Receptacle | $0.19 | 5 | $0.93 | https://www.lcsc.com/product-detail/C165948.html?s_z=n_q_korean%2520hroparts%2520elec%2520type-c-31-m-12&spm=wm.fly.bg.1.xh&lcsc_vid=R1cKX1cFEVUKX1ECR1FdBlRQFlAMUwFUElkPUwcDQwAxVlNeRlhZV1dRQ1dZUDsOAxUeFF5JWBYZEEoKFBINSQcJGk4%3D | 4 extra, MOQ 5 |
| XHXDZ 1207-P6.5MM Buzzer | $0.50 | 10 | $5.03 | https://www.aliexpress.us/item/3256810135642750.html | 9 extra, MOQ 10 |
| MCP73831T-2ATI/OT Charger | $0.76 | 1 | $0.76 | https://www.digikey.com/en/products/detail/microchip-technology/MCP73831T-2ATI-OT/964303 | |
| AP2112K-3.3TRG1 Voltage Regulator | $0.25 | 1 | $0.25 | https://www.digikey.com/en/products/detail/diodes-incorporated/AP2112K-3-3TRG1/4470746 | |
| 1825910-6 Tactile Switch | $0.13 | 3 | $0.39 | https://www.digikey.com/en/products/detail/te-connectivity-alcoswitch-switches/1825910-6/1632536 | duplicated entry in source list |
| TS-104-T-A Connector | $1.06 | 1 | $1.06 | https://www.digikey.com/en/products/detail/samtec-inc/TS-104-T-A/1105490 | |
| M20-9990246 Connector | $0.10 | 1 | $0.10 | https://www.digikey.com/en/products/detail/harwin-inc/M20-9990246/3728226 | |
| 1825910-6 Tactile Switch (duplicate) | $0.13 | 3 | $0.39 | https://www.digikey.com/en/products/detail/te-connectivity-alcoswitch-switches/1825910-6/1632536 | appears twice in CSV |
| RC0805JR-075K1L Resistor | $0.10 | 2 | $0.20 | https://www.digikey.com/en/products/detail/yageo/RC0805JR-075K1L/728338 | |
| RC0805FR-072KL Resistor | $0.10 | 1 | $0.10 | https://www.digikey.com/en/products/detail/yageo/RC0805FR-072KL/727664 | |
| RC0805FR-0710KL Resistor | $0.10 | 3 | $0.30 | https://www.digikey.com/en/products/detail/yageo/RC0805FR-0710KL/727535 | |
| RC0805FR-074K7L Resistor | $0.10 | 2 | $0.20 | https://www.digikey.com/en/products/detail/yageo/RC0805FR-074K7L/727929 | |
| GL12528 LDR | $0.31 | 5 | $1.56 | https://www.lcsc.com/product-detail/C559386.html?s_z=s_p_GL12528&spm=wm.fly.bg.0.xh&lcsc_vid=Q1kKUVEAQgNbV11QQgQPV1FVRVkLV11TE1hXUwBfEgcxVlNeRVBZUFxXQlhbVjsOAxUeFF5JWBYZEEoKFBINSQcJGk4dAgUUFAk%3D | 4 extra, MOQ 5 |
| CL21B105KAFNNNE Capacitor | $0.10 | 1 | $0.10 | https://www.digikey.com/en/products/detail/samsung-electro-mechanics/CL21B105KAFNNNE/3886724 | |
| CL21A475KAQNNNE Capacitor | $0.11 | 2 | $0.22 | https://www.digikey.com/en/products/detail/samsung-electro-mechanics/CL21A475KAQNNNE/3886902 | |
| HS96L03W2C03 Screen | $1.87 | 1 | $1.87 | https://www.aliexpress.us/item/2251832337475453.html | |
| DS3231MZ+TRL Clock | $12.68 | 1 | $12.68 | https://www.digikey.com/en/products/detail/analog-devices-inc-maxim-integrated/DS3231MZ-TRL/2754408 | |
| Battery | $6.20 | 1 | $6.20 | https://www.aliexpress.us/item/3256808658204287.html | |
| PCB | $0.40 | 5 | $2.00 | https://cart.jlcpcb.com/quote?stencilLayer=2&stencilWidth=100&stencilLength=100&stencilCounts=5&plateType=1&spm=Jlcpcb.Homepage.1010 | |
| PLA Housing | — | — | — | — | |
| M3 x 10mm Socket Cap Head Screws | $0.75 | 2 | $1.50 | https://accu-components.com/us/metric-cap-head-screws/16004-SSCF-M3-10-12-9 | |
| M3 x 5mm Socket Cap Head Screws | $0.45 | 4 | $1.80 | https://accu-components.com/us/metric-cap-head-screws/16001-SSCF-M3-5-12-9 | 4 is cheaper than 3 |
