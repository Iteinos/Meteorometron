# Meteorometron
Thermometer, Hygrometer, Photometer 3-in-1 Horticultural Sensor with Blynk Cloud

Ingredients:
ESP8266 Microcontroller
AHT10/AHT20 Thermo-Hygrometer Sensor
BH1750 Photometer Sensor
Blynk app with the template code, or build your own template

How to cook:
Connect everything over I2C, power the sensors via GPIO15 to save power. GPIO13 is the indicator light.
The reading refreshes every 10 minutes, you can change it at the expense of power consumption rate.
A 5000mAh battery should last around a month, with an efficient 3.3V DC-DC power supply.


