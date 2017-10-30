#include "Arduino.h"
#include <Adafruit_MotorShield.h>
#include <Adafruit_MS_PWMServoDriver.h>


Adafruit_MotorShield AFMS = Adafruit_MotorShield(); // Create the motor shield object with the default I2C address
Adafruit_DCMotor *rightMotor = AFMS.getMotor(3); // Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *leftMotor = AFMS.getMotor(4);

// Maximum speed to run motors (1-255)
const int max_speed = 120;

// Magic number that converts distance to time to drive motors
const long int l_to_ms = 10000;

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
const float x_scale = 0.025;
const float y_scale = -0.025;


// current rope lengths - init at (0,0)
float cur_lengths[2] = { 1/sqrt(2), 1/sqrt(2) };

// Because adafruit motorshield lib is dumb and FORWARD and BACKWARD aren't 1 and -1
int speed_to_dir(float speed){
  if(speed > 0)  return FORWARD;
  if(speed < 0)  return BACKWARD;
  return RELEASE;
}

void run_motors(float dl_l, float dl_r){
  /* Given dl_l and dl_r, move the motors by that much
   * dl_l (delta length left) in units: <> */

  float max_l = max(abs(dl_l), abs(dl_r));
  if(max_l == 0) return;

  Serial.print("run_motors ");
  Serial.print(max_l);
  Serial.print(" ");
  Serial.print(max_speed * (dl_l/max_l));
  Serial.print(" ");
  Serial.print(max_speed * (dl_r/max_l));
  Serial.print(" ");
  Serial.println(abs(max_l * l_to_ms));

  leftMotor->setSpeed(abs(max_speed * (dl_l/max_l)));
  rightMotor->setSpeed(abs(max_speed * (dl_r/max_l)));

  leftMotor->run(speed_to_dir(dl_l));
  rightMotor->run(speed_to_dir(dl_r));


  long start_time = millis();
  while(millis() < start_time + abs(max_l * l_to_ms)){
    delay(10);
  }

  // setSpeed(0) means breaking, run(RELEASE) allows motors to coast
  leftMotor->setSpeed(0);
  rightMotor->setSpeed(0);
  // leftMotor->run(RELEASE);
  // rightMotor->run(RELEASE);
}


void set_lengths(float len_l, float len_r){
  /* Moves the motors to set the string to the desired lengths */
  Serial.println("setting lr" + String(len_l) + " " + String(len_r));

  float delta_l_l = cur_lengths[0] - len_l;
  float delta_l_r = cur_lengths[1] - len_r;

  run_motors(delta_l_l, delta_l_r);

  cur_lengths[0] = len_l;
  cur_lengths[1] = len_r;
}

void set_position(float x, float y){
  /* Moves the marker to the position x,y
     x and y are floats between (0,1)
     Origin is the top left corner */
  // Note: changing the area from (0,1) requires many constants (refer to mathematica notebook)
  Serial.println("setting xy" + String(x) + " " + String(y));

  float new_length_l = sqrt(x*x + y*y);
  float new_length_r = sqrt(1 - 2*x + x*x + y*y);

  set_lengths(new_length_l, new_length_r);
}


void setup() {
  Serial.begin(9600);

  AFMS.begin();

  Serial.println("begin.");

  for(int i = 0; i < num_path; i++){
    Serial.print(path[i][0]);
    Serial.print(' ');
    Serial.println(path[i][1]);
    // Lots of messy stuff here.
    // set_position has origin in the top left, the 0.5's are there to move the origin to the center
    set_position((path[i][0] * x_scale) + 0.5, 0.5 - (path[i][1] * y_scale));
    // Let the motors stop
    delay(100);
  }

  leftMotor->run(RELEASE);
  rightMotor->run(RELEASE);
  Serial.println("done!");
}


void loop() {
  delay(10);
}
