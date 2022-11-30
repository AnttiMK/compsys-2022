/*
 * dataParser.c
 *
 *  Created on: 22.11.2022
 *      Author: Joonas
 */

#ifndef _DATAPARSER_H_
#define _DATAPARSER_H_

#include <math.h>
#include <stdio.h>

typedef enum {
    LIFT, SLIDE, JUMP, STAIRS, NONE
} Move;

void calculateSD(float data[7][100], float *x1, float *y1, float *z1, float *xg1, float *yg1, float *zg1);
void calculateSD2(float data[7][100], float *x1, float *y1, float *z1, float *xg1, float *yg1, float *zg1, int index);
void calculateVariance(float data[7][100], float *x2, float *y2, float *z2, float *xg2, float *yg2, float *zg2);
void movmentValue(float data[7][100], int *movment, int index, int dataIndex);
void movmentValue2(float data[7][100], int *movment, int index, int dataIndex);
void calculateVariance2(float data[4][100], float *varX, float *varY, float *varZ, int count);
Move recognizeMove(float x, float y, float z);


#endif
