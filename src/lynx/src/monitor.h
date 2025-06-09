#ifndef MONITOR_H
#define MONITOR_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4

extern Adafruit_SSD1306 display;
extern unsigned long previousMillis;

void setup_monitor();
void updateAnimation(const unsigned char PROGMEM image1[], const unsigned char PROGMEM image2[]);

#endif // MONITOR_H