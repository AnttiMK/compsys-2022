/*
 * math.c
 *
 *  Created on: 25.11.2022
 *      Author: Antti
 */

#include <math.h>
#include "math.h"

double binFracToDec(uint8_t num) {
    int i;
    double result = 0;
    for (i = 1; i <= 8; ++i) {
        uint8_t current = (num << (i - 1)) & 128;
        if (current == 128) {
            result += 1 / pow(2, i);
        }
    }
    return result;
}
