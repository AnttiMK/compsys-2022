/*
 * movmentTask.c
 *
 *  Created on: 22.11.2022
 *      Author: Joonas
 */

#include <math.h>
#include <stdio.h>
#include <functions/movmentSensor/movmentTask.h>

void recognizeMove(float x, float y, float z);

void recognizeMove(float x, float y, float z) {
    float x = x * 10;
    float y = y * 10;
    float z = z * 10;

    if ( (fabsf(x - 3) < 1.5) & (fabsf(y - 3) < 1.5) & (z > 85) ){
        sprintf("Nosto ylös liike");
    }

    if ( (fabsf(x - 5) < 1.5) & (fabsf(z - 5) < 1.5) & (y > 28) ) {
        sprintf("Liikutus sivuttain pöydällä");
    }

    else {
        sprintf("Ei tunnistettua liikettä");
    }

}