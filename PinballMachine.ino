#include <Playtune.h>

const int SOLENOID_DELAY = 100; //60
const int LED = 13;
const int SWITCH_READ_DELAY = 1500;
const int ANALOG_THRESHOLD = 200;

const int OUT_SLINGSHOT_LEFT = 22;
const int OUT_BALL_RETURN = 23;
const int OUT_SLINGSHOT_RIGHT = 24;
const int OUT_THUMPER_LEFT = 25;
const int OUT_THUMPER_RIGHT = 26;

const int OUT_MX0 = 28;
const int OUT_MX1 = 29;
const int OUT_MX2 = 30;
const int OUT_MX3 = 31;
const int OUT_MX4 = 32;
const int OUT_MX5 = 33;

const int OUT_LT0 = 36;
const int OUT_LT1 = 37;
const int OUT_LT2 = 38;
const int OUT_LT3 = 39;


const int IN_SW0 = A12; //36
const int IN_SW1 = A13; //37
const int IN_SW2 = A14; //38
const int IN_SW3 = A15; //39

bool swBallReturn = false;
bool swTilt = false;
bool swRightSpinner = false;
bool swRightExtraBallLane = false;
bool swA = false;
bool swStart = false;
bool swLeftTarget = false;
bool swLeftSlingShot = false;
bool swB = false;
bool swLeftThumperBumper = false;
bool swLeftBumper = false;
bool swLeftExtraBallLane = false;
bool swC = false;
bool swRightThumperBumper = false;
bool swLeftSpinner = false;
bool swLeftAdvanceLane = false;
bool swD = false;
bool mx4_sw1 = false;
bool swRightTarget = false;
bool swRightSlingShot = false;
bool swCenterTarget = false;
bool mx5_sw1 = false;
bool swRightBumper = false;
bool swRightAdvanceLane = false;

bool mx0_lt0 = true;
bool mx0_lt1 = true;
bool mx0_lt2 = true;
bool mx0_lt3 = true;
bool mx1_lt0 = true;
bool mx1_lt1 = true;
bool mx1_lt2 = true;
bool mx1_lt3 = true;
bool mx2_lt0 = true;
bool mx2_lt1 = true;
bool mx2_lt2 = true;
bool mx2_lt3 = true;
bool mx3_lt0 = true;
bool mx3_lt1 = true;
bool mx3_lt2 = true;
bool mx3_lt3 = true;
bool mx4_lt0 = true;
bool mx4_lt1 = true;
bool mx4_lt2 = true;
bool mx4_lt3 = true;
bool mx5_lt0 = true;
bool mx5_lt1 = true;
bool mx5_lt2 = true;
bool mx5_lt3 = true;


// Time (as returned by millis()) that solenoid should retract.
unsigned long timeRetractLeftSlingshot = 0, timeRetractRightSlingshot = 0;
unsigned long timeRetractLeftThumperBumper = 0, timeRetractRightThumperBumper = 0;
unsigned long timeRetractBallReturn = 0;

