#include "Arduino.h"
#include <Adafruit_MotorShield.h>
#include <Adafruit_MS_PWMServoDriver.h>


Adafruit_MotorShield AFMS = Adafruit_MotorShield(); // Create the motor shield object with the default I2C address
// Adafruit_DCMotor *rightMotor = AFMS.getMotor(3); // Select which 'port' M1, M2, M3 or M4. In this case, M1
// Adafruit_DCMotor *leftMotor = AFMS.getMotor(4);
Adafruit_StepperMotor *rightMotor = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *leftMotor = AFMS.getStepper(200, 2);

// Maximum speed to run motors (1-255)
const int max_speed = 12;

// Spacing between spools in mm
int interspool_spacing = 900;

// Conversion from mm to stepper motor steps
// Circumference
const float mm_to_steps = 144.0 /* Circumference in mm */ / 200 /* steps / rotation */;


// Number of points in the path (should be done programatically)
const int num_path = 25;
const float path[][2] =   {
  {0,0}, // Init @ origin
  {2,0}, // P
  {2,2},
  {0,2},
  {0,0},
  {0,4}, // end P
  {3,4}, // O
  {3,1.25},
  {4.25, 0},
  {5.25, 0},
  {6,1.25},
  {6,2.75},
  {5.25,4},
  {4.25,4},
  {3,2.75},
  {3,4}, // end O
  {7,4}, // E
  {7,0},
  {9,0},
  {7,0},
  {7,2},
  {9,2},
  {7,2},
  {7,4},
  {9,4}, // end E
  // {0,0}
};

// What to scale the path coordinates by
// Values after scaling should be -.5 to .5
const float x_scale = 1;
const float y_scale = -1;


int incomingByte = 0;


// current rope lengths
const int init_rope_length = (interspool_spacing/sqrt(2)) * mm_to_steps;
int cur_lengths[2] = { init_rope_length, init_rope_length };

// Because adafruit motorshield lib is dumb and FORWARD and BACKWARD aren't 1 and -1
int l_to_dir(float speed) {
  if(speed > 0)  return FORWARD;
  if(speed < 0)  return BACKWARD;
  return RELEASE;
}


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
   * dl_l (delta length left) in units step */

  if(dl_l == 0 && dl_r == 0) return;

  Serial.print("run_motors ");

  // Init variables of remaining steps
  int l_steps = abs(dl_l);
  int r_steps = abs(dl_r);

  // init variables dl, dr which hold how many steps to take on each loop
  int dl, dr;

  while ((l_steps > 0) || (r_steps > 0)) {
    //
    if (l_steps > r_steps){
      dl = 1;
      dr = r_steps / l_steps;
    }
    else {
      dl = l_steps / r_steps;
      dr = 1;
    }

    rightMotor->step(dl, l_to_dir(dl_r), DOUBLE);
    leftMotor->step(dr, l_to_dir(dl_l), DOUBLE);

    l_steps -= dl;
    r_steps -= dr;
  }
}


void set_lengths(int len_l, int len_r){
  /* Moves the motors to set the string to the desired lengths */
  Serial.println("setting lr" + String(len_l) + " " + String(len_r));

  int delta_l_l = cur_lengths[0] - len_l;
  int delta_l_r = cur_lengths[1] - len_r;

  run_motors(delta_l_l, delta_l_r);

  cur_lengths[0] = len_l;
  cur_lengths[1] = len_r;
}


void set_position(float x, float y){
  /* Moves the marker to the position x,y
     x and y are floats between (0,1)
     Origin is the top left corner */
  Serial.println("setting xy" + String(x) + " " + String(y));

  // The length of the square
  const int max_dim = 100;
  const int artboard_to_mm = interspool_spacing / max_dim;

  Serial.println("artboard_to_mm" + String(artboard_to_mm) + " " +mm_to_steps + " " + String(artboard_to_mm * mm_to_steps));

  float new_length_l = sqrt(x*x + y*y) * artboard_to_mm * mm_to_steps;
  float new_length_r = sqrt((max_dim - x)*(max_dim - x) + y*y) * artboard_to_mm * mm_to_steps;

  set_lengths(new_length_l, new_length_r);
}


void setup() {
  Serial.begin(9600);

  AFMS.begin();

  Serial.println("begin.");

  Serial.println("init ls " + String(cur_lengths[0]));


  leftMotor->setSpeed(12);
  rightMotor->setSpeed(12);
  // getPath;
  for(int i = 0; i < num_path; i++){
    Serial.print(path[i][0]);
    Serial.print(' ');
    Serial.println(path[i][1]);
    // Lots of messy stuff here.
    // set_position has origin in the top left, the 0.5's are there to move the origin to the center
    set_position((path[i][0] * x_scale) + 50, 50 - (path[i][1] * y_scale));
    // Let the motors stop
    delay(100);
  }

  leftMotor->release();
  rightMotor->release();
  Serial.println("done!");
}


void loop() {
  delay(10);
}
