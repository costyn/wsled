#define USE_OCTOWS2811

#include <Arduino.h>
#include <OctoWS2811.h>
#include <FastLED.h>
#include <TaskScheduler.h>

// #define AUTOADVANCE

#define NUM_LEDS_PER_STRIP 90
#define NUM_STRIPS 8
#define NUM_LEDS NUM_STRIPS * NUM_LEDS_PER_STRIP


// Nr 1
#define STRIP1_LEN 42
#define STRIP1_START NUM_LEDS_PER_STRIP * 1
#define STRIP1_END STRIP1_START + STRIP1_LEN

// Nr 2
#define STRIP2_LEN 48
#define STRIP2_START STRIP1_END
#define STRIP2_END STRIP2_START + STRIP2_LEN

// Nr 3
#define STRIP3_LEN 51
#define STRIP3_START 0
#define STRIP3_END STRIP3_START + STRIP3_LEN

// Nr 4
#define STRIP4_LEN 36
#define STRIP4_START NUM_LEDS_PER_STRIP * 4
#define STRIP4_END STRIP4_START + STRIP4_LEN

// Nr 5
#define STRIP5_LEN 36
#define STRIP5_START NUM_LEDS_PER_STRIP * 5
#define STRIP5_END STRIP5_START + STRIP5_LEN

// Nr 6
#define STRIP6_LEN 36
#define STRIP6_START NUM_LEDS_PER_STRIP * 6
#define STRIP6_END STRIP6_START + STRIP6_LEN

// Nr 7
#define STRIP7_LEN 36
#define STRIP7_START NUM_LEDS_PER_STRIP * 7
#define STRIP7_END STRIP7_START + STRIP7_LEN

// Nr 8
#define STRIP8_LEN 51
#define STRIP8_START NUM_LEDS_PER_STRIP * 2
#define STRIP8_END STRIP8_START + STRIP8_LEN

// Nr 9
#define STRIP9_LEN 48
#define STRIP9_START STRIP10_END
#define STRIP9_END STRIP9_START + STRIP9_LEN

// Nr 10
#define STRIP10_LEN 42
#define STRIP10_START NUM_LEDS_PER_STRIP * 3
#define STRIP10_END STRIP10_START + STRIP10_LEN

uint16_t stripStart[] = {
    STRIP1_START, 
    STRIP2_START, 
    STRIP3_START, 
    STRIP4_START, 
    STRIP5_START, 
    STRIP6_START, 
    STRIP7_START, 
    STRIP8_START, 
    STRIP9_START, 
    STRIP10_START
    };

uint16_t stripEnd[] = {
    STRIP1_END,
    STRIP2_END,
    STRIP3_END,
    STRIP4_END,
    STRIP5_END,
    STRIP6_END,
    STRIP7_END,
    STRIP8_END,
    STRIP9_END,
    STRIP10_END
};

uint16_t stripLen[] = {
    STRIP1_LEN,
    STRIP2_LEN,
    STRIP3_LEN,
    STRIP4_LEN,
    STRIP5_LEN,
    STRIP6_LEN,
    STRIP7_LEN,
    STRIP8_LEN,
    STRIP9_LEN,
    STRIP10_LEN
};

int ledMode = 5 ;

const char *routines[] = {
  "cylon",
  "fadeGlitter",
  "discoGlitter",
  "noiselava",
  "noiseparty",
  "wipe"
};

#define NUMROUTINES (sizeof(routines)/sizeof(char *)) //array size

// Prototype Methods
void strobeAll();
void strobeParts();
void strobePart(uint16_t startP, uint16_t numLeds, uint16_t delayTime );
void cylon();
void solid_red();
void fadeGlitter();
void identify();
void rainbow();
void black() ;
void wipe() ;
void discoGlitter();
void addGlitter( fract8 chanceOfGlitter);
void fadeSome(uint8_t fade_all_speed, uint16_t startPos, uint16_t endPos);
void fadeAll(uint8_t fade_all_speed) ;
void cylon1(uint16_t bpm, uint16_t startPos, uint16_t endPos, uint8_t hue);
void Fire2012() ;
void Fire2012x4() ;
void Fire2012all() ;
void fillnoise8(uint8_t currentPalette, uint8_t speed, uint8_t scale, boolean colorLoop ) ;
void fillnoise8lava() ;
void fillnoise8party() ;
void monitorJoystick() ;
void nextPattern() ;
void prevPattern() ;
void setTaskCallback() ;
void showPattern() ;


#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x)       Serial.print (x)
#define DEBUG_PRINTDEC(x)    Serial.print (x, DEC)
#define DEBUG_PRINTLN(x)     Serial.println (x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTLN(x)
#endif
