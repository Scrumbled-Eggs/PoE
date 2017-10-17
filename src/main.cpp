#include "Arduino.h"
#include <Adafruit_MotorShield.h>
#include <Adafruit_MS_PWMServoDriver.h>



Adafruit_MotorShield AFMS = Adafruit_MotorShield(); // Create the motor shield object with the default I2C address
Adafruit_DCMotor *leftMotor = AFMS.getMotor(3); // Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *rightMotor = AFMS.getMotor(4);

max_speed = 150;


void setup() {
  Serial.begin(9600);

  AFMS.begin();

  Serial.println("begin.");
}

void loop() {
  myMotor->setSpeed(i);
  myMotor->run(BACKWARD);
  myMotor->run(RELEASE);
}
