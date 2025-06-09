#include "monitor.h"
#include "bitmaps.h"
#include <SPI.h>
#include <Wire.h>
#include <Arduino.h>


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
unsigned long previousMillis = 0; // will store last time LED was updated

void setup_monitor()
{
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // Clear the buffer.
  display.clearDisplay();
  delay(4000);

  display.clearDisplay();
  display.drawBitmap(0, (32 - (ImageHeight1 / 2)), load, ImageWidth1, ImageHeight1, 1);
  display.display();
}

void updateAnimation(const unsigned char PROGMEM image1[], const unsigned char PROGMEM image2[])
{
  if (millis() - previousMillis >= 1500)
  {
    display.clearDisplay();
    display.drawBitmap(0, (32 - (ImageHeight1 / 2)), image1, ImageWidth1, ImageHeight1, 1);
    display.display();
  }
  if (millis() - previousMillis >= 2250)
  {
    display.clearDisplay();
    display.drawBitmap(0, (32 - (ImageHeight1 / 2)), image2, ImageWidth1, ImageHeight1, 1);
    display.display();

    previousMillis = millis();
  }
}