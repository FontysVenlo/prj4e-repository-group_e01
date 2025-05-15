#ifndef MICROPHONE_H
#define MICROPHONE_H

#include <Arduino.h>

// Pin definition
extern const int AMP_PIN;
extern const int AMP_PIN2; // Added for the second microphone

// Function declarations
void setupMicrophone();
void loopMicrophone();

#endif // MICROPHONE_H 