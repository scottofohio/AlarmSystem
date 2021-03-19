void printWiFiStatus() {
  //  use this for debugging wifi
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

void generateTriangle(int32_t amplitude, int32_t* buffer, uint16_t length) {
  // Generate a triangle wave signal with the provided amplitude and store it in
  // the provided buffer of size length.
  float delta = float(amplitude)/float(length);
  for (int i=0; i<length/2; ++i) {
    buffer[i] = -(amplitude/2)+delta*i;
  }
    for (int i=length/2; i<length; ++i) {
    buffer[i] = (amplitude/2)-delta*(i-length/2);
  }
}

void playWave(int32_t* buffer, uint16_t length, float frequency, float seconds) {
  // Play back the provided waveform buffer for the specified
  // amount of seconds.
  // First calculate how many samples need to play back to run
  // for the desired amount of seconds.
  uint32_t iterations = seconds*SAMPLERATE_HZ;
  // Then calculate the 'speed' at which we move through the wave
  // buffer based on the frequency of the tone being played.
  float delta = (frequency*length)/float(SAMPLERATE_HZ);
  // Now loop through all the samples and play them, calculating the
  // position within the wave buffer for each moment in time.
  for (uint32_t i=0; i<iterations; ++i) {
    uint16_t pos = uint32_t(i*delta) % length;
    int32_t sample = buffer[pos];
    // Duplicate the sample so it's sent to both the left and right channel.
    // It appears the order is right channel, left channel if you want to write
    // stereo sound.
    i2s.write(sample, sample);
  }
}
