#include <Wire.h>

const int i2cAddress =  2;

bool gameOn = false;
char solved = '0';
char newStrike = '0';
bool enterButtonPressed;

int numStrikes;
int numSolved;

int hintLEDs[] = {0, 0, 0, 0};


//////////////////////// Male pins
int outR = 3;
int outG = 4;
int outBlu = 5;
int outY = 6;
int outBla = 8;
int outW = 7;
////////////////////////
int red = 9;
int blu = 11;
int gre = 10;
int yel = 12;
int bla = 16;
int whi = 14;
int inWire[16][6] = {//female
  {blu,yel,red,whi,gre,bla},//0
  {whi,red,gre,blu,yel,bla},//1
  {gre,blu,red,whi,yel,bla},//2
  {gre,red,blu,whi,bla,yel},//3
  {whi,blu,bla,gre,red,yel},//4
  {bla,gre,blu,red,yel,whi},//5
  {whi,bla,yel,blu,red,gre},//6
  {red,whi,gre,bla,yel,blu},//7
  {red,gre,blu,yel,whi,bla},//8
  {gre,red,yel,blu,whi,bla},//9
  {blu,yel,red,gre,whi,bla},//10
  {red,gre,bla,blu,yel,whi},//11
  {yel,blu,bla,red,gre,whi},//12
  {blu,bla,red,whi,gre,yel},//13
  {bla,gre,whi,yel,blu,red},//14
  {blu,whi,gre,red,yel,bla},//15
};
int outWire[6] = {outR, outG, outBlu, outY, outBla, outW};
int hintCase = 1;
int btnPin = 15;
int greenLedPin = 13;
bool corr = false;
void setup() {
  // put your setup code here, to run once:
  pinMode(red, INPUT);
  pinMode(blu, INPUT);
  pinMode(gre, INPUT);
  pinMode(yel, INPUT);
  pinMode(whi, INPUT);
  pinMode(bla, INPUT);
  pinMode(outR, OUTPUT);
  pinMode(outG, OUTPUT);
  pinMode(outBlu, OUTPUT);
  pinMode(outY, OUTPUT);
  pinMode(outW, OUTPUT);
  pinMode(outBla, OUTPUT);
  pinMode(btnPin, INPUT);
  pinMode(greenLedPin, OUTPUT);
  
  Serial.begin(9600);

  Wire.begin(i2cAddress);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

  //initModule(); //used for testing

}

void loop() {
    if (gameOn) {
      
      if (solved == '0') {
        if (digitalRead(btnPin) == HIGH) {
  
          if (!enterButtonPressed) {
            if(check(hintCase)){
              solved = '1';
            }else{
              newStrike = '1';
            }
            enterButtonPressed = true;
           
          }
      }
      else{
        enterButtonPressed = false;
      }
    }
    
    
  }
}

bool check(int a){
    for(int i = 0; i < 6; i++){
      digitalWrite(outWire[i], HIGH);
      if(digitalRead(inWire[a][i]) != HIGH){
        Serial.println("reeeeeeeeeeeeeee");
        Serial.println(i);
        return false;
      }
      digitalWrite(outWire[i], LOW);
    }
    Serial.println("The wiring is right!");
    digitalWrite(greenLedPin, HIGH);
    return true;
}

void initModule() {
  numStrikes = 0;
  numSolved = 0;
  solved = '0';
  newStrike = '0';
  enterButtonPressed = false;
  digitalWrite(greenLedPin, LOW);
//Detect which test case it is
  hintCase = detectCase();
  Serial.print(hintCase);
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
    gameOn = false;  
    digitalWrite(greenLedPin, LOW);
  }
}

int detectCase(){
  int fir = hintLEDs[0];
  int sec = hintLEDs[1]*2;
  int thi = hintLEDs[2]*4;
  int fort = hintLEDs[3]*8;

  return fir+sec+thi+fort;
}
