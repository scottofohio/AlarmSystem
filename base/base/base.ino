
#include <SPI.h>
#include <WiFi101.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_ZeroI2S.h"
#define SAMPLERATE_HZ 44100
#define AMPLITUDE ((1<<28)-1)
#define WAV_SIZE 256
#define LOW_NOTE    220.00
#define HIGH_NOTE   261.63
float scale[] = {LOW_NOTE };
int32_t sawtooth[WAV_SIZE] = {0};
Adafruit_ZeroI2S i2s;
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID        // your network SSID (name)
char pass[] = SECRET_PASS;     // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

bool alarmStatus = false;

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiServer server(80);
void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip  = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.display();
  delay(1000);
  display.clearDisplay();
  display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
  generateSawtooth(AMPLITUDE, sawtooth, WAV_SIZE);
  // set the LED pin mode
  pinMode(5, OUTPUT);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);       // don't continue
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {

    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  server.begin();                           // start the web server on port 80
  printWiFiStatus();                        // you're connected now, so print out the status
}

void loop() {

  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,

    Serial.println("new client");           // print a message out the serial port

    String currentLine = "";                // make a String to hold incoming data from the client

    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                     // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:

          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.print("<h1>enter code</h1>");
            client.print("<form action='http://192.168.0.23/' method='GET' >");
            client.print("<input type='number' name='keycode' maxlength='6' size='6' />");
            client.print("<intput type='submit' value='Submit' /></form><br>");


            client.println();
            // break out of the while loop:
            break;
          } else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
        // Alarm Staus         
        if (alarmStatus == false) {
          // Door Open and Closes
          if (currentLine.endsWith("GET /door-1/open")) {
            digitalWrite(5, HIGH);
            alertText("Door 1", "Open");
          } else if (currentLine.endsWith("GET /door-1/closed")) {
            digitalWrite(5, LOW);
            alertText("Door 1", "Closed");
          }
        } else if (alarmStatus == true) {
          // Inster a timer and prompt keycode           
        }
      }
    }
    // close the connection:
    client.stop();
    //    Serial.println("client disonnected");
  }
}


void alertText(char *lineOne, char *lineTwo) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F(lineOne));
  display.setCursor(20, 20);
  display.println(F(lineTwo));
  display.display();
}
