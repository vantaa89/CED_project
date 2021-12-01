#include "car.h"


namespace{
    inline long microsecondsToCentimeters(long microseconds){
        return microseconds / 29 / 2;
    }
}


Car::Car(int sp=0, int rotSp=0){
    lts = Linetracers(LT_MODULE_L, LT_MODULE_F, LT_MODULE_R);
    speed = sp;
    rotatingSpeed = rotSp;
    Serial.begin(9600);
    mfrc522 = MFRC522(SS, RST);
    mfrc522.PCD_Init();
	pinMode(TRIGGER, OUTPUT);
	pinMode(ECHO, INPUT);
}

void Car::left(){
    digitalWrite(EN1, LOW);
    digitalWrite(EN2, HIGH);
    digitalWrite(ENA, rotatingSpeed);
    digitalWrite(EN3, LOW);
    digitalWrite(EN4, HIGH);
    digitalWrite(ENB, rotatingSpeed);
}

void Car::front(){
    digitalWrite(EN1, HIGH); 
    digitalWrite(EN2, LOW); 
    analogWrite(ENA, speed);
    digitalWrite(EN3, LOW); 
    digitalWrite(EN4, HIGH); 
    analogWrite(ENB, speed);
}

void Car::right(){
    digitalWrite(EN1, HIGH);
    digitalWrite(EN2, LOW);
    digitalWrite(ENA, rotatingSpeed);
    digitalWrite(EN3, HIGH);
    digitalWrite(EN4, LOW);
    digitalWrite(ENB, rotatingSpeed);
}

void Car::stop(){
    digitalWrite(EN1, LOW);
    digitalWrite(EN2, LOW);
    digitalWrite(ENA, 0);
    digitalWrite(EN3, LOW);
    digitalWrite(EN4, LOW);
    digitalWrite(ENB, 0);
}

void Car::update(){
    lts.lt_mode_update(stop_to_uturn, rfs);
    car_direction g_carDirection = lts.get_direction();
    

    if(g_carDirection != CAR_DIR_ST){
		if(uTurning){
			Serial.println("uturn off");
		}
		if(rotating){
			Serial.println("rotation off");
		}
		
		uTurning = false;
		rotating = false;
	}
    
    if(uTurning){
        left();
    }
    else if(rotating){
        if(rfs){
            right();
        }
        else{
            left();
        }
    }
    else if(g_carDirection == CAR_DIR_FW){
        Serial.println("Front");
		if(rfs && lts.get_prev_direction(1) == CAR_DIR_RR){
			rotating = true;
			Serial.println("Rotation on");
		}
		if(!rfs && lts.get_prev_direction(1) == CAR_DIR_LR){
			rotating = true;
			Serial.println("Rotation on");
		}

        front();
    }
    else if(g_carDirection == CAR_DIR_LF){
        Serial.println("Left");
        left();
    }
    else if(g_carDirection == CAR_DIR_RF){
        Serial.println("Right");
        right();
    }
    else if(g_carDirection == CAR_DIR_LR){
        Serial.println("Left Rotation");
        left();
    }
    else if(g_carDirection == CAR_DIR_RR){
        Serial.println("Right Rotation");
        right();
    }
    else if(g_carDirection == CAR_DIR_ST){
        // stop();
        Serial.println("Stop");
        if(lts.uturn(stop_to_uturn)){
			Serial.println("Uturn on");
			uTurning = true;
		}
    }
}

// Reads card and changes the value of the variables rfs and parking
void Car::detectCard(){
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
	byte size = 2 * sizeof(buffer);
	status = mfrc522.MIFARE_Read(block, buffer, &size);
	Serial.println("Card tagged");
	Serial.println(buffer[0]);
	if (buffer[0] == 0x21) {
		Serial.println(buffer[0]);
		byte mode = buffer[1];
		switch (mode) {
		case 0x00:
			rfs = true;
			parking = false;
			break;
		case 0x01:
			rfs = false;
			parking = false;
			break;
		case 0x02:
			parking = true;
			break;
		}
	}
	mfrc522.PICC_HaltA();
	mfrc522.PCD_StopCrypto1();
}

bool Car::isBlocked() const {
    long duration, cm;
	digitalWrite(TRIGGER, HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIGGER, LOW);
	duration = pulseIn(ECHO, HIGH);
	cm = microsecondsToCentimeters(duration);
	//Serial.print(cm);
	//Serial.println("cm");
	// Serial.println();
	if(0 < cm && cm < cm_threshold) return true;
	return false;
}

bool Car::isDark() const {
    int light_sensor = analogRead(LIGHT);
	//Serial.println(light_sensor);
	if(light_sensor > light_threshold){
		return true;
	}
	return false;
}

void Car::sensorUpdate() {
    lts.lt_mode_update(stop_to_uturn, rfs);
    detectCard();
    // for debugging
    Serial.print("rfs = ");
	Serial.println(rfs);
}
