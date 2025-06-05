#include <Arduino.h>
#include <Wire.h> // Included as it was in the original code snippet
#include "microphone.h"
#include "ultrasonic.h"
#include "gyroscope.h"
#include "movement_handler.h"
#include "monitor.h"
#include "bitmaps.h"

#define SDA 21
#define SCL 22

TaskHandle_t movement_task_handle = NULL;

// put function declarations here:
int myFunction(int, int);
void movementTask(void * arg);
void ultrasonicTask(void *arg);
void timer_callback(void* arg);
void listenAndTurn();

float distance;


void setup() {
  // put your setup code here, to run once:
  Wire.begin(SDA,SCL);
  Serial.begin(115200); // Starts the serial communication

  setupMovementHandler();
  setupMicrophone();
  setupUltrasonic();
  setupgyro();
  setup_monitor();
  int result = myFunction(2, 3);

  xTaskCreatePinnedToCore(movementTask, "Movement_Task", 2048, NULL, 7, &movement_task_handle, 0);
  xTaskCreate(ultrasonicTask, "Ultrasonic_Task", 2048, NULL, 5, NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
  listenAndTurn();

  loopgyro();
  
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

// Wrapper task for ultrasonic sensor
void ultrasonicTask(void *arg) {
  while (true) {
    distance = loopUltrasonic();
    // Optionally, do something with distance or add a delay
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void listenAndTurn(){
  double direction = loopMicrophone();
  int steering_angle = direction > 0 ? -30 : 30;
  manualMovement(steering_angle, 100);
  Serial.println("turning started");
  
  while(true){
    if(distance < 100){
      stopMovement();
      Serial.print("Movement stopped after turning, distance:");
      Serial.println(distance);
      break;
    }
    delay(30);
  }

  delay(500);
  manualMovement(0, 100);
  
  Serial.println("Movment started again");

  while(true){
    if(distance < 10){
      stopMovement();
      Serial.print("Movement stopped again, distance:");
      Serial.println(distance);
      break;
    }
    delay(30);
  }
}