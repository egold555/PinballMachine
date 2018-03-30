
const int DELAY = 250;
const int LED = 13;

const int OUT_SLINGSHOT_LEFT = 2;
const int OUT_SLINGSHOT_RIGHT = 3;
const int OUT_THUMPER_LEFT = 4;
const int OUT_THUMPER_RIGHT = 5;
const int OUT_BALL_RETUN = 6;

void setup() {          
  pinMode(LED, OUTPUT);      
  pinMode(OUT_SLINGSHOT_LEFT, OUTPUT);
  pinMode(OUT_SLINGSHOT_RIGHT, OUTPUT);
  pinMode(OUT_THUMPER_LEFT, OUTPUT);
  pinMode(OUT_THUMPER_RIGHT, OUTPUT);
  pinMode(OUT_BALL_RETUN, OUTPUT);
}

void loop() {
  
  pulse(OUT_SLINGSHOT_LEFT);
  pulse(OUT_SLINGSHOT_RIGHT);
  pulse(OUT_THUMPER_LEFT);
  pulse(OUT_THUMPER_RIGHT);
  pulse(OUT_BALL_RETUN);
  
}

void pulse(int thing){
  digitalWrite(thing, HIGH);
  digitalWrite(LED, HIGH);
  delay(DELAY);
  digitalWrite(thing, LOW);
  digitalWrite(LED, LOW);
  delay(DELAY);
}

