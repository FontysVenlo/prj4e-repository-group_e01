#include "microphone.h"

// Pin definition
const int AMP_PIN = 0; // First microphone
const int AMP_PIN2 = 4; // Second microphone (ensure this pin is appropriate for your hardware)

// Constants
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)

void setupMicrophone() {
  // analogRead doesn't require pinMode, so no changes needed here for a second pin.
}

void loopMicrophone() {
  unsigned long startMillis = millis(); // Start of sample window

  unsigned int sample1, sample2;

  unsigned int signalMax1 = 0;
  unsigned int signalMin1 = 1024;
  unsigned int signalMax2 = 0;
  unsigned int signalMin2 = 1024;

  // collect data for 50 mS for both microphones
  while (millis() - startMillis < sampleWindow)
  {
    sample1 = analogRead(AMP_PIN);
    if (sample1 < 4095)  // toss out spurious readings for mic 1
    {
      if (sample1 > signalMax1)
      {
        signalMax1 = sample1;
      }
      else if (sample1 < signalMin1)
      {
        signalMin1 = sample1;
      }
    }

    sample2 = analogRead(AMP_PIN2);
    if (sample2 < 4095)  // toss out spurious readings for mic 2
    {
      if (sample2 > signalMax2)
      {
        signalMax2 = sample2;
      }
      else if (sample2 < signalMin2)
      {
        signalMin2 = sample2;
      }
    }
  }

  // Calculate for Mic 1
  unsigned int peakToPeak1 = signalMax1 - signalMin1;
  double volts1 = (peakToPeak1 * 5.0) / 1024;

  // Calculate for Mic 2
  unsigned int peakToPeak2 = signalMax2 - signalMin2;
  double volts2 = (peakToPeak2 * 5.0) / 1024;

  // Calculate difference
  double differenceVolts = volts1 - volts2;

  Serial.println("--- Microphone Readings ---");
  Serial.print("Mic 1 Peak to Peak: ");
  Serial.println(peakToPeak1);
  Serial.print("Mic 1 Volts: ");
  Serial.println(volts1, 4); // Print volts with 4 decimal places for clarity
  Serial.print("Mic 1 Max raw sample: ");
  Serial.println(signalMax1);

  Serial.print("Mic 2 Peak to Peak: ");
  Serial.println(peakToPeak2);
  Serial.print("Mic 2 Volts: ");
  Serial.println(volts2, 4);
  Serial.print("Mic 2 Max raw sample: ");
  Serial.println(signalMax2);

  Serial.print("Difference (Volts1 - Volts2): ");
  Serial.println(differenceVolts, 4);
  Serial.println("-------------------------");
} 