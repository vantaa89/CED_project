#include <require_cpp11.h>
#include <MFRC522.h>
#include <deprecated.h>
#include <MFRC522Extended.h>

#include <SPI.h>
#include <MFRC522.h>


#define LT_MODULE_L A1
#define LT_MODULE_R A2
#define LT_MODULE_F A0
#define LIGHT  A3
#define LED   A5
#define SS 10
#define RST 9
#define TRIGGER A4
#define ECHO 8
#define ENA 6
#define EN1 7
#define EN2 3
#define EN3 4
#define EN4 2
#define ENB 5


#define STOP_TO_U_TURN 20

enum car_direction{
  CAR_DIR_FW,
  CAR_DIR_RF,
  CAR_DIR_LF, 
  CAR_DIR_ST,
  CAR_DIR_RR,
  CAR_DIR_LR
};

MFRC522 mfrc522(SS, RST);

int mode = 1;

car_direction prevDirections[STOP_TO_U_TURN];
car_direction g_carDirection;

int speed = 80;
int rotatingSpeed = 100;
int refreshInterval = 10;

bool uTurning = false;
bool lightOff = false;
bool proximity = false;
const int light_threshold = 500;

void init_line_tracer_modules(){
  pinMode(LT_MODULE_L, INPUT);
  pinMode(LT_MODULE_F, INPUT);
  pinMode(LT_MODULE_R, INPUT);
}

bool lt_isLeft(){
  int ret = digitalRead(LT_MODULE_L);
  return ret;
}

bool lt_isForward(){
  int ret = digitalRead(LT_MODULE_F);
  return ret;
}

bool lt_isRight(){
  int ret = digitalRead(LT_MODULE_R);
  return ret;
}

void updateDirections(car_direction* l, int listLength, car_direction new_direction){
  for(int i = listLength - 1; i > 0; --i) {
    l[i] = l[i-1];
  }
  l[0] = new_direction;
}

void lt_mode_update(){
  bool ll = lt_isLeft();
  bool ff = lt_isForward();
  bool rr = lt_isRight();
  if(mode==1){ // RFS

    if (!ll&&!ff&&!rr){ // 000
      g_carDirection = CAR_DIR_ST;
    }
    else if (rr){   // 001
      if(ff){ // rotation
        g_carDirection = CAR_DIR_RR;
      }
      else {
        g_carDirection = CAR_DIR_RF;
      }
    }
    else if (ll){   // 100
      if(ff){// rotation
        g_carDirection = CAR_DIR_LR;
      }
      else {
        g_carDirection = CAR_DIR_LF;
      }
    }
    else if (ff){   // 010
      g_carDirection = CAR_DIR_FW;
    }
  }
  
  else if(mode==2){ // LFS
    if (!ll&&!ff&&!rr){ // 000
      g_carDirection = CAR_DIR_ST;
    }
    else if (ll){   // 100
      if(ff){// rotation
        g_carDirection = CAR_DIR_LR;
      }
      else {
        g_carDirection = CAR_DIR_LF;
      }
    }
    else if (rr){   // 001
      if(ff){ // rotation
        g_carDirection = CAR_DIR_RR;
      }
      else {
        g_carDirection = CAR_DIR_RF;
      }
    }
    else if (ff){   // 010
      g_carDirection = CAR_DIR_FW;
    }
  }
  
  updateDirections(prevDirections, STOP_TO_U_TURN, g_carDirection);
}

