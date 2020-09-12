#include "octows1.h"

#define DEFAULT_BRIGHTNESS 140

CRGB leds[NUM_LEDS];

void fadeall(uint8_t fade_all_speed, uint16_t startPos, uint16_t endPos);
void cylon1(uint16_t bpm, uint16_t startPos, uint16_t endPos, uint8_t hue);
void cylonArray();
void setTaskCallback() ;

Scheduler runner;
Task taskShowPattern( 10, TASK_FOREVER, &cylonArray);

void setup() {
    delay(2000);
    LEDS.addLeds<OCTOWS2811>(leds, NUM_LEDS_PER_STRIP);
    fill_solid(leds, NUM_LEDS, CRGB::Black);  // cleanup any LEDs stuck on from previous boot
    LEDS.setBrightness(DEFAULT_BRIGHTNESS);
    LEDS.show();
    Serial.begin(115200);
    runner.init();
    runner.addTask(taskShowPattern);
    taskShowPattern.enable() ;
    setTaskCallback() ; // Set the callback according to ledMode
}

void loop() {
    runner.execute() ;
}

void cylonArray() {
    uint16_t bpm = 30 ;
    uint8_t hue = beat8( 8 );
    for( int i = 0; i < 10 ; i++ ) {
        cylon1(bpm, stripStart[i], stripEnd[i], hue);
    }
    delay(1);
}

void cylon1(uint16_t bpm, uint16_t startPos, uint16_t endPos, uint8_t hue) {
    leds[beatsin88( bpm*256, startPos, endPos - 1 )] = CHSV(hue, 255, 255) ;
    // Serial.println(beatsin88( bpm*256, startPos, endPos - 1 ));
    fadeall(245,startPos,endPos);
}

void fadeall(uint8_t fade_all_speed, uint16_t startPos, uint16_t endPos) {
  for (uint16_t i = startPos; i < endPos; i++) {
    leds[i].nscale8(fade_all_speed);
  }
}

void testOneStrip() {
    fill_solid(leds + STRIP4_START, STRIP4_LEN, CRGB::Red);
    LEDS.show();
}




void setTaskCallback() {
  // Prevent weird shit with bright patterns at times
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  LEDS.show();

  if ( strcmp(routines[ledMode], "cylon") == 0  ) {
    taskShowPattern.setCallback(&cylon) ;
    taskShowPattern.setInterval(5);
  } else

  if ( strcmp(routines[ledMode], "strobeParts") == 0  ) {
    taskShowPattern.setCallback(&strobeParts) ;
    taskShowPattern.setInterval(50);
  } else

  if ( strcmp(routines[ledMode], "strobeAll") == 0  ) {
    taskShowPattern.setCallback(&strobeAll) ;
    taskShowPattern.setInterval(5);
  } else

  if ( strcmp(routines[ledMode], "rainbow") == 0  ) {
    taskShowPattern.setCallback(&rainbow) ;
    taskShowPattern.setInterval(10);
  } else

  if ( strcmp(routines[ledMode], "fadeGlitter") == 0  ) {
    taskShowPattern.setCallback(&fadeGlitter) ;
    taskShowPattern.setInterval(5);
  } else

  if ( strcmp(routines[ledMode], "discoGlitter") == 0  ) {
    taskShowPattern.setCallback(&discoGlitter) ;
    taskShowPattern.setInterval(10);
  } else

  if ( strcmp(routines[ledMode], "fire2012") == 0  ) {
    taskShowPattern.setCallback(&Fire2012) ;
    taskShowPattern.setInterval(30);
  } else

  if ( strcmp(routines[ledMode], "fire2012x4") == 0  ) {
    taskShowPattern.setCallback(&Fire2012x4) ;
    taskShowPattern.setInterval(30);
  } else

  if ( strcmp(routines[ledMode], "fire2012all") == 0  ) {
    taskShowPattern.setCallback(&Fire2012all) ;
    taskShowPattern.setInterval(30);
  } else

  if ( strcmp(routines[ledMode], "noiselava") == 0  ) {
    taskShowPattern.setCallback(&fillnoise8lava) ;
    taskShowPattern.setInterval(10);
  } else

  if ( strcmp(routines[ledMode], "noiseparty") == 0  ) {
    taskShowPattern.setCallback(&fillnoise8party) ;
    taskShowPattern.setInterval(10);
  }

}