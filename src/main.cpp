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
const int max_speed = 500;
const int max_accel = max_speed * 3;

const int servo_pin = 10; // Only 9 & 10 are supported

#define DEBUG false
/***** End Config *****/


// Conversion from mm to stepper motor steps
const float steps_per_mm = 5; /* 200.0 steps per rotation / 40.0 Circumference in mm */;

LR_Step xy_to_lr(XY_Pos xy){
  LR_Step lr;

  lr.l = (int)(1.0 * sqrt((xy.x*xy.x) + (xy.y*xy.y)) * steps_per_mm);
  lr.r = (int)(1.0 * sqrt(((interspool_spacing - xy.x)*(interspool_spacing - xy.x)) + (xy.y*xy.y)) * steps_per_mm);
  return lr;
}


// Calculate init cable lengths
LR_Step cur_len = xy_to_lr(init_pos);


/* Test Path */
// Units are in mm
// -10 for marker engage
// -20 for marker disengage

const int num_path = 302;
const int path[][2] =   {
  //PYTHONSTARTFLAG
{-20,0},
{619,624},
{-10,0},
{619,661},
{619,661},
{631,661},
{631,661},
{632,661},
{633,661},
{635,661},
{636,660},
{637,660},
{638,659},
{639,659},
{639,658},
{640,657},
{641,656},
{642,655},
{642,654},
{643,653},
{643,652},
{643,650},
{644,649},
{644,648},
{644,646},
{644,644},
{644,643},
{644,641},
{644,639},
{644,638},
{644,636},
{643,635},
{643,634},
{643,632},
{642,631},
{642,630},
{641,629},
{640,628},
{639,627},
{639,627},
{638,626},
{637,625},
{636,625},
{635,625},
{633,624},
{632,624},
{631,624},
{619,624},
{619,624},
{619,624},
{-20,0},
{610,613},
{-10,0},
{631,613},
{631,613},
{633,613},
{635,614},
{637,614},
{639,615},
{641,615},
{643,616},
{644,617},
{646,618},
{647,619},
{648,621},
{649,622},
{650,624},
{651,626},
{652,628},
{653,630},
{653,632},
{653,635},
{654,637},
{654,640},
{654,643},
{654,645},
{654,648},
{653,651},
{653,653},
{653,655},
{652,657},
{651,659},
{650,661},
{649,663},
{648,665},
{647,666},
{646,667},
{644,668},
{643,669},
{641,670},
{639,671},
{637,671},
{635,672},
{633,672},
{631,672},
{619,672},
{619,672},
{619,711},
{619,711},
{610,711},
{610,711},
{610,613},
{610,613},
{610,613},
{-20,0},
{682,646},
{-10,0},
{681,646},
{680,647},
{679,647},
{677,648},
{676,648},
{675,649},
{674,650},
{673,651},
{673,652},
{672,654},
{671,655},
{670,657},
{670,659},
{669,661},
{669,663},
{668,665},
{668,667},
{668,670},
{668,672},
{668,675},
{668,677},
{668,680},
{668,682},
{668,684},
{669,686},
{669,688},
{670,690},
{670,692},
{671,694},
{672,695},
{673,697},
{673,698},
{674,699},
{675,700},
{676,701},
{677,702},
{679,702},
{680,703},
{681,703},
{682,703},
{684,703},
{685,703},
{686,702},
{687,702},
{688,701},
{689,700},
{690,699},
{691,698},
{692,697},
{693,695},
{694,694},
{694,692},
{695,690},
{695,688},
{696,686},
{696,684},
{696,682},
{697,680},
{697,677},
{697,675},
{697,672},
{697,670},
{696,667},
{696,665},
{696,663},
{695,661},
{695,659},
{694,657},
{694,656},
{693,654},
{692,653},
{691,651},
{690,650},
{689,649},
{688,648},
{687,648},
{686,647},
{685,647},
{684,646},
{682,646},
{682,646},
{-20,0},
{682,636},
{-10,0},
{684,636},
{687,636},
{688,637},
{690,638},
{692,639},
{694,640},
{695,641},
{697,643},
{698,644},
{699,646},
{701,648},
{702,651},
{703,653},
{703,656},
{704,659},
{705,661},
{705,664},
{705,668},
{706,671},
{706,675},
{706,678},
{705,682},
{705,685},
{705,688},
{704,691},
{703,693},
{703,696},
{702,699},
{701,701},
{699,703},
{698,705},
{697,707},
{695,708},
{694,710},
{692,711},
{690,712},
{688,712},
{687,713},
{684,713},
{682,713},
{680,713},
{678,713},
{676,712},
{674,712},
{673,711},
{671,710},
{669,708},
{668,707},
{667,705},
{665,703},
{664,701},
{663,699},
{662,696},
{661,693},
{661,691},
{660,688},
{660,685},
{659,682},
{659,678},
{659,675},
{659,671},
{659,668},
{660,664},
{660,661},
{661,659},
{661,656},
{662,653},
{663,651},
{664,648},
{665,646},
{667,644},
{668,643},
{669,641},
{671,640},
{673,639},
{674,638},
{676,637},
{678,636},
{680,636},
{682,636},
{682,636},
{-20,0},
{720,613},
{-10,0},
{763,613},
{763,613},
{763,625},
{763,625},
{729,625},
{729,625},
{729,654},
{729,654},
{761,654},
{761,654},
{761,665},
{761,665},
{729,665},
{729,665},
{729,700},
{729,700},
{763,700},
{763,700},
{763,711},
{763,711},
{720,711},
{720,711},
{720,613},
{720,613},
{720,613},
//PYTHONENDFLAG
{-20, 0},
{init_pos.x, init_pos.y}
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

  const bool DEBUG_MOTORS = false;

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

  if (DEBUG) {
    Serial.print("len: ");
    Serial.print(String(desired_lr.l));
    Serial.print(" ");
    Serial.print(String(desired_lr.r));
    Serial.println(" ");
    Serial.println();
  }

  // while(true){}

  LR_Step delta_lr = {
    desired_lr.l - cur_len.l,
    desired_lr.r - cur_len.r
  };

  run_motors(delta_lr);

  if (DEBUG) {
    Serial.print("cur len: ");
    Serial.print(cur_len.l);
    Serial.print(" ");
    Serial.print(cur_len.r);
    Serial.println(" ");
  }
}


