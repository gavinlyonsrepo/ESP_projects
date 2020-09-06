
Overview
--------------------------------------------
* Name: Gavin Lyons
* Title : ESP32_nokia
* Description:

ESP32  Wifi, Weather station sensor array(BMP180 DHT22) displayed to
nokia 5110 and asynchronous web server. The Esp32 reads in data from sensors 
BMP180 barometric pressure and DHT22 humidity and displays 
them to 3 places:
 
1. Nokia 5110 LCD  PCD8544
2. Serial monitor
3. A Webserver the IP address will be printed in (serial monitor)
The webserver data can be viewed  thru web-browser set to that IP address.
the webserver asynchronous  and is updated automatically every 10 seconds.

Connections
--------------------
| ESP32 PIN | Nokia 5110 |
| --- |  --- |
| 18 | LCD_CLK Pin 5 clock in |
| 19 | LCD_DIN Pin 4 data in |
| 33 | LCD_DC Pin 3 data/command |
| 32 | LCD_CE Pin 2 chip enable |
| 23 | LCD_RST Pin 1 reset |

| Num | Sensor| TFT |
| --- | --- | --- |
| 1 | BMP180 SDA | GPIO 21 SDA |
| 2 | BMP180 SCLK | D1 GPIO 22 SCLK |
| 3 | DHT22 sensor |  GPIO 4 |

These sensors need pull-up resistors on data lines, see data sheets.
 
Output
--------------------------

Webserver:

![ output1](https://github.com/gavinlyonsrepo/ESP_projects/blob/master/images/nokia2.jpg)

nokia 5110 screen:

![ output2](https://github.com/gavinlyonsrepo/ESP_projects/blob/master/images/nokia.jpg)
