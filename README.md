# LDTrainRemote
Remote for Lego Duplo Train based on ESP32 board

Description
Remote compatible with Lego Duplo Train

Introduction
This project is an adaptation of the work by James Clarke, as showcased on Instructables: https://www.instructables.com/Wooden-Bluetooth-Remote-for-Lego-Duplo-Train/

I appreciate and thank him for the great idea that inspired this project.

The Lego Duplo Train Remote is a fantastic way to engage with and enjoy the Lego Duplo Train.

Custom Section
Step 1:

Electronics and additional parts you need for this project:
1x ESP32 with 18650 Battery holder (ESP-Wroom-ESP32 NOT ESPWROOM-02) https://www.walmart.com/ip/ESP32-WiFi-Bluetooth-Development-Board-Small-IoT-Device-Compatible-With-18650-Battery-For-Programming-Projects-Wireless-Communication-Module/5627060020

4x Tactile Push Buttons 12x12x7.3mm https://www.amazon.com/TWTADE-Momentary-Tactile-Button-12x12x12mm/dp/B07CG7VTGD/ref=sr_1_5

1x Potentiometer 10k Ohm https://www.amazon.com/HiLetgo-Single-Joint-Potentiometer-Variable-Resistors/dp/B00MCK7JMS/ref=sr_1_1_sspa

Set of Electric wires https://www.amazon.com/StrivedayTM-Flexible-Silicone-electronic-electrics/dp/B01LH1FYR4/ref=sr_1_1_sspa

18x melting Nuts m2 3.5mm in diameter (3.2 might also work) https://www.amazon.com/WALENI-Knurled-Injection-Embedment-Insertion/dp/B0CC8BZHZL/ref=sr_1_5?th=1

18 m2 x 6mm countersunk head screws https://www.amazon.com/Hilitchi-210-Piece-Metric-Countersunk-Assortment/dp/B06Y3M62Z8/ref=sr_1_6

1 Power Switch https://www.amazon.de/dp/B08CY41YT5?ref=ppx_yo2ov_dt_b_product_details&th=1

the size of power switches varies ... so you might need to update the body STL for the right size. I ordered these in Germany the hole in the body STL is 12 * 19 mm.

Other:

Some glue for the throttle stick (optional)

Small piece of perfboard (optional)

Some shrink tubing (recommended)

Filament:

I used PLA in 8 different colors:

Green for the housing,
Silver top plate,
Red for the Stop push button (with white),
Blue for the Steam/Water push button (with white)
Yellow for the Horn/Music push button (with black),
White for the Light push button ( with black)
Black for holders (buttons and throttle lever holder),
Orange for the throttle lever
In addition you need:

a soldering iron with solder,
the right screwdriver for the m2 screws
some tools to cut the cables and to strip the cables insulation.
Step 2:

3D Print all parts
Print all the parts in the color of your choice.

Please note that the throttle stick needs to be printed twice, while all other parts only need to be printed once.

All Parts

Step 3:

Soldering the Electronics
Solder ground:
You will need 5 ground connections: 4 for the buttons and 1 for the potentiometer. Locate the ground pin on your ESP32 board and solder a small cable, around 5-10 cm in length, to it. Connect the other end of this cable to a piece of perfboard where you will solder an additional 5 cables, each around 10-20 cm in length.

(Another solution would be to daisy chain the ground cable: ESP32 GND -> potentiometer GND -> Button 1 GND -> Button 2 GND, and so on.)

PICTURE SOLDER Power Switch

GND cable harness

GND to ESP32

Solder GPIOs
Solder 6 cables, each approximately 15-25 cm in length, to the following ESP32 pins:

Pin 25: Button 1 (Yellow)

Pin 26: Button 2 (White)

Pin 27: Button 3 (Blue)

Pin 14: Button 4 (Red)

Pin 15: Potentiometer Data (Green)

VCC: Potentiometer + (Red)

… and connect them

*I have matched the colors of the buttons for a clear view, but you can use any colors you prefer.

Buttons & Poti

GPIO soldering

Power Switch:
Before soldering, it is important to either attach the power switch directly to the 3D printed body or use ferrules that fit the power switch.

Power Switch: On the side of the ESP32 board, there is a power switch attached. To bypass it, you need solder the new Power Switch to the left pin (ON) and the middle pin. Make sure to leave the switch on the board in the off position. A cable length of approximately 25 cm will be sufficient.

power switch

Power Switch soldering

Step 4:

Assemble all parts.
Inserting Melting Nuts:
To assemble the body, you will need to fuse in a total of 10 melting nuts. Six of these nuts are used to connect the top plate, while the remaining four are for attaching the ESP32. Additionally, the button holder requires four melting nuts to be attached to the top plate as well as the Throttle sick holder.

Melt Nuts

Melt Nuts

ESP32:
Screw the ESP32 onto the body.

Optional: Attach 18650 battery to ESP32 board.

Throttle Assembly:
Begin by inserting the potentiometer into the throttle stick holder. Ensure that the potentiometer is set to the middle position, although adjustments may be necessary in the software.
Next, insert the right part of the throttle stick and connect it with the potentiometer. Pass throttle stick through the top plate throttle stick hole.
The left throttle stick may require a bit more effort and force to insert, also pass it though the top plate. The Holder will need to stretch a bit.
Optionally, connect the stick halves with a small bar and consider using glue to secure them together.
Screw the holder onto the top plate.
Poti

Throttle Stick #1

Throttle Stick #2

Throttle Stick #3

Button Assembly:
The push buttons need to be placed on top of the button plate. Thread the buttons through the holes in the plate. In this step, you need to determine the correct placement for each button
Connect the 3D printed buttons to the push buttons.
Screw the button plate onto the top plate.
Buttons

Buttons

Hardware Finished

Step 5:

Flash the Software
To flash the software onto the ESP32, follow these steps:

Download the software from GitHub. You can find it here.
The software requires the Leguino Library, which you can download from here.
Additionally, the software requires the Button2 Library, which doesn't need to be downloaded separately.
You have two options to flash the software: PlatformIO or Arduino IDE.
PlatformIO:

Install PlatformIO plugin in Visual Studio Code (Picture1)
Restart Visual Studio Code when everything is finished (Picture2)
Pick the LDTTRAINREMOTE Folder ( not the SRC folder but the folder above)
Copy legoino-master into a fresh created "lib" folder next to the src folder (Picture3)
check spelling legoino-master in platformio.ini file ... sorry I will update the code (Picture3)
VS Code will install some prerequisites- wait until it is finished ... takes about (Picture4)
goto src / main.cpp hit build (the check icon in the lower bar)
upload the code with the -> icon
Step 6:

Enjoy ...
Activate the Remote:
Power up the Train. The Lego Duplo Train searches for Bluetooth connection for about 20 seconds after powering up
Start the Remote by switching it on.
The train will give a sound when it is successfully connected to the remote.
Category: Toy & Game Accessories

Tags

duplo
lego
remotecontrol
legocompatible
legoduplo
Model origin

The author marked this model as their own original creation. Imported from Thingiverse.

License 

Creative Commons License This work is licensed under a
Creative Commons (4.0 International License)
Attribution—Noncommercial—Share Alike

✖ | Sharing without ATTRIBUTION
✔ | Remix Culture allowed
✖ | Commercial Use
✖ | Free Cultural Works
✖ | Meets Open Definition