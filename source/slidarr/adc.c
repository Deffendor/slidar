/*
 * adc.c
 *
 *  Created on: May 12, 2019
 *      Author: mats
 */
#include <tm4c123gh6pm.h>
#include <stdint.h>

#include "adc.h"

void init_ADC() { // TODO make generic for other ports
    /////////////////////// Setup ADC0 (AIN0) at pin PE3  //////////////////
    //SYSCTL->RCGCGPIO |= 0x10;
    // TODO: add comments and use better names, also for the ADC stuff a in the endless main while loop
    SYSCTL_RCGCGPIO_R |= 0x10;
    SYSCTL_RCGCADC_R |= 1;

    GPIO_PORTE_AFSEL_R |= 8;
    GPIO_PORTE_DEN_R &= ~8;
    GPIO_PORTE_AMSEL_R |= 8;

    ADC0_ACTSS_R &= ~8;
    ADC0_EMUX_R &= ~0xF000;
    ADC0_SSMUX3_R  = 0;
    ADC0_SSCTL3_R |= 6;
    ADC0_ACTSS_R |= 8;
    /////////////////////// Setup ADC done ///////////////////////////////
}

void read_ADC(volatile int *val) { // TODO make generic for other ports
    int current_val;

    // read adc
    ADC0_PSSI_R |= 8;
    while((ADC0_RIS_R & 8) == 0);
    *val = ADC0_SSFIFO3_R;
    ADC0_ISC_R = 8;
}



