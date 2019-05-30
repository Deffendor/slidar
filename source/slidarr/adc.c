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
    /////////////////////// Setup ADC1 (AIN1) at pin PE2  //////////////////
    SYSCTL_RCGCGPIO_R |= 0x10;  // Enable clock for port E
    SYSCTL_RCGCADC_R |= 2;      // Enable clock for second adc (ADC1)

    GPIO_PORTE_AFSEL_R |= 4;    // select alternate function
    GPIO_PORTE_DEN_R &= ~4;     // disable digital
    GPIO_PORTE_AMSEL_R |= 4;    // enable analog

    ADC1_ACTSS_R &= ~8;         // disable sequencer for config
    ADC1_EMUX_R &= ~0xF000;     // start conversion by software
    ADC1_SSMUX3_R  = 1;         // choose channel 1
    ADC1_SSCTL3_R |= 6;         // one sample at a time
    ADC1_ACTSS_R |= 8;          // enable sequencer
    /////////////////////// Setup ADC done ///////////////////////////////
}

void readADC(volatile int *val) { // TODO make generic for other ports

    // read adc
    ADC1_PSSI_R |= 8;
    while((ADC1_RIS_R & 8) == 0);
    *val = ADC1_SSFIFO3_R;
    ADC1_ISC_R = 8;
}
