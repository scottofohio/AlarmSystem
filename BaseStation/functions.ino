
void sdSetup() {
  
 Serial.print("\nInitializing SD card...");

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    while (1);
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }
  
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

void alertText(char *lineOne, char *lineTwo, int fontSize) {
  bool lineHeight; 
  
  display.clearDisplay();
  display.setTextSize(fontSize);
  display.setTextColor(SSD1306_WHITE);

  if(fontSize == 1) {
    display.setCursor(10, 20);
    display.println(F(lineOne));
    display.setCursor(10, 30);
    display.println(F(lineTwo));
  } else {
    display.setCursor(10, 20);
    display.println(F(lineOne));
    display.setCursor(10, 40);
    display.println(F(lineTwo));
  }
  
  display.display();
}



void doorAlert(bool audioLoop) { 
  /*  
   *  Plays a sound when the door opens.  
   *  If alarm is armed and door opens it plays 
   *  until the alarm is deactivated by disabling the 
   *  reset of the doorChime bool.   
   *  
   *  If alarm is not armed the sound plays 
   *  once each time a door opens. 
   *  bool audioLoop passes in the soundAlarm bool in both
   *  uses to check if the alarm is going off or not. 
   *  
  */
  for (int i=0; i<2; ++i) {
    // Play the note for a quarter of a second.
    playWave(triangle, WAV_SIZE, scale[i], 0.15);
    // Pause for a tenth of a second between notes.
    delay(100);
  }   
  if(audioLoop == false) { 
    doorChime = false;   
  }
}



void serverSetup() { 
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
    alertText("connecting to...: ", ssid, 1);
    
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
    alertText("connected ", "Alarm ready to set", 1);
    
  }
}

void disableAlarm() {
  alertText("Alarm", "Disabled", 2);
  Serial.println("Alarm disabled");
  armedStatus = false;
  soundAlarm = false;           
}
