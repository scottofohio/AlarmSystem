#include <SPI.h>
#include <WiFi101.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_ZeroI2S.h"
#include <SPI.h>
#include "arduino_secrets.h"

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

int keyIndex = 0;              // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;
WiFiServer server(80);
bool alarmStatus = false;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

boolean doorChime = true;
boolean armedStatus = false;
boolean soundAlarm = false;
boolean resetOpenDoor = false;  

#define SAMPLERATE_HZ 44100  
#define AMPLITUDE     ((1<<29)-1) 
#define WAV_SIZE      256   
#define D4_HZ      293.66
#define F4_HZ      349.23

float scale[] = { F4_HZ, D4_HZ };
int32_t triangle[WAV_SIZE] = {0};

Adafruit_ZeroI2S i2s;

// Setup 
void setup() {
  Serial.begin(9600);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  display.display();
  delay(1000);
  display.clearDisplay();
  
  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);

   // Initialize the I2S transmitter.
  if (!i2s.begin(I2S_32_BIT, SAMPLERATE_HZ)) {
    Serial.println("Failed to initialize I2S transmitter!");
    while (1);
  }
  
  //  Aduio Setup 
  i2s.enableTx();
  generateTriangle(AMPLITUDE, triangle, WAV_SIZE);
  
  serverSetup();
  
  // start the web server on port 80     
  server.begin();   

  //  LED BLUE
  pinMode(5, OUTPUT);

  //  LED green
  pinMode(A1, OUTPUT);

  //  LED red
  pinMode(A2, OUTPUT);

}
  

void loop() {
   // listen for incoming clients
  WiFiClient client = server.available();
  
  // if you get a client,
  if (client) { 

    // make a String to hold incoming data from the client
    String currentLine = ""; 
    
    // loop while the client's connected
    while (client.connected()) { 
      
      // if there's bytes to read from the client,
      if (client.available()) {
        char c = client.read(); // read a byte, then
        Serial.write(c);        // print it out the serial monitor
        if (c == '\n') { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:

          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println(F("HTTP/1.1 200 OK"));
            client.println("Content-type:text/html");
        
            client.println();
            client.print("<!doctype html><html lang='e'><head><meta charset='utf-8'><title>Keypad</title></head>");
            client.print("<form action='/'><input type='hidden' id='key' value='true' name='alarm-toggle'>");
            client.print("<button style='padding: 20px; display: block; background-color: #fff; border: 5px solid red; width: 100%; margin-bottom: 20px;'>ALARM ON</button></form>");
            client.print("<form action='/'><input type='hidden' id='key' value='false' name='alarm-toggle'>");
            client.print("<button style='padding: 20px; display: block; background-color: #fff; border: 5px solid blue; width: 100%; margin-bottom: 20px;'>ALARM OFF</button></form>");
            client.print("</body></html>");

            client.println();
  
            break; // break out of the while loop:
          
          } else { // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        
        } else if (c != '\r')  { // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
        
        if(armedStatus == true) { 
          digitalWrite(A2, HIGH);
          digitalWrite(A1, LOW);
          if(soundAlarm == true) {
            alertText("ALARM", "ALARM", 2);
            doorAlert(soundAlarm);
           }
           
          if (currentLine.endsWith("GET /door-1/open") 
           || currentLine.endsWith("GET /door-2/open"))  {
            soundAlarm = true;     
          }
            
          if(currentLine.endsWith("GET /?alarm-toggle=false")) {
            alertText("Alarm Off", "", 2);;
            armedStatus = false;
            soundAlarm = false; 
          }   
             
        } else {
          digitalWrite(A1, HIGH);
          digitalWrite(A2, LOW);
           if(currentLine.endsWith("GET /?alarm-toggle=true")) {
            alertText("Alarm Set", "", 2);
            armedStatus = true;    
           } else if(currentLine.endsWith("GET /?alarm-toggle=false")) {
            alertText("Alarm ", "Off", 2);;
            armedStatus = false; 
           }  
          
          // Door Open Alarm Status Off 
          if (currentLine.endsWith("GET /door-1/open")
          || currentLine.endsWith("GET /door-2/open"))  {
            digitalWrite(5, HIGH);
            if(doorChime == true) { 
               doorAlert(soundAlarm);
            }
          } 
          // Door Closed Alarm Status Off 
          if (currentLine.endsWith("GET /door-1/closed")
          || currentLine.endsWith("GET /door-2/closed")) {
            digitalWrite(5, LOW);
            doorChime = true;
          }
        } 
      }
    }
    client.stop(); // close the connection:   
  }
} // end void loop; 
