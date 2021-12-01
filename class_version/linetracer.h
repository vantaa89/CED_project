#ifndef LINETRACER_H
#define LINETRACER_H

#include <Arduino.h>

enum car_direction{
	CAR_DIR_FW,
	CAR_DIR_RF,
	CAR_DIR_LF, 
	CAR_DIR_ST,
	CAR_DIR_RR,
	CAR_DIR_LR
};

class Linetracer{
public:
    Linetracer();
    Linetracer(int pin_);
    bool is_on() const;
private:
    int pin;
};


class Linetracers{
public:
    Linetracers();
    Linetracers(Linetracer l, Linetracer f, Linetracer r);
    void lt_mode_update(int stop_to_uturn, bool rfs);
    car_direction get_direction() const;
    car_direction get_prev_direction(int prev) const;
    bool uturn(int stop_to_uturn);
private:
    Linetracer left, front, right;
    static const int MAX_STOP_TO_UTURN = 30;
    car_direction g_carDirection;
    car_direction prevDirections[MAX_STOP_TO_UTURN];
    
    void updateDirections(int stop_to_uturn, car_direction new_direction);
};


#endif
