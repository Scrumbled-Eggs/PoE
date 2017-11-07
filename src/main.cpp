#include "Arduino.h"
#include <Adafruit_MotorShield.h>
#include <Adafruit_MS_PWMServoDriver.h>


Adafruit_MotorShield AFMS = Adafruit_MotorShield(); // Create the motor shield object with the default I2C address
// Adafruit_DCMotor *rightMotor = AFMS.getMotor(3); // Select which 'port' M1, M2, M3 or M4. In this case, M1
// Adafruit_DCMotor *leftMotor = AFMS.getMotor(4);
Adafruit_StepperMotor *rightMotor = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *leftMotor = AFMS.getStepper(200, 2);

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

int incomingByte = 0;
int ratio = 0;

void contract(int lSteps, int rSteps){
  while((lSteps > 0) || (rSteps > 0)) {
    if(rSteps>lSteps) {
      ratio = rSteps/lSteps;
      for(int i = 0; i <ratio; i++) {
        rightMotor->step(1, FORWARD, DOUBLE); //should just run the # of steps
        rSteps--;
      }
      leftMotor->step(1, FORWARD, DOUBLE);
      lSteps--;
    }
    // if(lSteps>rSteps) {
    else {
      ratio = lSteps/rSteps;
      for(int i = 0; i <ratio; i++) {
        leftMotor->step(1, FORWARD, DOUBLE); //should just run the # of steps
        lSteps--;
      }
      rightMotor->step(1, FORWARD, DOUBLE);
      rSteps--;
    }
  }
}

void release(int lSteps, int rSteps){
  while((lSteps > 0) || (rSteps > 0)) {
    if(rSteps>lSteps) {
      ratio = rSteps/lSteps;
      for(int i = 0; i <ratio; i++) {
        rightMotor->step(1, BACKWARD, DOUBLE); //should just run the # of steps
        rSteps--;
      }
      leftMotor->step(1, BACKWARD, DOUBLE);
      lSteps--;
    }
    // if(lSteps>rSteps) {
    else {
      ratio = lSteps/rSteps;
      for(int i = 0; i <ratio; i++) {
        leftMotor->step(1, BACKWARD, DOUBLE); //should just run the # of steps
        lSteps--;
      }
      rightMotor->step(1, BACKWARD, DOUBLE);
      rSteps--;
    }
  }
}

void right(int lSteps, int rSteps){
  while((lSteps > 0) || (rSteps > 0)) {
    if(rSteps>lSteps) {
      ratio = rSteps/lSteps;
      for(int i = 0; i <ratio; i++) {
        rightMotor->step(1, FORWARD, DOUBLE); //should just run the # of steps
        rSteps--;
      }
      leftMotor->step(1, BACKWARD, DOUBLE);
      lSteps--;
    }
    // if(lSteps>rSteps) {
    else {
      ratio = lSteps/rSteps;
      for(int i = 0; i <ratio; i++) {
        leftMotor->step(1, BACKWARD, DOUBLE); //should just run the # of steps
        lSteps--;
      }
      rightMotor->step(1, FORWARD, DOUBLE);
      rSteps--;
    }
  }
}

void left(int lSteps, int rSteps){
  while((lSteps > 0) || (rSteps > 0)) {
    if(rSteps>lSteps) {
      ratio = rSteps/lSteps;
      for(int i = 0; i <ratio; i++) {
        rightMotor->step(1, BACKWARD, DOUBLE); //should just run the # of steps
        rSteps--;
      }
      leftMotor->step(1, FORWARD, DOUBLE);
      lSteps--;
    }
    // if(lSteps>rSteps) {
    else {
      ratio = lSteps/rSteps;
      for(int i = 0; i <ratio; i++) {
        leftMotor->step(1, FORWARD, DOUBLE); //should just run the # of steps
        lSteps--;
      }
      rightMotor->step(1, BACKWARD, DOUBLE);
      rSteps--;
    }
  }
}

// current rope lengths - init at (0,0)
float cur_lengths[2] = { 1/sqrt(2), 1/sqrt(2) };

// Because adafruit motorshield lib is dumb and FORWARD and BACKWARD aren't 1 and -1
int speed_to_dir(float speed){
  if(speed > 0)  return FORWARD;
  if(speed < 0)  return BACKWARD;
  return RELEASE;
}

int* getPath() {
  while (Serial.available() > 0) {
    incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
  }
}

