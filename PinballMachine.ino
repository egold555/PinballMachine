
const int DELAY = 80;
const int LED = 13;

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
const int OUT_MX6 = 34;
const int OUT_MX7 = 35;

const int IN_SW0 = 36;
const int IN_SW1 = 37;
const int IN_SW2 = 38;
const int IN_SW3 = 39;


bool leftSlingshotSwitch = false;

void setup() {          
  pinMode(LED, OUTPUT);      
  pinMode(OUT_SLINGSHOT_LEFT, OUTPUT);
  pinMode(OUT_SLINGSHOT_RIGHT, OUTPUT);
  pinMode(OUT_THUMPER_LEFT, OUTPUT);
  pinMode(OUT_THUMPER_RIGHT, OUTPUT);
  pinMode(OUT_BALL_RETUN, OUTPUT);

  pinMode(OUT_MX1, OUTPUT);

  pinMode(IN_SW3, INPUT);
  
}

void loop() {
  pulse(OUT_SLINGSHOT_LEFT);
  pulse(OUT_SLINGSHOT_RIGHT);
  pulse(OUT_THUMPER_LEFT);
  pulse(OUT_THUMPER_RIGHT);
  pulse(OUT_BALL_RETUN);
  delay(3000);
 // checkSwitches();
  //triggerSolinoids();
}

void checkSwitches(){
  digitalWrite(OUT_MX1, HIGH);
  delayMicroseconds(50);
  leftSlingshotSwitch = (bool) digitalRead(IN_SW3);
  digitalWrite(OUT_MX1, LOW);
}

void triggerSolinoids(){
  if(leftSlingshotSwitch){
    pulse(OUT_SLINGSHOT_LEFT);
  }
}

void pulse(int thing){
  digitalWrite(thing, HIGH);
  digitalWrite(LED, HIGH);
  delay(DELAY);
  digitalWrite(thing, LOW);
  digitalWrite(LED, LOW);
  delay(DELAY);
}
