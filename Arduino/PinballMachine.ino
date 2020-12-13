/**@file PinballMachine.ino */
//in VS CODE, set '"C_Cpp.intelliSenseEngine": "Tag Parser",' in the settings json file in appdata
//https://github.com/microsoft/vscode-arduino/issues/438#issuecomment-343820369

//imports
#include "Playtune.h"             //Music http://blog.bentgeorge.com/?p=119
#include "Adafruit_LEDBackpack.h" //Display
#include "long2text.h"            //Peter
#include "run.h"                  //Modified version of https://github.com/billroy/run
//#include <EEPROM.h> //not used currently

#include "SFXAndMusic.h"
#include "ProtocolButtons.h"
#include "ProtocolLights.h"
#include "ProtocolSolinoids.h"
#include "ProtocolSounds.h"
#include "ProtocolSoundData.h"

//Constants for delays ahs such.
//TODO: DOcument more
const int SOLENOID_DELAY = 50;     //60
const int SWITCH_READ_DELAY = 800; //680
const int ANALOG_THRESHOLD = 200;
const int SPINNER_SCORE_DELAY = 75;
const unsigned int DEBOUNCE_DELAY = 20;

//Constants for pins
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
const int OUT_MX6 = 48;
const int OUT_MX7 = 50;
const int OUT_MX8 = 52;

const int OUT_LT0 = 36;
const int OUT_LT1 = 37;
const int OUT_LT2 = 38;
const int OUT_LT3 = 39;

const int IN_SW0 = A12;
const int IN_SW1 = A13;
const int IN_SW2 = A14;
const int IN_SW3 = A15;

const int IN_FLIPPER_LEFT = 2;
const int IN_FLIPPER_RIGHT = 3;

const int NOTE_1 = 44;
const int NOTE_2 = 45;
const int NOTE_3 = 46;

//defines a button on the pinball machine
typedef struct Button
{

  Button(int id) : id(id) {}

  int id;
  boolean sw = false; //Switch
  boolean swLast = sw;
  boolean pr = false; //Pressed
  long tm = 0;        //Time

  void updateBTN()
  {
    if (pr)
    {
      sendToSerialPR();
    }
    if (sw != swLast)
    {
      sendToSerialSW();
      swLast = sw;
    }
  }

  void sendToSerialSW()
  {

    Serial.print("BT-");
    Serial.print(id);
    Serial.print("-");
    Serial.print(sw);
    Serial.println();
  }

  void sendToSerialPR()
  {
    Serial.print("BP-");
    Serial.print(id);
    Serial.println();
  }

  /**
   * magical debouncing code that works and should never be touched
   * 
   * @param pin pin to debounce
   */
  void debounce(int pin)
  {
    bool oldSw = sw;
    bool newSw;
    newSw = analogRead(pin) > ANALOG_THRESHOLD;
    sw = newSw;

    pr = false;
    if (newSw && !oldSw)
    {
      unsigned long currtm = millis();
      if (currtm - tm > DEBOUNCE_DELAY)
      {
        pr = true;
      }
      tm = currtm;
    }
    updateBTN();
  }

  /**
    * magical debouncing code that works and should never be touched
    * 
    * @param pin pin to debounce
    */
  void debounceDigital(int pin)
  {
    bool oldSw = sw;
    bool newSw;
    newSw = digitalRead(pin) == LOW;
    sw = newSw;

    pr = false;
    if (newSw && !oldSw)
    {
      unsigned long currtm = millis();
      if (currtm - tm > DEBOUNCE_DELAY)
      {
        pr = true;
      }
      tm = currtm;
    }
    updateBTN();
  }

} Button;

