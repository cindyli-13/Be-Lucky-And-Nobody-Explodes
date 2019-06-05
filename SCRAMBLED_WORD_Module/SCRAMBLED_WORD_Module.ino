// Adapted from Michael Pilcher
// 2/9/2010

#include <LiquidCrystal.h>
#include <Wire.h>


const int i2cAddress =  4;

const int button1Pin = 10;
const int button2Pin = 9;
const int button3Pin = 8;
const int button4Pin = 7;
const int greenLEDPin = 13;


bool gameOn = false;
char solved = '0';
char newStrike = '0';
bool enterButtonPressed;

int numStrikes;
int numSolved;

int hintLEDs[] = {false, false, false, false};


const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// enter button is A1


byte LT[8] = {
  B00111,
  B01111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
byte UB[8] = {
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte RT[8] = {
  B11100,
  B11110,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
byte LL[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B01111,
  B00111
};
byte LB[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111
};
byte LR[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11110,
  B11100
};
byte UMB[8] = {
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111
};
byte LMB[8] = {
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111
};


char pool1[5] = {
  'a', 'b', 'c', 'd', 'e'
};
char pool2[5] = {
  'a', 'b', 'c', 'd', 'e'
};
char pool3[5] = {
  'a', 'b', 'c', 'd', 'e'
};
char pool4[5] = {
  'a', 'b', 'c', 'd', 'e'
};

String words[16] = {
  "left", "luck", "code", "read", "four", "drop", "push", "zero", 
  "play", "case", "hide", "hear", "idea", "path", "sock", "tail"  
};

int index1, index2, index3, index4;
char c1, c2, c3, c4;


void setup() {
  
  Wire.begin(i2cAddress);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);
  pinMode(button4Pin, INPUT);
  pinMode(A1, INPUT);
  pinMode(greenLEDPin, OUTPUT);

  randomSeed(analogRead(0));
  
  lcd.createChar(8,LT);
  lcd.createChar(1,UB);
  lcd.createChar(2,RT);
  lcd.createChar(3,LL);
  lcd.createChar(4,LB);
  lcd.createChar(5,LR);
  lcd.createChar(6,UMB);
  lcd.createChar(7,LMB);

  lcd.begin(16, 2);
}


void loop() {

  if (gameOn) {
    
    if (solved == '0') {

      if (digitalRead(button1Pin) == HIGH) {
        index1 = (index1+1) % 5;
        c1 = pool1[index1];
        displayChar(0, c1);
        delay(200);
      }
      if (digitalRead(button2Pin) == HIGH) {
        index2 = (index2+1) % 5;
        c2 = pool2[index2];
        displayChar(1, c2);
        delay(200);
      }
      if (digitalRead(button3Pin) == HIGH) {
        index3 = (index3+1) % 5;
        c3 = pool3[index3];
        displayChar(2, c3);
        delay(200);
      }
      if (digitalRead(button4Pin) == HIGH) {
        index4 = (index4+1) % 5;
        c4 = pool4[index4];
        displayChar(3, c4);
        delay(200);
      }
    
      if (digitalRead(A1) == HIGH) {

        if (!enterButtonPressed) {

           String w = "";
          w.concat(c1);
          w.concat(c2);
          w.concat(c3);
          w.concat(c4);
          
          if (isAWord(w)) {
            solved = '1';
            digitalWrite(greenLEDPin, HIGH);
          }
          else {
            newStrike = '1';
          }
        }
        enterButtonPressed = true;
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

  lcd.clear();

  // pick random word
  int r = random(0, 16);
  String w = words[r];
  pool1[0] = w.charAt(0);
  pool2[0] = w.charAt(1);
  pool3[0] = w.charAt(2);
  pool4[0] = w.charAt(3);

  // generate random chars in pool
  for (int i = 1; i < 5; i++) {
    int c = 97;

    do {
      c = random(97, 123);
    } while (c == 109 || c == 110 || c == 113 || c == 118 || c == 119);
    pool1[i] = c;
    
    do {
      c = random(97, 123);
    } while (c == 109 || c == 110 || c == 113 || c == 118 || c == 119);
    pool2[i] = c;

    do {
      c = random(97, 123);
    } while (c == 109 || c == 110 || c == 113 || c == 118 || c == 119);
    pool3[i] = c;

    do {
      c = random(97, 123);
    } while (c == 109 || c == 110 || c == 113 || c == 118 || c == 119);
    pool4[i] = c;
  }

  index1 = random(0,4);
  index2 = random(0,4);
  index3 = random(0,4);
  index4 = random(0,4);
  
  c1 = pool1[index1];
  c2 = pool2[index2];
  c3 = pool3[index3];
  c4 = pool4[index4];

  displayChar(0,c1);
  displayChar(1,c2);
  displayChar(2,c3);
  displayChar(3,c4);

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
  }
}


void displayChar(int index, char c) {

  // clear previous char
  lcd.setCursor(index*4, 0);
  lcd.write(' ');
  lcd.write(' ');
  lcd.write(' ');
  lcd.write(' ');
  lcd.setCursor(index*4, 1);
  lcd.write(' ');
  lcd.write(' ');
  lcd.write(' ');
  lcd.write(' ');
  
  switch (c) {
    case 'a':
      customA(index*4);
      break;
    case 'b':
      customB(index*4);
      break;
    case 'c':
      customC(index*4);
      break;
    case 'd':
      customD(index*4);
      break;
    case 'e':
      customE(index*4);
      break;
    case 'f':
      customF(index*4);
      break;
    case 'g':
      customG(index*4);
      break;
    case 'h':
      customH(index*4);
      break;
    case 'i':
      customI(index*4);
      break;
    case 'j':
      customJ(index*4);
      break;
    case 'k':
      customK(index*4);
      break;
    case 'l':
      customL(index*4);
      break;
    case 'm':
      customM(index*4);
      break;
    case 'n':
      customN(index*4);
      break;
    case 'o':
      customO(index*4);
      break;
    case 'p':
      customP(index*4);
      break;
    case 'q':
      customQ(index*4);
      break;
    case 'r':
      customR(index*4);
      break;
    case 's':
      customS(index*4);
      break;
    case 't':
      customT(index*4);
      break;
    case 'u':
      customU(index*4);
      break;
    case 'v':
      customV(index*4);
      break;
    case 'w':
      customW(index*4);
      break;
    case 'x':
      customX(index*4);
      break;
    case 'y':
      customY(index*4);
      break;
    case 'z': 
      customZ(index*4);
      break;
  }
}


void customA(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(8));
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(255);
  lcd.write(254);
  lcd.write(255);
}

void customB(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(255));
  lcd.write(6);
  lcd.write(5);
  lcd.setCursor(x, 1);
  lcd.write(255);
  lcd.write(7);
  lcd.write(2);
}

void customC(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(8));
  lcd.write(1);
  lcd.write(1);
  lcd.setCursor(x,1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(4);
}

void customD(int x)
{
  lcd.setCursor(x, 0); 
  lcd.write(byte(255));  
  lcd.write(1); 
  lcd.write(2);
  lcd.setCursor(x, 1); 
  lcd.write(255);  
  lcd.write(4);  
  lcd.write(5);
}

void customE(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(255));
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(x, 1);
  lcd.write(255);
  lcd.write(7);
  lcd.write(7); 
}

void customF(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(255));
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(x, 1);
  lcd.write(255);
}