const byte PROGMEM score [] = {
0x90,54, // Nonstop2k - The Producer's Resource?
0,240, 0x80, 0x90,54, 0x91,66, 0,240, 0x81, 0x80, 0x90,70, 0,240, 0x80, 0x90,54, 0,240, 
0x91,73, 0,120, 0x80, 0,120, 0x81, 0x90,54, 0x91,66, 0,240, 0x81, 0x80, 0x90,75, 0,240, 0x80, 
0x90,54, 0x91,78, 0,240, 0x81, 0x80, 0x90,54, 0,240, 0x80, 0x90,54, 0x91,66, 0,240, 0x81, 0x80, 
0x90,73, 0,240, 0x80, 0x90,54, 0,240, 0x91,68, 0,120, 0x80, 0,120, 0x81, 0x90,54, 0x91,66, 
0,240, 0x81, 0x80, 0x90,65, 0,240, 0x80, 0x90,54, 0x91,66, 0,240, 0x81, 0x80, 0x90,54, 0,240, 
0x80, 0x90,54, 0x91,66, 0,240, 0x81, 0x80, 0x90,70, 0,240, 0x80, 0x90,54, 0,240, 0x91,73, 0,120, 
0x80, 0,120, 0x81, 0x90,54, 0x91,66, 0,240, 0x81, 0x80, 0x90,75, 0,240, 0x80, 0x90,54, 0x91,78, 
0,240, 0x81, 0x80, 0x90,54, 0,240, 0x80, 0x90,54, 0x91,66, 0,240, 0x81, 0x80, 0x90,73, 0,240, 
0x80, 0x90,54, 0,240, 0x91,68, 0,120, 0x80, 0,120, 0x81, 0x90,54, 0x91,66, 0,240, 0x81, 0x80, 
0x90,65, 0,240, 0x80, 0x90,54, 0x91,66, 0,240, 0x81, 0x80, 0x90,51, 0,240, 0x91,58, 0x92,66, 
0,240, 0x80, 0x82, 0x81, 0x90,70, 0,240, 0x80, 0x90,58, 0x91,51, 0,240, 0x80, 0x90,73, 0,240, 
0x80, 0x81, 0x90,58, 0x91,66, 0,240, 0x81, 0x80, 0x90,75, 0,240, 0x80, 0x90,58, 0x91,78, 0,240, 
0x81, 0x80, 0x90,49, 0,240, 0x91,56, 0x92,66, 0,240, 0x80, 0x82, 0x81, 0x90,73, 0,240, 0x80, 0x90,56, 
0x91,49, 0,240, 0x80, 0x90,68, 0,240, 0x80, 0x81, 0x90,56, 0x91,66, 0,240, 0x81, 0x80, 0x90,65, 
0,240, 0x80, 0x90,56, 0x91,66, 0,240, 0x81, 0x80, 0x90,47, 0,240, 0x91,66, 0x92,54, 0,240, 
0x80, 0x82, 0x81, 0x90,70, 0,240, 0x80, 0x90,47, 0x91,54, 0,240, 0x81, 0x91,73, 0,240, 0x81, 0x80, 
0x90,66, 0x91,54, 0,240, 0x81, 0x80, 0x90,75, 0,240, 0x80, 0x90,78, 0x91,54, 0,240, 0x81, 0x80, 
0x90,47, 0,240, 0x91,66, 0x92,54, 0,240, 0x80, 0x82, 0x81, 0x90,73, 0,240, 0x80, 0x90,47, 0x91,54, 
0,240, 0x81, 0x91,68, 0,240, 0x81, 0x80, 0x90,66, 0x91,54, 0,240, 0x81, 0x80, 0x90,65, 0,240, 
0x80, 0x90,66, 0x91,54, 0,240, 0x81, 0x80, 0x90,44, 0,240, 0x91,56, 0x92,66, 0,240, 0x80, 0x82, 
0x81, 0x90,70, 0,240, 0x80, 0x90,56, 0x91,44, 0,240, 0x80, 0x90,73, 0,240, 0x80, 0x81, 0x90,56, 
0x91,66, 0,240, 0x81, 0x80, 0x90,75, 0,240, 0x80, 0x90,56, 0x91,78, 0,240, 0x81, 0x80, 0x90,46, 
0,240, 0x91,58, 0x92,66, 0,240, 0x80, 0x82, 0x81, 0x90,73, 0,240, 0x80, 0x90,58, 0x91,46, 0,240, 
0x80, 0x90,68, 0,240, 0x80, 0x81, 0x90,58, 0x91,66, 0,240, 0x81, 0x80, 0x90,65, 0,240, 0x80, 0x90,58, 
0x91,66, 0,240, 0x81, 0x80, 0x90,47, 0,240, 0x91,66, 0x92,54, 0,240, 0x80, 0x82, 0x81, 0x90,70, 
0,240, 0x80, 0x90,47, 0x91,54, 0,240, 0x81, 0x91,73, 0,240, 0x81, 0x80, 0x90,66, 0x91,54, 0,240, 
0x81, 0x80, 0x90,75, 0,240, 0x80, 0x90,78, 0x91,54, 0,240, 0x81, 0x80, 0x90,49, 0,240, 0x91,66, 
0x92,56, 0,240, 0x80, 0x82, 0x81, 0x90,73, 0,240, 0x80, 0x90,49, 0x91,56, 0,240, 0x81, 0x91,68, 
0,240, 0x81, 0x80, 0x90,66, 0x91,56, 0,240, 0x81, 0x80, 0x90,65, 0,240, 0x80, 0x90,66, 0x91,56, 
0,240, 0x81, 0x80, 0x90,54, 0,240, 0x80, 0x90,54, 0x91,66, 0,240, 0x81, 0x80, 0x90,70, 0,240, 
0x80, 0x90,54, 0,240, 0x91,73, 0,120, 0x80, 0,120, 0x81, 0x90,54, 0x91,66, 0,240, 0x81, 0x80, 
0x90,75, 0,240, 0x80, 0x90,54, 0x91,78, 0,240, 0x81, 0x80, 0x90,54, 0,240, 0x80, 0x90,54, 0x91,66, 
0,240, 0x81, 0x80, 0x90,70, 0,240, 0x80, 0x90,54, 0,240, 0x91,73, 0,120, 0x80, 0,120, 0x81, 
0x90,54, 0x91,66, 0,240, 0x81, 0x80, 0x90,75, 0,240, 0x80, 0x90,54, 0x91,78, 0,240, 0x81, 0x80, 
0x90,54, 0,240, 0x80, 0x90,54, 0x91,66, 0,240, 0x81, 0x80, 0x90,70, 0,240, 0x80, 0x90,54, 0,240, 
0x91,73, 0,120, 0x80, 0,120, 0x81, 0x90,54, 0x91,66, 0,240, 0x81, 0x80, 0x90,75, 0,240, 0x80, 
0x90,54, 0x91,78, 0,240, 0x81, 0x80, 0x90,54, 0,240, 0x80, 0x90,54, 0x91,66, 0,240, 0x81, 0x80, 
0x90,70, 0,240, 0x80, 0x90,54, 0,240, 0x91,73, 0,120, 0x80, 0,120, 0x81, 0x90,54, 0x91,66, 
0,240, 0x81, 0x80, 0x90,75, 0,240, 0x80, 0x90,54, 0x91,78, 0,240, 0x92,36, 0x81, 0x80, 0x90,51, 
0,240, 0x91,58, 0,240, 0x82, 0x80, 0x81, 0x90,36, 0x91,70, 0x92,39, 0,240, 0x81, 0x82, 0x91,58, 
0x92,51, 0,240, 0x80, 0x81, 0x90,36, 0x91,73, 0,240, 0x81, 0x82, 0x91,58, 0x92,66, 0,240, 0x80, 
0x82, 0x81, 0x90,36, 0x91,75, 0x92,39, 0,240, 0x81, 0x82, 0x91,58, 0x92,78, 0,240, 0x80, 0x82, 0x81, 
0x90,36, 0x91,49, 0,240, 0x92,56, 0,240, 0x80, 0x81, 0x82, 0x90,36, 0x91,73, 0x92,39, 0,240, 
0x81, 0x82, 0x91,56, 0x92,49, 0,240, 0x80, 0x81, 0x90,36, 0x91,68, 0,240, 0x81, 0x82, 0x91,56, 0x92,66, 
0,240, 0x80, 0x82, 0x81, 0x90,36, 0x91,65, 0x92,39, 0,240, 0x81, 0x82, 0x91,56, 0x92,66, 0,240, 
0x80, 0x82, 0x81, 0x90,36, 0x91,47, 0,240, 0x92,66, 0,240, 0x80, 0x81, 0x82, 0x90,36, 0x91,70, 0x92,39, 
0,240, 0x81, 0x82, 0x91,47, 0x92,54, 0,240, 0x80, 0x82, 0x90,36, 0x92,73, 0,240, 0x82, 0x81, 0x91,66, 
0x92,54, 0,240, 0x80, 0x82, 0x81, 0x90,36, 0x91,75, 0x92,39, 0,240, 0x81, 0x82, 0x91,78, 0x92,54, 
0,240, 0x80, 0x82, 0x81, 0x90,36, 0x91,47, 0,240, 0x92,66, 0,240, 0x80, 0x81, 0x82, 0x90,36, 0x91,73, 
0x92,39, 0,240, 0x81, 0x82, 0x91,47, 0x92,54, 0,240, 0x80, 0x82, 0x90,36, 0x92,68, 0,240, 0x82, 
0x81, 0x91,66, 0x92,54, 0,240, 0x80, 0x82, 0x81, 0x90,36, 0x91,65, 0x92,39, 0,240, 0x81, 0x82, 0x91,66, 
0x92,54, 0,240, 0x80, 0x82, 0x81, 0x90,36, 0x91,44, 0,240, 0x92,56, 0,240, 0x80, 0x81, 0x82, 0x90,36, 
0x91,70, 0x92,39, 0,240, 0x81, 0x82, 0x91,56, 0x92,44, 0,240, 0x80, 0x81, 0x90,36, 0x91,73, 0,240, 
0x81, 0x82, 0x91,56, 0x92,66, 0,240, 0x80, 0x82, 0x81, 0x90,36, 0x91,75, 0x92,39, 0,240, 0x81, 0x82, 
0x91,56, 0x92,78, 0,240, 0x80, 0x82, 0x81, 0x90,36, 0x91,46, 0,240, 0x92,58, 0,240, 0x80, 0x81, 
0x82, 0x90,36, 0x91,73, 0x92,39, 0,240, 0x81, 0x82, 0x91,58, 0x92,46, 0,240, 0x80, 0x81, 0x90,36, 
0x91,68, 0,240, 0x81, 0x82, 0x91,58, 0x92,66, 0,240, 0x80, 0x82, 0x81, 0x90,36, 0x91,65, 0x92,39, 
0,240, 0x81, 0x82, 0x91,58, 0x92,66, 0,240, 0x80, 0x82, 0x81, 0x90,36, 0x91,47, 0,240, 0x92,66, 
0,240, 0x80, 0x81, 0x82, 0x90,36, 0x91,70, 0x92,39, 0,240, 0x81, 0x82, 0x91,47, 0x92,54, 0,240, 
0x80, 0x82, 0x90,36, 0x92,73, 0,240, 0x82, 0x81, 0x91,66, 0x92,54, 0,240, 0x80, 0x82, 0x81, 0x90,36, 
0x91,75, 0x92,39, 0,240, 0x81, 0x82, 0x91,78, 0x92,54, 0,240, 0x80, 0x82, 0x81, 0x90,36, 0x91,49, 
0,240, 0x92,66, 0,240, 0x80, 0x81, 0x82, 0x90,36, 0x91,73, 0x92,39, 0,240, 0x81, 0x82, 0x91,49, 
0x92,56, 0,240, 0x80, 0x82, 0x90,36, 0x92,68, 0,240, 0x82, 0x81, 0x91,66, 0x92,56, 0,240, 0x80, 
0x82, 0x81, 0x90,36, 0x91,65, 0x92,39, 0,240, 0x81, 0x82, 0x91,66, 0x92,56, 0,240, 0x80, 0x82, 0x81, 
0xf0};

