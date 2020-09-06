
//********************** HEADER ***********************
/*
  Name : ESP_TFT_RTC
  Title : Esp8266 NodeMcu 12-e Wifi internet clock on TFT 1.44 ST7735
  Author: Gavin Lyons
  URL: https://github.com/gavinlyonsrepo/ESP8266_projects
*/


//******************** LIBRARIES ******************
// Wi-fi
#include <ESP8266WiFi.h>      // v1.0.2

// RTC
#include <WiFiUdp.h>          //
#include <NTPClient.h>        // v3.1.0
#include <TimeLib.h>          // 

// TFT
#include <SPI.h>              //
#include <TFT_ST7735.h>       // v1.0.5


// ******************** DEFINES ********************
// TFT connections
#define __CS  16  // D0 GPIO16
#define __DC  15   //D8 GPIO15
#define __RST 12  // D6 GPIO12 : optional can be tied to Vcc thru 10K Resistor
//SDA->(MOSI) D7 GPIO13
//SCL->(SCLK) D5 GPIO14
//Vcc to 3.3V GND to Gnd , LED to 3.3V(150 resistor)


// ******************** GLOBALS ********************
TFT_ST7735 tft = TFT_ST7735(__CS, __DC, __RST);

// set WiFi network SSID and password
const char *ssid     = "YOUR_SSID";
const char *password = "YOUR_PASSWORD";


// RTC
WiFiUDP ntpUDP;

// 'time.nist.gov' is used (default server) with 0 offset (0 seconds) 60 seconds (60000 milliseconds) update interval
NTPClient timeClient(ntpUDP, "time.nist.gov", 0, 60000);
unsigned long unix_epoch;


//**************** FUNCTIONS PROTOTYPES ***************
void RTC_display();
void SerialSetup();
void tftSetup();
void wifiSetup();

//******************** SETUP ************************
void setup(void)
{
  SerialSetup();
  tftSetup();
  wifiSetup();

  delay(2000);
  tft.fillRect(0, 30, tft.width(), 8, BLACK); //clear wifi-data
  timeClient.begin();
}



//******************* MAIN LOOP *****************
void loop()
{
  delay(1000);    // wait 1s
  timeClient.update();
  unix_epoch = timeClient.getEpochTime();   // get UNIX Epoch time
  RTC_display();
}


// ********************* FUNCTIONS ***********

// Function to setup serial comms
void SerialSetup()
{
  Serial.begin(9600);
  Serial.print("-------------Serial Comms up----------");
}

// Function to setup TFT
void tftSetup()
{
  tft.begin();     // initialize a ST7735S chip, black tab
  tft.fillScreen(BLACK);  // fill screen with black color
  tft.drawFastHLine(0, 25,  tft.width(), BLUE);    // draw horizontal blue line at position (0, 44)
  tft.drawFastHLine(0, 65, tft.width(), BLUE);  // draw horizontal blue line at position (0, 102)
  tft.setTextColor(WHITE, BLACK);     // set text color to white and black background
  tft.setTextScale(1);                 // text size = 1
  tft.setCursor(19, 05);              // move cursor to position (43, 10) pixel
  tft.print("ESP8266 NodeMCU");
  tft.setCursor(19, 15);               // move cursor to position (4, 27) pixel
  tft.print("Wi-Fi Internet Clock");
}


// Function to setup WIFI
void wifiSetup()
{

  WiFi.begin(ssid, password);
  Serial.print(" Connecting.");
  tft.setCursor(0, 30);              // move cursor to position (4, 27) pixel
  tft.print(" Connecting..");
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected");
  tft.print(" Connected");
}


// Function to setup Display of clock on TFT
void RTC_display()
{
   tft.setTextScale(1);
  char dow_matrix[7][10] = {"SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY"};
  static byte previous_dow = 0;
  // print day of the week
  if ( previous_dow != weekday(unix_epoch) )
  {
    previous_dow = weekday(unix_epoch);
    tft.fillRect(11, 30, 108, 14, BLACK);     // draw rectangle (erase day from the display)
    tft.setCursor(19, 30);
    tft.setTextColor(GREEN, BLACK);     // set text color to cyan and black background
    tft.print( dow_matrix[previous_dow - 1] );
  }

  // print date
  tft.setCursor(19, 40);
  tft.setTextColor(GREEN, BLACK);     // set text color to yellow and black background
  tft.printf( "%02u-%02u-%04u   ", day(unix_epoch), month(unix_epoch), year(unix_epoch) );
  // print time
  tft.setCursor(19, 50);
  tft.setTextColor(GREEN, BLACK);     // set text color to green and black background
  tft.printf( "%02u:%02u:%02u   ", hour(unix_epoch), minute(unix_epoch), second(unix_epoch) );
    // print time
  tft.setTextScale(5);
  tft.setCursor(5, 75);
  tft.setTextColor(RED, BLACK);     // set text color to green and black background
  tft.printf( "%02u:%02u", hour(unix_epoch), minute(unix_epoch));
}

//******************* EOF *****************