//Every button on the pinball machine
Button swBallReturn(PID_SW_BALL_RETURN);
Button swTilt(PID_SW_TILT);
Button swRightSpinner(PID_SW_RIGHT_SPINNER);
Button swRightExtraBallLane(PID_SW_RIGHT_EXTRA_BALL_LANE);
Button swA(PID_SW_A);
Button swStart(PID_SW_START);
Button swLeftTarget(PID_SW_LEFT_TARGET);
Button swLeftSlingShot(PID_SW_LEFT_SLING_SHOT);
Button swB(PID_SW_B);
Button swLeftThumperBumper(PID_SW_LEFT_THUMPER_BUMPER);
Button swLeftBumper(PID_SW_LEFT_BUMPER);
Button swLeftExtraBallLane(PID_SW_LEFT_EXTRA_BALL_LANE);
Button swC(PID_SW_C);
Button swRightThumperBumper(PID_SW_RIGHT_THUMPER_BUMPER);
Button swLeftSpinner(PID_SW_LEFT_SPINNER);
Button swLeftAdvanceLane(PID_SW_LEFT_ADVANCED_LANE);
Button swD(PID_SW_D);
//Button mx4_sw1;
Button swRightTarget(PID_SW_RIGHT_TARGET);
Button swRightSlingShot(PID_SW_RIGHT_SLING_SHOT);
Button swCenterTarget(PID_SW_CENTER_TARGET);
//Button mx5_sw1;
Button swRightBumper(PID_SW_RIGHT_BUMPER);
Button swRightAdvanceLane(PID_SW_RIGHT_ADVANCED_LANE);
Button swFlipperLeft(PID_SW_FLIPPER_LEFT);
Button swFlipperRight(PID_SW_FLIPPER_RIGHT);

//all the lights on the pinball machine
bool mx0_lt0 = false; //NONE
bool ltBonus8000 = false;
bool ltBonus9000 = false;
bool ltExtraBallRight = false;
bool ltA = true;
bool ltBonus6000 = false;
bool lt1 = true;
bool ltBonus1000 = false;
bool ltB = true;
bool ltBonus5000 = false;
bool ltDoubleBonus = false;
bool ltExtraBallLeft = false;
bool ltC = true;
bool ltBonus7000 = false;
bool ltBonus10000 = false;
bool ltBonus2000 = false;
bool ltD = true;
bool ltBonus3000 = false;
bool lt2 = true;
bool ltSamePlayerShoots = false;
bool lt3 = true;
bool ltBonus4000 = false;
bool ltTrippleBonus = false;
bool mx5_lt3 = false;   //NONE
bool ltBall2 = false;   //b2
bool ltBall3 = false;   //b3
bool ltBall4 = false;   //b4
bool ltBall5 = false;   //b5
bool ltPlayer1 = false; //p1
bool ltPlayer2 = false; //p2
bool ltPlayer3 = false; //p3
bool ltPlayer4 = false; //p4
bool mx8_lt0 = false;
bool mx8_lt1 = false;
bool mx8_lt2 = false;
bool ltBall1 = true; //b1

//The matrix display
Adafruit_AlphaNum4 display1 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 display2 = Adafruit_AlphaNum4();

//Music player
Playtune pt;

//Decare all functions
void setup(void);
void loop(void);
void scrollText(String msg);
void checkSwitchesAndLightLights(void);
void writeScore(long score);
void writeScore(long score, bool sound);
void playSFX(byte *sfx);
void writeDisplay(long num);
void writeDisplay(String msg);
void writeDisplay(int place, char in);
void updateDisplay(void);
void clearDisplay(void);
void incomingLightMessage(int light, boolean status);
void incomingMessage(const char *message);
void checkForIncomingMessages();
void updateSolinoids();
void incomingSolinoidMessage(int solinoid);
void incomingSoundPlayMessage(int soundNumber);
void incomingSoundDataMessage(int soundData);
void sendSoundFeedback(int id);
void checkForOutgoingMessages();

void setup()
{

  Serial.begin(57600);

  //init al the matrix's and the misc outputs
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
  pinMode(OUT_MX6, OUTPUT);
  pinMode(OUT_MX7, OUTPUT);
  pinMode(OUT_MX8, OUTPUT);

  pinMode(OUT_LT0, OUTPUT);
  pinMode(OUT_LT1, OUTPUT);
  pinMode(OUT_LT2, OUTPUT);
  pinMode(OUT_LT3, OUTPUT);

  //inputs
  pinMode(IN_FLIPPER_LEFT, INPUT_PULLUP);
  pinMode(IN_FLIPPER_RIGHT, INPUT_PULLUP);

  pinMode(IN_SW0, INPUT);
  pinMode(IN_SW1, INPUT);
  pinMode(IN_SW2, INPUT);
  pinMode(IN_SW3, INPUT);

  //init music generators voices's
  pt.tune_initchan(NOTE_1);
  pt.tune_initchan(NOTE_2);
  pt.tune_initchan(NOTE_3);

  //init the two displays
  display1.begin(0x70);
  display2.begin(0x71);

  //delay(200);
  writeDisplay("DEBUG");
  
  Serial.println("LG-This message is to get rid of junk");
  delay(200);
  Serial.println("LG-Ready");
}

