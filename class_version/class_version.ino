#include "car.h"

const int refreshInterval = 50;
int speed = 90, rotatingSpeed = 120;
Car car;

void setup(){
    car = Car(speed, rotatingSpeed);
}

void loop(){
    if(!car.isDark() && !car.isBlocked()){
        car.update();
    }
    else{
        car.stop();
    }
    delay(refreshInterval);
    car.sensorUpdate();
}