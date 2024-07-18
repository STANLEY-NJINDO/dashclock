/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ChronosESP32.h>
#include <graphics.h>
ChronosESP32 watch("ESP32 Watch");

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
 
 

  // Clear the buffer
  display.clearDisplay();

  
  testdrawcircle();    // Draw circles (outlines)

  testfillcircle();    // Draw circles (filled)



 

  testdrawstyles();    // Draw 'stylized' characters

  testscrolltext();    // Draw scrolling text

  watch.setConfigurationCallback(configCallback);
  watch.setNotificationCallback(notificationCallback);
  watch.setRingerCallback(ringerCallback);
  //  watch.setDataCallback(dataCallback);
  //  watch.setRawDataCallback(rawCallback);
  watch.begin();

  watch.set24Hour(true);
  watch.setBattery(80);

}

 

void testdrawcircle(void) {
  display.clearDisplay();

  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfillcircle(void) {
  display.clearDisplay();

  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=3) {
    // The INVERSE color is used so circles alternate white/black
    display.fillCircle(display.width() / 2, display.height() / 2, i, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn circle
    delay(1);
  }

  delay(2000);
}








void testdrawstyles(void) {
  display.clearDisplay();
display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("SANJEEWA"));
  display.setCursor(0, 16);
  display.println(F("ELECTRONIC"));
  display.display();      // Show initial text
  delay(1000);


}

void testscrolltext(void) {
  display.clearDisplay();


    display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("RIDE SAFE"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}
void notificationCallback(Notification notification)
{
  Serial.print("Notification received at ");
  Serial.println(notification.time);
  Serial.print("From: ");
  Serial.print(notification.app);
  Serial.print("\tIcon: ");
  Serial.println(notification.icon);
  Serial.println(notification.message);

  copyMsg(notification.message);
  msgIndex = 0;

  notify.time = millis();
  notify.active = true;
  
}

void ringerCallback(String caller, bool state)
{
  call = state;
  if (state)
  {
    Serial.print("Ringer: Incoming call from ");
    Serial.println(caller);
    msg0 = caller;

    
  }
  else
  {
    Serial.println("Ringer dismissed");
    
  }
}

void rawCallback(uint8_t *data, int length)
{
  Serial.println("Received Raw");
  for (int i = 0; i < length; i++)
  {
    Serial.printf("%02X ", data[i]);
  }
  Serial.println();
}

void dataCallback(uint8_t *data, int length)
{
  Serial.println("Received Data");
  for (int i = 0; i < length; i++)
  {
    Serial.printf("%02X ", data[i]);
  }
  Serial.println();
}

void configCallback(Config config, uint32_t a, uint32_t b)
{
  switch (config)
  {
  case CF_APP:
    // state is saved internally
    Serial.print("App Version Code [");
    Serial.print(a); // int code = watch.getAppCode();
    Serial.print("] Name -> ");
    Serial.println(watch.getAppVersion());
    break;
  }
}

void loop()
{
   watch.loop();
    if (call)
  {
    display.println("Incoming Call", CENTER, 12);
    display.println(msg0, CENTER, 34);
  }
  else
  {
    if (notify.active)
    {
      if (notify.time + notify.duration < millis())
      {
        // timer end
        notify.active = false;
        msgIndex = 0;
        
      }
      showNotification();
    }
    else
    {
      printLocalTime(); // display time
    }
  }

 display.display;
}
void showNotification()
{
  display.setTextSize(2);
  display.print(msg0, LEFT, 1 - scroll);
  display.print(msg1, LEFT, 12 - scroll);
  display.print(msg2, LEFT, 23 - scroll);
  display.print(msg3, LEFT, 34 - scroll);
  display.print(msg4, LEFT, 45 - scroll);
  display.print(msg5, LEFT, 56 - scroll);
}

void printLocalTime()
{
  display.print(watch.getAmPmC(false), RIGHT, 10);

  display.setTextSize(2);
  display.print(watch.getHourZ() + watch.getTime(":%M:%S"), CENTER, 0);
  display.setTextSize(2);
  display.print(watch.getTime("%a %d %b"), CENTER, 21);

  display.print(String(watch.getWeatherAt(0).temp) + "C", LEFT, 44);
  display.print(watch.getWeatherCity(), LEFT, 54);

  if (watch.isConnected())
  {
    display.drawBitmap(0, 15, bluetooth, 16, 16);
    display.drawRect(110, 23, 127, 30);
    display.drawRectFill(108, 25, 110, 28);
    display.drawRectFill(map(watch.getPhoneBattery(), 0, 100, 127, 110), 23, 127, 30);
    display.print(String(watch.getPhoneBattery()) + "%", RIGHT, 34);
  }
}

void copyMsg(String ms)
{
  msglen = ms.length();
  lines = ceil(float(msglen) / 21);
  switch (lines)
  {
  case 1:
    msg0 = ms.substring(0, msglen);
    msg1 = "";
    msg2 = "";
    msg3 = "";
    msg4 = "";
    msg5 = "";
    break;
  case 2:
    msg0 = ms.substring(0, 21);
    msg1 = ms.substring(21, msglen);
    msg2 = "";
    msg3 = "";
    msg4 = "";
    msg5 = "";
    break;
  case 3:
    msg0 = ms.substring(0, 21);
    msg1 = ms.substring(21, 42);
    msg2 = ms.substring(42, msglen);
    msg3 = "";
    msg4 = "";
    msg5 = "";
    break;
  case 4:
    msg0 = ms.substring(0, 21);
    msg1 = ms.substring(21, 42);
    msg2 = ms.substring(42, 63);
    msg3 = ms.substring(63, msglen);
    msg4 = "";
    msg5 = "";
    break;
  case 5:
    msg0 = ms.substring(0, 21);
    msg1 = ms.substring(21, 42);
    msg2 = ms.substring(42, 63);
    msg3 = ms.substring(63, 84);
    msg4 = ms.substring(84, msglen);
    msg5 = "";
    break;
  case 6:
    msg0 = ms.substring(0, 21);
    msg1 = ms.substring(21, 42);
    msg2 = ms.substring(42, 63);
    msg3 = ms.substring(63, 84);
    msg4 = ms.substring(84, 105);
    msg5 = ms.substring(105, msglen);
    break;
  default:

    break;
  }
}

