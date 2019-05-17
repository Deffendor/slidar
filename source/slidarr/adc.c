/*
 * adc.c
 *
 *  Created on: May 12, 2019
 *      Author: mats
 */
#include <tm4c123gh6pm.h>
#include <stdint.h>

#include "adc.h"

void initADC() { // TODO make generic for other ports
    /////////////////////// Setup ADC0 (AIN0) at pin PE3  //////////////////
    //SYSCTL->RCGCGPIO |= 0x10;
    // TODO: add comments and use better names, also for the ADC stuff a in the endless main while loop
    SYSCTL_RCGCGPIO_R |= 0x10;
    SYSCTL_RCGCADC_R |= 2;

    GPIO_PORTE_AFSEL_R |= 4;
    GPIO_PORTE_DEN_R &= ~4;
    GPIO_PORTE_AMSEL_R |= 4;

    ADC1_ACTSS_R &= ~8;
    ADC1_EMUX_R &= ~0xF000;
    ADC1_SSMUX3_R  = 1;
    ADC1_SSCTL3_R |= 6;
    ADC1_ACTSS_R |= 8;
    /////////////////////// Setup ADC done ///////////////////////////////
}

void readADC(volatile int *val) { // TODO make generic for other ports

    // read adc
    ADC1_PSSI_R |= 8;
    while((ADC1_RIS_R & 8) == 0);
    *val = ADC1_SSFIFO3_R;
    ADC1_ISC_R = 8;
}



