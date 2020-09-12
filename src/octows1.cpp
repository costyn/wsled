#include "octows1.h"

#define DEFAULT_BRIGHTNESS 80

CRGB leds[NUM_LEDS];

void setTaskCallback() ;

Scheduler runner;
Task taskShowPattern( 10, TASK_FOREVER, &wipe);

#ifdef AUTOADVANCE
void autoAdvanceLedMode() ;                       
Task taskAutoAdvanceLedMode( 15 * TASK_SECOND, TASK_FOREVER, &autoAdvanceLedMode);
#endif

// ************************************************************************
//                              SETUP
// ************************************************************************


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

    #ifdef AUTOADVANCE
    runner.addTask(taskAutoAdvanceLedMode);
    taskAutoAdvanceLedMode.enable() ;
    #endif
}

void loop() {

    runner.execute() ;
}

// ************************************************************************
//                              PATTERNS
// ************************************************************************

void testOneStrip(int i) {
    fill_solid(leds + stripStart[i], stripLen[i], CRGB::Red);
    LEDS.show();
}

void cylon() {
    uint16_t bpm = 30 ;
    uint8_t hue = beat8( 8 );
    for( int i = 0; i < 10 ; i++ ) {
        cylon1(bpm, stripStart[i], stripEnd[i], hue);
    }
    LEDS.show();
}

void cylon1(uint16_t bpm, uint16_t startPos, uint16_t endPos, uint8_t hue) {
    leds[beatsin88( bpm*256, startPos, endPos - 1 )] = CHSV(hue, 255, 255) ;
    // Serial.println(beatsin88( bpm*256, startPos, endPos - 1 ));
    fadeSome(245,startPos,endPos);
}

void wipe() {
    uint16_t bpm = 20 ;
    uint8_t hue = beat8( 12 );
    int i = beatsin8(bpm, 0, 9);
    fill_solid(leds + stripStart[i], stripLen[i], CHSV(hue, 255, 255) );
    LEDS.show();
    fadeAll(246);
}

void fadeSome(uint8_t fade_all_speed, uint16_t startPos, uint16_t endPos) {
  for (uint16_t i = startPos; i < endPos; i++) {
    leds[i].nscale8(fade_all_speed);
  }
}

void fadeAll(uint8_t fade_all_speed) {
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(fade_all_speed);
  }
}


void strobeAll() {
  if ( beat8(240) > 245 ) {
    fill_solid(leds, NUM_LEDS, CRGB::White );
  } else {
    fill_solid(leds, NUM_LEDS, CRGB::Black); // black
  }
  LEDS.show();
}

void fadeGlitter() {
  addGlitter(110);
  LEDS.show();
  fadeAll(246);
}

void discoGlitter() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  addGlitter( 255 ) ;
  LEDS.show();
}

void addGlitter( fract8 chanceOfGlitter)
{
  for ( uint8_t i = 0 ; i < 5 ; i++ ) {
    if ( random8() < chanceOfGlitter) {
      leds[ random16(NUM_LEDS) ] += CRGB::White;
    }
  }
}

void fillnoise8lava() {
  fillnoise8( 0, 1, 30, 1); // pallette, speed, scale, loop
}

void fillnoise8party() {
  fillnoise8( 1, 1, 30, 1); // pallette, speed, scale, loop
}

