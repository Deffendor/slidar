#include <stdint.h>
#include <tm4c123gh6pm.h>

/**
 * main.c
 */
int main(void)
{
    volatile int result;

    //SYSCTL->RCGCGPIO |= 0x10;
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


    while(1){
        ADC0_PSSI_R |= 8;
        while((ADC0_RIS_R & 8) == 0);
        result = ADC0_SSFIFO3_R;
        //ToDo: print to uart, can be read in debug mode atm
        ADC0_ISC_R = 8;
    }

	return 0;
}