/**
 * Arduino loop
 */
void loop()
{
  //runner();
  checkSwitchesAndLightLights();
  checkForIncomingMessages();
  updateSolinoids();
  checkForOutgoingMessages();
}

unsigned long timeRetractLeftSlingshot = 0, timeRetractRightSlingshot = 0;
unsigned long timeRetractLeftThumperBumper = 0, timeRetractRightThumperBumper = 0;
unsigned long timeRetractBallReturn = 0;
unsigned long timeNextSpinnerScore = 0;
bool snBallReturn = false;
bool snLeftSlingShot = false;
bool snRightSlingShot = false;
bool snLeftThumperBumper = false;
bool snRightThumperBumper = false;
void updateSolinoids()
{
  unsigned long currentTime = millis();
  if (snBallReturn)
  {
    digitalWrite(OUT_BALL_RETURN, HIGH);
    timeRetractBallReturn = currentTime + 100;
    snBallReturn = false;
  }
  // Fire solenoids that should be fired.
  if (snLeftSlingShot)
  {
    digitalWrite(OUT_SLINGSHOT_LEFT, HIGH);
    timeRetractLeftSlingshot = currentTime + SOLENOID_DELAY;
    snLeftSlingShot = false;
  }

  if (snRightSlingShot)
  {
    digitalWrite(OUT_SLINGSHOT_RIGHT, HIGH);
    timeRetractRightSlingshot = currentTime + SOLENOID_DELAY;
    snRightSlingShot = false;
  }

  if (snLeftThumperBumper)
  {
    digitalWrite(OUT_THUMPER_LEFT, HIGH);
    timeRetractLeftThumperBumper = currentTime + SOLENOID_DELAY;
    snLeftThumperBumper = false;
  }

  if (snRightThumperBumper)
  {
    digitalWrite(OUT_THUMPER_RIGHT, HIGH);
    timeRetractRightThumperBumper = currentTime + SOLENOID_DELAY;
    snRightThumperBumper = false;
  }

  // Retract solenoids that should be retracted.
  if (currentTime > timeRetractLeftSlingshot)
  {
    digitalWrite(OUT_SLINGSHOT_LEFT, LOW);
  }

  if (currentTime > timeRetractRightSlingshot)
  {
    digitalWrite(OUT_SLINGSHOT_RIGHT, LOW);
  }

  if (currentTime > timeRetractLeftThumperBumper)
  {
    digitalWrite(OUT_THUMPER_LEFT, LOW);
  }

  if (currentTime > timeRetractRightThumperBumper)
  {
    digitalWrite(OUT_THUMPER_RIGHT, LOW);
  }

  if (currentTime > timeRetractBallReturn)
  {
    digitalWrite(OUT_BALL_RETURN, LOW);
  }
}

/*
Check for incoming serial messages about the lights
*/

const int serialBufferSize = 128;
char serialBuffer[serialBufferSize];
int serialIndex = 0;

