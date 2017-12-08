#include "Arduino.h"
#include <Servo.h>
#include "SpeedyStepper.h"



SpeedyStepper leftMotor;
SpeedyStepper rightMotor;

Servo tool_servo;

bool erasing = false;

typedef void(*STATE_HANDLER_T)(void);

void drawing(void);
void recieving(void);

STATE_HANDLER_T state, last_state;

struct XY_Pos {
  long x;
  long y;
};

struct LR_Step {
  int l;
  int r;
};

/******* Config *******/
int interspool_spacing = 1220; // In mm
const XY_Pos init_pos = { 600, 500 }; // In mm

const int servo_marker = 90;
const int servo_off = 50;
const int servo_erase = 145;

const int eraser_offset = -18; //mm
// Maximum speed to run motors
const int max_speed = 500;
const int max_accel = 700;

const int servo_pin = 10; // Only 9 & 10 are supported
/***** End Config *****/
int pathLength;
XY_Pos loopPath[8];
XY_Pos newPoint;

bool motor_state;

// Conversion from mm to stepper motor steps
const float steps_per_mm = 5; /* 200.0 steps per rotation / 40.0 Circumference in mm */;

LR_Step xy_to_lr(XY_Pos xy){
  LR_Step lr;

  lr.l = 1.0 * sqrt((xy.x*xy.x) + (xy.y*xy.y)) * steps_per_mm;
  lr.r = 1.0 * sqrt(((interspool_spacing - xy.x)*(interspool_spacing - xy.x)) + (xy.y*xy.y)) * steps_per_mm;
  return lr;
}


// Calculate init cable lengths
LR_Step cur_len = xy_to_lr(init_pos);


/* Test Path */
// Units are in mm
// -10 for marker engage
// -20 for marker disengage

const int num_path = 9;
const float path[][2] =   {
  {600,500}, // Init @ origin
  {-10,0},
  {200,150},
  {1000,200},
  {1000,800},
  {600,800},
  {300,800},
  {600,500},
  // {600,500},
  // {600,600},
  // {400,600},
  // {400,400},
  // {500,400},
  // {600,500}, // End at origin
  {-20,0}
};


int sign(float x) {
  if (x>0) return 1;
  if (x<0) return -1;
  return 0;
}

void run_motors(LR_Step delta_l){
  /* Move lr steps*/

  /* TODO: Rewrite to run both steppers at the same time, with their speeds modulated to make slope.
           This will probably require acceleration for ramping up and down */

  if(delta_l.l == 0 && delta_l.r == 0) return;

  const bool DEBUG_MOTORS = true;

  leftMotor.setSpeedInStepsPerSecond((int)(1.0 * max_speed * abs(delta_l.l) / max(abs(delta_l.l), abs(delta_l.r))));
  rightMotor.setSpeedInStepsPerSecond((int)(1.0 * max_speed * abs(delta_l.r) / max(abs(delta_l.l), abs(delta_l.r))));




  if (DEBUG_MOTORS) {
    Serial.print("motors: ");
    Serial.print(delta_l.l);
    Serial.print(" ");
    Serial.print(delta_l.r);

    Serial.print(" speed: ");
    Serial.print((1.0 * abs(delta_l.l)) / max(abs(delta_l.l), abs(delta_l.r)));
    Serial.print(" ");
    Serial.print((int)(1.0 * max_speed * abs(delta_l.l) / max(abs(delta_l.l), abs(delta_l.r))));
    Serial.print(" ");
    Serial.println((int)(1.0 * max_speed * abs(delta_l.r) / max(abs(delta_l.l), abs(delta_l.r))));
  }

  leftMotor.setupRelativeMoveInSteps(delta_l.l);
  rightMotor.setupRelativeMoveInSteps(delta_l.r);

  while((!leftMotor.motionComplete()) || (!rightMotor.motionComplete())){
     leftMotor.processMovement();
     rightMotor.processMovement();
   }

  cur_len.l = cur_len.l + delta_l.l;
  cur_len.r = cur_len.r + delta_l.r;

  if (DEBUG_MOTORS) {
    Serial.print("motors done ");

    Serial.print(cur_len.l);
    Serial.print(" ");
    Serial.println(cur_len.r);
  }
}


