#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "pitches.h"

Adafruit_7segment timer = Adafruit_7segment();


const int fourBitAdderAddress = 1;
const int wiresAddress = 2;
const int morseCodeAddress = 3;
const int scrambledWordAddress = 4;
const int mazeAddress = 5;
const int rngAddress = 6;

const int startMinute = 5;
const int startSecond = 0;
int timerInterval = 1000;
const int brightness = 15;
const int beepNote = NOTE_G6;
const int beepDuration = 100;

const int buzzerPin = 8;
const int strikesLED1Pin = 5;
const int strikesLED2Pin = 6;
const int strikesLED3Pin = 7;
const int hintLED1Pin = 9; // blue
const int hintLED2Pin = 10; // yellow
const int hintLED3Pin = 11; // white
const int hintLED4Pin = 12; // green
const int resetButtonPin = 13;
const int addTimeButtonPin = 4;
const int removeTimeButtonPin = 3;

bool menuSelect;
bool addTimeButtonPressed;
bool removeTimeButtonPressed;
bool resetButtonPressed;


const int quarterNoteDuration = 500;

int loseMelodyLength = 28;
int gameOverMelody[] = {NOTE_FS5, NOTE_F5, NOTE_DS5, NOTE_CS5, NOTE_DS5, NOTE_AS4, NOTE_C5, NOTE_GS4, NOTE_DS5, NOTE_F5, NOTE_FS5, NOTE_GS5, NOTE_CS6, NOTE_AS5, 
                        NOTE_FS5, NOTE_F5, NOTE_DS5, NOTE_CS5, NOTE_DS5, NOTE_AS4, NOTE_C5, NOTE_GS4, NOTE_DS4, NOTE_F4, NOTE_FS4, NOTE_F4, NOTE_CS4, NOTE_DS4};
int gameOverDurations[] = {quarterNoteDuration/2, quarterNoteDuration/2, quarterNoteDuration/2, quarterNoteDuration/2, quarterNoteDuration/2, quarterNoteDuration/2, quarterNoteDuration, quarterNoteDuration, 
                           quarterNoteDuration/2, quarterNoteDuration/2, quarterNoteDuration, quarterNoteDuration, quarterNoteDuration, quarterNoteDuration*3,
                           quarterNoteDuration/2, quarterNoteDuration/2, quarterNoteDuration/2, quarterNoteDuration/2, quarterNoteDuration/2, quarterNoteDuration/2, quarterNoteDuration, quarterNoteDuration, 
                           quarterNoteDuration/2, quarterNoteDuration/2, quarterNoteDuration, quarterNoteDuration, quarterNoteDuration, quarterNoteDuration*3};


int winMelodyLength = 27;
int victoryMelody[] = {NOTE_G3, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_E5, NOTE_G5, NOTE_E5,
                       NOTE_GS3, NOTE_C4, NOTE_DS4, NOTE_GS4, NOTE_C5, NOTE_DS5, NOTE_GS5, NOTE_DS5,
                       NOTE_AS3, NOTE_D4, NOTE_F4, NOTE_AS4, NOTE_D5, NOTE_F5, NOTE_AS5, NOTE_AS5, NOTE_AS5, NOTE_AS5, NOTE_C6};
int victoryDurations[] = {quarterNoteDuration/4, quarterNoteDuration/4, quarterNoteDuration/4, quarterNoteDuration/4, quarterNoteDuration/4, quarterNoteDuration/4, quarterNoteDuration*3/4, quarterNoteDuration*3/4,
                          quarterNoteDuration/4, quarterNoteDuration/4, quarterNoteDuration/4, quarterNoteDuration/4, quarterNoteDuration/4, quarterNoteDuration/4, quarterNoteDuration*3/4, quarterNoteDuration*3/4,
                          quarterNoteDuration/4, quarterNoteDuration/4, quarterNoteDuration/4, quarterNoteDuration/4, quarterNoteDuration/4, quarterNoteDuration/4, quarterNoteDuration*3/4, 
                          quarterNoteDuration/4, quarterNoteDuration/4, quarterNoteDuration/4, quarterNoteDuration*3/2};


int minutes = startMinute;
int seconds = startSecond;
int timerLoop = timerInterval;


boolean gameOn = false;
long prev;


int numStrikes = 0;
boolean hintLEDs[] = {false, false, false, false};


boolean fourBitAdderSolved;
boolean wiresSolved;
boolean morseCodeSolved;
boolean scrambledWordSolved;
boolean mazeSolved;
boolean rngSolved;