// An incoming command has appeared.
void incomingMessage(const char *message)
{
  //Lights
  if (message[0] == 'L' && message[1] == 'T') {
    // LT-nn-0 or LT-nn-1: Turn light on or off.

    if (message[2] != '-')
    {
      return; // bad command.
    }

    // Always a two digit number
    if (!(message[3] >= '0' && message[3] <= '9' && message[4] >= '0' && message[5] <= '9'))
    {
      return; // bad command.
    }
    int lightNumber = 10 * (message[3] - '0') + (message[4] - '0');

    if (message[5] != '-')
    {
      return; // bad command
    }

    if (message[6] == '0')
    {
      incomingLightMessage(lightNumber, false);
    }
    else if (message[6] == '1')
    {
      incomingLightMessage(lightNumber, true);
    }

  }
  //Solinoid
  else if (message[0] == 'S' && message[1] == 'N') {
    // S-nn
    if (message[2] != '-')
    {
      return; // bad command.
    }

    // Always a two digit number
    if (!(message[3] >= '0' && message[3] <= '9' && message[4] >= '0' && message[5] <= '9'))
    {
      return; // bad command.
    }
    int solinoid = 10 * (message[3] - '0') + (message[4] - '0');
    
    incomingSolinoidMessage(solinoid);
  }
  //Display Score
  else if (message[0] == 'D' && message[1] == 'T') {
    // S-msg
    //writeDisplay(message);
    if (message[2] != '-')
    {
      return; // bad command.
    }
     writeDisplay(String(message).substring(3));
  }
  //sound
  else if (message[0] == 'S' && message[1] == 'P') {
    // SP-nn

    if (message[2] != '-')
    {
      return; // bad command.
    }

    // Always a two digit number
    if (!(message[3] >= '0' && message[3] <= '9' && message[4] >= '0' && message[5] <= '9'))
    {
      return; // bad command.
    }
    int soundNumber = 10 * (message[3] - '0') + (message[4] - '0');

    incomingSoundPlayMessage(soundNumber);

  }
  //Sound data messages
  else if (message[0] == 'S' && message[1] == 'D') {
    // SD-nn

    if (message[2] != '-')
    {
      return; // bad command.
    }

    // Always a two digit number
    if (!(message[3] >= '0' && message[3] <= '9' && message[4] >= '0' && message[5] <= '9'))
    {
      return; // bad command.
    }
    int soundData = 10 * (message[3] - '0') + (message[4] - '0');

    incomingSoundDataMessage(soundData);

  }
  else {
    Serial.print("ER-Got Unknown incoming message: ");
    Serial.println(message);
  }
}

void incomingSolinoidMessage(int solinoid)
{
  switch (solinoid)
  {
  case PID_SN_BALL_RETURN:
    snBallReturn = true;
    break;
  case PID_SN_LEFT_SLING_SHOT:
    snLeftSlingShot = true;
    break;
  case PID_SN_LEFT_THUMPER_BUMPER:
    snLeftThumperBumper = true;
    break;
  case PID_SN_RIGHT_SLING_SHOT:
    snRightSlingShot = true;
    break;
  case PID_SN_RIGHT_THUMPER_BUMPER:
    snRightThumperBumper = true;
    break;
  default:
    Serial.print("ER-Unknown Solinoid sent: ");
    Serial.println(solinoid);
    break;
  }
}

