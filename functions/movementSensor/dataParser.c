/*
 * dataParser.c
 *
 *  Created on: 22.11.2022
 *      Author: Joonas
 */


#include <math.h>
#include <stdio.h>
#include <functions/movmentSensor/dataParser.h>

// CREDITS: https://www.programiz.com/c-programming/examples/standard-deviation

float calculateSD(float *data[7][100]);

float calculateVariance(float *data[7][100]);

float calculateSD(float *data[7][100]) {

    float sumX = 0.0, meanX, SDX = 0.0;
    float sumY = 0.0, meanY, SDY = 0.0;
    float sumZ = 0.0, meanZ, SDZ = 0.0;
    float sumXg = 0.0, meanXg, SDXg = 0.0;
    float sumYg = 0.0, meanYg, SDYg = 0.0;
    float sumZg = 0.0, meanZg, SDZg = 0.0;

    int i;
    for (i = 0; i < 100; ++i) {

        sumX += *data[1][i];
        sumY += *data[2][i];
        sumZ += *data[3][i];
        sumXg += *data[4][i];
        sumYg += *data[5][i];
        sumZg += *data[6][i];
    }
    meanX = sumX / 100;
    meanY = sumY / 100;
    meanZ = sumZ / 100;
    meanXg = sumXg / 100;
    meanYg = sumYg / 100;
    meanZg = sumZg / 100;

    for (i = 0; i < 100; ++i) {

        SDX += pow(*data[1][i] - meanX, 2);
        SDY += pow(*data[2][i] - meanY, 2);
        SDZ += pow(*data[3][i] - meanZ, 2);
        SDXg += pow(*data[4][i] - meanX, 2);
        SDYg += pow(*data[5][i] - meanY, 2);
        SDZg += pow(*data[6][i] - meanZ, 2);
    }

    return sqrt(SDX / 100), sqrt(SDY / 100), sqrt(SDZ / 100),sqrt(SDXg / 100), sqrt(SDYg / 100), sqrt(SDZg / 100);
}

// CREDITS: https://www.sanfoundry.com/c-program-mean-variance-standard-deviation/

float calculateVariance(float *data[7][100]) {

    float averageX, std_deviationX, sumX = 0, sum1X = 0;
    float averageY, std_deviationY, sumY = 0, sum1Y = 0;
    float averageZ, std_deviationZ, sumZ = 0, sum1Z = 0;
    float averageXg, std_deviationXg, sumXg = 0, sum1Xg = 0;
    float averageYg, std_deviationYg, sumYg = 0, sum1Yg = 0;
    float averageZg, std_deviationZg, sumZg = 0, sum1Zg = 0;

    int i;
    for (i = 0; i < 100; ++i) {
        sumX += *data[1][i];
        sumY += *data[2][i];
        sumZ += *data[3][i];
        sumXg += *data[4][i];
        sumYg += *data[5][i];
        sumZg += *data[6][i];
    }

    averageX = sumX / 100;
    averageY = sumY / 100;
    averageZ = sumZ / 100;
    averageXg = sumXg / 100;
    averageYg = sumYg / 100;
    averageZg = sumZg / 100;

    for (i = 0; i < 100; ++i) {

        sum1X = sum1X + pow((*data[1][i] - averageX), 2);
        sum1Y = sum1Y + pow((*data[2][i] - averageY), 2);
        sum1Z = sum1Z + pow((*data[3][i] - averageZ), 2);
        sum1Xg = sum1Xg + pow((*data[4][i] - averageXg), 2);
        sum1Yg = sum1Yg + pow((*data[5][i] - averageYg), 2);
        sum1Zg = sum1Zg + pow((*data[6][i] - averageZg), 2);
    }

    std_deviationX = sqrt(sum1X / 100);
    std_deviationY = sqrt(sum1Y / 100);
    std_deviationZ = sqrt(sum1Z / 100);
    std_deviationXg = sqrt(sum1Xg / 100);
    std_deviationYg = sqrt(sum1Yg / 100);
    std_deviationZg = sqrt(sum1Zg / 100);


    return std_deviationX, std_deviationY, std_deviationZ, std_deviationXg, std_deviationYg, std_deviationZg;
}