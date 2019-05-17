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
    /*RCGCGPIO PORTF 0x20, PORTD 0x08               */
    SYSCTL_RCGCGPIO_R |= 0x28;      //F & D: activate clock for Both 0x28

    /*unlock and confirm is important at the begging*/
    GPIO_PORTF_LOCK_R = 0x4C4F434B; //F: unlock
    GPIO_PORTF_CR_R = 0x1F;         //F: confirm
    GPIO_PORTD_LOCK_R = 0x4C4F434B; //D: unlock
    GPIO_PORTD_CR_R = 0x0C;         //D: confirm
    /*Port F -> PF0 and PF4 & Port D -> PD2 and PD3 */
    GPIO_PORTF_DEN_R = 0x1F;        //F: Enable 0x1F same as (1<<PF0)|(1<<PF4);
    GPIO_PORTD_DEN_R = 0x0C;        //D: Enable
    GPIO_PORTF_DIR_R = 0x0E;        //F: input 0 for PF0 and PF4, output 1 for LED's
    GPIO_PORTD_DIR_R = 0x00;        //D: input 0 for PD2 and PD3
    GPIO_PORTF_PUR_R = 0x11;        //F: Pull-up resistor
    GPIO_PORTF_AFSEL_R = 0;         //F: just as I/O pin
    GPIO_PORTD_AFSEL_R = 0;         //D: just as I/O pin
    GPIO_PORTF_PCTL_R = 0;          //F: No set of any peripheral function
    GPIO_PORTD_PCTL_R = 0;          //D: No set of any peripheral function
    GPIO_PORTF_AMSEL_R = 0;         //F: ADC is disabled
    GPIO_PORTD_AMSEL_R = 0;         //D: ADC is disabled
    /*Interrupt for all 4 buttons PORTF and PORTD   */
    /*GPIO_PORTF_IM_R = 0x00;         //F: Mask, clearing the register
    GPIO_PORTD_IM_R = 0x00;         //D: Mask, clearing the register
    GPIO_PORTF_IS_R &= ~0x11;       //F: At edge-sensitive &= ~0x11;
    GPIO_PORTD_IS_R &= ~0x0C;       //D: At edge-sensitive not Level-sensitive
    GPIO_PORTF_IBE_R &= ~0x11;      //F: Interrupt generation by IEV
    GPIO_PORTD_IBE_R &= ~0x0C;      //D: Interrupt generation by IEV
    GPIO_PORTF_IEV_R &= ~0x11;      //F: Detecting falling edge or a Low level
    GPIO_PORTD_IEV_R &= ~0x0C;      //D: Detecting falling edge or a Low level
    GPIO_PORTF_ICR_R = 0x11;        //F: Flag - interrupt is cleared
    GPIO_PORTD_ICR_R = 0x0C;        //D: Flag - interrupt is cleared
    GPIO_PORTF_IM_R |= 0x11;        //F: ARM interrupt - Unmask
    GPIO_PORTD_IM_R |= 0x0C;        //D: ARM interrupt - Unmask
    /* NVIC - Exception Handlers - Configure Interrupt*/
    /*
    NVIC_PRI7_R =  (NVIC_PRI7_R & 0xFF00FFFF) | 0x00A00000; //F: Interrupt Priority 5 - Interrupt 2
    NVIC_PRI0_R =  (NVIC_PRI0_R & 0x00FFFFFF) | 0xA0000000; //D: Interrupt Priority 7 - Interrupt 31
    NVIC_EN0_R = 0x40000008;        //IRQn: Enable interrupt F->30 and D->3
    */
}

int readButton(int button) {
    if(button == 0)
        return (GPIO_PORTF_DATA_R & 0x01) == 0;    //PF0 0000 0001 is on
    else if(button == 1)
        return (GPIO_PORTF_DATA_R & 0x10) == 0;    //PF4 0001 0000 is on
    else if(button == 2)
        return (GPIO_PORTD_DATA_R & 0x04) == 0;    //PD2 0000 0100 is on
    else if(button == 3)
        return (GPIO_PORTD_DATA_R & 0x08) == 0;    //PD3 0000 1000 is on
    else
   return 0;
}
