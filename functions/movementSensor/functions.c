#include <math.h>
#include <stdio.h>

// CREDITS: https://www.programiz.com/c-programming/examples/standard-deviation
float calculateSD(float data[][]) {
    float sumX = 0.0, meanX, SDX = 0.0;
    float sumY = 0.0, meanY, SDY = 0.0;
    float sumZ = 0.0, meanZ, SDZ = 0.0;
    int i;
    for (i = 0; i < 100; ++i) {
        sumX += data[1][i];
        sumY += data[2][i];
        sumZ += data[3][i];
    }
    meanX = sumX / 100;
    meanY = sumY / 100;
    meanZ = sumZ / 100;
    for (i = 0; i < 100; ++i) {
        SDX += pow(data[1][i] - meanX, 2);
        SDY += pow(data[2][i] - meanY, 2);
        SDZ += pow(data[3][i] - meanZ, 2);
    }

    return sqrt(SDX / 100),sqrt(SDY / 100),sqrt(SDZ / 100);
}

// CREDITS: https://www.sanfoundry.com/c-program-mean-variance-standard-deviation/

float calculateVariance(float data[][]) {
    float averageX, std_deviationX, sumX = 0, sum1X = 0;
    float averageY, std_deviationY, sumY = 0, sum1Y = 0;
    float averageZ, std_deviationZ, sumZ = 0, sum1Z = 0;

    int i;
    for (i = 0; i < 100; ++i) {
        sumX += data[1][i];
        sumY += data[2][i];
        sumZ += data[3][i];
    }
    averageX = sumX / 100;
    averageY = sumY / 100;
    averageZ = sumZ / 100;

    for (i = 0; i < 100; ++i) {

        sum1X = sum1X + pow((data[1][i] - averageX), 2);
        sum1Y = sum1Y + pow((data[2][i] - averageY), 2);
        sum1Z = sum1Z + pow((data[3][i] - averageZ), 2);
    }
    std_deviationX = sqrt(sum1X / 100);
    std_deviationY = sqrt(sum1Y / 100);
    std_deviationZ = sqrt(sum1Z / 100);


    return std_deviationX, std_deviationY, std_deviationZ;
}