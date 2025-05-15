#include <Arduino.h>
#include <Wire.h> // Included as it was in the original code snippet
#include "microphone.h"
#include "ultrasonic.h"

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // Starts the serial communication
  setupMicrophone();
  setupUltrasonic();
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
  loopMicrophone();
  //float currentDistanceCm = loopUltrasonic();
  //Serial.print("Main loop - Distance (cm): ");
  //Serial.println(currentDistanceCm);
  delay(10); // Delay to match original measurement frequency
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}