int numSolved;


void setup() {
  #ifndef __AVR_ATtiny85__
    Serial.begin(9600);
  #endif
    timer.begin(0x70);

  Wire.begin();
  
  pinMode(buzzerPin, OUTPUT);
  pinMode(strikesLED1Pin, OUTPUT);
  pinMode(strikesLED2Pin, OUTPUT);
  pinMode(strikesLED3Pin, OUTPUT);
  pinMode(hintLED1Pin, OUTPUT);
  pinMode(hintLED2Pin, OUTPUT);
  pinMode(hintLED3Pin, OUTPUT);
  pinMode(hintLED4Pin, OUTPUT);
  pinMode(resetButtonPin, INPUT);
  pinMode(addTimeButtonPin, INPUT);
  pinMode(removeTimeButtonPin, INPUT);

  randomSeed(analogRead(0));

  timer.setBrightness(brightness);
  displayTime();
  
  Wire.beginTransmission(fourBitAdderAddress);
  Wire.write('1');
  Wire.endTransmission();
  Wire.beginTransmission(wiresAddress);
  Wire.write('1');
  Wire.endTransmission();
  Wire.beginTransmission(morseCodeAddress);
  Wire.write('1');
  Wire.endTransmission();
  Wire.beginTransmission(scrambledWordAddress);
  Wire.write('1');
  Wire.endTransmission();
  Wire.beginTransmission(mazeAddress);
  Wire.write('1');
  Wire.endTransmission();
  Wire.beginTransmission(rngAddress);
  Wire.write('1');
  Wire.endTransmission();

  menuSelect = true;
  addTimeButtonPressed = false;
  removeTimeButtonPressed = false;
}


void loop() {

  // reset button
  if (digitalRead(resetButtonPin) == HIGH) {

    if (!resetButtonPressed) {

      if (menuSelect) {
        gameOn = true;
        menuSelect = false;
        initGame();
      }
      else {
        gameOn = false;
        menuSelect = true;
      
        digitalWrite(strikesLED1Pin, LOW);
        digitalWrite(strikesLED2Pin, LOW);
        digitalWrite(strikesLED3Pin, LOW);
        digitalWrite(hintLED1Pin, LOW);
        digitalWrite(hintLED2Pin, LOW);
        digitalWrite(hintLED3Pin, LOW);
        digitalWrite(hintLED4Pin, LOW);
  
        Wire.beginTransmission(fourBitAdderAddress);
        Wire.write('1');
        Wire.endTransmission();
        Wire.beginTransmission(wiresAddress);
        Wire.write('1');
        Wire.endTransmission();
        Wire.beginTransmission(morseCodeAddress);
        Wire.write('1');
        Wire.endTransmission();
        Wire.beginTransmission(scrambledWordAddress);
        Wire.write('1');
        Wire.endTransmission();
        Wire.beginTransmission(mazeAddress);
        Wire.write('1');
        Wire.endTransmission();
        Wire.beginTransmission(rngAddress);
        Wire.write('1');
        Wire.endTransmission();
  
        // reset timer
        minutes = startMinute;
        seconds = startSecond;
        timerLoop = timerInterval;
  
        displayTime();
      }
      resetButtonPressed = true;
    }
  }
  else
    resetButtonPressed = false;
   
  // if timer is running (game is on)
  if (gameOn) {
    
    // every timer interval
    if (timerLoop <= 0) {

      timerLoop = timerInterval;
      
      boolean timeLeft = updateTimer();
      if (!timeLeft) {
        gameOn = false;
        playGameOverMelody();
      }
    }

    // request solved states from slave devices
    requestSolvedStates();

    long curr = millis();
    int delta = curr - prev;
    prev = curr;
    timerLoop -= delta;
  }

  else if (menuSelect) {

    if (digitalRead(addTimeButtonPin) == HIGH) {

      if (!addTimeButtonPressed) {

        // add time
        seconds += 30;
        if (seconds >= 60) {
          minutes++;
          seconds -= 60;
          if (minutes > 15) {
            minutes = 15;
            seconds = 30;
          }
        }
        displayTime();
        addTimeButtonPressed = true;
      }
    }
    else
      addTimeButtonPressed = false;

    if (digitalRead(removeTimeButtonPin) == HIGH) {

      if (!removeTimeButtonPressed) {

        seconds -= 30;
        if (seconds < 0) {
          seconds += 60;
          minutes--;
        }
        if (minutes == 0 && seconds == 0) {
            seconds = 30;
        }
        displayTime();
        removeTimeButtonPressed = true;
      }
    }
    else
      removeTimeButtonPressed = false;
  }
  
}