//used to toggle lights based off of a incoming status message
void incomingLightMessage(int light, boolean status)
{

  switch (light)
  {
  case PID_LT_TARGET_LEFT:
    lt1 = status;
    break;
  case PID_LT_TARGET_RIGHT:
    lt2 = status;
    break;
  case PID_LT_TARGET_CENTER:
    lt3 = status;
    break;
  case PID_LT_A:
    ltA = status;
    break;
  case PID_LT_B:
    ltB = status;
    break;
  case PID_LT_BALL_1:
    ltBall1 = status;
    break;
  case PID_LT_BALL_2:
    ltBall2 = status;
    break;
  case PID_LT_BALL_3:
    ltBall3 = status;
    break;
  case PID_LT_BALL_4:
    ltBall4 = status;
    break;
  case PID_LT_BALL_5:
    ltBall5 = status;
    break;
  case PID_LT_BONUS_1000:
    ltBonus1000 = status;
    break;
  case PID_LT_BONUS_2000:
    ltBonus2000 = status;
    break;
  case PID_LT_BONUS_3000:
    ltBonus3000 = status;
    break;
  case PID_LT_BONUS_4000:
    ltBonus4000 = status;
    break;
  case PID_LT_BONUS_5000:
    ltBonus5000 = status;
    break;
  case PID_LT_BONUS_6000:
    ltBonus6000 = status;
    break;
  case PID_LT_BONUS_7000:
    ltBonus7000 = status;
    break;
  case PID_LT_BONUS_8000:
    ltBonus8000 = status;
    break;
  case PID_LT_BONUS_9000:
    ltBonus9000 = status;
    break;
  case PID_LT_BONUS_10000:
    ltBonus10000 = status;
    break;
  case PID_LT_C:
    ltC = status;
    break;
  case PID_LT_D:
    ltD = status;
    break;
  case PID_LT_DOUBLE_BONUS:
    ltDoubleBonus = status;
    break;
  case PID_LT_EXTRA_BALL_LEFT:
    ltExtraBallLeft = status;
    break;
  case PID_LT_EXTRA_BALL_RIGHT:
    ltExtraBallRight = status;
    break;
  case PID_LT_PLAYER_1:
    ltPlayer1 = status;
    break;
  case PID_LT_PLAYER_2:
    ltPlayer2 = status;
    break;
  case PID_LT_PLAYER_3:
    ltPlayer3 = status;
    break;
  case PID_LT_PLAYER_4:
    ltPlayer4 = status;
    break;
  case PID_LT_SAME_PLAYER_SHOOTS:
    ltSamePlayerShoots = status;
    break;
  case PID_LT_TRIPPLE_BONUS:
    ltTrippleBonus = status;
    break;
  default:
    Serial.print("ER-Unknown Light Sent: ");
    Serial.println(light);
    break;
  }
}

//Sound messages
void incomingSoundPlayMessage(int soundNumber) {
    switch(soundNumber) {
      case PID_SP_ENDING_SONG: playSFX(SOUND_ENDING_SONG); break;
      case PID_SP_EXTRA_BALL: playSFX(SOUND_EXTRABALL); break;
      case PID_SP_NEW_BALL: playSFX(SOUND_NEWBALL); break;
      case PID_SP_POINT: playSFX(SOUND_POINT); break;
      case PID_SP_STARTUP: playSFX(SOUND_STARTUP); break;
      case PID_SP_TILT: playSFX(SOUND_TILT); break;
      case PID_SP_NAME_ENTER: playSFX(SOUND_NAME_ENTER); break;
      default: 
        Serial.print("ER-Unknown sound sent: ");
        Serial.println(soundNumber);
        break;
    }
}

void sendSoundFeedback(int id) {
  Serial.print("SD-");
  Serial.println(id);
}

//Sound data messages
void incomingSoundDataMessage(int soundData) {
  switch(soundData) {
    case PID_SD_STOP: pt.tune_stopscore(); break;
    default: 
        Serial.print("ER-Unknown sound data sent: ");
        Serial.println(soundData);
        break;
  }
}

// Read characters from the serial port and store into
// a buffer. If we get a newline, dispatch the stored command
// to incomingMessage();
void checkForIncomingMessages()
{
  while (Serial.available() > 0)
  {
    int b = Serial.read();
    if (b == '\n')
    {
      // Command terminator.
      serialBuffer[serialIndex++] = '\0'; // terminate string.
      incomingMessage(serialBuffer);
      serialIndex = 0;
    }
    else if (b > 0)
    {
      // Add to end of command.
      if (serialIndex >= serialBufferSize - 1)
      {
        // Buffer overflow -- should never happen. Just ignore stuff.
        serialIndex = 0;
      }
      serialBuffer[serialIndex++] = (char)b;
    }
  }
}

bool lastPlayTune;
void checkForOutgoingMessages() {

  //check for when a sound is playing and not playing.
   
  if(pt.tune_playing && !lastPlayTune) {
      lastPlayTune = true;
  }
  if(!pt.tune_playing && lastPlayTune) {
    lastPlayTune = false;
    sendSoundFeedback(PID_SD_FEEDBACK_STOPPED);
  }
}

long MSG_DELAY = 20;
long msgCount = 0;
int msgPos = 0;

/**
 * Scroll text accross the displadisplay
 * 
 * @param msg Text to display
 */
