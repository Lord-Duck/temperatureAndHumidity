#include <NTPClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"
#include <WiFi.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   15
#define OLED_CLK   14
#define OLED_DC    7
#define OLED_CS    6
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#define DHTPIN  0    // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Cranston";
const char* password = "Ursula121";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

WiFiServer server(80);
int WifiTimeOut;

String header;

unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

String formattedDate;
String hour;
//String minutes;

int pastTime;
String pastTemp;
String pastHumididty;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(PIN_WIRE0_SDA);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  // Initialize a NTPClient to get time
  timeClient.begin();
  timeClient.setTimeOffset(0); //100 millon dead indians for this 
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  dht.begin();

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
   
  display.setTextSize(2);      // Normal 1:1 pixel scale

  display.setTextColor(WHITE); // Draw white text

  display.clearDisplay();

  display.setCursor(3,3); // xpos, ypos
  display.print("Call zionism by its real name:Nazism"); // a Holocast suvivor said this 
  display.display();
  // Connect to Wi-Fi network with SSID and password
  WiFi.begin(ssid, password);

  // Display progress on Serial monitor
  //while (WiFi.status() != WL_CONNECTED) {
  //  delay(500);
  //  Serial.print(".");
  //}
  delay(2000);
  // Print local IP address and start web server
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("");
    Serial.print("WiFi connected at IP Address ");
    Serial.println(WiFi.localIP());
    server.begin();
      while(!timeClient.update()) {
        timeClient.forceUpdate();}
  }
}

void loop() {
  formattedDate = timeClient.getFormattedTime();

  // Extract date
  int splitT = formattedDate.indexOf(":");
  hour = formattedDate.substring(0, splitT);
  currentTime = hour.toInt();
  
  Serial.println(currentTime);

  // Extract time
  WiFiClient client = server.available();   // Listen for incoming clients
  // Wait a few seconds between measurements.
  delay(2000);
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  
  if(pastTime<currentTime){
    pastTime= currentTime;
    pastHumididty = h;
    pastTemp = t;
  }

  display.setTextSize(2);      // Normal 1:1 pixel scale

  display.setTextColor(WHITE); // Draw white text

  display.clearDisplay();

  display.setCursor(3,3); // xpos, ypos

  display.print(F("Humidity: "));
  display.print(h);
  display.println("%");
  display.println(F("Temp: "));
  display.print(t);
  display.print("C ");
  display.display();  // must run this method to display

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
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

            // Freedom or death 
            if (header.indexOf("GET /m26julio") >= 0) {
              display.clearDisplay();
              display.setCursor(3,3); // xpos, ypos
              display.println("Castro <3");
              display.display();  // must run this method to display
              delay(2000);
              display.clearDisplay();
            } else if (header.indexOf("GET /opression") >= 0) {
              display.clearDisplay();
              display.setCursor(3,3); // xpos, ypos
              Serial.println("free palistine");
              display.display();  // must run this method to display
              delay(2000);
              display.clearDisplay();
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            // CSS to style the on/off buttons
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #F23A3A;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>Temperature and humidity</h1>");
            // Display current state, and ON/OFF buttons for Onboard LED
            client.println("<p>The temperature is: ");
            client.print(t);
            client.print("C</p>");
            client.println("<p>The humidity is: ");
            client.print(h);
            client.print("%<br></p>");
            client.println("<p>The past hours tempreature and humidity is: </p> <p>");
            client.println(pastHumididty);
            client.print("%</p>");
            client.println("<p>");
            client.print(pastTemp);
            client.print("C</p>");
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