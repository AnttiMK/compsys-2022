/*
 * movmentTask.c
 *
 *  Created on: 22.11.2022
 *      Author: Joonas
 */

/*
 * NOTE: Legacy code, has a new implementation in dataParser.c
 */

//#include <math.h>
//#include <stdio.h>
//#include <xdc/runtime/System.h>
//
//#include <functions/movementSensor/movementTask.h>
//
//void recognizeMove(float x, float y, float z) {
//
//    if (x <= 1 && y <= 2 && z >= 5) {
//        System_printf("Nosto ylˆs\n");
//        System_flush();
//    } else if (x <= 1.5 && y >= 5 && z <= 1.5) {
//        System_printf("Liikutus sivuttain pˆyd‰ll‰\n");
//        System_flush();
//    } else if (x <= 4 && y >= 7 && z >= 13) {
//        System_printf("Pˆyd‰ll‰ hyppyytys\n");
//        System_flush();
//    } else if (x <= 2 && y >= 4 && z >= 4) {
//        System_printf("Porrasliike\n");
//        System_flush();
//    } else {
//        System_printf("Ei tunnistettua liikett‰!\n");
//        System_flush();
//    }
//
//}
//
//void recognizeMove2(int movmentValue) {
//
//    if (movmentValue == 3535) {
//        System_printf("Portaat\n");
//        System_flush();
//    }
//
//    else if (movmentValue == 3333) {
//        System_printf("Liike oikealle\n");
//        System_flush();
//
//    }
//
//    else if (movmentValue == 4444) {
//        System_printf("Liike vasemmalle\n");
//        System_flush();
//
//    }
//
//    else if (movmentValue == 5555) {
//        System_printf("Hissi yl√∂s\n");
//        System_flush();
//
//    }
//
//    else if (movmentValue == 6666) {
//        System_printf("Hissi alas\n");
//        System_flush();
//
//    }
//
//    else if (movmentValue == 6464) {
//        System_printf("Portaat alas\n");
//        System_flush();
//
//    }
//
//    else {
//        System_printf("2 hyppy√§\n");
//        System_flush();
//    }
//
//}
//
//void recognizeMove3(int movmentValue) {
//
//    if (movmentValue == 20202020) {
//        System_printf("Hissi yl√∂s\n");
//        System_flush();
//    }
//
//    else if (movmentValue == 21212121) {
//        System_printf("Hissi alas\n");
//        System_flush();
//
//    }
//
//    else if (movmentValue == 14151716 || movmentValue == 16141517
//            || movmentValue == 17161415 || movmentValue == 15171614) {
//        System_printf("Ympyr√§ tai salmiakki...\n");
//        System_flush();
//
//    }
//
//    else if (movmentValue == 16141614) {
//        System_printf("2 hyppy√§\n");
//        System_flush();
//
//    }
//
//    else {
//        System_printf("Ei tunnistettua liikett√§\n");
//        System_flush();
//    }
//
//}
