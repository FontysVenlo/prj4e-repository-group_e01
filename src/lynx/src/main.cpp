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
void waitForObstacle(float threshold);
void follow(void *arg);
double direction = 0;
float distance;

// Enum to define robot states
enum RobotState {
    IDLE,
    TURNING,
    MOVING_FORWARD,
    AVOIDING_OBSTACLE
};

RobotState currentState = IDLE;



int steering_angle = 0;

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
  xTaskCreate(follow, "Follow",2048,NULL,5,NULL);
}

void loop() {
  // Call follow() instead of listenAndTurn()

  //loopgyro();
  //manualMovement(30, 80); // Start moving forward
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


void listenAndTurn() {
    updateAnimation(listening1, listening2);
    double direction = loopMicrophone();
    
    updateAnimation(standard1, standard2);
    int steering_angle = direction > 0 ? -30 : 30;

    // Start turning
    manualMovement(steering_angle, 100);
    Serial.println("Turning started");
    waitForObstacle(50);

    // Resume forward movement
    delay(500);
    manualMovement(0, 100);
    Serial.println("Movement started again");
    waitForObstacle(10);
}

void waitForObstacle(float threshold) {
    while (true) {
        if (distance < threshold) {
            stopMovement();
            Serial.println(distance);
            break;
        }
        delay(30);
    }
}

void follow(void *arg) {
    while (true) {
        switch (currentState) {
            case IDLE:
                Serial.println("State: IDLE");
                updateAnimation(listening1, listening2);
                if (loopMicrophone() != 0) {
                    currentState = TURNING;
                }
                break;

            case TURNING:
                Serial.println("State: TURNING");
                updateAnimation(standard1, standard2);
                //direction = loopMicrophone();
                direction = 2;
                if (direction < -1 )
                {
                  steering_angle = -30;
                  Serial.println(direction);
                  Serial.println("Turning right");
                } else if (direction > 1) {
                  steering_angle = 30;
                  Serial.println(direction);
                  Serial.println("Turning left");
                } else {
                  steering_angle = 0;
                  Serial.println(direction);
                  Serial.println("No significant direction change detected, staying straight");
                }
                Serial.println(steering_angle);
                manualMovement(steering_angle, 100);
                waitForObstacle(25);
                vTaskDelay(pdMS_TO_TICKS(1000)); 
                currentState = MOVING_FORWARD;
                break;

            case MOVING_FORWARD:
                Serial.println("State: MOVING_FORWARD");
                manualMovement(0, 100);
                if (distance < 10) {
                    currentState = AVOIDING_OBSTACLE;
                }
                break;

            case AVOIDING_OBSTACLE:
                Serial.println("State: AVOIDING_OBSTACLE");
                stopMovement();
                currentState = IDLE;
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Prevent watchdog timer reset
    }
}