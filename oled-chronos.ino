#include <Adafruit_SSD1306.h>
#include <splash.h>

#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>

#include <ChronosESP32.h>

/*
   MIT License

   Copyright (c) 2021 Felix Biego

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#include <OLED_I2C.h>
#include <ChronosESP32.h>
#include <WS2812FX.h>
#include <graphics.h>


#define LED_COUNT 1
#define LED_PIN 27
#define BUILTINLED 2
#define BUTTON_PIN 0

OLED myOLED(21, 22); //(SDA, SCL)

ChronosESP32 watch("ESP32 Watch");
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

ChronosTimer notify;

extern uint8_t SmallFont[], MediumNumbers[];

static bool deviceConnected = false;
static int id = 0;
long timeout = 10000, timer = 0, scrTimer = 0;
bool rotate = false, flip = false, hr24 = true, screenOff = false, scrOff = false, b1, call = false;
int scroll = 0, bat = 0, lines = 0, msglen = 0, msgIndex = 0;
char msg[126];
String msg0, msg1, msg2, msg3, msg4, msg5;


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

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting Watch");
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUILTINLED, OUTPUT);

myOLED.begin(SSD1306_128X32);
   
    // In case the library failed to allocate enough RAM for the display buffer...



  watch.setConfigurationCallback(configCallback);
  watch.setNotificationCallback(notificationCallback);
  watch.setRingerCallback(ringerCallback);
  //  watch.setDataCallback(dataCallback);
  //  watch.setRawDataCallback(rawCallback);
  watch.begin();

  watch.set24Hour(true);
  watch.setBattery(80);

  


}

void loop()
{

  watch.loop();
 


    
    
    watch.clearNotifications();

  digitalWrite(BUILTINLED, watch.isConnected());

  if (call)
  {
    myOLED.print("Incoming Call", CENTER, 12);
    myOLED.print(msg0, CENTER, 34);
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

  myOLED.update();
}

void showNotification()
{
  myOLED.setFont(SmallFont);
  myOLED.print(msg0, LEFT, 1 - scroll);
  myOLED.print(msg1, LEFT, 12 - scroll);
  myOLED.print(msg2, LEFT, 23 - scroll);
  myOLED.print(msg3, LEFT, 34 - scroll);
  myOLED.print(msg4, LEFT, 45 - scroll);
  myOLED.print(msg5, LEFT, 56 - scroll);
}

void printLocalTime()
{
  watch.clearNotifications();
  myOLED.print(watch.getAmPmC(false), RIGHT, 10);

  myOLED.setFont(MediumNumbers);
  myOLED.print(watch.getHourZ() + watch.getTime(":%M:%S"), CENTER, 0);
  myOLED.setFont(SmallFont);
  myOLED.print(watch.getTime("%a %d %b"), CENTER, 21);

  myOLED.print(String(watch.getWeatherAt(0).temp) + "C", LEFT, 44);
  myOLED.print(watch.getWeatherCity(), LEFT, 54);

 
    myOLED.drawBitmap(0, 15, bluetooth, 16, 16);
    myOLED.drawRect(110, 23, 127, 30);
    myOLED.drawRectFill(108, 25, 110, 28);
    myOLED.drawRectFill(map(watch.getPhoneBattery(), 0, 100, 127, 110), 23, 127, 30);
    myOLED.print(String(watch.getPhoneBattery()) + "%", RIGHT, 34);
   myOLED.update();
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
