/*
 * uart.c
 *
 *  Created on: May 12, 2019
 *      Author: mats
 */

#include "uart.h"
#include <tm4c123gh6pm.h>
#include <stdint.h>

void init_UART() {

    /////////////////////// Setup UART4 //////////////////////////////////
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R4; // UART4 clock
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2; // PortC clock
    UART4_CTL_R = UART4_CTL_R & ~UART_CTL_UARTEN; // Enable UART4
    // After enabling, might need to wait for end of transmission
    // and flush the FIFO like stated in the datasheet page 918

    // Setting the BaudRate
    // BaudRate = SysClk / (16 x ClkDiv)
    // so at 9600 BaudRate and Clock at 16MHz(default)
    // -> 104.1666666
    // UARTIBRD = 104
    // UARTFBRD = 0.166666 x 64 + 0.5 (0.5 is to round) -> 11
    UART4_IBRD_R = 104; //8 for 115200
    UART4_FBRD_R = 11;  //44 for 115200

    UART4_CC_R = 0; // Select UART4 clock source (internal in this case)
    UART4_LCRH_R = 96; //or 0x60 / 1 stop bit, no FIFO, no parity, 8 bit data

    UART4_CTL_R = UART4_CTL_R | UART_CTL_RXE | UART_CTL_TXE; // Enable Tx and Rx
    UART4_CTL_R = UART4_CTL_R | UART_CTL_UARTEN; // Enable UART again

    GPIO_PORTC_DEN_R = GPIO_PORTC_DEN_R | 0x30; // Set DEN
    GPIO_PORTC_AFSEL_R = GPIO_PORTC_AFSEL_R | 0x30; // Setting to alternate function
    GPIO_PORTC_PCTL_R = GPIO_PORTC_PCTL_R | 0x00110000; // Set the mux to the U4Tx and U4Rx

    delayMs(1); // UART needs a little time
    /////////////////////// Setup UART4 done /////////////////////////////
}

// receive one byte
char UART4Rx(void){
    char c;
    while((UART4_FR_R & 0x10) != 0);
    c = UART4_DR_R;
    return c;
}

// transmit one byte
void UART4Tx(char c){
    while((UART4_FR_R & 0x20) != 0);
    UART4_DR_R = c;
}
