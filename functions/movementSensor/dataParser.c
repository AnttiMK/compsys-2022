/*
 dataParser.c

 Created on: 22.11.2022
 Author: Joonas
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <functions/movementSensor/dataParser.h>

// CREDITS: https://www.programiz.com/c-programming/examples/standard-deviation

void calculateSD(float data[7][100], float *x1, float *y1, float *z1,
                 float *xg1, float *yg1, float *zg1) {

    float sumX = 0.0, meanX, SDX = 0.0;
    float sumY = 0.0, meanY, SDY = 0.0;
    float sumZ = 0.0, meanZ, SDZ = 0.0;
    float sumXg = 0.0, meanXg, SDXg = 0.0;
    float sumYg = 0.0, meanYg, SDYg = 0.0;
    float sumZg = 0.0, meanZg, SDZg = 0.0;

    int i;
    for (i = 0; i < 100; ++i) {

        sumX += data[1][i];
        sumY += data[2][i];
        sumZ += data[3][i];
        sumXg += data[4][i];
        sumYg += data[5][i];
        sumZg += data[6][i];
    }
    meanX = sumX / 100;
    meanY = sumY / 100;
    meanZ = sumZ / 100;
    meanXg = sumXg / 100;
    meanYg = sumYg / 100;
    meanZg = sumZg / 100;

    for (i = 0; i < 100; ++i) {

        SDX += pow(data[1][i] - meanX, 2);
        SDY += pow(data[2][i] - meanY, 2);
        SDZ += pow(data[3][i] - meanZ, 2);
        SDXg += pow(data[4][i] - meanXg, 2);
        SDYg += pow(data[5][i] - meanYg, 2);
        SDZg += pow(data[6][i] - meanZg, 2);
    }

    *x1 = sqrt(SDX / 100);
    *y1 = sqrt(SDY / 100);
    *z1 = sqrt(SDZ / 100);
    *xg1 = sqrt(SDXg / 100);
    *yg1 = sqrt(SDYg / 100);
    *zg1 = sqrt(SDZg / 100);
}

void movmentValue(float data[7][100], int *movment, int index, int dataIndex) {

    float sumX = 0.0;
    float sumY = 0.0;
    float sumZ = 0.0;

    int i;
    for (i = dataIndex; i < dataIndex + 25; ++i) {

        sumX += data[1][i];
        sumY += data[2][i];
        sumZ += data[3][i] - 1;
    }

    sumX = sumX / 25;
    sumY = sumY / 25;
    sumZ = sumZ / 25;

    char msg[30];
    sprintf(msg, "SumX: %f\n", sumX);
    System_printf(msg);
    System_flush();
    memset(msg, 0, 30);

    sprintf(msg, "SumY: %f\n", sumY);
    System_printf(msg);
    System_flush();
    memset(msg, 0, 30);

    sprintf(msg, "SumZ: %f\n", sumZ);
    System_printf(msg);
    System_flush();
    memset(msg, 0, 30);

    if (fabsf(sumX) > fabsf(sumY) && fabsf(sumX) > fabsf(sumZ)) {
        if (sumX > 0.0) {
            *movment += 1 * pow(10, index);

            System_printf("Liike X positiivinen = Eteenpäin \n");
            System_flush();

        } else {
            *movment += 2 * pow(10, index);

            System_printf("Liike X negatiivinen = Taaksepäin \n");
            System_flush();
        }
    } else if (fabsf(sumY) > fabsf(sumZ)) {
        if (sumY > 0.0) {

            *movment += 3 * pow(10, index);

            System_printf("Liike Y positiivinen = Oikealla \n");
            System_flush();

        } else {

            *movment += 4 * pow(10, index);

            System_printf("Liike Y negatiivinen = Vasemmalle \n");
            System_flush();
        }
    } else if (sumZ > 0.0) {

        *movment += 5 * pow(10, index);

        System_printf("Liike Z positiivinen = Ylöspäin \n");
        System_flush();

    } else {

        *movment += 6 * pow(10, index);

        System_printf("Liike Z negatiivinen = Alaspäin \n");
        System_flush();
    }
}

void calculateSD2(float data[7][100], float *x1, float *y1, float *z1,
                  float *xg1, float *yg1, float *zg1, int index) {
    float sumX = 0.0, meanX, SDX = 0.0;
    float sumY = 0.0, meanY, SDY = 0.0;
    float sumZ = 0.0, meanZ, SDZ = 0.0;
    float sumXg = 0.0, meanXg, SDXg = 0.0;
    float sumYg = 0.0, meanYg, SDYg = 0.0;
    float sumZg = 0.0, meanZg, SDZg = 0.0;

    int i;
    for (i = index; i < index + 25; ++i) {

        sumX += data[1][i];
        sumY += data[2][i];
        sumZ += data[3][i];
        sumXg += data[4][i];
        sumYg += data[5][i];
        sumZg += data[6][i];
    }
    meanX = sumX / 25;
    meanY = sumY / 25;
    meanZ = sumZ / 25;
    meanXg = sumXg / 25;
    meanYg = sumYg / 25;
    meanZg = sumZg / 25;

    for (i = index; i < index + 25; ++i) {

        SDX += pow(data[1][i] - meanX, 2);
        SDY += pow(data[2][i] - meanY, 2);
        SDZ += pow(data[3][i] - meanZ, 2);
        SDXg += pow(data[4][i] - meanXg, 2);
        SDYg += pow(data[5][i] - meanYg, 2);
        SDZg += pow(data[6][i] - meanZg, 2);
    }

    *x1 = sqrt(SDX / 25);
    *y1 = sqrt(SDY / 25);
    *z1 = sqrt(SDZ / 25);
    *xg1 = sqrt(SDXg / 25);
    *yg1 = sqrt(SDYg / 25);
    *zg1 = sqrt(SDZg / 25);

}

// CREDITS: https://www.sanfoundry.com/c-program-mean-variance-standard-deviation/

void calculateVariance(float data[7][100], float *x2, float *y2, float *z2,
                       float *xg2, float *yg2, float *zg2) {

    float averageX, sumX = 0.0, sum1X = 0.0;
    float averageY, sumY = 0.0, sum1Y = 0.0;
    float averageZ, sumZ = 0.0, sum1Z = 0.0;
    float averageXg, sumXg = 0.0, sum1Xg = 0.0;
    float averageYg, sumYg = 0.0, sum1Yg = 0.0;
    float averageZg, sumZg = 0.0, sum1Zg = 0.0;

    int i;
    for (i = 0; i < 100; ++i) {
        sumX += data[1][i];
        sumY += data[2][i];
        sumZ += data[3][i];
        sumXg += data[4][i];
        sumYg += data[5][i];
        sumZg += data[6][i];
    }

    averageX = sumX / 100;
    averageY = sumY / 100;
    averageZ = sumZ / 100;
    averageXg = sumXg / 100;
    averageYg = sumYg / 100;
    averageZg = sumZg / 100;

    for (i = 0; i < 100; ++i) {

        sum1X = sum1X + pow((data[1][i] - averageX), 2);
        sum1Y = sum1Y + pow((data[2][i] - averageY), 2);
        sum1Z = sum1Z + pow((data[3][i] - averageZ), 2);
        sum1Xg = sum1Xg + pow((data[4][i] - averageXg), 2);
        sum1Yg = sum1Yg + pow((data[5][i] - averageYg), 2);
        sum1Zg = sum1Zg + pow((data[6][i] - averageZg), 2);
    }

    *x2 = sum1X / 100;
    *y2 = sum1Y / 100;
    *z2 = sum1Z / 100;
    *xg2 = sum1Xg / 100;
    *yg2 = sum1Yg / 100;
    *zg2 = sum1Zg / 100;

}

void movmentValue2(float data[7][100], int *movment, int index, int dataIndex) {

    float sumX = 0.0;
    float sumY = 0.0;
    float sumZ = 0.0;

    float viisto = 0.0;

    int i;
    for (i = dataIndex; i < dataIndex + 25; ++i) {

        sumX += data[1][i];
        sumY += data[2][i];
        sumZ += data[3][i];
    }

    sumX = sumX / 25;
    sumY = sumY / 25;
    sumZ = sumZ / 25;
    viisto = fabsf(sumY - sumZ);

    if ((fabsf(sumX) < 0.01) && (fabsf(sumY) < 0.01) && (fabsf(sumZ) < 0.01)) // nämä rajat pitää määritellä tarkemmin
            {
        *movment += 11 * pow(10, index);

        System_printf("Liikkumatta  \n");
        System_flush();
    }

    else if ((fabsf(sumX) > fabsf(sumY)) & (fabsf(sumX) > fabsf(sumZ))) {

        if (sumX > 0.0) {
            *movment += 12 * pow(10, index);

            System_printf("Liike X positiivinen = Eteenpäin \n");
            System_flush();

        } else {
            *movment += 13 * pow(10, index);

            System_printf("Liike X negatiivinen = Taaksepäin \n");
            System_flush();
        }

    }

    else if (viisto < 2.00) { // liikuttu Y ja Z akselilla melkein yhtäpaljon

        if (sumZ > 0.0 && sumY > 0.0) {
            *movment += 14 * pow(10, index);

            System_printf("Liike Koilinen \n");
            System_flush();

        } else if (sumZ > 0.0 && sumY < 0.0) {
            *movment += 15 * pow(10, index);

            System_printf("Liike Luode \n");
            System_flush();
        }

        else if (sumZ < 0.0 && sumY > 0.0) {
            *movment += 16 * pow(10, index);

            System_printf("Liike Kaakko \n");
            System_flush();
        } else {
            *movment += 17 * pow(10, index);

            System_printf("Liike Lounas Hehe \n");
            System_flush();

        }

    }

    else if (fabsf(sumY) > fabsf(sumZ)) {

        if (sumY > 0.0) {

            *movment += 18 * pow(10, index);

            System_printf("Liike Y positiivinen = Oikealla \n");
            System_flush();

        } else {

            *movment += 19 * pow(10, index);

            System_printf("Liike Y negatiivinen = Vasemmalle \n");
            System_flush();
        }

    } else {
        if (sumZ > 0.0) {

            *movment += 20 * pow(10, index);

            System_printf("Liike Z positiivinen = Ylöspäin \n");
            System_flush();

        } else {

            *movment += 21 * pow(10, index);

            System_printf("Liike Z negatiivinen = Alaspäin \n");
            System_flush();
        }
    }
}

void calculateVariance2(float data[4][100], float *varX, float *varY, float *varZ, int count) {

    float averageX, sumX = 0.0, sum1X = 0.0;
    float averageY, sumY = 0.0, sum1Y = 0.0;
    float averageZ, sumZ = 0.0, sum1Z = 0.0;


    int i;
    for (i = 0; i < count; ++i) {
        sumX += data[1][i];
        sumY += data[2][i];
        sumZ += data[3][i];
    }

    averageX = sumX / count;
    averageY = sumY / count;
    averageZ = sumZ / count;

    for (i = 0; i < count; ++i) {

        sum1X = sum1X + pow((data[1][i] - averageX), 2);
        sum1Y = sum1Y + pow((data[2][i] - averageY), 2);
        sum1Z = sum1Z + pow((data[3][i] - averageZ), 2);

    }

    *varX = sum1X * 100 / count;
    *varY = sum1Y * 100 / count;
    *varZ = sum1Z * 100 / count;
}

Move recognizeMove(float x, float y, float z) {
    if (x <= 1 && y <= 2 && z >= 5) {
//        System_printf("Nosto yl�s\n");
//        System_flush();
        return LIFT;
    } else if (x <= 1.5 && y >= 5 && z <= 1.5) {
//        System_printf("Liikutus sivuttain p�yd�ll�\n");
//        System_flush();
        return SLIDE;
    } else if (x <= 4 && y >= 7 && z >= 13) {
//        System_printf("P�yd�ll� hyppyytys\n");
//        System_flush();
        return JUMP;
    } else if (x <= 2 && y >= 4 && z >= 4) {
//        System_printf("Porrasliike\n");
//        System_flush();
        return STAIRS;
    } else {
//        System_printf("Ei tunnistettua liikett�!\n");
//        System_flush();
        return NONE;
    }

}
