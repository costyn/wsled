#include <Arduino.h>
#include <FastLED.h>

// 2,14,7,8,6,20,21,5

#define STRIP1_LEN 42
#define STRIP2_LEN 48
#define STRIP3_LEN 51
#define STRIP12_LEN STRIP1_LEN + STRIP2_LEN

#define STRIP1_START 0
#define STRIP1_END STRIP1_LEN - 1
#define STRIP2_START STRIP1_LEN
#define STRIP2_END STRIP2_START + STRIP2_LEN - 1
#define STRIP3_START 0
#define STRIP3_END STRIP3_LEN - 1

CRGB strip12[STRIP12_LEN];
CRGB strip3[STRIP3_LEN];

#define LEGWING_LEN 36

CRGB strip4[LEGWING_LEN];
CRGB strip5[LEGWING_LEN];
CRGB strip6[LEGWING_LEN];
CRGB strip7[LEGWING_LEN];


#define STRIP8_LEN 51
#define STRIP9_LEN 48
#define STRIP10_LEN 42
#define STRIP910_LEN STRIP10_LEN + STRIP9_LEN

#define STRIP10_START 0
#define STRIP10_END STRIP10_LEN - 1
#define STRIP9_START STRIP10_LEN
#define STRIP9_END STRIP9_START + STRIP9_LEN - 1
#define STRIP8_START 0
#define STRIP8_END STRIP8_LEN - 1

CRGB strip8[STRIP8_LEN];
CRGB strip910[STRIP910_LEN];

void fillStrip( uint8_t stripId, CHSV chsv );

void setup() {
    FastLED.addLeds<NEOPIXEL, 14>(strip12, STRIP12_LEN);
    FastLED.addLeds<NEOPIXEL, 2>(strip3, STRIP3_LEN);
    FastLED.addLeds<NEOPIXEL, 6>(strip4, LEGWING_LEN);
    FastLED.addLeds<NEOPIXEL, 20>(strip5, LEGWING_LEN);
    FastLED.addLeds<NEOPIXEL, 21>(strip6, LEGWING_LEN);
    FastLED.addLeds<NEOPIXEL, 5>(strip7, LEGWING_LEN);

    FastLED.addLeds<NEOPIXEL, 7>(strip8, STRIP8_LEN);
    FastLED.addLeds<NEOPIXEL, 8>(strip910, STRIP910_LEN);

    FastLED.setBrightness(100);
    // Serial.begin(115200);
}

void sweepLeftRight() {
    for( int i = 0 ; i < 10 ; i++ ) {
        FastLED.clear();
        fillStrip( i, CHSV(i*15,255,255));
        FastLED.show();
        delay(50);
    }
}

void fillStrip( uint8_t stripId, CHSV chsv ){
    switch(stripId)
    {
        case 1:
        fill_solid(strip12, STRIP1_LEN, chsv);
        break;

        case 2:
        fill_solid(strip12 + STRIP2_START, STRIP2_LEN, chsv);
        break;

        case 3:
        fill_solid(strip3, STRIP3_LEN, chsv);
        break;

        case 4:
        fill_solid(strip4, LEGWING_LEN, chsv);
        break;

        case 5:
        fill_solid(strip5, LEGWING_LEN, chsv);
        break;

        case 6:
        fill_solid(strip6, LEGWING_LEN, chsv);
        break;

        case 7:
        fill_solid(strip7, LEGWING_LEN, chsv);
        break;

        case 8:
        fill_solid(strip8, LEGWING_LEN, chsv);
        break;

        case 9:
        fill_solid(strip910, STRIP9_LEN, chsv);
        break;

        case 10:
        fill_solid(strip910, STRIP10_LEN, chsv);
        break;
    }
}

void bouncers() {
    int pos1 = beatsin8(50,STRIP1_START,STRIP1_END);
    int pos2 = beatsin8(50,STRIP2_START,STRIP2_END);
    int pos3 = beatsin8(50,STRIP3_START,STRIP3_END);
    int pos_leg = beatsin8(50,0,LEGWING_LEN-1);
    strip12[pos1]   = CHSV(beatsin8(10)+0,255,255);
    strip12[pos2]   = CHSV(beatsin8(10)+30,255,255);
    strip3[pos3]    = CHSV(beatsin8(10)+60,255,255);
    strip4[pos_leg] = CHSV(beatsin8(10)+90,255,255);
    strip5[pos_leg] = CHSV(beatsin8(10)+120,255,255);
    strip6[pos_leg] = CHSV(beatsin8(10)+150,255,255);
    strip7[pos_leg] = CHSV(beatsin8(10)+180,255,255);

    strip910[pos1]  = CHSV(beatsin8(10)+210,255,255);
    strip910[pos2]  = CHSV(beatsin8(10)+230,255,255);
    strip8[pos3]    = CHSV(beatsin8(10)+250,255,255);

    FastLED.show();
    FastLED.clear();
    delay(10);
}

void loop() {
    sweepLeftRight();
}

