/*
 Changes:
 Code changed to a State Machine
 Addition of Code Reset
 */

int rgbRed = 41;
int rgbGreen = 40;
int rgbBlue = 43;

int btnAIn = 15;

const int btn1 = 1;
const int btn2 = 2;
const int btn3 = 3;
const int btn4 = 4;

const int btn4Low = 680;
const int btn4High = 687;
const int btn3Low = 820;
const int btn3High = 826;
const int btn2Low = 944;
const int btn2High = 951;
const int btn1Low = 1010;
const int btn1High = 1025;

const int codeLength = 6;
int lockCode[codeLength] = {btn1, btn1, btn2, btn3, btn4, btn4}; //fixed lockCode
int code[codeLength];
int codeI = 0;

int resetCount = 0;
int resetLim = 3;
int resetCurr = 0;

#define SNorm 0
#define SBtnPres 1
#define SCodeRst 2
#define SCodeCorrect 3
#define SCodeIncorrect 4

int state;

void setup(){
  Serial.begin(9600);
  pinMode(rgbRed,OUTPUT);
  pinMode(rgbBlue,OUTPUT);
  pinMode(rgbGreen,OUTPUT);
  pinMode(btnAIn,INPUT);
  state = SNorm;
}

int tmpBtnState = LOW;
void loop(){
  switch(state){
  case SNorm:
    {
      int btnReading = analogRead(btnAIn);
      Serial.println(btnReading);
      tmpBtnState = ButtonIdentifier(btnReading);
      if(tmpBtnState!=LOW) state = SBtnPres;
      break;
    }

  case SBtnPres:
    {
      Serial.println(tmpBtnState);
      digitalWrite(rgbBlue,HIGH);
      delay(10);
      digitalWrite(rgbBlue,LOW);
      code[codeI] = (tmpBtnState);
      codeI++;
      if(codeI == codeLength){
        if(array_cmp(code,lockCode)){
          state = SCodeCorrect;
        }
        else{
          state = SCodeIncorrect;
        }
        codeI=0;
        memset(code,0,codeLength);
      }
      else{
        state = SCodeRst;
      }
      WaitForBtnRelease();
      break;

    case SCodeRst:
      {
        Serial.println("activated");
        unsigned long btnUpT = millis();
        Serial.println(btnUpT);
        while(true){
          int btnReading = analogRead(btnAIn);  
          int tmpBtnState = ButtonIdentifier(btnReading);
          //   Serial.println(tmpBtnState);
          if(tmpBtnState !=LOW){
            state = SBtnPres;
            break;
          }
          if(millis() > (btnUpT + 5000)){
            memset(code,0,codeLength);
            codeI=0;
            state = SNorm;
            break;
          }  
        }
        break;
      }
    case SCodeCorrect:
      {
        resetCurr=0;
        digitalWrite(rgbGreen,HIGH);
        unsigned long startT = millis();
        while(millis()< (startT +2000)){
          int lockChngButton = LOW;
          lockChngButton = ButtonIdentifier(analogRead(btnAIn));
          if(lockChngButton==btn4){
            ChangeCode();   
          }
        }
        digitalWrite(rgbGreen,LOW);
        state = SNorm;
        break;
      }

    case SCodeIncorrect:
      {
        digitalWrite(rgbRed,HIGH);
        resetCurr++;
        if(resetCurr==resetLim){
          resetCount++;
          delay(10000 * resetCount);
          digitalWrite(rgbRed,LOW);
          resetCurr=0;
        }
        else{
          delay(2000);
          digitalWrite(rgbRed,LOW);
        }
        state = SNorm;
        break;
      }
    }
  }
}

boolean array_cmp(int a[], int b[]){
  int n;

  // test each element to be the same. if not, return false
  for (n=0;n<4;n++) if (a[n]!=b[n]) return false;

  //ok, if we have not returned yet, they are equal :)
  return true;
}

int ButtonIdentifier(int Reading){
  if(Reading > btn1Low && Reading < btn1High){
    return btn1;
  }
  else if(Reading > btn2Low && Reading < btn2High){
    return btn2;
  }
  else if(Reading > btn3Low && Reading < btn3High){
    return btn3;
  }
  else if(Reading > btn4Low && Reading < btn4High){
    return btn4;
  }
  else{
    //no button pressed
    return LOW;
  }
}


void ChangeCode(){
  //Serial.println("at ChangeCode");
  for(int x=0;x<2;x++){
    digitalWrite(rgbGreen,HIGH);
    digitalWrite(rgbRed,HIGH);
    delay(200);
    digitalWrite(rgbGreen,LOW);
    digitalWrite(rgbRed,LOW);
    delay(100);
  }
  for(int j=0; j<codeLength; j++){
    int btnPress = LOW;
    while(btnPress==LOW){
      //   Serial.print(" waiting");
      btnPress = ButtonIdentifier(analogRead(btnAIn));
    }
    //   Serial.print("\nnew code at ");
    //  Serial.print(j);
    //  Serial.print(" ");
    //Serial.println(btnPress);
    lockCode[j]=btnPress;  
    WaitForBtnRelease();
  }
}

void WaitForBtnRelease(){
  int btnReading=1;
  while(btnReading!=LOW){
    btnReading = analogRead(btnAIn);
  }
}