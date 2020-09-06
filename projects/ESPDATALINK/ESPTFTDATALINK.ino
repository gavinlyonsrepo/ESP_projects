
// ********************** HEADER ***********************

/*
  Name : ESP_TFT_DATALINK
  Title : Esp8266 NodeMcu 12-e Wifi Weather station webserver
  Author: Gavin Lyons
  URL: https://github.com/gavinlyonsrepo/ESP_projects
*/


// ******************** LIBRARIES ******************
#include <ESP8266WiFi.h>      // v1.0.2
#include <TFT_ST7735.h>       // v1.0.1 on TFT 1.44 ST7735 [URL](https://github.com/sumotoy/TFT_ST7735/tree/1.0p1)
#include <SFE_BMP180.h>       // v1.4.0
#include <Adafruit_Sensor.h> //v1.1.4 [URL](https://github.com/adafruit/Adafruit_Sensor)
#include <DHT.h>  //1.3.10 
#include <DHT_U.h> // 1.3.10

// ******************** DEFINES ********************
// TFT connections
#define __CS  16  // D0 GPIO16
#define __DC  15   //D8 GPIO15
#define __RST 12  // D6 GPIO12 : optional can be tied to Vcc thru 10K Resistor
//SDA->(MOSI) D7 GPIO13
//SCL->(SCLK) D5 GPIO14
//Vcc to 3.3V GND to Gnd , LED to 3.3V(150 resistor)

// Sensor connections
#define DHTPIN 0 // Digital pin connected to the DHT sensor, D3 GPIO 0 
#define DHTTYPE    DHT22 // DHT 22 (AM2302)
//BMP 180 I2C :: SDA -> D2 GPIO 4 SDA :: SCLK -> D1 GPIO 5 SCLK 

#define INITDELAY 2000  //mS
#define SENSOR_READ_DELAY 10000 //mS


// ******************** GLOBALS ********************

TFT_ST7735 tft = TFT_ST7735(__CS, __DC, __RST); // Object for TFT
SFE_BMP180 pressure; // Object for BMP180
DHT_Unified dht(DHTPIN, DHTTYPE); //Object for DHT22


/* WIFI related */
/* **NOTE NB  Replace with your network credentials NOTE NB*/
const char* ssid     = "YOUR SSID";
const char* password = "YOUR WIFI Password";
WiFiServer server(80);
String header; // Variable to store the HTTP request

/* Timing related to replace delay statment in loop*/
unsigned long previousMillis = 0; // will store last time LED was updated
const long interval = SENSOR_READ_DELAY; // interval at which to read sensors (milliseconds)

//Just a test counter
uint16_t  counter = 0;

// **************** FUNCTIONS PROTOTYPES ***************
void SerialSetup();
void tftSetup();
void bmp180Setup();
void WifiSetup();

float bmp180Read(uint8_t);
float DHT22ReadTemperature();
float DHT22ReadHumidity();
void UpdateCount();

void DisplayWebpage();

// ******************** SETUP ************************
void setup() {
  delay(INITDELAY);
  SerialSetup();
  bmp180Setup();
  tftSetup();
  WifiSetup();
  dht.begin();
}

// ******************* MAIN LOOP *****************
void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    bmp180Read(3);
    DHT22ReadTemperature();
    DHT22ReadHumidity();
    UpdateCount();
  }

  DisplayWebpage();
}

// ********************* FUNCTION SPACE ***********