void run_motors(float dl_l, float dl_r){
  /* Given dl_l and dl_r, move the motors by that much
   * dl_l (delta length left) in units: <> */

  float max_l = max(abs(dl_l), abs(dl_r));
  if(max_l == 0) return;

  Serial.print("run_motors ");
  // Serial.print(max_l);
  // Serial.print(" ");
  // Serial.print(max_speed * (dl_l/max_l));
  // Serial.print(" ");
  // Serial.print(max_speed * (dl_r/max_l));
  // Serial.print(" ");
  // Serial.println(abs(max_l * l_to_ms));

  // leftMotor->setSpeed(abs(max_speed * (dl_l/max_l)));
  // rightMotor->setSpeed(abs(max_speed * (dl_r/max_l)));
  int direction = 0;
  if (dl_r < 0) {
    direction++;
  }
  if (dl_l < 0) {
    direction++;
    direction++;
  }
  dl_l = abs(dl_l);
  dl_r = abs(dl_r);
  Serial.print(direction);
  Serial.print(" ");
  Serial.print(dl_l);
  Serial.print(" ");
  Serial.print(dl_r);

  switch(direction) {
    case 0: { contract(dl_l, dl_r);
            Serial.println(" contract");
            break; }
    case 1: { right(dl_l,dl_r);
            Serial.println(" right");
            break; }
    case 2: { left(dl_l,dl_r);
            Serial.println(" left");
            break; }
    case 3: { release(dl_l,dl_r);
            Serial.println(" release");
            break; }
  }

  // leftMotor->run(speed_to_dir(dl_l));
  // rightMotor->run(speed_to_dir(dl_r));


  // long start_time = millis();
  // while(millis() < start_time + abs(max_l * l_to_ms)){
  //   delay(10);
  // }

  // setSpeed(0) means breaking, run(RELEASE) allows motors to coast
  // leftMotor->setSpeed(0);
  // rightMotor->setSpeed(0);
  // leftMotor->run(RELEASE);
  // rightMotor->run(RELEASE);
}


void set_lengths(float len_l, float len_r){
  /* Moves the motors to set the string to the desired lengths */
  Serial.println("current lr" + String(cur_lengths[0]) + " " + String(cur_lengths[1]));
  Serial.println("setting lr" + String(len_l) + " " + String(len_r));

  float delta_l_l = cur_lengths[0] - len_l;
  float delta_l_r = cur_lengths[1] - len_r;
  // int delta_l_l = cur_lengths[0] - len_l;
  // int delta_l_r = cur_lengths[1] - len_r;

  run_motors(delta_l_l, delta_l_r);
  // run_motors(40, 40);

  cur_lengths[0] = len_l;
  cur_lengths[1] = len_r;
}

void set_position(float x, float y){
  /* Moves the marker to the position x,y
     x and y are floats between (0,1)
     Origin is the top left corner */
  // Note: changing the area from (0,1) requires many constants (refer to mathematica notebook)
  // Serial.println("setting xy" + String(x) + " " + String(y));

  float new_length_l = sqrt(x*x + y*y);
  float new_length_r = sqrt(1 - 2*x + x*x + y*y);

  set_lengths(new_length_l, new_length_r);
}


void setup() {
  Serial.begin(9600);

  AFMS.begin();

  Serial.println("begin.");

  // getPath;
  // for(int i = 0; i < num_path; i++){
  //   Serial.print(path[i][0]);
  //   Serial.print(' ');
  //   Serial.println(path[i][1]);
  //   // Lots of messy stuff here.
  //   // set_position has origin in the top left, the 0.5's are there to move the origin to the center
  //   set_position((path[i][0] * x_scale) + 0.5, 0.5 - (path[i][1] * y_scale));
  //   // Let the motors stop
  //   delay(100);
  // }


  leftMotor->setSpeed(12);
  rightMotor->setSpeed(12);

  // contract(20, 40);
  // release(40,20);
  // release(20,40);
  // contract(40,20);

  // run_motors(40,80);
  // run_motors(-80,-40);
  // run_motors(-40,-80);
  // run_motors(80,40);

  // run_motors(40,45);
  // run_motors(-40,-45);

  // set_lengths(10, 10);
  // set_lengths(40, 40);
  // set_lengths(40,45);
  // set_lengths(-40,-45);

  getPath;
  for(int i = 0; i < num_path; i++){
    // Serial.print(path[i][0]);
    // Serial.print(' ');
    // Serial.println(path[i][1]);
    // Lots of messy stuff here.
    // set_position has origin in the top left, the 0.5's are there to move the origin to the center
    set_position(((path[i][0] * x_scale) + 50)*1000, 1000*(0.5 - (path[i][1] * y_scale)));
    // Let the motors stop
    delay(100);
  }


  // // right(150);
  // Forward is anti-clockwise
  // // Backward is anti-clockwise
  // leftMotor->step(150, FORWARD, DOUBLE);
  // delay(100);
  // leftMotor->step(150, BACKWARD, DOUBLE);
  // // leftMotor->step(450, BACKWARD, INTERLEAVE);
  // rightMotor->step(50, FORWARD, DOUBLE);
  // delay(100);
  // // rightMotor->setSpeed(-12);
  // rightMotor->step(50, BACKWARD, DOUBLE);

  leftMotor->release();
  rightMotor->release();
  Serial.println("done!");
}


void loop() {
  delay(10);
}