void customG(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(8));
  lcd.write(1);
  lcd.write(1);
  lcd.setCursor(x,1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(2);
}

void customH(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(255));
  lcd.write(4);
  lcd.write(255);
  lcd.setCursor(x, 1);
  lcd.write(255);
  lcd.write(254);
  lcd.write(255); 
}

void customI(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(1));
  lcd.write(255);
  lcd.write(1);
  lcd.setCursor(x,1);
  lcd.write(4);
  lcd.write(255);
  lcd.write(4);
}

void customJ(int x)
{
  lcd.setCursor(x+2,0);
  lcd.write(byte(255));
  lcd.setCursor(x,1);
  lcd.write(4);
  lcd.write(4);
  lcd.write(5);
}

void customK(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(255));
  lcd.write(4);
  lcd.write(5);
  lcd.setCursor(x,1);
  lcd.write(255);
  lcd.write(254);
  lcd.write(2); 
}

void customL(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(255));
  lcd.setCursor(x,1);
  lcd.write(255);
  lcd.write(4);
  lcd.write(4);
}

void customM(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(8));
  lcd.write(3);
  lcd.write(5);
  lcd.write(2);
  lcd.setCursor(x,1);
  lcd.write(255);
  lcd.write(254);
  lcd.write(254);
  lcd.write(255);
}

void customN(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(8));
  lcd.write(2);
  lcd.write(254);
  lcd.write(255);
  lcd.setCursor(x,1);
  lcd.write(255);
  lcd.write(254);
  lcd.write(3);
  lcd.write(5);
}

void customO(int x)
{
  lcd.setCursor(x, 0); 
  lcd.write(byte(8));  
  lcd.write(1); 
  lcd.write(2);
  lcd.setCursor(x, 1); 
  lcd.write(3);  
  lcd.write(4);  
  lcd.write(5);
}

void customP(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(255));
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(255);
}

void customQ(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(8));
  lcd.write(1);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(255);
  lcd.write(4);
}

void customR(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(255));
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x,1);
  lcd.write(255);
  lcd.write(254);
  lcd.write(2); 
}

void customS(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(8));
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(x, 1);
  lcd.write(7);
  lcd.write(7);
  lcd.write(5);
}

void customT(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(1));
  lcd.write(255);
  lcd.write(1);
  lcd.setCursor(x,1);
  lcd.write(254);
  lcd.write(255);
}

void customU(int x)
{
  lcd.setCursor(x, 0); 
  lcd.write(byte(255));  
  lcd.write(254); 
  lcd.write(255);
  lcd.setCursor(x, 1); 
  lcd.write(3);  
  lcd.write(4);  
  lcd.write(5);
}

void customV(int x)
{
  lcd.setCursor(x, 0); 
  lcd.write(byte(3));  
  lcd.write(254);
  lcd.write(254); 
  lcd.write(5);
  lcd.setCursor(x+1, 1); 
  lcd.write(2);  
  lcd.write(8);
}

void customW(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(255));
  lcd.write(254);
  lcd.write(254);
  lcd.write(255);
  lcd.setCursor(x,1);
  lcd.write(3);
  lcd.write(8);
  lcd.write(2);
  lcd.write(5);
}

void customX(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(3));
  lcd.write(4);
  lcd.write(5);
  lcd.setCursor(x,1);
  lcd.write(8);
  lcd.write(254);
  lcd.write(2); 
}

void customY(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(3));
  lcd.write(4);
  lcd.write(5);
  lcd.setCursor(x+1,1);
  lcd.write(255);
}

void customZ(int x)
{
  lcd.setCursor(x,0);
  lcd.write(byte(1));
  lcd.write(6);
  lcd.write(5);
  lcd.setCursor(x, 1);
  lcd.write(8);
  lcd.write(7);
  lcd.write(4);
}

bool isAWord(String w) {
  for (int i = 0; i < 16; i++) {
    if (w.equals(words[i]))
      return true;
  }
  return false;
}
