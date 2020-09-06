//********************** HEADER ***********************
/*
  Name : ESP32_servo
  Title : WIFI controled Door system featuring Webserver , Servo and HC-SR04 sensor  |
  Author: Gavin Lyons
  URL: https://github.com/gavinlyonsrepo/ESP_projects
*/

#include <WiFi.h>
#include <ESP32Servo.h>

Servo myservo;  // create servo object to control a servo

// Servo GPIO pin
static const int servoPin = 18;

// Network credentials
const char* ssid     = "yourSSID";
const char* password = "yourPassword";

// Web server on port 80 (http)
WiFiServer server(80);

// Variable to store the HTTP request
String header;
// Decode HTTP GET value
String valueString = String(5);
int pos1 = 0;
int pos2 = 0;


unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000; //milliseconds

// HC SR04 sensor
#define TRIGGER 2
#define ECHO 27
long duration;
int distance;

// ------------------ Function prototypes ---------
void DisplayServo(void);
void UltraSensor(void);

// --------------------- Setup -----------------------
void setup() {

  // Allow allocation of all timers for servo library
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  // Set servo PWM frequency to 50Hz
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 500, 2400); 

  // HC SR04 sensor
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);

  // Connect to Wi-Fi network with SSID and password
  Serial.begin(115200);
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

// --------------------- Main Loop -----------------------
void loop()
{
  DisplayServo();
}
// ------------------ End of Main ---------------------------


// ------------------ Function Space -------------------------
void DisplayServo(void)
{
  // Listen for incoming clients
  WiFiClient client = server.available();

  // Client Connected
  if (client) {
    // Set timer references
    currentTime = millis();
    previousTime = currentTime;

    // Print to serial port
    Serial.println("New Client.");

    // String to hold data from client
    String currentLine = "";

    // Do while client is cponnected
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK) and a content-type
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Display the HTML web page

            // HTML Header
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta http-equiv=\"refresh\" content=\"15\" name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            // CSS
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto;}");
            client.println(".slider { -webkit-appearance: none; width: 300px; height: 25px; border-radius: 10px; background: #ffffff; outline: none;  opacity: 0.7;-webkit-transition: .2s;  transition: opacity .2s;}");
            client.println(".slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; border-radius: 50%; background: #ff3410; cursor: pointer; }</style>");
     
            // Get JQuery
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");

            // servo title
            client.println("</head><body style=\"background-color:lightsteelblue;\"><h1 style=\"color:#ff3410;\">Servo Control</h1>");

            // Position display
            client.println("<h2 style=\"color:#000000;\">Position: <span id=\"servoPos\"></span>&#176;</h2>");

            // Slider control
            client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider\" onchange=\"servo(this.value)\" value=\"" + valueString + "\"/>");

            // Javascript
            client.println("<script>var slider = document.getElementById(\"servoSlider\");");
            client.println("var servoP = document.getElementById(\"servoPos\"); servoP.innerHTML = slider.value;");
            client.println("slider.oninput = function() { slider.value = this.value; servoP.innerHTML = this.value; }");
            client.println("$.ajaxSetup({timeout:1000}); function servo(pos) { ");
            client.println("$.get(\"/?value=\" + pos + \"&\"); {Connection: close};}</script>");
            
            // Sensor title
            client.println("</head><body style=\"background-color:lightsteelblue;\"><h1 style=\"color:#ff3410;\">HC-SR04 Sensor</h1>");
            // Distance
            UltraSensor();
            client.println("<h2 style=\"color:#000000;\">Distance in cm:  </h2>");
            client.println("<h2 style=\"color:#000000;\">");
            client.println(distance);
            client.println("</h2>");

            // End page
            client.println("</body></html>");
            
            // GET data
            if (header.indexOf("GET /?value=") >= 0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');

              // String with motor position
              valueString = header.substring(pos1 + 1, pos2);

              // Move servo into position
              myservo.write(valueString.toInt());
              // Print value to serial monitor
              Serial.print("Val = ");
              Serial.println(valueString);

            }
            // The HTTP response ends with another blank line
            client.println();

            // Break out of the while loop
            break;

          } else {
            // New lline is received, clear currentLine
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


void UltraSensor(void)
{
  // Clears the TRIGGER
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);
  // Sets the TRIGGER on HIGH state for 10 micro seconds
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  //delay(500);
}
