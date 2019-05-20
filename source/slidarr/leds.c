/*
 * leds.c
 *
 *  Created on: May 20, 2019
 *      Author: mats
 */
#include "leds.h"
#include <stdint.h>

void initLEDs() {


}

void setLED(int led, int state) {
    switch (led) {
        case 0: RED_LED = (state && 1); break;
        case 1: GREEN_LED = (state && 1); break;
        case 2: BLUE_LED = (state && 1); break;
    }
}