void scrollText(String msg)
{
  int len = msg.length();
  if ((msgCount % MSG_DELAY) == 0)
  {
    clearDisplay();
    writeDisplay(0, msg.charAt((msgPos) % len));
    writeDisplay(1, msg.charAt((msgPos + 1) % len));
    writeDisplay(2, msg.charAt((msgPos + 2) % len));
    writeDisplay(3, msg.charAt((msgPos + 3) % len));
    writeDisplay(4, msg.charAt((msgPos + 4) % len));
    writeDisplay(5, msg.charAt((msgPos + 5) % len));
    writeDisplay(6, msg.charAt((msgPos + 6) % len));
    writeDisplay(7, msg.charAt((msgPos + 7) % len));
    updateDisplay();

    msgPos++;
    if (msgPos == len)
    {
      msgPos = 0;
    }
  }

  msgCount++;
}

/**
 * Check and update the giant matrix's for all the lights and buttons
 * 
 */
void checkSwitchesAndLightLights()
{

  digitalWrite(OUT_MX0, HIGH);
  digitalWrite(OUT_LT0, mx0_lt0);
  digitalWrite(OUT_LT1, ltBonus8000);
  digitalWrite(OUT_LT2, ltBonus9000);
  digitalWrite(OUT_LT3, ltExtraBallRight);

  delayMicroseconds(SWITCH_READ_DELAY);

  swBallReturn.debounce(IN_SW0);
  swTilt.debounce(IN_SW1);
  swRightSpinner.debounce(IN_SW2);
  swRightExtraBallLane.debounce(IN_SW3);

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX0, LOW);

  ///////////////////////////////

  digitalWrite(OUT_MX1, HIGH);

  digitalWrite(OUT_LT0, ltA);
  digitalWrite(OUT_LT1, ltBonus6000);
  digitalWrite(OUT_LT2, lt1);
  digitalWrite(OUT_LT3, ltBonus1000);

  delayMicroseconds(SWITCH_READ_DELAY);

  swA.debounce(IN_SW0);
  swStart.debounce(IN_SW1);
  swLeftTarget.debounce(IN_SW2);
  swLeftSlingShot.debounce(IN_SW3);

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX1, LOW);

  //////////////////

  digitalWrite(OUT_MX2, HIGH);

  digitalWrite(OUT_LT0, ltB);
  digitalWrite(OUT_LT1, ltBonus5000);
  digitalWrite(OUT_LT2, ltDoubleBonus);
  digitalWrite(OUT_LT3, ltExtraBallLeft);

  delayMicroseconds(SWITCH_READ_DELAY);

  swB.debounce(IN_SW0);
  swLeftThumperBumper.debounce(IN_SW1);
  swLeftBumper.debounce(IN_SW2);
  swLeftExtraBallLane.debounce(IN_SW3);

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX2, LOW);

  /////////////////////

  digitalWrite(OUT_MX3, HIGH);

  digitalWrite(OUT_LT0, ltC);
  digitalWrite(OUT_LT1, ltBonus7000);
  digitalWrite(OUT_LT2, ltBonus10000);
  digitalWrite(OUT_LT3, ltBonus2000);

  delayMicroseconds(SWITCH_READ_DELAY);

  swC.debounce(IN_SW0);
  swRightThumperBumper.debounce(IN_SW1);
  swLeftSpinner.debounce(IN_SW2);
  swLeftAdvanceLane.debounce(IN_SW3);

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX3, LOW);

  /////////////////////////////

  digitalWrite(OUT_MX4, HIGH);

  digitalWrite(OUT_LT0, ltD);
  digitalWrite(OUT_LT1, ltBonus3000);
  digitalWrite(OUT_LT2, lt2);
  digitalWrite(OUT_LT3, ltSamePlayerShoots);

  delayMicroseconds(SWITCH_READ_DELAY);

  swD.debounce(IN_SW0);
  //mx4_sw1.debounce(IN_SW1);
  swRightTarget.debounce(IN_SW2);
  swRightSlingShot.debounce(IN_SW3);

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX4, LOW);

  ///////////////////////////////

  digitalWrite(OUT_MX5, HIGH);

  digitalWrite(OUT_LT0, lt3);
  digitalWrite(OUT_LT1, ltBonus4000);
  digitalWrite(OUT_LT2, ltTrippleBonus);
  digitalWrite(OUT_LT3, mx5_lt3);

  delayMicroseconds(SWITCH_READ_DELAY);

  swCenterTarget.debounce(IN_SW0);
  //mx5_sw1.debounce(IN_SW1);
  swRightBumper.debounce(IN_SW2);
  swRightAdvanceLane.debounce(IN_SW3);

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX5, LOW);

  ///////////////////////////////

  digitalWrite(OUT_MX6, HIGH);

  digitalWrite(OUT_LT0, ltBall2);
  digitalWrite(OUT_LT1, ltBall3);
  digitalWrite(OUT_LT2, ltBall4);
  digitalWrite(OUT_LT3, ltBall5);

  delayMicroseconds(SWITCH_READ_DELAY);

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX6, LOW);

  ///////////////////////////////

  digitalWrite(OUT_MX7, HIGH);

  digitalWrite(OUT_LT0, ltPlayer1);
  digitalWrite(OUT_LT1, ltPlayer2);
  digitalWrite(OUT_LT2, ltPlayer3);
  digitalWrite(OUT_LT3, ltPlayer4);

  delayMicroseconds(SWITCH_READ_DELAY);

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX7, LOW);

  ///////////////////////////////

  digitalWrite(OUT_MX8, HIGH);

  digitalWrite(OUT_LT0, mx8_lt0);
  digitalWrite(OUT_LT1, mx8_lt1);
  digitalWrite(OUT_LT2, mx8_lt2);
  digitalWrite(OUT_LT3, ltBall1);

  delayMicroseconds(SWITCH_READ_DELAY);

  digitalWrite(OUT_LT0, LOW);
  digitalWrite(OUT_LT1, LOW);
  digitalWrite(OUT_LT2, LOW);
  digitalWrite(OUT_LT3, LOW);

  digitalWrite(OUT_MX8, LOW);

  swFlipperLeft.debounceDigital(IN_FLIPPER_LEFT);
  swFlipperRight.debounceDigital(IN_FLIPPER_RIGHT);
}

