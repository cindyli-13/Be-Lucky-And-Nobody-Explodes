/*
Keypad Pin 1 –> Arduino Pin 7
Keypad Pin 2 –> Arduino Pin 5
Keypad Pin 3 –> Arduino Pin 8
Keypad Pin 4 –> Arduino Pin 2
Keypad Pin 5 –> Arduino Pin 6
Keypad Pin 6 –> Arduino Pin 3
Keypad Pin 7 –> Arduino Pin 4
*/
#include <Wire.h>
#include <Arduino.h>
#include <TM1637Display.h>
#include <Keypad.h>
#define CLK 9
#define DIO 10

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

const int i2cAddress =  6;
const int greenLEDPin = 13;

bool corrCode = false;
int disNum = 0000;
int firDig = 0;
int secDig = 0;
int thrDig = 0;
int forDig = 0;
int count = 0;
bool rolling = true;
TM1637Display display(CLK, DIO);
int passCode[16][7] = {
  {7,3,5,5,6,0,8},//0
  {9,0,6,1,5,7,3},//1
  {7,5,0,8,7,0,9},//2
  {6,4,5,6,2,2,9},//3
  {2,9,1,6,7,3,3},//4
  {5,0,3,2,6,1,4},//5
  {7,7,1,3,7,3,6},//6
  {6,9,4,7,6,4,7},//7
  {0,7,5,2,2,1,6},//8
  {9,8,7,4,6,8,7},//9
  {8,1,9,7,4,9,8},//10
  {8,8,0,8,0,2,5},//11
  {4,6,6,8,7,1,3},//12
  {2,3,7,5,7,8,0},//13
  {9,4,3,7,2,3,0},//14
  {0,0,8,0,6,6,7},//15
};
int code[] = {0,0,0,0,0,0,0};
int codeInd = 0;
int hintCase = 0;

bool gameOn = false;
char solved = '0';
char newStrike = '0';
char event = 'z';
bool enterButtonPressed;

int numStrikes;
int numSolved;

int hintLEDs[] = {false, false, false, false};


void setup() {
  randomSeed(analogRead(0));
  Wire.begin(i2cAddress);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  
  display.setBrightness(0x0f);
  Serial.begin(9600);
  pinMode(greenLEDPin,OUTPUT);
}

void loop() {
 

   if (gameOn) {
    
    if (solved == '0') {
        char key = keypad.getKey();
        
        if(!corrCode){
          disNum = random(9999);
          display.showNumberDec(disNum, false);
          delay(50);
        }
      
         if (key != NO_KEY){
            if(codeInd<=6){
                Serial.println(key);
                code[codeInd] = key;
                codeInd++;
                  
              
          }
           if (key == '#'){
                    if(correct(code)){
                        firDig = random(9);
                        secDig = random(9);
                        thrDig = random(9);
                        forDig = random(9);
                        btnPress();
                        corrCode = true;
                        solved = '1';
                        digitalWrite(greenLEDPin, HIGH);
                }
          
             }
        }
     
      
    }
   }
   
}

void initModule() {
  numStrikes = 0;
  numSolved = 0;
  solved = '0';
  newStrike = '0';
  enterButtonPressed = false;
  codeInd = 0;
  rolling = true;
  count = 0;
  digitalWrite(greenLEDPin, LOW);
  //Detect which test case it is
  hintCase = detectCase();
  Serial.print(hintCase);
  gameOn = true;
  corrCode = false;
}

void requestEvent() {
  Wire.write(solved);
  Wire.write(newStrike);
  Wire.write(event);
  event = 'z';
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
    gameOn = false;  
    digitalWrite(greenLEDPin, LOW);
  }
}

void btnPress(){
  while(rolling){
  count++;
    if(count>40){
      display.showNumberDec(forDig,false,1);
      rolling = false;
    }
    else if(count>25){
      disNum = random(9);
      display.showNumberDec(thrDig,false,2);
      display.showNumberDec(disNum,false,1);
      delay(50);
    }else if(count>10){
      disNum = random(99);
      display.showNumberDec(secDig,false,3);
      display.showNumberDec(disNum,false,2);
      delay(50);
    }
    else{
      disNum = random(999);
      display.showNumberDec(firDig,false,4);
      display.showNumberDec(disNum,false,3);
      delay(50);
    }
  }
              if(forDig == 0 && thrDig == 0){ //Instant defuse 0 - 99 (1%)
                event = 'a';
                Serial.println(event);
              }
              else if(forDig == 0 && thrDig == 1){ //Instant kill 100 - 199 (1%)
                event = 'b';
                Serial.println(event);
              }
              else if(forDig == 0 && 2 <= thrDig && thrDig <= 9){ //Nothing Happens 200 - 999 (8%)
                event = 'c';
                Serial.println(event);
              }
              else if((1 <= forDig && forDig < 3) || (forDig == 3 && thrDig < 5 )){ //Strike 1000 - 3499 (25%)
                event = 'd';  
                Serial.println(event);
              }
              else if(forDig < 6){ //Remove Strike 3500 - 5999 (25%)
                event = 'e';
                Serial.println(event);
              }
              else if(forDig == 6){ //Timer Speed up 6000 - 6999 (10%) 
                event = 'f';
                Serial.println(event);
              }
              else if(forDig == 7){ //Timer Slow down 7000 - 7999(10%)
                event = 'g';
                Serial.println(event);
              }
              else if(forDig == 8){ //Add time 8000 - 8999(10%)
                event = 'h';
                Serial.println(event);
              }
              else if(forDig == 9){ //Subtract time 9000 - 9999(10%)
                event = 'i';
                Serial.println(event);
              }
}

boolean correct(int arr[]){
  for(int i = 0; i < 7; i++){
    if(arr[i] - 48 != passCode[hintCase][i]){
      Serial.println("The code is wrong!");
      for(int i = 0; i < 7; i++){
        code[i] = 0;
      }
      codeInd = 0;
      newStrike = '1';
      return false;
    }
  }
  Serial.println("The code is correct!");
  return true;
}

int detectCase(){
  int fir = hintLEDs[0];
  int sec = hintLEDs[1]*2;
  int thi = hintLEDs[2]*4;
  int fort = hintLEDs[3]*8;

  return fir+sec+thi+fort;
}