void car_update(){
  if(g_carDirection != CAR_DIR_ST){
    if(uTurning){
      Serial.println("uturn off");
      uTurning = false;
    }
  }
  if(uTurning){
    digitalWrite(EN1, LOW);
    digitalWrite(EN2, HIGH);
    analogWrite(ENA, rotatingSpeed);
    digitalWrite(EN3, LOW);
    digitalWrite(EN4, HIGH);
    analogWrite(ENB, rotatingSpeed);
  }
  else if (g_carDirection == CAR_DIR_FW){
    Serial.println("Front");
    digitalWrite(EN1, HIGH); 
    digitalWrite(EN2, LOW); 
    analogWrite(ENA, speed);
    digitalWrite(EN3, LOW); 
    digitalWrite(EN4, HIGH); 
    analogWrite(ENB, speed);
  }

  else if (g_carDirection == CAR_DIR_LF){
    Serial.println("Left");
    digitalWrite(EN1, LOW);
    digitalWrite(EN2, HIGH);
    analogWrite(ENA, rotatingSpeed);
    digitalWrite(EN3, LOW);
    digitalWrite(EN4, HIGH);
    analogWrite(ENB, rotatingSpeed);    
  }

  else if (g_carDirection == CAR_DIR_RF){
    Serial.println("Right");
    digitalWrite(EN1, HIGH);
    digitalWrite(EN2, LOW);
    analogWrite(ENA, rotatingSpeed);
    digitalWrite(EN3, HIGH);
    digitalWrite(EN4, LOW);
    analogWrite(ENB, rotatingSpeed);    
  }

  else if (g_carDirection == CAR_DIR_LR){
    Serial.println("Left rotation");
    digitalWrite(EN1, LOW);
    digitalWrite(EN2, HIGH);
    digitalWrite(ENA, rotatingSpeed);
    digitalWrite(EN3, LOW);
    digitalWrite(EN4, HIGH);
    digitalWrite(ENB, rotatingSpeed);
  }

  else if (g_carDirection == CAR_DIR_RR){
    Serial.println("Right rotation");
/*    Serial.println("Front");
    digitalWrite(EN1, HIGH); 
    digitalWrite(EN2, LOW); 
    analogWrite(ENA, speed);
    digitalWrite(EN3, LOW); 
    digitalWrite(EN4, HIGH); 
    analogWrite(ENB, speed);
    delay(200);*/
    digitalWrite(EN1, HIGH);
    digitalWrite(EN2, LOW);
    digitalWrite(ENA, rotatingSpeed);
    digitalWrite(EN3, HIGH);
    digitalWrite(EN4, LOW);
    digitalWrite(ENB, rotatingSpeed);
  }
  
  else if (g_carDirection == CAR_DIR_ST){
    // blank
    Serial.println("Stop");
    
    if(uTurn(prevDirections, STOP_TO_U_TURN)){
        uTurning = true;
      Serial.println("uturn on");
    }
    
  }
}

bool uTurn(car_direction* l, int listLength){
  bool ret = true;
  for(int i = 0; i < listLength; ++i){
    if(l[i] != CAR_DIR_ST) ret = false;
  }
  return ret;
}


void setup() {
  Serial.begin(9600);
  SPI.begin();
    mfrc522.PCD_Init();
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  
}

void loop() {
  if(!lightOff && !proximity) {
    car_update();
  }
  else { // 암실이거나 앞에 물체 감지
    digitalWrite(EN1, LOW);
    digitalWrite(EN2, LOW);
    digitalWrite(ENA, 0);
    digitalWrite(EN3, LOW);
    digitalWrite(EN4, LOW);
    digitalWrite(ENB, 0);
  }
  delay(refreshInterval);
  lt_mode_update();
  get_light();
  checkUltrasonic();
}


void get_light(){
  int light_sensor = analogRead(LIGHT);
  //Serial.println(light_sensor);
  if(light_sensor > light_threshold){
    lightOff = true;
  }
  else lightOff = false;
}

void detectCard(){
  if(!mfrc522.PICC_IsNewCardPresent()){
    return;
  }
  // RFID card detected
  if(!mfrc522.PICC_ReadCardSerial()){
    return;
  }
  Serial.println("new card present");
  MFRC522::MIFARE_Key key;
  for(int i = 0; i < 6; ++i){
    key.keyByte[i] = 0xff;
  } 
  byte buffer[16];
  byte block = 6;
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  byte size = 2*sizeof(buffer);
  status = mfrc522.MIFARE_Read(block, buffer, &size);
  Serial.println("Card tagged");
  Serial.println(buffer[0]);
  if (buffer[0] == 0x21) {
    Serial.println(buffer[0]);
    byte mode = buffer[1];
    switch (mode) {
    case 0x00:
      mode=1;
      break;
    case 0x01:
      mode=2;
      break;
    case 0x02:
      mode=3;
      break;
    }
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void checkUltrasonic(){
  long duration, cm;
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  cm = microsecondsToCentimeters(duration);
  //Serial.print(cm);
  //Serial.println("cm");
  // Serial.println();
  if(0 < cm && cm < 15) proximity = true;
  else proximity = false;
}

inline long microsecondsToCentimeters(long microseconds){
  return microseconds / 29 / 2;
}