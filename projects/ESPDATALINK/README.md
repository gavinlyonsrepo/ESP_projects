
Overview
--------------------------------------------
* Name: Gavin Lyons
* Title : ESPTFTCLOCK
* Description:

ESP8266 NodeMCU ESP-12e module v0.1 Wifi. Weather station sensor array(BMP180 DHT22) displayed to
TFT 1.44 ST7735 and webserver. The Esp8266 reads in data from sensors 
BMP180 barometric pressure and DHT22 humidity and displays 
them to 3 places:
 
1. TFT 1.44 SPI ST7735 128*128 "red pcb" module
2. Serial monitor
3. A Webserver the IP address will be printed in (serial monitor)
The webserver data can be viewed  thru web-browser set to that IP address.


Connections
--------------------
| Num | NODEMCU PIN | TFT |
| --- | --- | --- |
| 1  | 3.3V | LED thru a ~200 ohm resistor |
| 2  | D5 GPIO14 |  SCLK |
| 3  | D7 GPIO13 | SDA/MOSI |
| 4  | D8 GPI0 15 | A0/DC |
| 5  | D6 GPIO12 | RESET, optional can be tied to 3.3V thru pull-up resistor 4.7-10K |
| 6  | D0 GPIO16 | CS |
| 7  | GND | GND |
| 8 | VCC | VCC 3.3V  |

| Num | Sensor| TFT |
| --- | --- | --- |
| 1 | BMP180 SDA | D2 GPIO 4 SDA |
| 2 | BMP180 SCLK | D1 GPIO 5 SCLK |
| 3 | DHT22 sensor | D3 GPIO 0 |

These sensors need pull-up resistors on data lines, see data sheets.
 
Output
--------------------------

Webserver:

![ output1](https://github.com/gavinlyonsrepo/ESP_projects/blob/master/images/TFTDATALINK1.jpg)

TFT screen:

![ output2](https://github.com/gavinlyonsrepo/ESP_projects/blob/master/images/TFTDATALINK2.jpg)
