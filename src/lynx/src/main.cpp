#include <Arduino.h>
#include <Wire.h> // Included as it was in the original code snippet
#include "microphone.h"
#include "ultrasonic.h"
#include "gyroscope.h"
#include "movement_handler.h"

#define SDA 21
#define SCL 22

TaskHandle_t movement_task_handle = NULL;

// put function declarations here:
int myFunction(int, int);
void movementTask(void * arg);
void timer_callback(void* arg);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // Starts the serial communication
  setupMicrophone();
  setupUltrasonic();
  setupgyro();
  int result = myFunction(2, 3);

  Wire.begin(SDA,SCL);

  Wire.beginTransmission(0x20);
  Wire.write(0x01); //IODIRB
  Wire.write(0x00); //ALL OUTPUT
  Wire.endTransmission();

  xTaskCreate(movementTask, "Movement_Task", 2048, NULL, 7, &movement_task_handle);
}

void loop() {
  // put your main code here, to run repeatedly:
  loopMicrophone();
  loopUltrasonic();
  loopgyro();
  delay(1000); // Delay to match original measurement frequency
  //float currentDistanceCm = loopUltrasonic();
  //Serial.print("Main loop - Distance (cm): ");
  //Serial.println(currentDistanceCm);
  delay(10); // Delay to match original measurement frequency
  
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

void timer_callback(void* arg) {
  if (movement_task_handle != NULL) {
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
      vTaskNotifyGiveFromISR(movement_task_handle, &xHigherPriorityTaskWoken);
      if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
      }
  }
}

void movementTask(void * arg){	

  initializeMovementHandler();

  //timer creation
  const esp_timer_create_args_t timer_args = {
    .callback = &timer_callback   
  };
  esp_timer_handle_t timer_handle;
  esp_timer_create(&timer_args, &timer_handle);
  esp_timer_start_periodic(timer_handle, 20000); //50Hz (every 20ms)

  while(true){
    //wait for timer
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    updateMovement();
  }
}