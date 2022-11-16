#include "songs.h"

static Song nokia_song;
static int nokia_melody[] = {NOTE_E5, 8, NOTE_D5, 8, NOTE_FS4, 4, NOTE_GS4, 4,
    NOTE_CS5, 8, NOTE_B4, 8, NOTE_D4, 4, NOTE_E4, 4,
    NOTE_B4, 8, NOTE_A4, 8, NOTE_CS4, 4, NOTE_E4, 4,
    NOTE_A4, 2, 0};

Song *nokia() {
    nokia_song.tempo = 180;
    nokia_song.melody = &nokia_melody;
    return &nokia_song;
}

static Song got_song;
static int got_melody[] = {NOTE_G4,8, NOTE_C4,8, NOTE_DS4,16, NOTE_F4,16, NOTE_G4,8, NOTE_C4,8, NOTE_DS4,16, NOTE_F4,16, //1
                    NOTE_G4,8, NOTE_C4,8, NOTE_DS4,16, NOTE_F4,16, NOTE_G4,8, NOTE_C4,8, NOTE_DS4,16, NOTE_F4,16,
                    NOTE_G4,8, NOTE_C4,8, NOTE_E4,16, NOTE_F4,16, NOTE_G4,8, NOTE_C4,8, NOTE_E4,16, NOTE_F4,16,
                    NOTE_G4,8, NOTE_C4,8, NOTE_E4,16, NOTE_F4,16, NOTE_G4,8, NOTE_C4,8, NOTE_E4,16, NOTE_F4,16,
                    NOTE_G4,-4, NOTE_C4,-4,//5

                    NOTE_DS4,16, NOTE_F4,16, NOTE_G4,4, NOTE_C4,4, NOTE_DS4,16, NOTE_F4,16, //6
                    NOTE_D4,-1, //7 and 8
                    NOTE_F4,-4, NOTE_AS3,-4,
                    NOTE_DS4,16, NOTE_D4,16, NOTE_F4,4, NOTE_AS3,-4,
                    NOTE_DS4,16, NOTE_D4,16, NOTE_C4,-1, //11 and 12

                    //repeats from 5
                    NOTE_G4,-4, NOTE_C4,-4,//5

                    NOTE_DS4,16, NOTE_F4,16, NOTE_G4,4, NOTE_C4,4, NOTE_DS4,16, NOTE_F4,16, //6
                    NOTE_D4,-1, //7 and 8
                    NOTE_F4,-4, NOTE_AS3,-4,
                    NOTE_DS4,16, NOTE_D4,16, NOTE_F4,4, NOTE_AS3,-4,
                    NOTE_DS4,16, NOTE_D4,16, NOTE_C4,-1, //11 and 12
                    NOTE_G4,-4, NOTE_C4,-4,
                    NOTE_DS4,16, NOTE_F4,16, NOTE_G4,4,  NOTE_C4,4, NOTE_DS4,16, NOTE_F4,16,

                    NOTE_D4,-2,//15
                    NOTE_F4,-4, NOTE_AS3,-4,
                    NOTE_D4,-8, NOTE_DS4,-8, NOTE_D4,-8, NOTE_AS3,-8,
                    NOTE_C4,-1,
                    NOTE_C5,-2,
                    NOTE_AS4,-2,
                    NOTE_C4,-2,
                    NOTE_G4,-2,
                    NOTE_DS4,-2,
                    NOTE_DS4,-4, NOTE_F4,-4,
                    NOTE_G4,-1,

                    NOTE_C5,-2,//28
                    NOTE_AS4,-2,
                    NOTE_C4,-2,
                    NOTE_G4,-2,
                    NOTE_DS4,-2,
                    NOTE_DS4,-4, NOTE_D4,-4,
                    NOTE_C5,8, NOTE_G4,8, NOTE_GS4,16, NOTE_AS4,16, NOTE_C5,8, NOTE_G4,8, NOTE_GS4,16, NOTE_AS4,16,
                    NOTE_C5,8, NOTE_G4,8, NOTE_GS4,16, NOTE_AS4,16, NOTE_C5,8, NOTE_G4,8, NOTE_GS4,16, NOTE_AS4,16,

                    REST,4, NOTE_GS5,16, NOTE_AS5,16, NOTE_C6,8, NOTE_G5,8, NOTE_GS5,16, NOTE_AS5,16,
                    NOTE_C6,8, NOTE_G5,16, NOTE_GS5,16, NOTE_AS5,16, NOTE_C6,8, NOTE_G5,8, NOTE_GS5,16, NOTE_AS5,16, 0};

Song *got() {
    got_song.tempo = 85;
    got_song.melody = &got_melody;
    return &got_song;
}