void fillnoise8(uint8_t currentPalette, uint8_t speed, uint8_t scale, boolean colorLoop ) {
  static uint16_t noise[NUM_LEDS];

  const CRGBPalette16 palettes[] = { LavaColors_p, PartyColors_p, OceanColors_p } ;

  static uint16_t x = random16();
  static uint16_t y = random16();
  static uint16_t z = random16();

  // If we're runing at a low "speed", some 8-bit artifacts become visible
  // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
  // The amount of data smoothing we're doing depends on "speed".
  uint16_t dataSmoothing = 0;

  if ( speed < 50) {
    dataSmoothing = 200 - (speed * 4);
  }

  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    uint16_t ioffset = scale * i;

    uint16_t data = inoise8(x + ioffset, y, z);

    // The range of the inoise8 function is roughly 16-238.
    // These two operations expand those values out to roughly 0..255
    // You can comment them out if you want the raw noise data.
    data = qsub8(data, 16);
    data = qadd8(data, scale8(data, 39));

    if ( dataSmoothing ) {
      uint16_t olddata = noise[i];
      uint16_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
      data = newdata;
    }

    noise[i] = data;
  }

  z += speed;

  // apply slow drift to X and Y, just for visual variation.
  x += speed / 8;
  y -= speed / 16;

  static uint8_t ihue = 0;

  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    // We use the value at the i coordinate in the noise
    // array for our brightness, and a 'random' value from NUM_LEDS - 1
    // for our pixel's index into the color palette.

    uint16_t index = noise[i];
    uint16_t bri =   noise[NUM_LEDS - i];
    // uint8_t bri =  sin(noise[NUM_LEDS - i]);  // more light/dark variation

    // if this palette is a 'loop', add a slowly-changing base value
    if ( colorLoop) {
      index += ihue;
    }

    // brighten up, as the color palette itself often contains the
    // light/dark dynamic range desired
    if ( bri > 127 ) {
      bri = 255;
    } else {
      bri = dim8_raw( bri * 2);
    }

    CRGB color = ColorFromPalette( palettes[currentPalette], index, bri);
    leds[i] = color;
  }
  ihue += 1;

  LEDS.show();
}



// ************************************************************************
//                              HELPER FUNCTIONS
// ************************************************************************


void setTaskCallback() {
  // Prevent weird shit with bright patterns at times
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  LEDS.show();

  if ( strcmp(routines[ledMode], "cylon") == 0  ) {
    taskShowPattern.setCallback(&cylon) ;
    taskShowPattern.setInterval(5);
  } else

  // if ( strcmp(routines[ledMode], "strobeParts") == 0  ) {
  //   taskShowPattern.setCallback(&strobeParts) ;
  //   taskShowPattern.setInterval(50);
  // } else

  // if ( strcmp(routines[ledMode], "strobeAll") == 0  ) {
  //   taskShowPattern.setCallback(&strobeAll) ;
  //   taskShowPattern.setInterval(5);
  // } else

  // if ( strcmp(routines[ledMode], "rainbow") == 0  ) {
  //   taskShowPattern.setCallback(&rainbow) ;
  //   taskShowPattern.setInterval(10);
  // } else

  if ( strcmp(routines[ledMode], "fadeGlitter") == 0  ) {
    taskShowPattern.setCallback(&fadeGlitter) ;
    taskShowPattern.setInterval(5);
  } else

  if ( strcmp(routines[ledMode], "discoGlitter") == 0  ) {
    taskShowPattern.setCallback(&discoGlitter) ;
    taskShowPattern.setInterval(10);
  } else

  if ( strcmp(routines[ledMode], "wipe") == 0  ) {
    taskShowPattern.setCallback(&wipe) ;
    taskShowPattern.setInterval(10);
  } else

  // if ( strcmp(routines[ledMode], "fire2012") == 0  ) {
  //   taskShowPattern.setCallback(&Fire2012) ;
  //   taskShowPattern.setInterval(30);
  // } else

  // if ( strcmp(routines[ledMode], "fire2012x4") == 0  ) {
  //   taskShowPattern.setCallback(&Fire2012x4) ;
  //   taskShowPattern.setInterval(30);
  // } else

  // if ( strcmp(routines[ledMode], "fire2012all") == 0  ) {
  //   taskShowPattern.setCallback(&Fire2012all) ;
  //   taskShowPattern.setInterval(30);
  // } else

  if ( strcmp(routines[ledMode], "noiselava") == 0  ) {
    taskShowPattern.setCallback(&fillnoise8lava) ;
    taskShowPattern.setInterval(10);
  } else

  if ( strcmp(routines[ledMode], "noiseparty") == 0  ) {
    taskShowPattern.setCallback(&fillnoise8party) ;
    taskShowPattern.setInterval(10);
  }

}

void nextLedMode() {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    LEDS.show() ; // clear the canvas - prevent power fluctuations if a next pattern has lots of brightness
    ledMode++;
    DEBUG_PRINTLN(ledMode);
    if (ledMode == NUMROUTINES ) {
    ledMode = 0;
    }
    setTaskCallback() ;
}

#ifdef AUTOADVANCE
void autoAdvanceLedMode() {
  nextLedMode() ;
}
#endif

