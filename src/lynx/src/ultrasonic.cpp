#include "ultrasonic.h"

// Pin definitions
const int trigPin = 13;
const int echoPin = 36;

// Define sound speed in cm/µs
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

void setupUltrasonic() {
  pinMode(trigPin, OUTPUT);      // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);       // Sets the echoPin as an Input
}

float loopUltrasonic() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance in cm
  distanceCm = duration * SOUND_SPEED / 2;
  
  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);
  return distanceCm;
} 