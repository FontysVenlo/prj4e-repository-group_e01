#include "microphone.h"

// Pin definition
const int AMP_PIN = 0; // First microphone
const int AMP_PIN2 = 4; // Second microphone 

// Constants
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
const unsigned long trackingDuration = 10000; // 10 seconds in milliseconds

void setupMicrophone() {

}

double loopMicrophone() {
  unsigned long startMillis = millis(); // Start of sample window
  unsigned long trackingStartMillis = millis(); // Start of 10-second tracking

  double maxDifference = 0; // Variable to track the maximum difference

  while (millis() - trackingStartMillis < trackingDuration) {
    unsigned int sample1, sample2;

    unsigned int signalMax1 = 0;
    unsigned int signalMin1 = 1024;
    unsigned int signalMax2 = 0;
    unsigned int signalMin2 = 1024;

    // Collect data for 50 mS for both microphones
    while (millis() - startMillis < sampleWindow) {
      sample1 = analogRead(AMP_PIN);
      if (sample1 < 4095) {
        if (sample1 > signalMax1) {
          signalMax1 = sample1;
        } else if (sample1 < signalMin1) {
          signalMin1 = sample1;
        }
      }

      sample2 = analogRead(AMP_PIN2);
      if (sample2 < 4095) {
        if (sample2 > signalMax2) {
          signalMax2 = sample2;
        } else if (sample2 < signalMin2) {
          signalMin2 = sample2;
        }
      }
    }

    // Reset the sample window start time
    startMillis = millis();

    // Calculate for Mic 1
    unsigned int peakToPeak1 = signalMax1 - signalMin1;
    double volts1 = (peakToPeak1 * 5.0) / 1024;

    // Calculate for Mic 2
    unsigned int peakToPeak2 = signalMax2 - signalMin2;
    double volts2 = (peakToPeak2 * 5.0) / 1024;

    // Calculate the difference
    double difference = abs(volts1 - volts2);

    // Update the maximum difference
    if (difference > maxDifference) {
      maxDifference = difference;
    }
  }
  return maxDifference; // Return the maximum difference found during the 10 seconds
  // Print the maximum difference after 10 seconds
  Serial.println("--- Maximum Difference ---");
  Serial.print("Max Difference (Volts): ");
  Serial.println(maxDifference, 4);
  Serial.println("--------------------------");
}