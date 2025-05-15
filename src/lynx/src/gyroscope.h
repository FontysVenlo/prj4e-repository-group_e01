#ifndef GYROSCOPE_H
#define GYROSCOPE_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Function declarations
void setupgyro(void);
void loopgyro(void);

#endif // GYROSCOPE_H