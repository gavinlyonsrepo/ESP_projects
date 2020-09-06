// ********************** HEADER ***********************
/*
  Name : ESP32 Nokia 
  Title : Asynchronous web server display wetaher station data from two sensors also displayed to Nokia 5110 LCD. 
  Author: Gavin Lyons
  URL: https://github.com/gavinlyonsrepo/ESP_projects
*/


// ******************** LIBRARIES ******************
#include <Wire.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include <NOKIA5110_TEXT.h> //  v2.1.0 my library https://github.com/gavinlyonsrepo/NOKIA5110_TEXT
#include <DHT.h>
#include <DHT_U.h>
#include <SFE_BMP180.h>  // https://github.com/LowPowerLab/SFE_BMP180

// ******************** DEFINES *************************
//LCD Nokia 5110 pinout left to right
// RST 1/ CD 2/ DC 3/ DIN 5/ CLK 5 / VCC 6 /LED 7 /GND 8
#define RST 23 // Reset pin
#define CE 32  //Chip enable
#define DC 33 // data or command
#define DIN 19 // Serial Data input
#define CLK 18 // Serial clock
#define inverse  false // set to true to invert display pixel color
#define contrast 0xBE // default is 0xBF set in LCDinit, Try 0xB1<-> 0xBF if your display is too dark
#define bias 0x12 // LCD bias mode 1:48: Try 0x12 or 0x13 or 0x14

#define INITDELAY 3000  //mS
#define SENSOR_READ_DELAY 65000 //mS

// Sensor connections note  BMP180 wired on SDA-21 SCL-22 
#define DHTPIN 4 // Digital pin connected to the DHT sensor, GPIO 4
#define DHTTYPE    DHT22 // DHT 22 (AM2302)

// ******************  GLOBALS ******************
// Create an LCD object
NOKIA5110_TEXT mylcd(RST, CE, DC, DIN, CLK);
DHT_Unified dht(DHTPIN, DHTTYPE); //Object for DHT22
SFE_BMP180 pressure; // Object for BMP180 wired on SDA-21 SCL-22 

/* WIFI related */
/* **NOTE NB  Replace with your network credentials NOTE NB*/
const char* ssid     = "YOUR SSID";
const char* password = "YOUR WIFI Password";
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
String header; // Variable to store the HTTP reques

//Just a test counter
uint16_t  counter = 0;

/* Timing related to replace delay statment in loop*/
unsigned long previousMillis = 0; // will store last time LED was updated
const long interval = SENSOR_READ_DELAY; // interval at which to read sensors (milliseconds)

const unsigned char splashscreen [] PROGMEM = {
  // weather station icon for splashscreen, 84x48px
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0x3f, 0x3f, 
  0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x3f, 0x7f, 0x7f, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xfe, 0x78, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x83, 0x87, 0xc7, 0xc7, 0xc7, 0xc7, 
  0xc7, 0xc7, 0xc7, 0xc7, 0xc7, 0xc7, 0xc7, 0xc7, 0xc7, 0xc7, 0xc7, 0x87, 0x07, 0x07, 0x07, 0x07, 
  0x07, 0xc7, 0xc7, 0xc7, 0xc7, 0xc7, 0xc7, 0xc7, 0xc7, 0xc7, 0xc7, 0xc7, 0xc7, 0x87, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x87, 0xc7, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf1, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 
  0x83, 0x83, 0x83, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x83, 0x83, 0x63, 0x63, 0x63, 0x63, 0x03, 
  0x03, 0x03, 0x03, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x0e, 0x1f, 0x3f, 0x3f, 0x7f, 0x71, 0x60, 0x60, 0x60, 0x61, 0x71, 0x79, 0x31, 0x31, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 
  0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// **************** FUNCTIONS PROTOTYPES *********************
void SerialSetup();
void NokiaSetup();
void bmp180Setup();
void WifiSetup();

String bmp180Read(uint8_t);
String DHT22ReadTemperature(void);
String DHT22ReadHumidity(void);
void UpdateCount(void);


// ***************** SETUP ******************
void setup() {
  SerialSetup();
  bmp180Setup();
  dht.begin();
  NokiaSetup();
  WifiSetup();
}

  // ******************* MAIN LOOP *****************
void loop() {
//Async webserver it will refresh data every 10 seconds  

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) //every 65 seconds update LCD in case webserver has no clients.
  {
    previousMillis = currentMillis;
    NokiaDisplay();
  }
}


// ********************* FUNCTION SPACE ***********

/* Function to setup nokia */
void NokiaSetup(void)
{
  mylcd.LCDInit(inverse, contrast, bias); // init  the LCD
  mylcd.LCDClear(0x00);  
  mylcd.LCDgotoXY(0, 0);
  mylcd.LCDCustomChar(splashscreen, sizeof(splashscreen) / sizeof(unsigned char), 0x00, true);
  delay(INITDELAY);
  mylcd.LCDClear(0x00); // Clear whole screen
  NokiaDisplay();
}

/* Function to display nokia data*/
void NokiaDisplay(void)
{
  DHT22ReadTemperature();
  DHT22ReadHumidity();
  bmp180Read(2);
  bmp180Read(3);
}

/* Function to setup serial comms */
void SerialSetup(void)
{
  Serial.begin(115200);
  Serial.println("-------------ESP32 Serial Comms up----------");
}

/* Function to read sensor DHT22 temperature
   Returns: temperature  as a float */
