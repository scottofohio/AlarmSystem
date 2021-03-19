# Home Alarm System
Arduino based alarm system

## Hardware used 
- [Arduino MKR1000](https://store.arduino.cc/usa/arduino-mkr1000), used as the alarm server / base station.
- [Arduino Nano 33 IOT](https://store.arduino.cc/usa/nano-33-iot) used for door sensors  
- [2 color OLED Display](https://www.amazon.com/UCTRONICS-SSD1306-Self-Luminous-Display-Raspberry/dp/B072Q2X2LL/ref=sr_1_7) used for base station display(any i2c oled display will work) 
- [NTE Magnetic Alarm Reed](https://www.ntepartsdirect.com/ENG/PRODUCT/54-630) used for door sensor(any reed alarm switch will work)
- [Adafruit I2S 3W Class D Amplifier Breakout](https://www.adafruit.com/product/3006) 
- A Speaker. Check the amp specifications and get a speaker to match that. 
- LEDs 
- 10k resistor used for the Alarm Reed Ground 
- 1k resisor, used for all LEDs 

## Required Software Libraries
All of these can be installed via the Arduino IDE. 
- [SPI](https://www.arduino.cc/en/reference/SPI)
- [Adafruit_ZeroI2S](https://github.com/adafruit/Adafruit_ZeroI2S) - For the AMP breakout 
- [WiFi101](https://github.com/arduino-libraries/WiFi101)
- [WiFiNINA](https://github.com/arduino-libraries/WiFiNINA)
- [Wire](https://www.arduino.cc/en/reference/wire)
- [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)


## Pin Configurations 

### MKR1000 

#### LED 
- Pin 5  
- 1k resistor to ground

#### OLED Dislay
- VCC to 5v
- GND to GND 
- SCL to SCL 
- SDA to SDA 

#### i2s Amplifier 
- VIN to 5v
- GND to GND
- LRCLK to PIN 3 
- BCLK to PIN 2 
- DIN to A6

### NANO 33 IOT
#### LED 
-  PIN D13 
-  1k resistor to GND

#### MAgnetic Reed Switch 
- Pin ~D2 
- 10k resistor to ground 
