/*
 *  timer.c
 *
 *  Created on: May 21, 2019
 *      Author: soren
 */

#include "sttimer.h"
#include <tm4c123gh6pm.h>
#include <stdint.h>

void initSysTickTimer(int timeInUs){
    int ticks = timeInUs * 16;
    SYSTICK_RELOAD_R = ticks; //default, which leads to 1 second (almost maximum)
    SYSTICK_CTRL_R = 5; //enable, no interrupt, using system clock of 16MHz
}

// should be in the range of 1 to 1,000,000
void setSysTickInterval(int timeInUs){ //time in microseconds
    int ticks = timeInUs * 16;
    SYSTICK_RELOAD_R = ticks;
}
