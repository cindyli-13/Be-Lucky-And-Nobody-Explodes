#include <LiquidCrystal.h>
#include <Wire.h>


const int i2cAddress =  3;

const int greenLEDPin = 13;
const int enterButtonPin = 6;
const int blinkLEDPin = 7;

// A2 pin is potentiometer read

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int unitTime = 170;


bool gameOn = false;
char solved = '0';
char newStrike = '0';
bool enterButtonPressed;

int numStrikes;
int numSolved;

int hintLEDs[] = {false, false, false, false};

double frequency;
String correctWord;
int correctWordIndex;

String words[16] = {
  "share", "shell", "shake", "bomb", "board", "bored", "hello", "world", "where", "there",
  "game", "gave", "five", "area", "arena", "air"
};
double frequencies[16] = {
  3.505, 3.515, 3.522, 3.532, 3.535, 3.542, 3.545, 3.552, 3.555, 3.565, 3.572, 3.575, 3.582,
  3.592, 3.595, 3.600
};


// 0 = dot
// 1 = dash
// 2 = space between parts of same letter
// 3 = space between letters
// 4 = space between words

// share
int share[26] = {
  0, 2, 0, 2, 0, // s
  3, 
  0, 2, 0, 2, 0, 2, 0, // h
  3,
  0, 2, 1, // a
  3, 
  0, 2, 1, 2, 0, // r
  3,
  0, // e
  4
};

// shell
int shell[32] = {
  0, 2, 0, 2, 0, // s
  3,
  0, 2, 0, 2, 0, 2, 0, // h
  3,
  0, // e
  3,
  0, 2, 1, 2, 0, 2, 0, // l
  3,
  0, 2, 1, 2, 0, 2, 0, // l
  4
};

// shake
int shake[26] = {
  0, 2, 0, 2, 0, // s
  3, 
  0, 2, 0, 2, 0, 2, 0, // h
  3,
  0, 2, 1, // a
  3, 
  1, 2, 0, 2, 1, // k
  3, 
  0, // e 
  4
};

// bomb
int bomb[26] = {
  1, 2, 0, 2, 0, 2, 0, // b
  3,
  1, 2, 1, 2, 1, // o
  3,
  1, 2, 1, // m
  3, 
  1, 2, 0, 2, 0, 2, 0, // b
  4
};

// board
int board[30] = {
  1, 2, 0, 2, 0, 2, 0, // b
  3,
  1, 2, 1, 2, 1, // o
  3,
  0, 2, 1, // a
  3, 
  0, 2, 1, 2, 0, // r
  3, 
  1, 2, 0, 2, 0, // d
  4
};

// bored
int bored[28] = {
  1, 2, 0, 2, 0, 2, 0, // b
  3,
  1, 2, 1, 2, 1, // o
  3,
  0, 2, 1, 2, 0, // r
  3, 
  0, // e
  3, 
  1, 2, 0, 2, 0, // d
  4
};

// hello
int hello[32] = {
  0, 2, 0, 2, 0, 2, 0, // h
  3,
  0, // e
  3, 
  0, 2, 1, 2, 0, 2, 0, // l
  3,
  0, 2, 1, 2, 0, 2, 0, // l
  3,
  1, 2, 1, 2, 1, // o
  4
};

// world
int world[32] = {
  0, 2, 1, 2, 1, // w
  3, 
  1, 2, 1, 2, 1, // o
  3, 
  0, 2, 1, 2, 0, // r
  3, 
  0, 2, 1, 2, 0, 2, 0, // l
  3,
  1, 2, 0, 2, 0, // d
  4
};

// where
int where[24] = {
  0, 2, 1, 2, 1, // w
  3,
  0, 2, 0, 2, 0, 2, 0, // h
  3,
  0, // e
  3,
  0, 2, 1, 2, 0, // r
  3,
  0, // e
  4
};

// there
int there[20] = {
  1, // t
  3,
  0, 2, 0, 2, 0, 2, 0, // h
  3,
  0, // e
  3,
  0, 2, 1, 2, 0, // r
  3,
  0, // e
  4
};

// game
int game[16] = {
  1, 2, 1, 2, 0, // g
  3,
  0, 2, 1, // a
  3, 
  1, 2, 1, // m
  3,
  0, // e
  4
};

// gave
int gave[20] = {
  1, 2, 1, 2, 0, // g
  3,
  0, 2, 1, // a
  3, 
  0, 2, 0, 2, 0, 2, 1, // v
  3,
  0, // e
  4
};

// five
int five[22] = {
  0, 2, 0, 2, 1, 2, 0, //f
  3,
  0, 2, 0, // i
  3,
  0, 2, 0, 2, 0, 2, 1, // v
  3,
  0, // e
  4
};

// area
int area[16] = {
  0, 2, 1, // a
  3,
  0, 2, 1, 2, 0, // r
  3,
  0, // e
  3,
  0, 2, 1, // a
  4
};

// arena
int arena[20] = {
  0, 2, 1, // a
  3,
  0, 2, 1, 2, 0, // r
  3,
  0, // e
  3,
  1, 2, 0, // n
  3,
  0, 2, 1, // a
  4
};

// air
int air[14] = {
  0, 2, 1, // a
  3,
  0, 2, 0, // i
  3,
  0, 2, 1, 2, 0, // r
  4
};


int timeLeft;
int morseIndex;

const int lcdRefreshInterval = 50;
int lcdTimeLeft;

long prev;


