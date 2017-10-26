#include "Arduino.h"
#include <Adafruit_MotorShield.h>
#include <Adafruit_MS_PWMServoDriver.h>



Adafruit_MotorShield AFMS = Adafruit_MotorShield(); // Create the motor shield object with the default I2C address
Adafruit_DCMotor *leftMotor = AFMS.getMotor(3); // Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *rightMotor = AFMS.getMotor(4);

const int max_speed = 70;

float cur_lengths[2] = { 1/sqrt(2), 1/sqrt(2) };


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

int speed_to_dir(float speed){
  if(speed > 0)  return FORWARD;
  if(speed < 0)  return BACKWARD;
  return RELEASE;
}

void run_motors(float dl_l, float dl_r){
  // Given dl_l and dl_r, move the motors by that much
  // dl_l (delta length left) in units: <>
  const long int l_to_ms = 100000;

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

  leftMotor->run(RELEASE);
  rightMotor->run(RELEASE);
}


void set_lengths(float len_l, float len_r){
  /* Moves the motors to the desired lengths */
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
   */
  Serial.println("setting xy" + String(x) + " " + String(y));

  float new_length_l = sqrt(x*x + y*y);
  float new_length_r = sqrt(1 - 2*x + x*x + y*y);

  set_lengths(new_length_l, new_length_r);
}

const int num_path = 4;
const float path[][2] =   {{0,  0},
                          {0,   1},
                          {0,   0.5},
                          {0.5, 0.5}};
const float x_scale = 0.1;
const float y_scale = 0.1;


void setup() {
  Serial.begin(9600);

  AFMS.begin();

  Serial.println("begin.");

  for(int i = 0; i < num_path; i++){
    Serial.print(path[i][0]);
    Serial.print(' ');
    Serial.println(path[i][1]);
    set_position((path[i][0] * x_scale) + 0.5, 0.5 - (path[i][1] * y_scale));
    delay(500);
  }

  Serial.println("done!");
}


void loop() {
  delay(10);
}
