#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

// Pin definitions
extern const int trigPin;
extern const int echoPin;

// Function declarations
void setupUltrasonic();
float loopUltrasonic();

#endif // ULTRASONIC_H 