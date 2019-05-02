/*
 * Purpose: UART Experiment
 * Hardware:  TM4C123 Tiva board
 * Link: https://github.com/jspicer-ltu/Tiva-C-Embedded/tree/master/Experiment5-UART
 *
 * Pin Connection:
 * TM4C123 -------- HC05 Bluetooth Module
 * PE_4 (U5RX) --> TX.Bluetooth
 * PE_5 (U5TX) --> RX.Bluetooth
 * GND ----------> GND
 * PE_3 (PIN6,A0) -> P.Signal
 */

#include "common.h"
#include <stdio.h>

#include "tm4c123gh6pm.h"
#include "src/PLL_Util.h"
#include "src/SysTick_Util.h"
#include "src/GPIO_Util.h"
#include "src/UART_Util.h"

#define RED_LED    (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 1*4)))   //GPIO_PIN_1
#define GREEN_LED  (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 3*4)))   //GPIO_PIN_2
#define BLUE_LED   (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 2*4)))   //GPIO_PIN_3



 // ADC result
volatile uint32_t result =0;
float voltage;
const float resolution = (3.3f / 4096.0f);
uint8_t onesPlace;
uint8_t tenthsPlace =0;

//for board button push 1 and push 2
uint8_t input = 0;
uint8_t isSW1Down = 0;
uint8_t isSW2Down = 0;

void init_uart(){
	PLL_Init80MHz();
	GPIO_PortF_Init();
	GPIO_PortF_EnableDO(PIN_1 | PIN_2 | PIN_3, DRIVE_2MA);
	GPIO_PortF_EnableDI(PIN_4, PULL_UP);
	UART_UART5_Init();
	SysTick_Init();
}

void init_button(){
    // Enable the GPIO port that is used for the on-board LED.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){}
    // Enable the GPIO pin for the LED (PF3-PF1).  Set the direction as output, and enable the GPIO pin for digital function.
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1);
    // PIN_0 is normally locked.  Need to write a special passcode and commit it.
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTF_CR_R = 0x1;
    // Enable the GPIO pin for the switches (PF0 and PF4).  Set the direction as input, and enable the GPIO pin for digital function.
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
    // Enabled the weak pullup for the input pins.
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

void init_adc(){
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
}

int main(){
    init_uart();
    init_adc();
	init_button();
	UART_UART5_WriteString("Lets started \n");
	while(1){
//	    UART_UART5_WriteString("Select number form 1 to 6 \n");
//	    led_v = UART_UART5_ReadChar();
	    input = (uint8_t)GPIOPinRead(GPIO_PORTF_BASE,   GPIO_PIN_0 | GPIO_PIN_4);
        isSW1Down = ~(input >> 4) & 0x01;
        isSW2Down = ~(input) & 0x01;

        if (isSW1Down) {        //button 1 pressed
            RED_LED |= 1;
            BLUE_LED &= 0;
            GREEN_LED &= 0;
        }
        else if(isSW2Down) {    //button 2 pressed
            GREEN_LED |= 1;
            BLUE_LED &= 0;
            RED_LED &= 0;
        }
        else{                   //read from potentiometer
            BLUE_LED |= 1;
            RED_LED &= 0;
            GREEN_LED &= 0;

            // read adc
            ADC0_PSSI_R |= 8;
            while((ADC0_RIS_R & 8) == 0);
            result = ADC0_SSFIFO3_R;
            ADC0_ISC_R = 8;
            voltage = (float)result * resolution;
            onesPlace = (uint8_t)voltage;
            tenthsPlace = (voltage - (float)onesPlace + 0.05f) * 10;
//          sensor_value = (char*) &result;
            UART_OutUDec(tenthsPlace);
            UART_UART5_WriteString("\n");
        }
	    SysTick_Wait10ms(70);
	}
}
