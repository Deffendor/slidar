/*
 * uart.h
 *
 *  Created on: May 12, 2019
 *      Author: mats
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void initUART(int baudRate);
void UART4Tx(char c);
char UART4Rx(void);
void UART5Tx(uint8_t c);
char UART5Rx(void);

#endif /* UART_H_ */
