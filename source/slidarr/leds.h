/*
 * leds.h
 *
 *  Created on: May 20, 2019
 *      Author: mats
 */

#ifndef LEDS_H_
#define LEDS_H_

#define RED_LED    (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 1*4)))   //GPIO_PIN_1
#define GREEN_LED  (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 3*4)))   //GPIO_PIN_2
#define BLUE_LED (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 2*4))) //GPIO_PIN_3

#endif /* LEDS_H_ */
