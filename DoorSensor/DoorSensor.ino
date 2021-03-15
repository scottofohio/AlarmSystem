#include <SPI.h>
#include <WiFiNINA.h>

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;
IPAddress server(192,168,0,23);  // numeric IP for Google (no DNS)
WiFiClient client;
int doorSwitch = 2;
int switchState = LOW;
int Led = 13;
int doorId = 1;
void setup() {
   Serial.begin(9600);
   pinMode(doorSwitch, INPUT_PULLUP); 
   pinMode(Led, OUTPUT);
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWiFiStatus();
  Serial.println("\nStarting connection to server...");
}

void loop() {
  int switchState = digitalRead(doorSwitch);
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    if (switchState== LOW ) {
      Serial.println("door closed"); 
      client.println("GET /door-1/closed HTTP/1.1");
      client.println("Host: 192.168.0.23");
      client.println("Connection: close");
      digitalWrite(Led, LOW);
    } else if(switchState == HIGH){
     Serial.println("door open");
     client.println("GET /door-1/open HTTP/1.1");
     client.println("Host: 192.168.0.23");
     client.println("Connection: close");
     client.println();
      digitalWrite(Led, HIGH);
    }
  }
  delay(500);
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