void initGame() {

  delay(1000);
  
  // reset strikes
  numStrikes = 0;

  // reset hint LEDs
  for (int i = 0; i < 4; i++) {
    int n = random(0,2);

    if (n == 1)
      hintLEDs[i] = true;
    else
      hintLEDs[i] = false;
  }

  // reset timer interval
  timerInterval = 1000;

  digitalWrite(strikesLED1Pin, LOW);
  digitalWrite(strikesLED2Pin, LOW);
  digitalWrite(strikesLED3Pin, LOW);
  digitalWrite(hintLED1Pin, hintLEDs[0]);
  digitalWrite(hintLED2Pin, hintLEDs[1]);
  digitalWrite(hintLED3Pin, hintLEDs[2]);
  digitalWrite(hintLED4Pin, hintLEDs[3]);

  // reset solved states
  fourBitAdderSolved = false;
  wiresSolved = false;
  morseCodeSolved = false;
  scrambledWordSolved = false;
  mazeSolved = false;
  rngSolved = false;

  numSolved = 0;

  // reset puzzle modules
  Wire.beginTransmission(fourBitAdderAddress);
  transmitHintLEDStates();
  Wire.endTransmission();
  Wire.beginTransmission(wiresAddress);
  transmitHintLEDStates();
  Wire.endTransmission();
  Wire.beginTransmission(morseCodeAddress);
  transmitHintLEDStates();
  Wire.endTransmission();
  Wire.beginTransmission(scrambledWordAddress);
  transmitHintLEDStates();
  Wire.endTransmission();
  Wire.beginTransmission(mazeAddress);
  transmitHintLEDStates();
  Wire.endTransmission();
  Wire.beginTransmission(rngAddress);
  transmitHintLEDStates();
  Wire.endTransmission();
  
  prev = millis();
  gameOn = true;
}


boolean updateTimer() {

  // decrement time by 1 second:
  seconds -= 1;
  if (seconds < 0) {
    seconds = 59;
    minutes -= 1;
  }

  // display time on timer
  displayTime();

  // play buzzer
  tone(buzzerPin, beepNote, beepDuration);

  // game is over when time reaches 0:00
  if (minutes == 0 && seconds == 0)
    return false;
  else
    return true;
}


void displayTime() {

  timer.writeDigitNum(0, minutes / 10);
  timer.writeDigitNum(1, minutes % 10);
  timer.writeDigitNum(3, seconds / 10);
  timer.writeDigitNum(4, seconds % 10);
  timer.drawColon(true);
  timer.writeDisplay();
}


void playGameOverMelody() {

  delay(1000);
  for (int i = 0; i < loseMelodyLength; i++) {
    tone(buzzerPin, gameOverMelody[i], gameOverDurations[i] - 30);
    delay(gameOverDurations[i]);
  }
}


void playVictoryMelody() {

  delay(1000);
  for (int i = 0; i < winMelodyLength; i++) {
    tone(buzzerPin, victoryMelody[i], victoryDurations[i] - 30);
    delay(victoryDurations[i]);
  }
}


