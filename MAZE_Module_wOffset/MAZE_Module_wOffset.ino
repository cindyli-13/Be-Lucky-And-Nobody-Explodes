#include <Wire.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
// Which pin on the Arduino is connected to the NeoPixels
#define PIN        6
// How many NeoPixels are attached to the Arduino
#define NUMPIXELS 64

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const int i2cAddress =  5;
const int greenLEDPin = 13;
const int upPin = 2;
const int downPin = 3;
const int leftPin = 4;
const int rightPin = 5;
bool gameOn = false;
char solved = '0';
char newStrike = '0';
bool btnPressed = false;
int pos;
int mvmt;
int mvmtIndex = 0;
int pathNum;
//0 = right
//1 = left
//2 = up
//3 = down
int path[9][31] = 
{{3,1,2,2,2,0,0,2,0,2,0,3,3,0,3,1,3,0,0,3,0,2,2,1,2,2,2,0,2,1,4},//1
{1,1,2,2,0,0,2,0,3,0,2,0,3,3,0,2,2,0,3,3,3,1,3,1,1,1,1,3,4,4,4},//2
{1,2,1,1,2,2,0,0,0,0,3,3,3,3,3,3,1,1,1,1,2,0,2,2,1,3,1,3,1,4,4},//3
{1,2,0,2,1,2,2,1,2,0,0,0,3,3,0,3,0,3,3,4,4,4,4,4,4,4,4,4,4,4,4},//4
{3,1,1,2,1,2,1,3,1,3,3,3,3,3,0,3,0,0,2,4,4,4,4,4,4,4,4,4,4,4,4},//5
{3,0,0,2,2,2,2,1,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},//6
{3,0,2,0,3,0,0,2,2,2,1,2,0,0,0,0,3,3,1,4,4,4,4,4,4,4,4,4,4,4,4},//7
{1,2,0,0,0,2,2,2,2,0,0,0,3,3,3,3,3,3,3,1,1,2,0,2,1,1,4,4,4,4,4},//8
{3,3,3,0,0,3,1,1,1,1,2,0,2,2,2,2,2,2,1,3,1,1,1,3,0,3,1,3,3,3,3}};//9

int startPos[] = {48,25,37,41,13,36,23,41,28};
int endPos[] = {14,41,48,44,50,14,21,43,55};
int hint1[] = {10,27,19,16,26,21,10,1,49};
int hint2[] = {24,55,33,46,49,42,44,21,38};
int numStrikes;
int numSolved;

int hintLEDs[] = {false, false, false, false};

int hintCase;

void setup() {
  // put your setup code here, to run once:
  Wire.begin(i2cAddress);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

  Serial.begin(9600);
  pinMode(greenLEDPin,OUTPUT);
  pinMode(upPin, INPUT);
  pinMode(downPin, INPUT);
  pinMode(rightPin, INPUT);
  pinMode(leftPin, INPUT);
  
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
  #endif
  
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  
  pixels.setBrightness(10);

 // initModule();
 
}

void loop() {
  pixels.show();
  // put your main code here, to run repeatedly:
    if (gameOn) {
        if (solved == '0') {
        
            if(digitalRead(rightPin) == HIGH){
              if(btnPressed == false){
              Serial.println("right button");
              mvmt = 0;
                if(check(mvmt)){
                pixels.setPixelColor(pos, pixels.Color(0, 0, 0));
                pos++;
                pixels.setPixelColor(pos, pixels.Color(0, 0, 150));
                }
              }
              btnPressed = true;
            }
            else if(digitalRead(leftPin) == HIGH){
              if(btnPressed == false){
              Serial.println("left button");
              mvmt = 1;
                if(check(mvmt)){
                pixels.setPixelColor(pos, pixels.Color(0, 0, 0));
                pos--;
                pixels.setPixelColor(pos, pixels.Color(0, 0, 150));
                }
              }
              btnPressed = true;
            }
            else if(digitalRead(upPin) == HIGH){
              if(btnPressed == false){
              Serial.println("up button");
              mvmt = 2;
                if(check(mvmt)){
                  if(pos>=18 && pos<=22){
                    pixels.setPixelColor(pos, pixels.Color(0, 0, 0));
                    pos-=7;
                    pixels.setPixelColor(pos, pixels.Color(0, 0, 150));
                  }else{
                    pixels.setPixelColor(pos, pixels.Color(0, 0, 0));
                    pos-=8;
                    pixels.setPixelColor(pos, pixels.Color(0, 0, 150));
                  } 
                }
              }
              btnPressed = true;
            }
            else if(digitalRead(downPin) == HIGH){
              if(btnPressed == false){
              Serial.println("down button");
              Serial.println(pos);
              mvmt = 3;
                if(check(mvmt)){
                  if(pos>=11 && pos<=15){
                    pixels.setPixelColor(pos, pixels.Color(0, 0, 0));
                    pos+=7;
                    pixels.setPixelColor(pos, pixels.Color(0, 0, 150));
                  }else{
                    pixels.setPixelColor(pos, pixels.Color(0, 0, 0));
                    pos+=8;
                    pixels.setPixelColor(pos, pixels.Color(0, 0, 150));
                  }
                }
                Serial.println(pos);
              }
                 btnPressed = true;
            }
            else
                btnPressed = false;
            
      
          }
          if(pos == endPos[pathNum]){
            solved = '1';
            digitalWrite(greenLEDPin, HIGH);
          }
          
        
    }

}

bool check(int a){
  if( a == path[pathNum][mvmtIndex]){
    mvmtIndex++;
    return true;
  }else{
    newStrike = '1';   
    Serial.println("wrong path!");     
    return false;
  }
}

void initModule() {
  numStrikes = 0;
  numSolved = 0;
  solved = '0';
  newStrike = '0';
  btnPressed = false;
  digitalWrite(greenLEDPin, LOW);
  //Detect which test case it is
  hintCase = detectCase();
  pathNum = detectPath();
  pos = startPos[pathNum];
  Serial.print("hint:");
  Serial.println(hintCase);
  Serial.print("path:");
  Serial.println(pathNum);
  gameOn = true;
  pixels.clear();
  pixels.setPixelColor(pos, pixels.Color(0, 0, 150));//player start position
  pixels.setPixelColor(hint1[pathNum], pixels.Color(0, 150, 0));//hint 1 position
  pixels.setPixelColor(hint2[pathNum], pixels.Color(0, 150, 0));//hint 2 position
  mvmtIndex = 0;
  
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
    digitalWrite(greenLEDPin, LOW);
  }
}

int detectCase(){
  int fir = hintLEDs[0];
  int sec = hintLEDs[1]*2;
  int thi = hintLEDs[2]*4;
  int fort = hintLEDs[3]*8;

  return fir+sec+thi+fort;
}

int detectPath(){
  if( 0<= hintCase && hintCase <=2){
    return 0;
  }else if(3<= hintCase && hintCase <=4){
    return 1;
  }
  else if(5<= hintCase && hintCase <=6){
    return 2;
  }
  else if(7<= hintCase && hintCase <=8){
    return 3;
  }
  else if(9<= hintCase && hintCase <=10){
    return 4;
  }
  else if(11<= hintCase && hintCase <=12){
    return 6;
  }
  else if(hintCase == 13){
    return 7;
  }
  else if(hintCase == 14){
    return 8;
  }
  else if(hintCase == 15){
    return 5;
  }
}