/**
 * Writes the score to the screen and plays the score sound
 * 
 * @param score 
 */
void writeScore(long score)
{
  writeScore(score, true);
}

/**
 * Writes the score to the screen
 * 
 * @param score players score
 * @param sound should we play the score sound?
 */
void writeScore(long score, bool sound)
{
  writeDisplay(score);
  if (sound)
  {
    if (pt.tune_playing)
    {
      pt.tune_stopscore();
    }
    playSFX(SOUND_POINT);
  }
}

/**
 * Play the music / SFX music. Defined in the arrays above
 * 
 * @param sfx Array of music to play
 */
void playSFX(byte *sfx)
{
  if (!pt.tune_playing)
  {
    pt.tune_playscore(sfx);
  }
}

/**
 * Write a long to the display
 * 
 * @param num number to display on screen
 */
void writeDisplay(long num)
{
  char buffer[9];
  long2text(num, buffer);
  writeDisplay(buffer);
}

/**
 * Writes a string to the display(s)
 * 
 * @param msg String to write
 */
void writeDisplay(String msg)
{
  clearDisplay(); //Not sure if I need to clear every time, Long run?
  for (int i = 0; i < 8; i++)
  {
    if (i < msg.length())
    {
      writeDisplay(i, msg.charAt(i));
    }
  }
  updateDisplay(); //Just like clear, should we auto update in long run?
}

/**
 * Writes a character to a certian place on the display(s)
 * 
 * @param place Where to write it 0-7. Each display has 4 characters
 * @param in Character to display. Displays all ASCII and some unicode characters
 */
void writeDisplay(int place, char in)
{
  if (isprint(in))
  {
    if (place > 3)
    {
      display2.writeDigitAscii(place - 4, in);
    }
    else
    {
      display1.writeDigitAscii(place, in);
    }
  }
}

/**
 * Updates both display units as one display
 * 
 */
void updateDisplay()
{
  display1.writeDisplay();
  display2.writeDisplay();
}

/**
 * Clears both display units as one display
 * 
 */
void clearDisplay()
{
  display1.clear();
  display2.clear();
  updateDisplay();
}
