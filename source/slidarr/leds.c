/*
 * leds.c
 *
 *  Created on: May 20, 2019
 *      Author: mats
 */
#include "leds.h"
#include <stdint.h>
#include "tm4c123gh6pm.h"

void initLEDs() {
    SYSCTL_RCGCGPIO_R |= 0x20;      // enable clock for PORTF
    GPIO_PORTF_DIR_R |= 0x0E;       // set pins 1,2,3 as output
    GPIO_PORTF_DEN_R |= 0x0E;       // enable pin a digital
}

void setLED(int led, int state) {
    switch (led) {
        case 0: RED_LED = (state && 1); break;
        case 1: GREEN_LED = (state && 1); break;
        case 2: BLUE_LED = (state && 1); break;
    }
}


