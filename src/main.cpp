#include "Arduino.h"
#include <Adafruit_MotorShield.h>
#include <Adafruit_MS_PWMServoDriver.h>



Adafruit_MotorShield AFMS = Adafruit_MotorShield(); // Create the motor shield object with the default I2C address
Adafruit_DCMotor *leftMotor = AFMS.getMotor(3); // Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *rightMotor = AFMS.getMotor(4);

const int max_speed = 70;

void drawVertically(bool dir, int time){
  leftMotor->setSpeed(max_speed);
  rightMotor->setSpeed(max_speed);

  if(dir){
    leftMotor->run(FORWARD);
    rightMotor->run(FORWARD);
  } else {
    leftMotor->run(BACKWARD);
    rightMotor->run(BACKWARD);
  }

  delay(time);

  leftMotor->run(RELEASE);
  rightMotor->run(RELEASE);
}


void setup() {
  Serial.begin(9600);

  AFMS.begin();

  Serial.println("begin.");
}



void randomWalk(int length_ms){
  // Get random speed and directions
  int motorSpeeds[] = { random(max_speed), random(max_speed) };
  int motorDirections[] = { random(1) == 1, random(1) == 1 };

  // Set motorspeeds and directions
  leftMotor->setSpeed(motorSpeeds[0]);
  rightMotor->setSpeed(motorSpeeds[1]);
  leftMotor->run(motorDirections[0]);
  rightMotor->run(motorDirections[1]);

  // Wait length_ms
  delay(length_ms);

  // Turn off motors
  leftMotor->run(RELEASE);
  rightMotor->run(RELEASE);
}

void loop() {
  drawVertically(true, 500);
  delay(500);
  drawVertically(false, 500);
  delay(500);
}
