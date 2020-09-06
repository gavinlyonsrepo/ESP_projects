
Overview
--------------------------------------------
* Name: Gavin Lyons
* Title : ESPTFTCLOCK
* Description: 

ESP8266 NodeMCU ESP-12e module v0.1 Wifi internet clock displayed on a TFT 1.44 ST7735.
The ESP8266 fetches the time from internet and displays it on a TFT screen.
in this case a TFT 1.44 SPI ST7735 128*128 "red pcb" module.
 
Libraries
--------------------------------------------
1. <ESP8266WiFi.h>      // v1.0.2
2. <WiFiUdp.h>          //
3. <NTPClient.h>        // v3.1.0
4. <TimeLib.h>          // 
5. <SPI.h>              //
6. <TFT_ST7735.h>       // v1.0.5

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

Output
-----------------------
![NODE](https://github.com/gavinlyonsrepo/ESP_projects/blob/master/images/TFTCLOCK.jpg)
