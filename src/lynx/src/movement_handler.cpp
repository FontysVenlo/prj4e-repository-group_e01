#include <Arduino.h>
#include <ESP32Servo.h>
#include "movement_handler.h"
#include <freertos/semphr.h>
#include <math.h>
#include <Wire.h>


#define PWMB 2
#define PWMA 5
#define SERVO_PIN 12


static float distance(float px, float py);
static void steeringController();
static void updateModel();
static void updateServo(float steering_angle_radians);
static void updateMotors(int motor_percentage);

static VehicleState state;
static Servo servo;
static Path path;
static int current_target;
static bool movement_stop;
static bool manual_movement;

static const float wheelbase = 20.0;
static const float lr = 6.0;
static const float lf = 14.0;
static const float dt = 0.02;
static const float look_ahead_distance = 10.0;


static float distance(float px, float py){
    float dx = px - state.x;
    float dy = py - state.y;

    return sqrt( dx * dx + dy * dy );
}

static void steeringController(){

    for(int i = current_target; i < path.length; i++){
        float px = path.points[i].x;
        float py = path.points[i].y;
        float distance_to_target = distance(px,py);
        
        if (distance_to_target >= look_ahead_distance) {

            // Transform to vehicle coordinates
            float dx = px - state.x;
            float dy = py - state.y;
            float local_x = dx * cos(-state.psi) - dy * sin(-state.psi);
            float local_y = dx * sin(-state.psi) + dy * cos(-state.psi);
            
            current_target = i;

 
            // Curvature and steering
            float curvature = (2 * local_y) / (look_ahead_distance * look_ahead_distance);
            float steering_angle = atan(curvature * wheelbase);

            const float max_steering = 30.0 * PI / 180.0;
            if (steering_angle > max_steering) steering_angle = max_steering;
            if (steering_angle < -max_steering) steering_angle = -max_steering;
            state.delta = steering_angle;

            float steering_factor = 1.0 - (abs(state.delta) / (30.0 * PI / 180)) * 0.3;

            state.v = 32 * steering_factor;
            return;
          }
    }
    //No target found
    state.delta = 0;
    current_target = path.length;
    return;
}

static void updateModel(){
    
    float beta = atan((lr / wheelbase) * tan(state.delta));

    float xdot = state.v * (cos(beta + state.psi));
    float ydot = state.v * (sin(beta + state.psi));
    float psidot = (state.v / wheelbase) * cos(beta) * tan(state.delta);

    state.x = state.x +  dt * xdot;
    state.y = state.y +  dt * ydot;
    state.psi = fmod(state.psi + dt * psidot, 2 * PI);
    if (state.psi > PI) state.psi -= 2 * PI;
    if (state.psi < -PI) state.psi += 2 * PI;
}

static void updateServo(float steering_angle_radians){
    int degree = (int)(-steering_angle_radians * 180.0 / PI * 1.15) + 94;
    servo.write(degree); 
}

static void updateMotors(int motor_percentage){
    
    int pwm_value;
    pwm_value = motor_percentage > 100 ? 100 : motor_percentage;

    int value = 0;
    if(motor_percentage > 0){
        value = 9;
    }else{
        pwm_value = 100;
    }

    Wire.beginTransmission(0x20);
    Wire.write(0x13);//PortsB
    Wire.write(value);
    Wire.endTransmission();

    analogWrite(PWMA, pwm_value * 1.2);
    analogWrite(PWMB, pwm_value * 1.2);
}

void initializeMovementHandler(){

    servo.attach(SERVO_PIN);
    servo.write(93);

    //set state of model to 0
    state.x = 0.0;
    state.y = 0.0;
    state.psi = 0.0;
    state.v = 0.0;
    state.delta = 0.0;

    //stop movement 
    movement_stop = true;
    updateMotors(0); 

    //set path
    Point start[] = {{0,0}};
    Path new_path = {
        .points = start,
        .length = 1
    };
    setPath(new_path, 1);

}

void manualMovement(int steering_angle, int motor_percentage){
    manual_movement = true;
    movement_stop = false;
    state.delta = steering_angle * PI / 180;
    updateServo(state.delta);
    float steering_factor = 1.0 - (abs(state.delta) / (30.0 * PI / 180)) * 0.3;
    state.v = 32 * steering_factor;
    updateMotors(motor_percentage);
}

void startMovement(){
    manual_movement = false;
    movement_stop = false;
}

void stopMovement(){
    movement_stop = true;  
}

void updateMovement(){

    if(current_target >= path.length && !manual_movement){
        movement_stop = true;
    }

    if(movement_stop){
        state.v = 0;
        updateMotors(0);
        return;
    }

    if(manual_movement){
        updateModel();
        return;
    }

    steeringController();
    updateModel();

    if (state.v > 0) {
        updateMotors(100);  
    } else {
        updateMotors(0);   
    }

    updateServo(state.delta);
    
}


void setPath(Path new_path, int new_target){
    if(movement_stop){

        if (path.points != NULL) {
            free(path.points);
            path.points = NULL;
        }

        path.points = (Point*) malloc(sizeof(Point) * new_path.length);
        if (path.points == NULL) {
            return;
        }

        for (int i = 0; i < new_path.length; i++) {
            path.points[i] = new_path.points[i];
        }

        path.length = new_path.length;
        current_target = new_target;
    }
}

void resetState(){
    if(movement_stop){
        state.x = 0.0;
        state.y = 0.0;
        state.psi = 0.0;
        state.v = 0.0;
        state.delta = 0.0;
    }
}

void generateRightCirclePath(Point* path, float radius, int sample_size){
  float step_size = 2 * PI / sample_size;
  for(int i = 0; i < sample_size; i++){
      path[i].x = radius * cos(step_size * ((sample_size/4 - i + sample_size) % sample_size));
      path[i].y = radius * sin(step_size * ((sample_size/4 - i + sample_size) % sample_size)) - radius;
  }
}

void generateLeftCirclePath(Point* path, float radius, int sample_size){
  float step_size = 2 * PI / sample_size;
  for(int i = 0; i < sample_size; i++){
    path[i].x = radius * cos(step_size * ((3 * sample_size/4 + i + 1) % sample_size));
    path[i].y = radius * sin(step_size * ((3 * sample_size/4 + i + 1) % sample_size)) + radius;
  }
}

void generateSinePath(Point* path, float amplitude, float wavelength, int sample_size) {
  float step_size = (2 * PI) / sample_size; 
  for (int i = 0; i < sample_size; i++) {
      path[i].x = i * wavelength / sample_size;
      path[i].y = amplitude * sin((2 * PI * i) / sample_size);
  }
}

void setupMovementHandler(){

  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);

  analogWrite(PWMA, 120);
  analogWrite(PWMB, 120);

  Wire.beginTransmission(0x20);
  Wire.write(0x01); //IODIRB
  Wire.write(0x00); //ALL OUTPUT
  Wire.endTransmission();

  Wire.beginTransmission(0x20);
  Wire.write(0x13);//PortsB
  Wire.write(0);//All ports off
  Wire.endTransmission();
}


