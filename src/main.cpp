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
{23,49}
{23,61}
{23,61}
{27,61}
{27,61}
{28,61}
{28,61}
{28,61}
{29,61}
{29,61}
{29,60}
{30,60}
{30,60}
{30,60}
{30,59}
{31,59}
{31,59}
{31,58}
{31,58}
{31,57}
{31,57}
{31,57}
{32,56}
{32,55}
{32,55}
{32,54}
{32,54}
{31,53}
{31,53}
{31,52}
{31,52}
{31,51}
{31,51}
{31,51}
{30,50}
{30,50}
{30,50}
{30,50}
{29,49}
{29,49}
{29,49}
{28,49}
{28,49}
{28,49}
{27,49}
{23,49}
{23,49}
{23,49}
{20,45}
{27,45}
{27,45}
{28,45}
{29,45}
{29,45}
{30,46}
{30,46}
{31,46}
{31,46}
{32,47}
{32,47}
{33,48}
{33,48}
{34,49}
{34,49}
{34,50}
{34,51}
{34,51}
{35,52}
{35,53}
{35,54}
{35,55}
{35,56}
{35,57}
{35,58}
{34,58}
{34,59}
{34,60}
{34,60}
{34,61}
{33,62}
{33,62}
{32,63}
{32,63}
{31,63}
{31,64}
{30,64}
{30,64}
{29,64}
{29,65}
{28,65}
{27,65}
{23,65}
{23,65}
{23,78}
{23,78}
{20,78}
{20,78}
{20,45}
{20,45}
{20,45}
{44,56}
{44,56}
{43,56}
{43,56}
{43,57}
{42,57}
{42,57}
{42,57}
{41,58}
{41,58}
{41,59}
{40,59}
{40,60}
{40,60}
{40,61}
{40,62}
{40,62}
{40,63}
{39,64}
{39,65}
{39,66}
{39,66}
{39,67}
{40,68}
{40,69}
{40,69}
{40,70}
{40,71}
{40,71}
{40,72}
{41,72}
{41,73}
{41,73}
{42,74}
{42,74}
{42,74}
{43,75}
{43,75}
{43,75}
{44,75}
{44,75}
{45,75}
{45,75}
{45,75}
{46,75}
{46,74}
{47,74}
{47,74}
{47,73}
{47,73}
{48,72}
{48,72}
{48,71}
{48,71}
{49,70}
{49,69}
{49,69}
{49,68}
{49,67}
{49,66}
{49,66}
{49,65}
{49,64}
{49,63}
{49,62}
{49,62}
{49,61}
{48,60}
{48,60}
{48,59}
{48,59}
{47,58}
{47,58}
{47,57}
{47,57}
{46,57}
{46,57}
{45,56}
{45,56}
{45,56}
{44,56}
{44,56}
{44,53}
{45,53}
{46,53}
{46,53}
{47,53}
{47,54}
{48,54}
{49,54}
{49,55}
{49,55}
{50,56}
{50,57}
{51,58}
{51,58}
{51,59}
{51,60}
{52,61}
{52,62}
{52,63}
{52,64}
{52,66}
{52,67}
{52,68}
{52,69}
{52,70}
{51,71}
{51,72}
{51,73}
{51,74}
{50,74}
{50,75}
{49,76}
{49,76}
{49,77}
{48,77}
{47,78}
{47,78}
{46,78}
{46,78}
{45,78}
{44,78}
{44,78}
{43,78}
{42,78}
{42,78}
{41,78}
{40,77}
{40,77}
{39,76}
{39,76}
{39,75}
{38,74}
{38,74}
{37,73}
{37,72}
{37,71}
{37,70}
{37,69}
{37,68}
{36,67}
{36,66}
{36,64}
{37,63}
{37,62}
{37,61}
{37,60}
{37,59}
{37,58}
{38,58}
{38,57}
{39,56}
{39,55}
{39,55}
{40,54}
{40,54}
{41,54}
{42,53}
{42,53}
{43,53}
{44,53}
{44,53}
{44,53}
{57,45}
{71,45}
{71,45}
{71,49}
{71,49}
{60,49}
{60,49}
{60,59}
{60,59}
{70,59}
{70,59}
{70,62}
{70,62}
{60,62}
{60,62}
{60,74}
{60,74}
{71,74}
{71,74}
{71,78}
{71,78}
{57,78}
{57,78}
{57,45}
{57,45}
{57,45}
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
