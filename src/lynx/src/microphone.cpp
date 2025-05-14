#include "microphone.h"

// Pin definition
const int AMP_PIN = 39;

// Constants
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

void setupMicrophone() {
  // Nothing specific to set up for the microphone pin here as analogRead doesn't require pinMode.
  // If there were specific setup, like for an I2S microphone, it would go here.
}

void loopMicrophone() {
  unsigned long startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(AMP_PIN);
    if (sample < 1024)  // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;  // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  double volts = (peakToPeak * 5.0) / 1024;  // convert to volts

  Serial.print("Microphone Peak to Peak: ");
  Serial.println(peakToPeak);
  Serial.print("Microphone Volts: ");
  Serial.println(volts);
} 