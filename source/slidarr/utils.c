/*
 * utils.c
 *
 *  Created on: May 12, 2019
 *      Author: mats
 */
#include "utils.h"

// delay n ms at 16MHz
void delayMs(int n){
    int i, j;
    for(i = 0; i < n; i++)
        for(j = 0; j < 3180; j++)
            {}
}