void setup() {

  Wire.begin(i2cAddress);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  
  pinMode(A2, INPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(enterButtonPin, INPUT);
  pinMode(blinkLEDPin, OUTPUT);

  randomSeed(analogRead(0));

  lcd.begin(16, 2);
}

void loop() {

  if (gameOn) {
    
    if (solved == '0') {

      if (digitalRead(enterButtonPin) == HIGH) {

        if (!enterButtonPressed) {

          int index = 0;
          for (int i = 0; i < 16; i++) {
            if (frequencies[i] == frequency)
              index = i;
          }
      
          if (index == correctWordIndex) {
            solved = '1';
            digitalWrite(greenLEDPin, HIGH);
            digitalWrite(blinkLEDPin, LOW);
          }
          else {
            newStrike = '1';
          }
          enterButtonPressed = true;
        }
      }
      else
        enterButtonPressed = false;
      
      if (timeLeft <= 0) {
    
        int n = 0;
        
        switch(correctWordIndex) {
          
          case 0:
          
            morseIndex = (morseIndex + 1) % 26;
            n = share[morseIndex];
            break;
    
          case 1:
    
            morseIndex = (morseIndex + 1) % 32;
            n = shell[morseIndex];
            break;
    
          case 2:
    
            morseIndex = (morseIndex + 1) % 26;
            n = shake[morseIndex];
            break;
    
          case 3:
    
            morseIndex = (morseIndex + 1) % 26;
            n = bomb[morseIndex];
            break;
    
          case 4:
    
            morseIndex = (morseIndex + 1) % 30;
            n = board[morseIndex];
            break;
    
          case 5:
    
            morseIndex = (morseIndex + 1) % 28;
            n = bored[morseIndex];
            break;
    
          case 6:
    
            morseIndex = (morseIndex + 1) % 32;
            n = hello[morseIndex];
            break;
    
          case 7:
    
            morseIndex = (morseIndex + 1) % 32;
            n = world[morseIndex];
            break;
    
          case 8:
    
            morseIndex = (morseIndex + 1) % 24;
            n = where[morseIndex];
            break;
    
          case 9:
    
            morseIndex = (morseIndex + 1) % 20;
            n = there[morseIndex];
            break;
    
          case 10:
    
            morseIndex = (morseIndex + 1) % 16;
            n = game[morseIndex];
            break;
    
          case 11:
    
            morseIndex = (morseIndex + 1) % 20;
            n = gave[morseIndex];
            break;
    
          case 12:
    
            morseIndex = (morseIndex + 1) % 22;
            n = five[morseIndex];
            break;
    
          case 13:
    
            morseIndex = (morseIndex + 1) % 16;
            n = area[morseIndex];
            break;
    
          case 14:
    
            morseIndex = (morseIndex + 1) % 20;
            n = arena[morseIndex];
            break;
    
          case 15:
    
            morseIndex = (morseIndex + 1) % 14;
            n = air[morseIndex];
            break;
        }
    
        switch (n) {
          
          case 0:
          
            timeLeft = unitTime;
            digitalWrite(blinkLEDPin, HIGH);
            break;
    
          case 1:
    
            timeLeft = unitTime*3;
            digitalWrite(blinkLEDPin, HIGH);
            break;
    
          case 2:
    
            timeLeft = unitTime;
            digitalWrite(blinkLEDPin, LOW);
            break;
    
          case 3:
    
            timeLeft = unitTime*3;
            digitalWrite(blinkLEDPin, LOW);
            break;
    
          case 4:
    
            timeLeft = unitTime*7;
            digitalWrite(blinkLEDPin, LOW);
            break;
        }
      }
    
      if (lcdTimeLeft <= 0) {
        updateFrequency();
        displayFrequency();
        lcdTimeLeft = lcdRefreshInterval;
      }
    
      long curr = millis();
      int delta = curr - prev;
      prev = curr;
      timeLeft -= delta;
      lcdTimeLeft -= delta;
    }
  }
  
}


void initModule() {
  numStrikes = 0;
  numSolved = 0;
  solved = '0';
  newStrike = '0';
  enterButtonPressed = false;
  digitalWrite(greenLEDPin, LOW);
  digitalWrite(blinkLEDPin, LOW);

  lcd.clear();

  lcd.setCursor(9, 0);
  lcd.print("MHz");

  timeLeft = 0;
  morseIndex = -1;

  lcdTimeLeft = 0;

  correctWordIndex = random(0, 16);
  correctWord = words[correctWordIndex];
  
  prev = millis();

  gameOn = true;
}


void requestEvent() {
  Wire.write(solved);
  Wire.write(newStrike);
  newStrike = '0';
}


void receiveEvent(int numBytes) {

  if (numBytes == 4) {
    char hint1 = Wire.read();
    char hint2 = Wire.read();
    char hint3 = Wire.read();
    char hint4 = Wire.read();
  
    if (hint1 == '1')
      hintLEDs[0] = 1;
    else
      hintLEDs[0] = 0;
    if (hint2 == '1')
      hintLEDs[1] = 1;
    else
      hintLEDs[1] = 0;
    if (hint3 == '1')
      hintLEDs[2] = 1;
    else
      hintLEDs[2] = 0;
    if (hint4 == '1')
      hintLEDs[3] = 1;
    else
      hintLEDs[3] = 0;
    
    initModule(); 
  }

  else if (numBytes == 2) {
    char c = Wire.read();
    numStrikes = c - 48;
    c = Wire.read();
    numSolved = c - 48;
  }

  else if (numBytes == 1) {
    char c = Wire.read();
    lcd.clear();
    gameOn = false;  
    digitalWrite(greenLEDPin, LOW);
    digitalWrite(blinkLEDPin, LOW);
  }
}


void updateFrequency() {

  int pot = (analogRead(A2) - 11) / 64;
  frequency = frequencies[pot];
}


void displayFrequency() {

  lcd.setCursor(3,0);
  lcd.print(String(frequency, 3));
}
