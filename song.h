#ifndef __SONG_H_ 
#define __SONG_H_

#include <stdio.h>
#include <inttypes.h>

typedef struct Song {
    int tempo;
    int melody[100];
    int size;
} Song;


#endif
