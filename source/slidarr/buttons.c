/*
 * buttons.c
 *
 *  Created on: May 12, 2019
 *      Author: mats
 */

#include <stdint.h>
#include "buttons.h"
#include "tm4c123gh6pm.h"

void initButtons() {
    // Switch 1 (PF0) used for calibration, switch 2 (PF4) used to scroll

    /*RCGCGPIO PORTF 0x20              */
    SYSCTL_RCGCGPIO_R |= 0x20;          //F: activate clock

    /*unlock and confirm, is important at the beginning*/
    GPIO_PORTF_LOCK_R = 0x4C4F434B;     //F: unlock
    GPIO_PORTF_CR_R = 0x1F;             //F: confirm

    /* Port F -> PF0 and PF4*/
    GPIO_PORTF_DEN_R |= 0x11;           //F: Enable 0x11 same as (1<<PF0)|(1<<PF4);
    GPIO_PORTF_DIR_R &= ~0x11;          //F: input 0 for PF0 and PF4
    GPIO_PORTF_PUR_R |= 0x11;           //F: enable pull-up resistor
}

int readButton(int button) {
    if(button == 0)
        return (GPIO_PORTF_DATA_R & 0x01) == 0;    //PF0 0000 0001 is on
    else if(button == 1)
        return (GPIO_PORTF_DATA_R & 0x10) == 0;    //PF4 0001 0000 is on
    else
   return 0;
}
