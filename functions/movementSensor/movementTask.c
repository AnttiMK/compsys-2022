/*
 * movmentTask.c
 *
 *  Created on: 22.11.2022
 *      Author: Joonas
 */

#include <math.h>
#include <stdio.h>
#include <xdc/runtime/System.h>

#include <functions/movementSensor/movementTask.h>

void recognizeMove(float x, float y, float z) {

    if ((fabsf(x * 100 - 3) < 1.5) & (fabsf(y * 100 - 3) < 1.5)
            & (z * 100 > 85)) {
        System_printf("Nosto ylös liike");
        System_flush();
    }

    if ((x * 100 < 9) && (z * 100 < 9) && (y * 100 > 20)) {
        System_printf("Liikutus sivuttain poydalla\n");
        System_flush();
    }

    if ((x * 100 < 10) && (fabsf(y * 100 - 11) < 3) && (fabsf(z * 100 - 11) < 3)) {
        System_printf("x: %f\n", x * 100);
        System_flush();
        System_printf("abs y: %f\n", fabsf(y * 100 - 11));
                System_flush();
        System_printf("abs z: %f\n", fabsf(z * 100 - 11));
                System_flush();
        System_printf("Porras_liike\n");
        System_flush();
    }
    else {
        System_printf("Ei tunnistettua liiketta\n");
        System_flush();
    }

}

void recognizeMove2(int movmentValue) {

    if (movmentValue == 3535){
        System_printf("Portaat\n");
        System_flush();
    } 

    else if (movmentValue == 3333) {
        System_printf("Liike oikealle\n");
        System_flush();

    }

     else if (movmentValue == 4444) {
        System_printf("Liike vasemmalle\n");
        System_flush();

    }

    else if (movmentValue == 5555) {
        System_printf("Hissi ylös\n");
        System_flush();

    }

    else if (movmentValue == 6666) {
        System_printf("Hissi alas\n");
        System_flush();

    }

    else if (movmentValue == 6464) {
        System_printf("Portaat alas\n");
        System_flush();

    }
    
    else {
        System_printf("2 hyppyä\n");
        System_flush();
    }

}

void recognizeMove3(int movmentValue) {

    if (movmentValue == 20202020){
        System_printf("Hissi ylös\n");
        System_flush();
    } 

    else if (movmentValue == 21212121) {
        System_printf("Hissi alas\n");
        System_flush();

    }

     else if (movmentValue == 14151716 || movmentValue == 16141517  || movmentValue == 17161415  || movmentValue == 15171614) {
        System_printf("Ympyrä tai salmiakki...\n");
        System_flush();

    }

    else if (movmentValue == 16141614) {
        System_printf("2 hyppyä\n");
        System_flush();

    }
  
    else {
        System_printf("Ei tunnistettua liikettä\n");
        System_flush();
    }

}
