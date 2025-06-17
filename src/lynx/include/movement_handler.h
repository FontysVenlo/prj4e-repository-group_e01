#ifndef MOVEMENT_HANDLER
#define MOVEMENT_HANDLER


/** 
 * A point on a path.
 * 
 * Fields:
 * float x : x-position of the point
 * float y : y-position of the point
 **/ 
typedef struct {
    float x;
    float y;
}Point;

/** 
 * A path described by points.
 * 
 * Fields:
 * Point* points : pointer to an array of points
 * int path_length : length of the path
 **/ 
typedef struct {
    Point* points;
    int length;
}Path;

/** 
 * The state of the vehicle.
 * 
 * Fields:
 * float x: x-position
 * float y: y-position
 * float psi: heading angle
 * float v: velocity
 * float delta: steering angle
 **/ 
typedef struct {
    float x;
    float y;
    float psi;
    float v;
    float delta;
}VehicleState;

/** 
 * Sets up the movement handler 
 **/
void setupMovementHandler();

/** 
 * Move Manually 
 **/
void manualMovement(int steering_angle, int motor_percentage);


/** 
 * Start following a path.
 **/
void startMovement();

/** 
 * Stop moving.
 * Stops the motors and sets a value that prevents movement updates.
 **/
void stopMovement();

/** 
 * Set the current path followed by the controller.
 * Movement should be stopped before setting a new path.
 * 
 * @param path the path to be set
 * @param new_target index of the point to start at
 * 
 **/
void setPath(Path new_path, int new_target);

/** 
 * Reset the state of the model.
 * Movement should be stopped before resetting the state.
 **/
void resetState();

/** 
 * Initializes the movement handler.
 * Called internally by the task.
 **/
void initializeMovementHandler();

/** 
 *  Calulates steering angle and updates model and motors.
 *  Called internally by the task.
 **/
void updateMovement();

/** 
 *  Generated a counter clockwise circle path.
 **/
void generateLeftCirclePath(Point* path, float radius, int sample_size);

/** 
 *  Generated a clockwise circle path.
 **/
void generateRightCirclePath(Point* path, float radius, int sample_size);

/** 
 *  Generated a sine wave path.
 **/
void generateSinePath(Point* path, float amplitude, float wavelength, int sample_size);

#endif