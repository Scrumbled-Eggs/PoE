#include "Arduino.h"
#include <Servo.h>
#include <Adafruit_MotorShield.h>
#include <Adafruit_MS_PWMServoDriver.h>


Adafruit_MotorShield AFMS = Adafruit_MotorShield(); // Create the motor shield object with the default I2C address

Adafruit_StepperMotor *rightMotor = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *leftMotor = AFMS.getStepper(200, 2);

Servo tool_servo;
const int servo_pin = 9; // Only 9 & 10 are supported


// Maximum speed to run motors
const int max_speed = 12;

// Width between spools in mm
const int interspool_spacing = 1000;

// Conversion from mm to stepper motor steps
const float steps_per_mm = 200 /* steps per rotation */ / 144.0 /* Circumference in mm */;

// Artboard size
const int max_dim = 100;
const int artboard_to_mm = interspool_spacing / max_dim;

// Old test path code, please remove
// Units are in width/artboard_dims mm
const float x_scale = 2;
const float y_scale = -2;

const int num_path = 29;
const float path[][2] =   {
  {0,0}, // Init @ origin
  {-10,0},
  {2,0}, // P
  {2,2},
  {0,2},
  {-20,0},
  {0,0},
  {-10,0},
  {0,4}, // end P
  {-20,0},
  {3,4}, // O
  {-10,0},
  {3,0},
  {6,0},
  {6,4},
  {3,4}, // end O
  {-20,0},
  {7,4}, // E
  {-10,0},
  {7,0},
  {9,0},
  {7,0},
  {7,2},
  {9,2},
  {7,2},
  {7,4},
  {9,4}, // end E
  {-20,0},
  {0,0}
};



// current cable lengths
const int init_rope_length = (interspool_spacing/sqrt(2)) * steps_per_mm;
int cur_lengths[2] = { init_rope_length, init_rope_length };

// Because adafruit motorshield lib is dumb and FORWARD and BACKWARD aren't 1 and -1
int l_to_dir(float speed) {
  if(speed > 0)  return FORWARD;
  if(speed < 0)  return BACKWARD;
  return RELEASE;
}

// TODO: Make this set the path
// int incomingByte = 0;
// int* getPath() {
//   while (Serial.available() > 0) {
//     incomingByte = Serial.read();
//
//     // say what you got:
//     Serial.print("I received: ");
//     Serial.println(incomingByte, DEC);
//   }
// }


void run_motors(int dl_l, int dl_r){
  /* Given dl_l and dl_r, move the motors by that much
     dl_l (delta length left) in units step */

  /* TODO: Rewrite to run both steppers at the same time, with their speeds modulated to make slope.
           This will probably require acceleration for ramping up and down */
  Serial.print("motors: ");
  Serial.print(dl_l);
  Serial.print(" ");
  Serial.println(dl_r);

  if(dl_l == 0 && dl_r == 0) return;

  // Init variables of remaining steps
  int l_steps = abs(dl_l);
  int r_steps = abs(dl_r);

  // init variables dl, dr which hold how many steps to take on each loop
  int dl, dr;

  while ((l_steps > 0) || (r_steps > 0)) {
    // Sets the smaller step to 1 and the larger step to ratio between r_steps and l_steps
    // Serial.print(l_steps);
    // Serial.print(" ");
    // Serial.print(r_steps);
    // Serial.print(" ");

    if (l_steps > r_steps){
      dl = l_steps / r_steps;
      dr = min(r_steps, 1);
    }
    else {
      dl = min(l_steps, 1);
      dr = r_steps / l_steps;
    }

    // Serial.print(dl);
    // Serial.print(" ");
    // Serial.print(dr);
    // Serial.print(" ");

    rightMotor->step(dl, l_to_dir(dl_r), DOUBLE);
    leftMotor->step(dr, l_to_dir(dl_l), DOUBLE);

    l_steps -= dl;
    r_steps -= dr;
    // Serial.print(l_steps);
    // Serial.print(" ");
    // Serial.println(r_steps);
  }
}


void set_lengths(int len_l, int len_r){
  /* Moves the motors to set the string to the desired lengths in steps */

  int delta_l_l = cur_lengths[0] - len_l;
  int delta_l_r = cur_lengths[1] - len_r;

  run_motors(delta_l_l, delta_l_r);
  // run_motors(40, 40);

  cur_lengths[0] = len_l;
  cur_lengths[1] = len_r;
}


void set_position(float x, float y){
  /* Moves the marker to the position x,y
     x and y are floats between (0,1)
     Origin is the top left corner */

  // Serial.println("setting xy" + String(x) + " " + String(y));

  float new_length_l = sqrt(x*x + y*y) * artboard_to_mm * steps_per_mm;
  float new_length_r = sqrt((max_dim - x)*(max_dim - x) + y*y) * artboard_to_mm * steps_per_mm;

  set_lengths(new_length_l, new_length_r);
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
  Serial.println(init_rope_length);

  // Run through the hard coded path
  for(int i = 0; i < num_path; i++){
    // Lots of messy stuff here.
    // The 50's are there to move the origin of the path to the center of the board
    if (path[i][0] == -10){
      tool_servo.write(90);
      Serial.println("Marker Down");
      delay(500);
    } else if (path[i][0] == -20){
      tool_servo.write(48);
      Serial.println("Marker Up");
      delay(500);
    } else {
      Serial.print(path[i][0] * x_scale);
      Serial.print(' ');
      Serial.print(path[i][1] * y_scale);
      Serial.print(' ');
      set_position((path[i][0] * x_scale) + 50, 50 - (path[i][1] * y_scale));
    }
    // Let the motors rest
    delay(10);
  }
  //
  // leftMotor->release();
  // rightMotor->release();
  Serial.println("done!");
}


void loop() {
  // Spin the wheels
  delay(10);
}
