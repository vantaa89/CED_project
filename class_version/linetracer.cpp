#include "linetracer.h"



Linetracer::Linetracer(): pin(-1) {
    // blank
}

Linetracer::Linetracer(int pin_){
    pin = pin_;
    pinMode(pin, INPUT);
}

bool Linetracer::is_on() const{
    return digitalRead(pin);
}


Linetracers::Linetracers(){

}

Linetracers::Linetracers(Linetracer l, Linetracer f, Linetracer r){
    left = l, front = f, right = r;
}

void Linetracers::lt_mode_update(int stop_to_uturn, bool rfs){
    bool ll = left.is_on();
    bool ff = front.is_on();
    bool rr = front.is_on();
    if(rfs){
        if (!ll&&!ff&&!rr){ // 000
			g_carDirection = CAR_DIR_ST;
			Serial.println("ST");
		}
		else if (rr){   // 001
			if(ff){ // rotation
				g_carDirection = CAR_DIR_RR;
				Serial.println("RR");
			}
			else {
				g_carDirection = CAR_DIR_RF;
				Serial.println("RF");
			}
		}
		else if (ll){   // 100
			if(ff){// rotation
				g_carDirection = CAR_DIR_LR;
				Serial.println("LR");
			}
			else {
				g_carDirection = CAR_DIR_LF;
				Serial.println("LF");
			}
		}
		else if (ff){   // 010
			g_carDirection = CAR_DIR_FW;
			Serial.println("FW");
		}
    }
    else{
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

    updateDirections(stop_to_uturn, g_carDirection);
    
}

void Linetracers::updateDirections(int stop_to_uturn, car_direction new_direction){
    for(int i = stop_to_uturn - 1; i > 0; --i) {
		prevDirections[i] = prevDirections[i-1];
	}
	prevDirections[0] = new_direction;
}

bool Linetracers::uturn(int stop_to_uturn){
	bool ret = true;
	for(int i = 0; i < stop_to_uturn; ++i){
		if(prevDirections[i] != CAR_DIR_ST) ret = false;
	}
	return ret;
}

car_direction Linetracers::get_direction() const{
    return g_carDirection;
}

car_direction Linetracers::get_prev_direction(int prev) const{
    return prevDirections[prev];
}