Playtune pt;
void setup() {

  Serial.begin(57600);


  pinMode(LED, OUTPUT);
  pinMode(OUT_SLINGSHOT_LEFT, OUTPUT);
  pinMode(OUT_SLINGSHOT_RIGHT, OUTPUT);
  pinMode(OUT_THUMPER_LEFT, OUTPUT);
  pinMode(OUT_THUMPER_RIGHT, OUTPUT);
  pinMode(OUT_BALL_RETURN, OUTPUT);

  pinMode(OUT_MX0, OUTPUT);
  pinMode(OUT_MX1, OUTPUT);
  pinMode(OUT_MX2, OUTPUT);
  pinMode(OUT_MX3, OUTPUT);
  pinMode(OUT_MX4, OUTPUT);
  pinMode(OUT_MX5, OUTPUT);

  pinMode(OUT_LT0, OUTPUT);
  pinMode(OUT_LT1, OUTPUT);
  pinMode(OUT_LT2, OUTPUT);
  pinMode(OUT_LT3, OUTPUT);

  pinMode(IN_SW0, INPUT);
  pinMode(IN_SW1, INPUT);
  pinMode(IN_SW2, INPUT);
  pinMode(IN_SW3, INPUT);

  //Serial.println("Setup Initalised. Hello World!");

  pt.tune_initchan (44);
  pt.tune_initchan (45);
  pt.tune_initchan (46);
}



