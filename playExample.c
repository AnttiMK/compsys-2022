#include <stdio.h>
#include <stdlib.h>

#include "song.h"
#include "song.c"


int playsong() {

    Song temp = nokia();


    int notes = sizeof(temp.melody) / sizeof(temp.melody[0]) / 2;

    // this calculates the duration of a whole note in ms
    int wholenote = (60000 * 4) / temp.tempo;

    int divider = 0; 
    int noteDuration = 0;
    // iterate over the notes of the melody.
    // Remember, the array is twice the number of notes (notes + durations)

    int thisNote;

    for (thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

        // calculates the duration of each note
        divider = temp.melody[thisNote + 1];

        if (divider > 0) {
            // regular note, just proceed
            noteDuration = (wholenote) / divider;

        } else if (divider < 0) {
            // dotted notes are represented with negative durations!!
            noteDuration = (wholenote) / abs(divider);
            noteDuration *= 1.5; // increases the duration in half for dotted notes

        }

        // tone(buzzer, temp.melody[thisNote], noteDuration * 0.9);
        //printf("%d %f\n", temp.melody[thisNote], noteDuration * 0.9);

        
        //delay(noteDuration);
        //printf("%d\n", noteDuration);

        //noTone(buzzer);
    }

    return 0;

}