#ifndef CAR_H
#define CAR_H

#include <require_cpp11.h>
#include <MFRC522.h>
#include <deprecated.h>
#include <MFRC522Extended.h>

#include <SPI.h>
#include <MFRC522.h>

#include "linetracer.h"

#define LT_MODULE_L A1
#define LT_MODULE_R A2
#define LT_MODULE_F A0
#define LIGHT	A3
#define LED 	A5
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


class Car {
public:
    Car(int sp, int rotSp);
    void update();
    void detectCard();
    bool isBlocked() const;
    bool isDark() const;
    void sensorUpdate();
    //  Driving
    void right();
    void front();
    void left();
    void stop(); 
private:
    Linetracers lts; // class to control the line tracers
    MFRC522 mfrc522; // RFID module


    bool rotating = false, uTurning = false, rfs = true, parking = false;
    int speed, rotatingSpeed;
    int stop_to_uturn = 5;
    
    static const int cm_threshold = 15;
    static const int light_threshold = 500;
};

#endif