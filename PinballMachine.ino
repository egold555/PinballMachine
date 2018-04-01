
const int DELAY = 80;
const int LED = 13;

const int  SWITCH_READ_DELAY = 5000;

const int OUT_SLINGSHOT_LEFT = 22;
const int OUT_BALL_RETUN = 23;
const int OUT_SLINGSHOT_RIGHT = 24;
const int OUT_THUMPER_LEFT = 25;
const int OUT_THUMPER_RIGHT = 26;

const int OUT_MX0 = 28;
const int OUT_MX1 = 29;
const int OUT_MX2 = ;
const int OUT_MX3 = 31;
const int OUT_MX4 = 32;
const int OUT_MX5 = 33;

const int IN_SW0 = 36;
const int IN_SW1 = 37;
const int IN_SW2 = 38;
const int IN_SW3 = 39;

bool mx0_sw0 = false;
bool mx0_sw1 = false;
bool mx0_sw2 = false;
bool mx0_sw3 = false;
bool mx1_sw0 = false;
bool mx1_sw1 = false;
bool mx1_sw2 = false;
bool mx1_sw3 = false;
bool mx2_sw0 = false;
bool mx2_sw1 = false;
bool mx2_sw2 = false;
bool mx2_sw3 = false;
bool mx3_sw0 = false;
bool mx3_sw1 = false;
bool mx3_sw2 = false;
bool mx3_sw3 = false;
bool mx4_sw0 = false;
bool mx4_sw1 = false;
bool mx4_sw2 = false;
bool mx4_sw3 = false;
bool mx5_sw0 = false;
bool mx5_sw1 = false;
bool mx5_sw2 = false;
bool mx5_sw3 = false;


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
  debug();
  delay(3000);
}

void checkSwitches() {

  delayMicroseconds(SWITCH_READ_DELAY);
  digitalWrite(OUT_MX0, HIGH);
  delayMicroseconds(SWITCH_READ_DELAY);
  mx0_sw0 = (bool) digitalRead(IN_SW0);
  mx0_sw1 = (bool) digitalRead(IN_SW1);
  mx0_sw2 = (bool) digitalRead(IN_SW2);
  mx0_sw3 = (bool) digitalRead(IN_SW3);
  digitalWrite(OUT_MX0, LOW);


  delayMicroseconds(SWITCH_READ_DELAY);
  digitalWrite(OUT_MX1, HIGH);
  delayMicroseconds(SWITCH_READ_DELAY);
  mx1_sw0 = (bool) digitalRead(IN_SW0);
  mx1_sw1 = (bool) digitalRead(IN_SW1);
  mx1_sw2 = (bool) digitalRead(IN_SW2);
  mx1_sw3 = (bool) digitalRead(IN_SW3);
  digitalWrite(OUT_MX1, LOW);

  delayMicroseconds(SWITCH_READ_DELAY);
  digitalWrite(OUT_MX2, HIGH);
  delayMicroseconds(SWITCH_READ_DELAY);
  mx2_sw0 = (bool) digitalRead(IN_SW0);
  mx2_sw1 = (bool) digitalRead(IN_SW1);
  mx2_sw2 = (bool) digitalRead(IN_SW2);
  mx2_sw3 = (bool) digitalRead(IN_SW3);
  digitalWrite(OUT_MX2, LOW);

  delayMicroseconds(SWITCH_READ_DELAY);
  digitalWrite(OUT_MX3, HIGH);
  delayMicroseconds(SWITCH_READ_DELAY);
  mx3_sw0 = (bool) digitalRead(IN_SW0);
  mx3_sw1 = (bool) digitalRead(IN_SW1);
  mx3_sw2 = (bool) digitalRead(IN_SW2);
  mx3_sw3 = (bool) digitalRead(IN_SW3);
  digitalWrite(OUT_MX3, LOW);

  delayMicroseconds(SWITCH_READ_DELAY);
  digitalWrite(OUT_MX4, HIGH);
  delayMicroseconds(SWITCH_READ_DELAY);
  mx4_sw0 = (bool) digitalRead(IN_SW0);
  mx4_sw1 = (bool) digitalRead(IN_SW1);
  mx4_sw2 = (bool) digitalRead(IN_SW2);
  mx4_sw3 = (bool) digitalRead(IN_SW3);
  digitalWrite(OUT_MX4, LOW);

  delayMicroseconds(SWITCH_READ_DELAY);
  digitalWrite(OUT_MX5, HIGH);
  delayMicroseconds(SWITCH_READ_DELAY);
  mx5_sw0 = (bool) digitalRead(IN_SW0);
  mx5_sw1 = (bool) digitalRead(IN_SW1);
  mx5_sw2 = (bool) digitalRead(IN_SW2);
  mx5_sw3 = (bool) digitalRead(IN_SW3);
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

  Serial.print("mx0_sw0 = ");
  Serial.print(mx0_sw0);
  Serial.println();

  Serial.print("mx0_sw1 = ");
  Serial.print(mx0_sw1);
  Serial.println();

  Serial.print("mx0_sw2 = ");
  Serial.print(mx0_sw2);
  Serial.println();

  Serial.print("mx0_sw3 = ");
  Serial.print(mx0_sw3);
  Serial.println();

  Serial.print("mx1_sw0 = ");
  Serial.print(mx1_sw0);
  Serial.println();

  Serial.print("mx1_sw1 = ");
  Serial.print(mx1_sw1);
  Serial.println();

  Serial.print("mx1_sw2 = ");
  Serial.print(mx1_sw2);
  Serial.println();

  Serial.print("mx1_sw3 = ");
  Serial.print(mx1_sw3);
  Serial.println();

  Serial.print("mx2_sw0 = ");
  Serial.print(mx2_sw0);
  Serial.println();

  Serial.print("mx2_sw1 = ");
  Serial.print(mx2_sw1);
  Serial.println();

  Serial.print("mx2_sw2 = ");
  Serial.print(mx2_sw2);
  Serial.println();

  Serial.print("mx2_sw3 = ");
  Serial.print(mx2_sw3);
  Serial.println();

  Serial.print("mx3_sw0 = ");
  Serial.print(mx3_sw0);
  Serial.println();

  Serial.print("mx3_sw1 = ");
  Serial.print(mx3_sw1);
  Serial.println();

  Serial.print("mx3_sw2 = ");
  Serial.print(mx3_sw2);
  Serial.println();

  Serial.print("mx3_sw3 = ");
  Serial.print(mx3_sw3);
  Serial.println();

  Serial.print("mx4_sw0 = ");
  Serial.print(mx4_sw0);
  Serial.println();

  Serial.print("mx4_sw1 = ");
  Serial.print(mx4_sw1);
  Serial.println();

  Serial.print("mx4_sw2 = ");
  Serial.print(mx4_sw2);
  Serial.println();

  Serial.print("mx4_sw3 = ");
  Serial.print(mx4_sw3);
  Serial.println();

  Serial.print("mx5_sw0 = ");
  Serial.print(mx5_sw0);
  Serial.println();

  Serial.print("mx5_sw1 = ");
  Serial.print(mx5_sw1);
  Serial.println();

  Serial.print("mx5_sw2 = ");
  Serial.print(mx5_sw2);
  Serial.println();

  Serial.print("mx5_sw3 = ");
  Serial.print(mx5_sw3);
  Serial.println();

  Serial.println("--------------------------------------------------------");
  Serial.println();

}