void set_position(XY_Pos xy){
  /* Moves the marker to the position x,y
     x and y are in mm from top left corner */
  if (DEBUG) {
    Serial.println("xy: " + String(xy.x) + " " + String(xy.y) + " ");
  }

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

  leftMotor.setSpeedInStepsPerSecond(max_speed);
  rightMotor.setSpeedInStepsPerSecond(max_speed);

  leftMotor.setAccelerationInStepsPerSecondPerSecond(max_accel);
  rightMotor.setAccelerationInStepsPerSecondPerSecond(max_accel);


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
      if (DEBUG) {
        Serial.println("Marker Down");
      }
      erasing = false;
      delay(500);
    } else if (path[i][0] == -20){
      tool_servo.write(servo_off);
      if (DEBUG) {
        Serial.println("Marker Up");
      }
      erasing = false;
      delay(500);
    } else if (path[i][0] == -30){
      tool_servo.write(servo_erase);
      if (DEBUG) {
        Serial.println("Marker Erase");
      }
      erasing = true;
      delay(500);
    } else {
      next_xy.x = path[i][0];
      next_xy.y = path[i][1] * -1;


      if (erasing){
        next_xy.y += eraser_offset;
      }
      if (DEBUG) {
        Serial.print(next_xy.y);
        Serial.print(' ');
        Serial.println(next_xy.x);
      }

      set_position(next_xy);
    }
    // Let the motors rest
    // delay(100);
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
