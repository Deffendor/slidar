/*
 *  timer.h
 *
 *  Created on: May 21, 2019
 *      Author: soren
 */

#ifndef STTIMER_H_
#define STTIMER_H_

#define SYSTICK_CTRL_R (*((volatile uint32_t *)0xE000E010))
#define SYSTICK_RELOAD_R (*((volatile uint32_t *)0xE000E014))
#define SYSTICK_CURRENT_R (*(volatile uint32_t *)0xE000E018))

void initSysTickTimer();
void setSysTickInterval(int timeInUs); //time in micro seconds
void waitForSystick(); //time in micro seconds

#endif /* STTIMER_H_ */