void loop() {
  if(!pt.tune_playing){
    pt.tune_playscore (score);
  }
  checkSwitchesAndLightLights();
  solinoids();
  //debug();
  //delay(3000);
}

void solinoids() {
  unsigned long currentTime = millis();

  // Fire solenoids that should be fired.
  if (swLeftSlingShot) {
    digitalWrite(OUT_SLINGSHOT_LEFT, HIGH);
    timeRetractLeftSlingshot = currentTime + SOLENOID_DELAY;
  }

  if (swRightSlingShot) {
    digitalWrite(OUT_SLINGSHOT_RIGHT, HIGH);
    timeRetractRightSlingshot = currentTime + SOLENOID_DELAY;
  }

  if (swLeftThumperBumper) {
    digitalWrite(OUT_THUMPER_LEFT, HIGH);
    timeRetractLeftThumperBumper = currentTime + SOLENOID_DELAY;
  }

  if (swRightThumperBumper) {
    digitalWrite(OUT_THUMPER_RIGHT, HIGH);
    timeRetractRightThumperBumper = currentTime + SOLENOID_DELAY;
  }

  if (swBallReturn) {
    digitalWrite(OUT_BALL_RETURN, HIGH);
    timeRetractBallReturn = currentTime + SOLENOID_DELAY;
  }

  // Retract solenoids that should be retracted.
  if (currentTime > timeRetractLeftSlingshot) {
    digitalWrite(OUT_SLINGSHOT_LEFT, LOW);
  }

  if (currentTime > timeRetractRightSlingshot) {
    digitalWrite(OUT_SLINGSHOT_RIGHT, LOW);
  }

  if (currentTime > timeRetractLeftThumperBumper) {
    digitalWrite(OUT_THUMPER_LEFT, LOW);
  }

  if (currentTime > timeRetractRightThumperBumper) {
    digitalWrite(OUT_THUMPER_RIGHT, LOW);
  }

  if (currentTime > timeRetractBallReturn) {
    digitalWrite(OUT_BALL_RETURN, LOW);
  }

}

