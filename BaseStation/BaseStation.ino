
#include <SPI.h>
#include <WiFi101.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <millisDelay.h>

#include "audio.h"
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
unsigned long off_time;
boolean alarmState =false;
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
  i2s.enableTx();

  // Generate waveforms.
  generateSine(AMPLITUDE, sine, WAV_SIZE);
  generateSawtooth(AMPLITUDE, sawtooth, WAV_SIZE);
  generateTriangle(AMPLITUDE, triangle, WAV_SIZE);
  generateSquare(AMPLITUDE, square, WAV_SIZE);

  // set the LED pin mode
  pinMode(5, OUTPUT);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true)
      ; // don't continue
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {

    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid); // print the network name (SSID);
    alertText("Connecting to: ",ssid); 
    
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  server.begin();    // start the web server on port 80
  printWiFiStatus(); // you're connected now, so print out the status 
  
}

void loop() {
   // listen for incoming clients
  WiFiClient client = server.available();
  
  // if you get a client,
  if (client) { 

    // print a message out the serial port
    Serial.println("new client"); 
    
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
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print("<h1>enter code</h1>");
            client.print("<form action='http://192.168.0.23/' method='GET' >");
            client.print("<input type='number' name='keycode' maxlength='6' size='6' />");
            client.print("<intput type='submit' value='Submit' /></form><br>");
            client.println();
            
            break; // break out of the while loop:
          }  else { // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r')  { // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
        if(armedStatus == true) { 
          // Is the alarm timer on and is it greater than or equal to off_time
          if ((alarmState) && (millis()>=off_time))  {
            
            digitalWrite(5,LOW);
            alarmState = false;
            } else if (!alarmState)  { 
              if(currentLine.endsWith("GET /door-1/open")) {
                digitalWrite(5, HIGH);
                alarmState = true;
                off_time = millis() + 10000;
                if(currentLine.endsWith("GET /keycode/disable") { 
                  alarmState = false;
                  armedStatus = false;               
                } else {
                  // Set Alarm Here                                   
                } 
             } 
          }
        } else {
          // Door Open Alarm Status Off 
          if (currentLine.endsWith("GET /door-1/open"))  {
            alertText("Door 1", "Open");
            if(doorChime == true) { 
               audioPlay();
            }
          }
          // Door Closed Alarm Status Off 
          if (currentLine.endsWith("GET /door-1/closed")) {
            digitalWrite(5, LOW);
            alertText("Door 1", "Closed");
            doorChime = true;
          }
        } 
      }
    }
    client.stop(); // close the connection:   
  }
} // end void loop; 
void alertText(char *lineOne, char *lineTwo) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println(F(lineOne));
  display.setCursor(10, 40);
  display.println(F(lineTwo));
  display.display();
}

void audioPlay() { 
  for (int i=0; i<2; ++i) {
    // Play the note for a quarter of a second.
    playWave(triangle, WAV_SIZE, scale[i], 0.15);
    // Pause for a tenth of a second between notes.
    delay(100);
  }
 doorChime = false;   
}

void printWiFiStatus() {
  
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
 
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  
}
