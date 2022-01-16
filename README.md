# ESP32_MQTT_Template_Example
Template I created for setting up an ESP32 MQTT sensor in Home Assistant. 

Open it in VS Code / PlatformIO.  

This works with an ESP32 development board that I have.  I use the program to publish data to an MQTT broker running on Home Assistant.   

Make sure you update the network and MQTT broker information at the top of the program. 

You should be able to update the platformio.ini file to upload the code to different boards.  

When I set up one of my ESP8266 boards I will update the code to include build options for both processors. 