void set_lengths(LR_Step desired_lr){
  /* Moves the motors to set the string to the desired lengths in steps */

  LR_Step delta_lr = {
    desired_lr.l - cur_len.l,
    desired_lr.r - cur_len.r
  };

  run_motors(delta_lr);

}


void set_position(XY_Pos xy){
  /* Moves the marker to the position x,y
     x and y are in mm from top left corner */

  LR_Step new_lr = {
    (int)(1.0 * sqrt( (xy.x*xy.x) + (xy.y*xy.y) ) * steps_per_mm),
    (int)(1.0 * sqrt( ((interspool_spacing - xy.x)*(interspool_spacing - xy.x)) + (xy.y*xy.y) ) * steps_per_mm)
  };

  set_lengths(new_lr);
}

int c[4];
int readInteger(){
  for(int i =0; i<4; i++) {
    c[i] = Serial.read();  //gets one byte from serial buffer
    delay(10);
  }
}

XY_Pos getPoint(){
  newPoint.x = readInteger();
  newPoint.y = readInteger();
  Serial.flush();
  return newPoint;
}

void getPath(){
  for (int i=0; i<pathLength; i++){
    loopPath[i] = getPoint();
    Serial.print(String(loopPath[i].x) + " " + String(loopPath[i].y));
    delay(400);
  }
}

void setup() {
  Serial.begin(9600);

  // To enable the motor shield, write LOW to pin 8
  pinMode(8, OUTPUT);
  motor_state = LOW;
  digitalWrite(8, motor_state);

  leftMotor.connectToPins(2,5); // X on shield
  rightMotor.connectToPins(3,6); // Y on shield

  leftMotor.setSpeedInStepsPerSecond(max_speed);
  rightMotor.setSpeedInStepsPerSecond(max_speed);

  leftMotor.setAccelerationInStepsPerSecondPerSecond(max_accel);
  rightMotor.setAccelerationInStepsPerSecondPerSecond(max_accel);


  tool_servo.attach(servo_pin);
  tool_servo.write(48);

  Serial.println("done!");



  XY_Pos next_xy;

  // Run through the hard coded path
  for(int i = 0; i < num_path; i++){
    // Lots of messy stuff here.
    if (path[i][0] == -10){
      tool_servo.write(servo_marker);
      Serial.println("Marker Down");
      erasing = false;
      delay(500);
    } else if (path[i][0] == -20){
      tool_servo.write(servo_off);
      Serial.println("Marker Up");
      erasing = false;
      delay(500);
    } else if (path[i][0] == -30){
      tool_servo.write(servo_erase);
      Serial.println("Marker Erase");
      erasing = true;
      delay(500);
    } else {
      next_xy.x = path[i][0];
      next_xy.y = path[i][1];


      if (erasing){
        next_xy.y += eraser_offset;
      }

      set_position(next_xy);
    }
    // Let the motors rest
    delay(100);
  }


  state= recieving;
  last_state=(STATE_HANDLER_T)NULL;
}

void recieving(void) {
  // If we are entering the state, init
  if (state != last_state) {
    last_state = state;
  }

  // Perform state tasks
    //recieve a point

  // Check for state transitions
    // if the data was recieved and the point is a valid point, draw

  // If we are leaving the state, clean up
  if (state != last_state) {

  }
}

void drawing(void) {
  // If we are entering the state, init
  if (state != last_state) {
    last_state = state;
  }
  // Perform state tasks
    //draw the next point

  // Check for state transitions
  if (motor_state == LOW) {
    state = recieving;
  }

  // If we are leaving the state, clean up
  if (state != last_state) {

  }

}

void loop() {

  state();
  // Serial.flush();
  // // Serial.println("Loop");
  //
  if (Serial.available()) {
    delay(100);
    pathLength = readInteger();
    // delay(50);
    Serial.print(pathLength);
    // Serial.flush();
    // XY_Pos loopPath[pathLength];
    getPath();

    for(int i = 0; i < pathLength; i++){


        set_position(loopPath[i]);

    }

  // Serial.flush();
  }
}