void requestSolvedStates() {

  bool updateModules = false;
  
  if (!fourBitAdderSolved) {
    Wire.requestFrom(fourBitAdderAddress, 2);
    char c = Wire.read();
    if (c == '1') {
      fourBitAdderSolved = true;
      numSolved++;
      updateModules = true;
    }
    c = Wire.read();
    if (c == '1') {
      numStrikes++;
      updateStrikes();
      updateModules = true;
    }
  }
  
  if (!wiresSolved) {
    Wire.requestFrom(wiresAddress, 2);
    char c = Wire.read();
    if (c == '1') {
      wiresSolved = true;
      numSolved++;
      updateModules = true;
    }
    c = Wire.read();
    if (c == '1') {
      numStrikes++;
      updateStrikes();
      updateModules = true;
    }
  }
  
  if (!morseCodeSolved) {
    Wire.requestFrom(morseCodeAddress, 2);
    char c = Wire.read();
    if (c == '1') {
      morseCodeSolved = true;
      numSolved++;
      updateModules = true;
    }
    c = Wire.read();
    if (c == '1') {
      numStrikes++;
      updateStrikes();
      updateModules = true;
    }
  }
  
  if (!scrambledWordSolved) {
    Wire.requestFrom(scrambledWordAddress, 2);
    char c = Wire.read();
    if (c == '1') {
      scrambledWordSolved = true;
      numSolved++;
      updateModules = true;
    }
    c = Wire.read();
    if (c == '1') {
      numStrikes++;
      updateStrikes();
      updateModules = true;
    }
  }
  
  if (!mazeSolved) {
    Wire.requestFrom(mazeAddress, 2);
    char c = Wire.read();
    if (c == '1') {
      mazeSolved = true;
      numSolved++;
      updateModules = true;
    }
    c = Wire.read();
    if (c == '1') {
      numStrikes++;
      updateStrikes();
      updateModules = true;
    }
  }
  
  if (!rngSolved) {
    Wire.requestFrom(rngAddress, 3);
    char c = Wire.read();
    if (c == '1') {
      rngSolved = true;
      numSolved++;
      updateModules = true;
    }
    c = Wire.read();
    if (c == '1') {
      numStrikes++;
      updateStrikes();
      updateModules = true;
    }
    
    c = Wire.read();
  
    switch (c) {

      // instant defuse
      case 'a':
        gameOn = false;
        playVictoryMelody();
        break;
      
      // instant kill
      case 'b':
        numStrikes = 3;
        updateStrikes();
        break;

      // nothing happens
      case 'c':
        break;

      // strike
      case 'd':
        numStrikes++;
        updateStrikes();
        updateModules = true;
        break;

      // remove strike
      case 'e':
        numStrikes--;
        if (numStrikes < 0)
          numStrikes = 0;
        updateStrikes();
        updateModules = true;
        break;

      // speed up
      case 'f':
        timerInterval /= 2;
        break;

      // slow down
      case 'g':
        timerInterval *= 2;
        break;

      // add time
      case 'h':
        seconds += 30;
        if (seconds >= 60) {
          minutes++;
          seconds -= 60;
        }
        break;

      // remove time
      case 'i':
        seconds -= 30;
        if (seconds < 0) {
          seconds += 60;
          minutes--;
          if (minutes < 0) {
            minutes = 0;
            seconds = 0;
          }
        }
        break;
    }
  }
  
  // if modules need to be updated of game states
  if (updateModules) {

    char numStrikesChar = numStrikes + '0';
    char numSolvedChar = numSolved + '0';
      
    Wire.beginTransmission(fourBitAdderAddress);
    Wire.write(numStrikesChar);
    Wire.write(numSolvedChar);
    Wire.endTransmission();
    Wire.beginTransmission(wiresAddress);
    Wire.write(numStrikesChar);
    Wire.write(numSolvedChar);
    Wire.endTransmission();
    Wire.beginTransmission(morseCodeAddress);
    Wire.write(numStrikesChar);
    Wire.write(numSolvedChar);
    Wire.endTransmission();
    Wire.beginTransmission(scrambledWordAddress);
    Wire.write(numStrikesChar);
    Wire.write(numSolvedChar);
    Wire.endTransmission();
    Wire.beginTransmission(mazeAddress);
    Wire.write(numStrikesChar);
    Wire.write(numSolvedChar);
    Wire.endTransmission();
    Wire.beginTransmission(rngAddress);
    Wire.write(numStrikesChar);
    Wire.write(numSolvedChar);
    Wire.endTransmission();
  }
    
  // if all modules are solved
  if (numSolved == 6 && gameOn) {
    gameOn = false;
    playVictoryMelody();
  }
}


void updateStrikes() {

  if (numStrikes <= 0) {
    digitalWrite(strikesLED1Pin, LOW);
    digitalWrite(strikesLED2Pin, LOW);
    digitalWrite(strikesLED3Pin, LOW);
  }
  
  if (numStrikes == 1) {
    digitalWrite(strikesLED1Pin, HIGH);
    digitalWrite(strikesLED2Pin, LOW);
    digitalWrite(strikesLED3Pin, LOW);
  }
  
  if (numStrikes == 2) {
    digitalWrite(strikesLED1Pin, HIGH);
    digitalWrite(strikesLED2Pin, HIGH);
    digitalWrite(strikesLED3Pin, LOW);
  }
  
  if (numStrikes >= 3) {
    digitalWrite(strikesLED1Pin, HIGH);
    digitalWrite(strikesLED2Pin, HIGH);
    digitalWrite(strikesLED3Pin, HIGH);
    gameOn = false;
    playGameOverMelody();
  }
}


void transmitHintLEDStates() {
  for (int i = 0; i < 4; i++) {
    if (hintLEDs[i])
      Wire.write('1');
    else
      Wire.write('0');
  }
}
