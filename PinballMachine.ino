
const int DELAY = 80;
const int LED = 13;
const int  SWITCH_READ_DELAY = 5000;
const int ANALOG_THRESHOLD = 100;

const int OUT_SLINGSHOT_LEFT = 22;
const int OUT_BALL_RETUN = 23;
const int OUT_SLINGSHOT_RIGHT = 24;
const int OUT_THUMPER_LEFT = 25;
const int OUT_THUMPER_RIGHT = 26;

const int OUT_MX0 = 28;
const int OUT_MX1 = 29;
const int OUT_MX2 = 30;
const int OUT_MX3 = 31;
const int OUT_MX4 = 32;
const int OUT_MX5 = 33;

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


void setup() {

  Serial.begin(57600);
  
  
  pinMode(LED, OUTPUT);
  pinMode(OUT_SLINGSHOT_LEFT, OUTPUT);
  pinMode(OUT_SLINGSHOT_RIGHT, OUTPUT);
  pinMode(OUT_THUMPER_LEFT, OUTPUT);
  pinMode(OUT_THUMPER_RIGHT, OUTPUT);
  pinMode(OUT_BALL_RETUN, OUTPUT);

  pinMode(OUT_MX0, OUTPUT);
  pinMode(OUT_MX1, OUTPUT);
  pinMode(OUT_MX2, OUTPUT);
  pinMode(OUT_MX3, OUTPUT);
  pinMode(OUT_MX4, OUTPUT);
  pinMode(OUT_MX5, OUTPUT);

  pinMode(IN_SW0, INPUT);
  pinMode(IN_SW1, INPUT);
  pinMode(IN_SW2, INPUT);
  pinMode(IN_SW3, INPUT);
  
  Serial.println("Setup Initalised. Hello World!");
}

void loop() {
  //  pulse(OUT_SLINGSHOT_LEFT);
  //  pulse(OUT_SLINGSHOT_RIGHT);
  //  pulse(OUT_THUMPER_LEFT);
  //  pulse(OUT_THUMPER_RIGHT);
  //  pulse(OUT_BALL_RETUN);
  //  delay(3000);
  checkSwitches();
  //solinoids();
  debug();
  delay(3000);
}

void solinoids(){
  if(swD){
    pulse(OUT_SLINGSHOT_LEFT);
  }
}

void checkSwitches() {

  delayMicroseconds(SWITCH_READ_DELAY);
  digitalWrite(OUT_MX0, HIGH);
  delayMicroseconds(SWITCH_READ_DELAY);
  swBallReturn = analogRead(IN_SW0) > ANALOG_THRESHOLD;
  swTilt = analogRead(IN_SW1) > ANALOG_THRESHOLD;
  swRightSpinner = analogRead(IN_SW2) > ANALOG_THRESHOLD;
  swRightExtraBallLane = analogRead(IN_SW3) > ANALOG_THRESHOLD;
  digitalWrite(OUT_MX0, LOW);


  delayMicroseconds(SWITCH_READ_DELAY);
  digitalWrite(OUT_MX1, HIGH);
  delayMicroseconds(SWITCH_READ_DELAY);
  swA = analogRead(IN_SW0) > ANALOG_THRESHOLD;
  swStart = analogRead(IN_SW1) > ANALOG_THRESHOLD;
  swLeftTarget = analogRead(IN_SW2) > ANALOG_THRESHOLD;
  swLeftSlingShot = analogRead(IN_SW3) > ANALOG_THRESHOLD;
  digitalWrite(OUT_MX1, LOW);

  delayMicroseconds(SWITCH_READ_DELAY);
  digitalWrite(OUT_MX2, HIGH);
  delayMicroseconds(SWITCH_READ_DELAY);
  swB = analogRead(IN_SW0) > ANALOG_THRESHOLD;
  swLeftThumperBumper = analogRead(IN_SW1) > ANALOG_THRESHOLD;
  swLeftBumper = analogRead(IN_SW2) > ANALOG_THRESHOLD;
  swLeftExtraBallLane = analogRead(IN_SW3) > ANALOG_THRESHOLD;
  digitalWrite(OUT_MX2, LOW);

  delayMicroseconds(SWITCH_READ_DELAY);
  digitalWrite(OUT_MX3, HIGH);
  delayMicroseconds(SWITCH_READ_DELAY);
  swC = analogRead(IN_SW0) > ANALOG_THRESHOLD;
  swRightThumperBumper = analogRead(IN_SW1) > ANALOG_THRESHOLD;
  swLeftSpinner = analogRead(IN_SW2) > ANALOG_THRESHOLD;
  swLeftAdvanceLane = analogRead(IN_SW3) > ANALOG_THRESHOLD;
  //Serial.print("33 RAW: ");
  //Serial.println(analogRead(IN_SW3));
  digitalWrite(OUT_MX3, LOW);

  delayMicroseconds(SWITCH_READ_DELAY);
  digitalWrite(OUT_MX4, HIGH);
  delayMicroseconds(SWITCH_READ_DELAY);
  swD = analogRead(IN_SW0) > ANALOG_THRESHOLD;
 // Serial.print("40 RAW: ");
  //Serial.println(analogRead(IN_SW0));
  mx4_sw1 = analogRead(IN_SW1) > ANALOG_THRESHOLD;
  swRightTarget = analogRead(IN_SW2) > ANALOG_THRESHOLD;
  swRightSlingShot = analogRead(IN_SW3) > ANALOG_THRESHOLD;
  digitalWrite(OUT_MX4, LOW);

  delayMicroseconds(SWITCH_READ_DELAY);
  digitalWrite(OUT_MX5, HIGH);
  delayMicroseconds(SWITCH_READ_DELAY);
  swCenterTarget = analogRead(IN_SW0) > ANALOG_THRESHOLD;
  mx5_sw1 = analogRead(IN_SW1) > ANALOG_THRESHOLD;
  swRightBumper = analogRead(IN_SW2) > ANALOG_THRESHOLD;
  swRightAdvanceLane = analogRead(IN_SW3) > ANALOG_THRESHOLD;
  digitalWrite(OUT_MX5, LOW);


}

void pulse(int thing) {
  digitalWrite(thing, HIGH);
  digitalWrite(LED, HIGH);
  delay(DELAY);
  digitalWrite(thing, LOW);
  digitalWrite(LED, LOW);
  delay(DELAY);
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

