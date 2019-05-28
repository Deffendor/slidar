/*
 *  titimer.c
 *
 *  Created on: May 21, 2019
 *      Author: soren
 */

#include "titimer.h"
#include <tm4c123gh6pm.h>
#include <stdint.h>

void initTiTimer(int timeInUs){
    SYSCTL_RCGCTIMER_R |= 1;    // enable clock

    TIMER0_CTL_R = 0;           // disable to configure
    TIMER0_CFG_R = 0x04;        // use 16 bit
    TIMER0_TAMR_R = 0x01;       // one-shot mode

    // setting time, prescaler at 250-1 with 64000-1 leads to ~1 second
    // to have 50ms for the button debounce use 160 and 5000 at 16MHz
    TIMER0_TAILR_R = 5000 -1;   // counter
    TIMER0_TAPR_R = 160 - 1;    // prescaler
    TIMER0_ICR_R = 0x01;        // clear interrupt flag for A0

    // setup interrupts
    NVIC_PRI4_R = 0xE0;         // set priority level to 7
    NVIC_EN0_R |= 0x00080000;   // enable interrupt 19

    TIMER0_IMR_R |= 0x01;       // enable timeout interrupt

    // example of how to start, this will be done when the buttons are pressed for the slidar
    //TIMER0_CTL_R |= 0x01; // enable after config
}
