#ifndef MICROPHONE_H
#define MICROPHONE_H

#include <Arduino.h>

// Pin definition
extern const int AMP_PIN;

// Function declarations
void setupMicrophone();
void loopMicrophone();

#endif // MICROPHONE_H 