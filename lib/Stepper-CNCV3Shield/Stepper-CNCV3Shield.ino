
//      ******************************************************************
//      *                                                                *
//      *         Stepper Test Program for CNC V3 Stepper Board          *
//      *                                                                *
//      *            Stan Reifel                     10/23/2017          *
//      *                                                                *
//      ******************************************************************


#include <EEPROM.h>
#include "SpeedyStepper.h"


//
// pin assignments
//
const int LED_PIN = 13;
const int MOTOR_X_STEP = 2;
const int MOTOR_Y_STEP = 3;
const int MOTOR_Z_STEP = 4;
const int MOTOR_X_DIR = 5;
const int MOTOR_Y_DIR = 6;
const int MOTOR_Z_DIR = 7;
const int STEPPERS_ENABLE = 8;
const int LIMIT_SWITCH_X = 9;
const int LIMIT_SWITCH_Y = 10;
const int LIMIT_SWITCH_Z = 11;
const int SPINDLE_ENABLE = 12;
const int SPINDLE_DIRECTION = 13;



//
// create the stepper motor object
//
SpeedyStepper stepperY;


//
// local vars
//


// ---------------------------------------------------------------------------------
//                              Hardware and software setup
// ---------------------------------------------------------------------------------

//
// top level setup function
//
void setup()
{ 
  byte contrastValue;

  //
  // configure IO pins
  //
  pinMode(LED_PIN, OUTPUT);   
  pinMode(STEPPERS_ENABLE, OUTPUT);   


  //
  // create the stepper object & enable them
  //
  stepperY.connectToPins(MOTOR_Y_STEP, MOTOR_Y_DIR);
  digitalWrite(STEPPERS_ENABLE, LOW);

 
 
  Serial.begin(9600);
}


//
//
//
void loop()
{
  //
  // set the speed in steps/second and acceleration in steps/second/second
  //
  stepperY.setSpeedInStepsPerSecond(800);
  stepperY.setAccelerationInStepsPerSecondPerSecond(800);
  stepperY.setStepsPerRevolution(800);            // 200 for no microstepping

  //
  // move 200 steps in the backward direction
  //
  stepperY.moveToPositionInSteps(800);
  delay(100);
  
  //
  // move to an absolute position of 200 steps
  //
  stepperY.moveToPositionInSteps(0);

  delay(1500);
}

// -------------------------------------- End --------------------------------------