String DHT22ReadTemperature(void)
{
  UpdateCount();
  static char outstrTemp[8];
  // Get temperature event and print its value.
  Serial.println("DHT22 data!");
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
    return "error";
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
    dtostrf(event.temperature, 6, 2, outstrTemp);
    mylcd.LCDFont(2); // Set the font
    mylcd.LCDgotoXY(0, 0); // (go to (X , Y) (0-84 columns, 0-5 blocks) top left corner
    mylcd.LCDString("DHT22"); 
    mylcd.LCDFont(1); // Set the font
    mylcd.LCDgotoXY(0, 1); // (go to (X , Y) (0-84 columns, 0-5 blocks) top left corner
    mylcd.LCDString("T=");
    mylcd.LCDString(outstrTemp); //print
    mylcd.LCDString(" C");
    return String(event.temperature);
  }

}

/* Function to read sensor DHT22 humidity
   Returns: humidity  as a float */
String DHT22ReadHumidity(void)
{
  static char outstrHum[8];
  // Get humidity event and print its value.
  sensors_event_t event;
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
    return "error";
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
    mylcd.LCDFont(1); // Set the font
    mylcd.LCDgotoXY(0, 2); 
    dtostrf(event.relative_humidity, 6, 2, outstrHum);
    mylcd.LCDString("H=");
    mylcd.LCDString(outstrHum); //print
    mylcd.LCDString(" %"); //print
    return String(event.relative_humidity);
  }
}


/* Function to update and display test count */
void UpdateCount (void)
{
    if (counter++ == 65530) counter = 0;
     Serial.println(counter);
}

/* Function to setup pressure sensor BMP180 */
void bmp180Setup()
{
  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    Serial.println("BMP180 init fail\n\n");
    while (1); // Pause forever.
  }
}

/* Function to read pressure sensor BMP180
  Parameter :  uint8_t choice  if 2 returns temperature as float
  if 3 returns pressure as a float
  Returns : float, see above
*/
String bmp180Read(uint8_t choice = 2)
{
  char status;
  double BMP180temperature = 0;
  double BMP180pressure = 0;
  static char outstrtemp[8];
  static char outstrpress[8];
  
  Serial.println("BMP180 data");
  // You must first get a temperature measurement to perform a pressure reading.
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);
    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable BMP180temperature.
    // Function returns 1 if successful, 0 if failure.
    status = pressure.getTemperature(BMP180temperature);
    if (status != 0)
    {
      if (choice == 2)
      {
        // Print out the measurement:
        Serial.print("temperature: ");
        Serial.print(BMP180temperature, 2);
        Serial.println(" deg C, ");
        dtostrf(BMP180temperature, 6, 2, outstrtemp);
        mylcd.LCDFont(2); 
        mylcd.LCDgotoXY(0, 3); 
        mylcd.LCDString("BMP180");
        mylcd.LCDFont(1); // Set the font
        mylcd.LCDgotoXY(0, 4); // (go to (X , Y) (0-84 columns, 0-5 blocks) top left corner
        mylcd.LCDString("T=");
        mylcd.LCDString(outstrtemp); //print
        mylcd.LCDString(" C");
        return outstrtemp;
       }
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable BMP180pressure.
        // Note also that the function requires the previous temperature measurement (BMP180temperature).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(BMP180pressure, BMP180temperature);
        if (status != 0)
        {
          if (choice == 3)
          {
            // Print out the measurement:
            Serial.print("absolute pressure: ");
            Serial.print(BMP180pressure, 2);
            Serial.println(" mb, ");
            dtostrf(BMP180pressure, 6, 2, outstrpress);
            mylcd.LCDFont(1); // Set the font
            mylcd.LCDgotoXY(0, 5); // (go to (X , Y) (0-84 columns, 0-5 blocks) top left corner
            mylcd.LCDString("P=");
            mylcd.LCDString(outstrpress); //print
            mylcd.LCDString(" mb");
            return  outstrpress;
          }
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
return "Error";
}

// HTML page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
     background-color:lightsteelblue;
    }
    h1 { color:#ff3410; }
    h2 { font-size: 3.0rem; }
    p1 { font-size: 2.0rem; }
    p2 { font-size: 2.0rem; }
    p3 { font-size: 2.0rem; }
    p4 { font-size: 2.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP32 Weather Station</h2>
  <h1>DHT22 Sensor</h1>
  <p1>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&#8451</sup>
  </p1>
  <br>
  <p2>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">&#8453</sup>
  </p2>
  <br>
  <h1>BMP180 Sensor</h1>
  <p3>  
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="bmptemp">%BMPTEMP%</span>
    <sup class="units">&#8453</sup>
  </p3>
  <br>
  <p4> 
    <i class="fas fa-file-powerpoint" style="color:#00add6;"></i>
    <span class="dht-labels">Pressure</span>
    <span id="pressure">%PRESSURE%</span>
    <sup class="units">mb</sup>
  </p4>
</body>
<script>

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("bmptemp").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/bmptemp", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("pressure").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/pressure", true);
  xhttp.send();
}, 10000 ) ;

</script>

</html>)rawliteral";

// Replaces placeholder with values
String processor(const String& var){
  
  if(var == "TEMPERATURE"){
    return DHT22ReadTemperature();
  }
  else if(var == "HUMIDITY"){
    return DHT22ReadHumidity();
  }
  else if(var == "BMPTEMP"){
    return bmp180Read(2);
  }
  else if(var == "PRESSURE"){
    return bmp180Read(3);
  }
  return String();
}

/* Function to setup the wifi */
void WifiSetup(void)
{
  uint8_t wificount = 0;
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    wificount++;
    if (wificount == 60){
    	NokiaDisplay(); // if wifi down update display. 
    	Serial.println("Failed to connect Wifi ");
    	delay(SENSOR_READ_DELAY);
      wificount = 0;		
    } 
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", DHT22ReadTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", DHT22ReadHumidity().c_str());
  });
  
   server.on("/bmptemp", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", bmp180Read(2).c_str());
  });
   server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", bmp180Read(3).c_str());
  });
  // Start server
  server.begin();
}
