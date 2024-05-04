## Summary

We receive the Data sensed by the Arduino on pin A1 and send it through the software serial to the ESP32 (server), 
the ESP32 connected to the wifi and posting this info on the local host server address, 
this data (pin A1 reading) is displayed on the local webpage (client) when we fetch the sensor data (hit the sensor data button).
The local webpage contains five emergency buttons, by pressing each button the webpage sends an emergency level to the server, 
the ESP32 will blink the LED corresponding to the emergency received on the server. 

## This repo includes:
A firmware for an Adrduino uno that was coded on Arduino IDE
A firmware for an ESP32 that was coded on Arduino IDE
The index.html file is for coding a webpage that is displayed on the local host server address
The python file is also running on the local host and the local port.


>[!Caution]
The local host ip address changes, edit it in the code prior uploading it on the hardware.
