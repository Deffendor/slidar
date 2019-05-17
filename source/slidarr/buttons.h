/*
 * buttons.h
 *
 *  Created on: May 12, 2019
 *      Author: mats
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#define LED_RED     0x02  //PF1 0000 0010
#define LED_BLUE    0x04  //PF2 0000 0100
#define LED_GREEN   0x08  //PF3 0000 1000
#define LED_WHITE   0x0E  //ALL 0000 1110

void initButtons();
int readButton(int button);

#endif /* BUTTONS_H_ */
