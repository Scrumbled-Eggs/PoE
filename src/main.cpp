#include "Arduino.h"
#include <Servo.h>
#include "SpeedyStepper.h"



SpeedyStepper leftMotor;
SpeedyStepper rightMotor;

Servo tool_servo;

bool erasing = false;

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
const XY_Pos init_pos = { 600, 600 }; // In mm

const int servo_marker = 90;
const int servo_off = 50;
const int servo_erase = 145;

const int eraser_offset = -18; //mm
// Maximum speed to run motors
const int max_speed = 50;

const int servo_pin = 10; // Only 9 & 10 are supported
/***** End Config *****/


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

const int num_path = 27;
const float path[][2] =   {
  //PYTHONSTARTFLAG 
{-10,0}
{6,8}
{-20,0}
{6,20}
{6,20}
{10,20}
{10,20}
{10,20}
{11,20}
{11,20}
{12,20}
{12,20}
{12,19}
{13,19}
{13,19}
{13,19}
{13,18}
{14,18}
{14,18}
{14,17}
{14,17}
{14,16}
{14,16}
{14,16}
{14,15}
{14,14}
{14,14}
{14,13}
{14,13}
{14,12}
{14,12}
{14,11}
{14,11}
{14,10}
{14,10}
{14,10}
{13,9}
{13,9}
{13,9}
{13,9}
{12,8}
{12,8}
{12,8}
{11,8}
{11,8}
{10,8}
{10,8}
{6,8}
{6,8}
{6,8}
{-10,0}
{3,4}
{-20,0}
{10,4}
{10,4}
{11,4}
{11,4}
{12,4}
{13,5}
{13,5}
{14,5}
{14,5}
{15,6}
{15,6}
{16,7}
{16,7}
{16,8}
{17,8}
{17,9}
{17,10}
{17,10}
{17,11}
{18,12}
{18,13}
{18,14}
{18,15}
{18,16}
{17,17}
{17,17}
{17,18}
{17,19}
{17,19}
{16,20}
{16,21}
{16,21}
{15,22}
{15,22}
{14,22}
{14,23}
{13,23}
{13,23}
{12,23}
{11,24}
{11,24}
{10,24}
{6,24}
{6,24}
{6,37}
{6,37}
{3,37}
{3,37}
{3,4}
{3,4}
{3,4}
{-10,0}
{27,15}
{-20,0}
{27,15}
{26,15}
{26,15}
{25,16}
{25,16}
{25,16}
{24,16}
{24,17}
{24,17}
{24,18}
{23,18}
{23,19}
{23,19}
{23,20}
{23,21}
{22,21}
{22,22}
{22,23}
{22,24}
{22,25}
{22,25}
{22,26}
{22,27}
{22,28}
{23,28}
{23,29}
{23,30}
{23,30}
{23,31}
{24,31}
{24,32}
{24,32}
{24,33}
{25,33}
{25,33}
{25,34}
{26,34}
{26,34}
{27,34}
{27,34}
{28,34}
{28,34}
{28,34}
{29,34}
{29,33}
{29,33}
{30,33}
{30,32}
{30,32}
{31,31}
{31,31}
{31,30}
{31,30}
{31,29}
{32,28}
{32,28}
{32,27}
{32,26}
{32,25}
{32,25}
{32,24}
{32,23}
{32,22}
{32,21}
{32,21}
{31,20}
{31,19}
{31,19}
{31,18}
{31,18}
{30,17}
{30,17}
{30,16}
{29,16}
{29,16}
{29,16}
{28,15}
{28,15}
{28,15}
{27,15}
{27,15}
{-10,0}
{27,12}
{-20,0}
{28,12}
{29,12}
{29,12}
{30,12}
{30,13}
{31,13}
{31,13}
{32,14}
{32,14}
{33,15}
{33,16}
{34,17}
{34,17}
{34,18}
{34,19}
{35,20}
{35,21}
{35,22}
{35,23}
{35,25}
{35,26}
{35,27}
{35,28}
{35,29}
{34,30}
{34,31}
{34,32}
{34,33}
{33,33}
{33,34}
{32,35}
{32,35}
{31,36}
{31,36}
{30,37}
{30,37}
{29,37}
{29,37}
{28,37}
{27,37}
{26,37}
{26,37}
{25,37}
{24,37}
{24,37}
{23,36}
{23,36}
{22,35}
{22,35}
{21,34}
{21,33}
{21,33}
{20,32}
{20,31}
{20,30}
{20,29}
{20,28}
{19,27}
{19,26}
{19,25}
{19,23}
{19,22}
{20,21}
{20,20}
{20,19}
{20,18}
{20,17}
{21,17}
{21,16}
{21,15}
{22,14}
{22,14}
{23,13}
{23,13}
{24,13}
{24,12}
{25,12}
{26,12}
{26,12}
{27,12}
{27,12}
{-10,0}
{40,4}
{-20,0}
{54,4}
{54,4}
{54,8}
{54,8}
{43,8}
{43,8}
{43,18}
{43,18}
{53,18}
{53,18}
{53,21}
{53,21}
{43,21}
{43,21}
{43,33}
{43,33}
{54,33}
{54,33}
{54,37}
{54,37}
{40,37}
{40,37}
{40,4}
{40,4}
{40,4}
//PYTHONENDFLAG
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

  const bool DEBUG_MOTORS = false;

  if (DEBUG_MOTORS) {
    Serial.print("motors: ");
    Serial.print(delta_l.l);
    Serial.print(" ");
    Serial.println(delta_l.r);
  }
  if(delta_l.l == 0 && delta_l.r == 0) return;

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

  // To enable the motor shield, write LOW to pin 8
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);

  leftMotor.connectToPins(2,5); // X on shield
  rightMotor.connectToPins(3,6); // Y on shield

  leftMotor.setSpeedInStepsPerSecond(500);
  rightMotor.setSpeedInStepsPerSecond(500);

  leftMotor.setAccelerationInStepsPerSecondPerSecond(700);
  rightMotor.setAccelerationInStepsPerSecondPerSecond(700);


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

  // To enable the motor shield, write LOW to pin 8
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);

  Serial.println("done!");
}


void loop() {
  // Spin the wheels
  delay(10);
}
