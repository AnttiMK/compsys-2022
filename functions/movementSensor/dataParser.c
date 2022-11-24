/*
 dataParser.c

 Created on: 22.11.2022
 Author: Joonas
 */

#include <math.h>
#include <stdio.h>
#include <functions/movementSensor/dataParser.h>

// CREDITS: https://www.programiz.com/c-programming/examples/standard-deviation

void calculateSD(float data[7][100], float *x1, float *y1, float *z1, float *xg1, float *yg1, float *zg1) {

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

void calculateSD2(float data[7][100], float *x1, float *y1, float *z1, float *xg1, float *yg1, float *zg1, int index) {
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

void calculateVariance(float data[7][100], float *x2, float *y2, float *z2, float *xg2, float *yg2, float *zg2) {

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
