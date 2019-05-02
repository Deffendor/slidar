// File:  UART_Util.h
// Author: JS
// Date:  9/9/17
// Purpose: UART configuration and read/write utilities
// Hardware:  TM4C123 Tiva board

void UART_UART5_Init(void);                 // Initializes the PLL, UART, SysTick, and ports needed for the program.
void UART_UART5_WriteChar(char c);          //
void UART_UART5_WriteString(char* sz);      //Transmits message String to Bluetooth. ("Hello!")
char UART_UART5_ReadChar(void);             //Receive message from Bluetooth. x = UART_UART5_ReadChar()

void UART_OutUDec(uint32_t n);              //UART_OutChar