void checkSwitchesAndLightLights() {

  digitalWrite(OUT_MX0, HIGH);
  digitalWrite(OUT_LT0, mx0_lt0);
  digitalWrite(OUT_LT1, mx0_lt1);
  digitalWrite(OUT_LT2, mx0_lt2);
  digitalWrite(OUT_LT3, mx0_lt3);

  delayMicroseconds(SWITCH_READ_DELAY);

  swBallReturn = analogRead(IN_SW0) > ANALOG_THRESHOLD;
  swTilt = analogRead(IN_SW1) > ANALOG_THRESHOLD;
  swRightSpinner = analogRead(IN_SW2) > ANALOG_THRESHOLD;
  swRightExtraBallLane = analogRead(IN_SW3) > ANALOG_THRESHOLD;

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX0, LOW);

  ///////////////////////////////

  digitalWrite(OUT_MX1, HIGH);

  digitalWrite(OUT_LT0, mx1_lt0);
  digitalWrite(OUT_LT1, mx1_lt1);
  digitalWrite(OUT_LT2, mx1_lt2);
  digitalWrite(OUT_LT3, mx1_lt3);

  delayMicroseconds(SWITCH_READ_DELAY);
  swA = analogRead(IN_SW0) > ANALOG_THRESHOLD;
  swStart = analogRead(IN_SW1) > ANALOG_THRESHOLD;
  swLeftTarget = analogRead(IN_SW2) > ANALOG_THRESHOLD;
  swLeftSlingShot = analogRead(IN_SW3) > ANALOG_THRESHOLD;

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX1, LOW);

  //////////////////

  digitalWrite(OUT_MX2, HIGH);

  digitalWrite(OUT_LT0, mx2_lt0);
  digitalWrite(OUT_LT1, mx2_lt1);
  digitalWrite(OUT_LT2, mx2_lt2);
  digitalWrite(OUT_LT3, mx2_lt3);

  delayMicroseconds(SWITCH_READ_DELAY);
  swB = analogRead(IN_SW0) > ANALOG_THRESHOLD;
  swLeftThumperBumper = analogRead(IN_SW1) > ANALOG_THRESHOLD;
  swLeftBumper = analogRead(IN_SW2) > ANALOG_THRESHOLD;
  swLeftExtraBallLane = analogRead(IN_SW3) > ANALOG_THRESHOLD;

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX2, LOW);

  /////////////////////

  digitalWrite(OUT_MX3, HIGH);

  digitalWrite(OUT_LT0, mx3_lt0);
  digitalWrite(OUT_LT1, mx3_lt1);
  digitalWrite(OUT_LT2, mx3_lt2);
  digitalWrite(OUT_LT3, mx3_lt3);

  delayMicroseconds(SWITCH_READ_DELAY);
  swC = analogRead(IN_SW0) > ANALOG_THRESHOLD;
  swRightThumperBumper = analogRead(IN_SW1) > ANALOG_THRESHOLD;
  swLeftSpinner = analogRead(IN_SW2) > ANALOG_THRESHOLD;
  swLeftAdvanceLane = analogRead(IN_SW3) > ANALOG_THRESHOLD;

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX3, LOW);

  /////////////////////////////

  digitalWrite(OUT_MX4, HIGH);

  digitalWrite(OUT_LT0, mx4_lt0);
  digitalWrite(OUT_LT1, mx4_lt1);
  digitalWrite(OUT_LT2, mx4_lt2);
  digitalWrite(OUT_LT3, mx4_lt3);

  delayMicroseconds(SWITCH_READ_DELAY);
  swD = analogRead(IN_SW0) > ANALOG_THRESHOLD;
  mx4_sw1 = analogRead(IN_SW1) > ANALOG_THRESHOLD;
  swRightTarget = analogRead(IN_SW2) > ANALOG_THRESHOLD;
  swRightSlingShot = analogRead(IN_SW3) > ANALOG_THRESHOLD;

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX4, LOW);


  ///////////////////////////////

  digitalWrite(OUT_MX5, HIGH);

  digitalWrite(OUT_LT0, mx5_lt0);
  digitalWrite(OUT_LT1, mx5_lt1);
  digitalWrite(OUT_LT2, mx5_lt2);
  digitalWrite(OUT_LT3, mx5_lt3);

  delayMicroseconds(SWITCH_READ_DELAY);
  swCenterTarget = analogRead(IN_SW0) > ANALOG_THRESHOLD;
  mx5_sw1 = analogRead(IN_SW1) > ANALOG_THRESHOLD;
  swRightBumper = analogRead(IN_SW2) > ANALOG_THRESHOLD;
  swRightAdvanceLane = analogRead(IN_SW3) > ANALOG_THRESHOLD;

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX5, LOW);


}

