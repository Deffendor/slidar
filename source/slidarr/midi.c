/*
 * midi.c
 *
 *  Created on: May 12, 2019
 *      Author: mats
 */
#include "midi.h"

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

