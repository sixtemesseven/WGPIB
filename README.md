# WGPIB Project
The goal is to have a wireless GPIB adapter based on the ESP32 chipset (running Arduino). 
The board features SN7516x GPIB compatible output buffers. However, 
the Arduino-GPIB software will have to be modified before those can be used. 

Currently I'm running a modified variant of the Arduino-GPIB library:
https://github.com/mathiashelsen/agipibi
Which runs on the ESP32 and works fine with 3.3V to 5V level shifters (only tested with a single device).