void debug() {

  Serial.print("swBallReturn = ");
  Serial.print(swBallReturn);
  Serial.println();

  Serial.print("swTilt = ");
  Serial.print(swTilt);
  Serial.println();

  Serial.print("swRightSpinner = ");
  Serial.print(swRightSpinner);
  Serial.println();

  Serial.print("swRightExtraBallLane = ");
  Serial.print(swRightExtraBallLane);
  Serial.println();

  Serial.print("swA = ");
  Serial.print(swA);
  Serial.println();

  Serial.print("swStart = ");
  Serial.print(swStart);
  Serial.println();

  Serial.print("swLeftTarget = ");
  Serial.print(swLeftTarget);
  Serial.println();

  Serial.print("swLeftSlingShot = ");
  Serial.print(swLeftSlingShot);
  Serial.println();

  Serial.print("swB = ");
  Serial.print(swB);
  Serial.println();

  Serial.print("swLeftThumperBumper = ");
  Serial.print(swLeftThumperBumper);
  Serial.println();

  Serial.print("swLeftBumper = ");
  Serial.print(swLeftBumper);
  Serial.println();

  Serial.print("swLeftExtraBallLane = ");
  Serial.print(swLeftExtraBallLane);
  Serial.println();

  Serial.print("swC = ");
  Serial.print(swC);
  Serial.println();

  Serial.print("swRightThumperBumper = ");
  Serial.print(swRightThumperBumper);
  Serial.println();

  Serial.print("swLeftSpinner = ");
  Serial.print(swLeftSpinner);
  Serial.println();

  Serial.print("swLeftAdvanceLane = ");
  Serial.print(swLeftAdvanceLane);
  Serial.println();

  Serial.print("swD = ");
  Serial.print(swD);
  Serial.println();

  Serial.print("mx4_sw1 = ");
  Serial.print(mx4_sw1);
  Serial.println();

  Serial.print("swRightTarget = ");
  Serial.print(swRightTarget);
  Serial.println();

  Serial.print("swRightSlingShot = ");
  Serial.print(swRightSlingShot);
  Serial.println();

  Serial.print("swCenterTarget = ");
  Serial.print(swCenterTarget);
  Serial.println();

  Serial.print("mx5_sw1 = ");
  Serial.print(mx5_sw1);
  Serial.println();

  Serial.print("swRightBumper = ");
  Serial.print(swRightBumper);
  Serial.println();

  Serial.print("swRightAdvanceLane = ");
  Serial.print(swRightAdvanceLane);
  Serial.println();

  Serial.println("--------------------------------------------------------");
  Serial.println();

}