/* Function to setup the wifi */
void WifiSetup(void)
{
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

/* Function to display page with sensor data to webserver */
void DisplayWebpage()
{
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the table
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial;}");
            client.println("table { border-collapse: collapse; width:35%; margin-left:auto; margin-right:auto; }");
            client.println("th { padding: 12px; background-color: #00cc00; color: white; }");
            client.println("tr { border: 5px #ddd; padding: 12px; }");
            client.println("tr:hover { background-color: #bcbcbc; }");
            client.println("td { border: 5px solid; padding: 12px; }");
            client.println(".sensor { color:white; font-weight: bold; background-color: #bcbcbc; padding: 1px; }");

            // Web Page Heading
            client.println("</style></head><body><h1>ESP8266 Weather station</h1>");
            client.println("<table><tr><th>SENSOR</th><th>OUTPUT</th></tr>");

            client.println("<tr><td>BMP180 Temperature</td><td><span class=\"sensor\">");
            client.println(bmp180Read(2), 2);
            client.println(" *C</span></td></tr>");

            client.println("<tr><td>BMP180 Pressure</td><td><span class=\"sensor\">");
            client.println(bmp180Read(3), 2);
            client.println(" mBar</span></td></tr>");

            client.println("<tr><td>DHT22 Temperature</td><td><span class=\"sensor\">");
            client.println(DHT22ReadTemperature(), 2);
            client.println(" *C</span></td></tr>");

            client.println("<tr><td>DHT22 Humdity</td><td><span class=\"sensor\">");
            client.println(DHT22ReadHumidity(), 2);
            client.println(" %</span></td></tr>");

            client.println("<tr><td>Test Count</td><td><span class=\"sensor\">");
            client.println(counter);
            client.println("</span></td></tr>");
            
            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

/* Function to read sensor DHT22 temperature
   Returns: temperature  as a float */
float DHT22ReadTemperature(void)
{
  // Get temperature event and print its value.
  Serial.println("DHT22 data!");
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
    return 0;
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
    tft.setTextColor(GREEN, BLACK);
    tft.setCursor(19, 75);
    tft.print("DHT22 SENSOR");
    tft.setCursor(19, 85);
    tft.print("Temperature ");
    tft.print(event.temperature, 2);
    tft.print(" deg C");
    return event.temperature;
  }

}

/* Function to read sensor DHT22 humidity
   Returns: humidity  as a float */
float DHT22ReadHumidity(void)
{
  // Get humidity event and print its value.
  sensors_event_t event;
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
    return 0;
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
    tft.setTextColor(GREEN, BLACK);
    tft.setCursor(19, 95);
    tft.print("Humidity ");
    tft.print(event.relative_humidity, 2);
    tft.print(" %");
    return event.relative_humidity;
  }
}
/* Function to setup serial comms */
void SerialSetup()
{
  Serial.begin(9600);
  Serial.println("-------------Serial Comms up----------");
}

/* Function to setup TFT */
void tftSetup()
{
  tft.begin();     // initialize a ST7735S chip,  tab
  tft.fillScreen(BLACK);  // fill screen with black color
  tft.drawFastHLine(0, 30,  tft.width(), BRIGHT_RED);    // draw horizontal blue line at position (0, 30)
  tft.drawFastHLine(0, 65,  tft.width(), BRIGHT_RED);
  tft.drawFastHLine(0, 105,  tft.width(), BRIGHT_RED);
  tft.setTextColor(WHITE, BLACK);     // set text color to white and black background
  tft.setTextScale(1);                 // text size = 1
  tft.setCursor(19, 5);
  tft.print("ESP8266 NodeMCU");
  tft.setCursor(19, 17);
  tft.print("Weather Station");
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
float bmp180Read(uint8_t choice = 2)
{
  char status;
  double BMP180temperature = 0;
  double BMP180pressure = 0;

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
      // Print out the measurement:
      Serial.print("temperature: ");
      Serial.print(BMP180temperature, 2);
      Serial.println(" deg C, ");
      tft.setTextColor(YELLOW, BLACK);
      tft.setCursor(19, 35);
      tft.print("BMP180 SENSOR");
      tft.setCursor(19, 45);
      tft.print("Temperature ");
      tft.print(BMP180temperature, 2);
      tft.print(" deg C");
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
          // Print out the measurement:
          Serial.print("absolute pressure: ");
          Serial.print(BMP180pressure, 2);
          Serial.println(" mb, ");
          tft.setCursor(19, 55);
          tft.print("Pressure ");
          tft.print(BMP180pressure, 2);
          tft.print(" mb");

        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");

  if (choice == 2)
    return BMP180temperature;
  else if (choice == 3)
    return  BMP180pressure;
  else
    return 0;
}

/* Function to update and display test count */
void UpdateCount (void)
{
    if (counter++ == 65530) counter = 0;
    tft.setCursor(19, 110);
    tft.setTextColor(WHITE, BLACK);
    tft.print("Test Count: ");
    tft.print(counter);
}
