#include <Wire.h>


const int i2cAddress =  1;

const int enterButtonPin = 10;
const int greenLEDPin = 13;

// D12 pin is S0
// A1 pin is S1
// A2 pin is S2
// A3 pin is S3
// D11 pin is S4

const int a3Pin = 5;
const int a2Pin = 4;
const int a1Pin = 3;
const int a0Pin = 2;

const int b3Pin = 9;
const int b2Pin = 8;
const int b1Pin = 7;
const int b0Pin = 6;


bool gameOn = false;
char solved = '0';
char newStrike = '0';
bool enterButtonPressed;

int numStrikes;
int numSolved;

int hintLEDs[] = {false, false, false, false};


int a0, a1, a2, a3;
int sum;


void setup() {
  
  Wire.begin(i2cAddress);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  
  pinMode(enterButtonPin, INPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A7, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(a3Pin, OUTPUT);
  pinMode(a2Pin, OUTPUT);
  pinMode(a1Pin, OUTPUT);
  pinMode(a0Pin, OUTPUT);
  pinMode(b3Pin, INPUT);
  pinMode(b2Pin, INPUT);
  pinMode(b1Pin, INPUT);
  pinMode(b0Pin, INPUT);

  randomSeed(analogRead(0));
}


void loop() {

  if (gameOn) {

    if (solved == '0') {

      if (digitalRead(enterButtonPin) == HIGH) {

        if (!enterButtonPressed) {

          int b0 = digitalRead(b0Pin);
          int b1 = digitalRead(b1Pin);
          int b2 = digitalRead(b2Pin);
          int b3 = digitalRead(b3Pin);
      
          int a = binToDec(0, a3, a2, a1, a0);
          int b = binToDec(0, b3, b2, b1, b0);
      
          int s = a + b;
  
          bool correctSum = (s == sum);
  
          
          int s4 = s / 16;
          s %= 16;
          int s3 = s / 8;
          s %= 8;
          int s2 = s / 4;
          s %= 4;
          int s1 = s / 2;
          s %= 2;
          int s0 = s;
          
          digitalWrite(12, s0);
          digitalWrite(A1, s1);
          digitalWrite(A2, s2);
          digitalWrite(A3, s3);
          digitalWrite(11, s4);
  
          if (correctSum) {
            solved = '1';
            digitalWrite(greenLEDPin, HIGH);
          }
          else {
            newStrike = '1';
          }
          enterButtonPressed = true;
        }        
      }
      else
        enterButtonPressed = false;
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

  digitalWrite(12, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  digitalWrite(11, LOW);
  
  updateSum();
  
  int a = sum + 1;
  do {
    a0 = random(0,2);
    a1 = random(0,2);
    a2 = random(0,2);
    a3 = random(0,2); 

    a = binToDec(0, a3, a2, a1, a0);
  } while (a > sum);
  
  digitalWrite(a0Pin, a0);
  digitalWrite(a1Pin, a1);
  digitalWrite(a2Pin, a2);
  digitalWrite(a3Pin, a3);
  
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

    updateSum();
  }

  if (numBytes == 1) {
    char c = Wire.read();
    gameOn = false;  
    digitalWrite(greenLEDPin, LOW);
    digitalWrite(12, LOW);
    digitalWrite(A1, LOW);
    digitalWrite(A2, LOW);
    digitalWrite(A3, LOW);
    digitalWrite(11, LOW);
    digitalWrite(a0Pin, LOW);
    digitalWrite(a1Pin, LOW);
    digitalWrite(a2Pin, LOW);
    digitalWrite(a3Pin, LOW);
  }
}


int binToDec(int n4, int n3, int n2, int n1, int n0) {
  return (n0*1 + n1*2 + n2*4 + n3*8 + n4*16);
}


void updateSum() {

  if (hintLEDs[0] && hintLEDs[1])
    sum = 15;
  else if (hintLEDs[2])
    sum = numStrikes + 7;
  else if (!hintLEDs[0])
    sum = 8;
  else if (hintLEDs[0] && hintLEDs[3])
    sum = numSolved + 8;
  else if (!hintLEDs[1])
    sum = 12;    
  else
    sum = 3;
}
