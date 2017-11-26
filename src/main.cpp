#include "Arduino.h"
#include <Adafruit_MotorShield.h>
#include <Adafruit_MS_PWMServoDriver.h>


Adafruit_MotorShield AFMS = Adafruit_MotorShield(); // Create the motor shield object with the default I2C address

Adafruit_StepperMotor *rightMotor = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *leftMotor = AFMS.getStepper(200, 2);


// Maximum speed to run motors
const int max_speed = 12;

// Width between spools in mm
const int interspool_spacing = 900;

// Conversion from mm to stepper motor steps
const float mm_to_steps = 144.0 /* Circumference in mm */ / 200 /* steps per rotation */;

// Artboard size
const int max_dim = 100;
const int artboard_to_mm = interspool_spacing / max_dim;

// Old test path code, please remove
// Units are in width/artboard_dims mm
const float x_scale = 1;
const float y_scale = -1;

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



// current cable lengths
const int init_rope_length = (interspool_spacing/sqrt(2)) * mm_to_steps;
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

  if(dl_l == 0 && dl_r == 0) return;

  // Init variables of remaining steps
  int l_steps = abs(dl_l);
  int r_steps = abs(dl_r);

  // init variables dl, dr which hold how many steps to take on each loop
  int dl, dr;

  while ((l_steps > 0) || (r_steps > 0)) {
    // Sets the smaller step to 1 and the larger step to ratio between r_steps and l_steps
    if (l_steps > r_steps){
      dl = max(l_steps, 1);
      dr = r_steps / l_steps;
    }
    else {
      dl = l_steps / r_steps;
      dr = max(r_steps, 1);
    }

    rightMotor->step(dl, l_to_dir(dl_r), DOUBLE);
    leftMotor->step(dr, l_to_dir(dl_l), DOUBLE);

    l_steps -= dl;
    r_steps -= dr;
  }
}


void set_lengths(int len_l, int len_r){
  /* Moves the motors to set the string to the desired lengths */

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

  float new_length_l = sqrt(x*x + y*y) * artboard_to_mm * mm_to_steps;
  float new_length_r = sqrt((max_dim - x)*(max_dim - x) + y*y) * artboard_to_mm * mm_to_steps;

  set_lengths(new_length_l, new_length_r);
}


void setup() {
  Serial.begin(9600);

  AFMS.begin();

  Serial.println("begin.");


  leftMotor->setSpeed(max_speed);
  rightMotor->setSpeed(max_speed);

  // Run through the hard coded path
  // for(int i = 0; i < num_path; i++){
  //   Serial.print(path[i][0]);
  //   Serial.print(' ');
  //   Serial.println(path[i][1]);
  //   // Lots of messy stuff here.
  //   // The 50's are there to move the origin of the path to the center of the board
  //   set_position((path[i][0] * x_scale) + 50, 50 - (path[i][1] * y_scale));
  //   // Let the motors rest
  //   delay(10);
  // }

  leftMotor->release();
  rightMotor->release();
  Serial.println("done!");
  Serial.flush();
}

int readInteger(){
  int c[4];
  for(int i =0; i<4; i++) {
    c[i] = Serial.read();  //gets one byte from serial buffer
    delay(10);
  }
  return c[0] + 256*c[1];
}

int * getPath(){
  int length;
  length = readInteger();
  int path[length];
  for (int i=0; i<length; i++){
    path[i] = i;
  }
  // Serial.println(length);
  // Serial.flush();
  return path;
}

int readString;
void loop() {
  // Spin the wheels
  // Serial.println("ready");
  // while(Serial.available()) {
  //   Serial.println(Serial.read());
  // }
  // Serial.println(cmd_id);
  // char inByte = ' ';
  // if(Serial.available()){ // only send data back if data has been sent
  //   char inByte = Serial.read(); // read the incoming data
  //   Serial.println(inByte); // send the data back in a new line so that it is not all one long line
  //   }
  // delay(100); // delay for 1/10 of a second
  // while(!Serial.available()) {}
  // serial read section
  while (Serial.available())
  {
    // if (Serial.available() >0)
    // {
      // int c[4];
      // for(int i =0; i<4; i++) {
      //   c[i] = Serial.read();  //gets one byte from serial buffer
      //   delay(10);
      // }

      // // int c = Serial.parseInt();
      // readString = c[0] + 256*c[1];

      int * x;
      x = getPath();
      Serial.println(sizeof(x));
      Serial.flush();
      // readString = readInteger()
      // Serial.println(readString);
      // Serial.flush();

      // Serial.write(byte(c[0]));
      // Serial.write(byte(c[1]));
      // readString += c; //makes the string readString
    // }
  }

  // if (readString.length() >0)
  // {
  //   // Serial.print("Arduino received: ");  
  //   // Serial.println(readString + "\n"); //see what was received
  // }

  // delay(50);

  // serial write section

  // char ard_sends = '1';
  // Serial.print("Arduino sends: ");
  // Serial.println(ard_sends);
  // Serial.print("\n");
  Serial.flush();
}
