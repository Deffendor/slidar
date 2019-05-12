#include <stdint.h>
#include <tm4c123gh6pm.h>

void delayMs(int n);
void UART4Tx(char c);
char UART4Rx(void);
void noteOn(int note, int velocity);
void noteOff(int note, int velocity);
void pitchbend(int value);

/**
 * main.c
 */
int main(void)
{
    volatile int result; // ADC result
    char c; // character to send via UART

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


    while(1){
        // read adc
        ADC0_PSSI_R |= 8;
        while((ADC0_RIS_R & 8) == 0);
        result = ADC0_SSFIFO3_R;
        ADC0_ISC_R = 8;

        // send raw upper 8 bits via uart
        result = result >> 4; // looses the lower 4 bit, just to make it fit to 8 byte for now
        c = result;
        //UART4Tx(c);

        noteOn(0x40, 0x40);
        delayMs(1000);

        noteOff(0x40, 0x40);
        delayMs(1000);
    }

	//return 0;
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

// delay n ms at 16MHz
void delayMs(int n){
    int i, j;
    for(i = 0; i < n; i++)
        for(j = 0; j < 3180; j++)
            {}
}

void noteOn(int note, int velocity){
    char command = 0x90;
    char parameterKey = (char)note;
    char parameterVelocity = (char)velocity;
    UART4Tx(command);
    UART4Tx(parameterKey);
    UART4Tx(parameterVelocity);
}

void noteOff(int note, int velocity){
    char command = 0x80;
    char parameterKey = (char)note;
    char parameterVelocity = (char)velocity;
    UART4Tx(command);
    UART4Tx(parameterKey);
    UART4Tx(parameterVelocity);
}

void pitchbend(int value){
    char command = 0xE0;
    int lsbValue = value & 0x7F;
    int msbValue = value & 0x3F80;
    msbValue = msbValue >> 7;
    char parameterLsb = (char)lsbValue;
    char parameterMsb = (char)msbValue;
    UART4Tx(command);
    UART4Tx(parameterLsb);
    UART4Tx(parameterMsb);
}








