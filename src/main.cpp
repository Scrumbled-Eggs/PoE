#include "Arduino.h"
#include <Servo.h>
#include <Adafruit_MotorShield.h>
#include <Adafruit_MS_PWMServoDriver.h>


Adafruit_MotorShield AFMS = Adafruit_MotorShield(); // Create the motor shield object with the default I2C address

Adafruit_StepperMotor *leftMotor = AFMS.getStepper(200, 2);
Adafruit_StepperMotor *rightMotor = AFMS.getStepper(200, 1);

Servo tool_servo;


struct XY_Pos {
  long x;
  long y;
};

struct LR_Step {
  int l;
  int r;
};

/******* Config *******/
int interspool_spacing = 1000; // In mm
const XY_Pos init_pos = { 500, 500 }; // In mm

const int servo_marker = 90;
const int servo_off = 50;

// Maximum speed to run motors
const int max_speed = 50;

const int servo_pin = 9; // Only 9 & 10 are supported
/***** End Config *****/


// Conversion from mm to stepper motor steps
const float steps_per_mm = 1.38889; /* 200.0 steps per rotation / 144.0 Circumference in mm */;

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
  {500,500}, // Init @ origin
  {-10,0},
  {600,500},
  {600,600},
  {400,600},
  {400,400},
  {500,400},
  {500,500}, // End at origin
  {-20,0}
};


// Because adafruit motorshield lib is dumb and FORWARD and BACKWARD aren't 1 and -1
int l_to_dir(float speed) {
  if(speed > 0)  return BACKWARD;
  if(speed < 0)  return FORWARD;
  return RELEASE;
}

int sign(float x) {
  if (x>0) return 1;
  if (x<0) return -1;
  return 0;
}

void run_motors(LR_Step delta_l){
  /* Move lr steps*/

  /* TODO: Rewrite to run both steppers at the same time, with their speeds modulated to make slope.
           This will probably require acceleration for ramping up and down */

  const bool DEBUG_MOTORS = false;

  if (DEBUG_MOTORS) {
    Serial.print("motors: ");
    Serial.print(delta_l.l);
    Serial.print(" ");
    Serial.println(delta_l.r);
  }
  if(delta_l.l == 0 && delta_l.r == 0) return;

  // Init variables of remaining steps
  LR_Step remain_steps;
  remain_steps.l = abs(delta_l.l);
  remain_steps.r = abs(delta_l.r);

  int l_dir = sign(delta_l.l);
  int r_dir = sign(delta_l.r);


  while ((remain_steps.l > 0) || (remain_steps.r > 0)) {
    // Sets the smaller step to 1 and the larger step to ratio between r_steps and l_steps
    // Serial.print(l_steps);
    // Serial.print(" ");
    // Serial.print(r_steps);
    // Serial.print(" ");

    if (remain_steps.l > remain_steps.r){
      leftMotor->step(1, l_to_dir(delta_l.l), DOUBLE);
      remain_steps.l -= 1;

      cur_len.l += 1 * l_dir;
        if (DEBUG_MOTORS) Serial.print(" 1 0 ");
    }
    else {
      rightMotor->step(1, l_to_dir(delta_l.r), DOUBLE);
      remain_steps.r -= 1;

      cur_len.r += 1 * r_dir;
        if (DEBUG_MOTORS) Serial.print(" 0 1 ");
    }

    if (DEBUG_MOTORS) {
      Serial.print(cur_len.l);
      Serial.print(" ");
      Serial.println(cur_len.r);
    }
  }
}


void set_lengths(LR_Step desired_lr){
  /* Moves the motors to set the string to the desired lengths in steps */

  Serial.print("len: ");
  Serial.print(desired_lr.l);
  Serial.print(" ");
  Serial.print(desired_lr.r);
  Serial.print(" ");

  LR_Step delta_lr = {
    desired_lr.l - cur_len.l,
    desired_lr.r - cur_len.r
  };

  run_motors(delta_lr);

  Serial.print("cur len: ");
  Serial.print(cur_len.l);
  Serial.print(" ");
  Serial.print(cur_len.r);
  Serial.println(" ");
}


void set_position(XY_Pos xy){
  /* Moves the marker to the position x,y
     x and y are in mm from top left corner */

  Serial.print("xy: " + String(xy.x) + " " + String(xy.y) + " ");

  LR_Step new_lr = {
    (int)(1.0 * sqrt( (xy.x*xy.x) + (xy.y*xy.y) ) * steps_per_mm),
    (int)(1.0 * sqrt( ((interspool_spacing - xy.x)*(interspool_spacing - xy.x)) + (xy.y*xy.y) ) * steps_per_mm)
  };

  set_lengths(new_lr);
}


void setup() {
  Serial.begin(9600);

  AFMS.begin();
  leftMotor->setSpeed(max_speed);
  rightMotor->setSpeed(max_speed);

  rightMotor->step(1, FORWARD, DOUBLE);
  leftMotor->step(1, FORWARD, DOUBLE);

  tool_servo.attach(servo_pin);
  tool_servo.write(48);


  Serial.println("begin.");

  Serial.print("Init lengths:");
  Serial.print(cur_len.l);
  Serial.print(" ");
  Serial.println(cur_len.r);

  XY_Pos next_xy;

  // Run through the hard coded path
  for(int i = 0; i < num_path; i++){
    // Lots of messy stuff here.
    if (path[i][0] == -10){
      tool_servo.write(servo_marker);
      Serial.println("Marker Down");
      delay(500);
    } else if (path[i][0] == -20){
      tool_servo.write(servo_off);
      Serial.println("Marker Up");
      delay(500);
    } else {
      next_xy.x = path[i][0];
      next_xy.y = path[i][1];

      set_position(next_xy);
    }
    // Let the motors rest
    delay(500);
  }

  Serial.println("done!");
}


void loop() {
  // Spin the wheels
  delay(10);
}
