#include "octows1.h"

// ************************************************************************
//                              GLOBAL CONFIG
// ************************************************************************

#define DEFAULT_BRIGHTNESS 80
// #define AUTOADVANCE
#define DEFAULT_LEDMODE 10

// ************************************************************************
//                              GLOBAL VARIABLES
// ************************************************************************


CRGB leds[NUM_LEDS];
int _ledMode = DEFAULT_LEDMODE ;
void setTaskCallback() ;
Scheduler runner;
Task taskShowPattern( 10, TASK_FOREVER, &cylon);
Task taskCheckButtonPress( TASK_CHECK_BUTTON_PRESS_INTERVAL, TASK_FOREVER, &checkButtonPress);  

#ifdef AUTOADVANCE
void autoAdvanceLedMode() ;                       
Task taskAutoAdvanceLedMode( 15 * TASK_SECOND, TASK_FOREVER, &autoAdvanceLedMode);
#endif

Button nextPatternButton(BUTTON_PIN);

uint8_t _speed ;


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
    setTaskCallback() ; // Set the callback according to _ledMode

    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    pinMode(POT_HIGH_PIN, OUTPUT);
    digitalWrite(POT_HIGH_PIN, HIGH);
    pinMode(POT_BRIGHTNESS_PIN, INPUT);

    #ifdef AUTOADVANCE
    runner.addTask(taskAutoAdvanceLedMode);
    taskAutoAdvanceLedMode.enable() ;
    #endif

    nextPatternButton.begin();
    runner.addTask(taskCheckButtonPress);
    taskCheckButtonPress.enable() ;
}

// ************************************************************************
//                              MAIN
// ************************************************************************


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
    uint16_t bpm = map8(_speed,5,60) ;
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

void cylonBig() {
    uint16_t bpm = map8(_speed,5,60) ;
    // uint16_t bpm = 20 ;
    // uint8_t hue = beat8( 8 );
    uint8_t hue = map8(_speed,3,20) ;
    uint16_t wavePos = beatsin8(bpm, 0, 48+36);
    if( wavePos < 42 ) {
      leds[STRIP1_START + wavePos] = CHSV(hue, 255, 255);
      leds[STRIP2_START + wavePos] = CHSV(hue, 255, 255);
      leds[STRIP3_START + wavePos] = CHSV(hue, 255, 255);
      leds[STRIP8_START + wavePos] = CHSV(hue, 255, 255);
      leds[STRIP9_START + wavePos] = CHSV(hue, 255, 255);
      leds[STRIP10_START + wavePos] = CHSV(hue, 255, 255);
    }
    if( wavePos < 48 ) {
      leds[STRIP2_START + wavePos] = CHSV(hue, 255, 255);
      leds[STRIP3_START + wavePos] = CHSV(hue, 255, 255);
      leds[STRIP8_START + wavePos] = CHSV(hue, 255, 255);
      leds[STRIP9_START + wavePos] = CHSV(hue, 255, 255);
    }
    if( wavePos < 51 ) {
      leds[STRIP3_START + wavePos] = CHSV(hue, 255, 255);
      leds[STRIP8_START + wavePos] = CHSV(hue, 255, 255);
    }
    if( wavePos > 48 ) {
      leds[STRIP4_START + wavePos - 48] = CHSV(hue, 255, 255);
      leds[STRIP5_START + wavePos - 48] = CHSV(hue, 255, 255);
      leds[STRIP6_START + wavePos - 48] = CHSV(hue, 255, 255);
      leds[STRIP7_START + wavePos - 48] = CHSV(hue, 255, 255);
    }
    LEDS.show();
    fadeAll(map8(_speed,240,250));
}


void wipe() {
    uint16_t bpm = map8(_speed,5,60) ;
    uint8_t hue = map8(_speed,3,20) ;
    int i = beatsin8(bpm, 0, 9);
    fill_solid(leds + stripStart[i], stripLen[i], CHSV(hue, 255, 255) );
    LEDS.show();
    fadeAll(map8(_speed,240,250));
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
  if ( beat8(_speed) > 245 ) {
    fill_solid(leds, NUM_LEDS, CRGB::White );
  } else {
    fill_solid(leds, NUM_LEDS, CRGB::Black); // black
  }
  LEDS.show();
}

void fadeGlitter() {
  addGlitter(110);
  LEDS.show();
  fadeAll(map8(_speed,240,250));
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


// Fire2012 by Mark Kriegsman, July 2012

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
#define COOLING  70
// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120

void Fire2012()
{
  // Array of temperature readings at each simulation cell
    static byte heat[10][NUM_LEDS_PER_STRIP];

  for( int strip = 0 ; strip < 10; strip++ ) {
    // Step 1.  Cool down every cell a little
      for( int i = 0; i < stripLen[strip]; i++) {
        heat[strip][i] = qsub8( heat[strip][i],  random8(0, ((COOLING * 10) / stripLen[strip]) + 2));
      }

      // Step 2.  Heat from each cell drifts 'up' and diffuses a little
      for( int k= stripLen[strip] - 1; k >= 2; k--) {
        heat[strip][k] = (heat[strip][k - 1] + heat[strip][k - 2] + heat[strip][k - 2] ) / 3;
      }

      // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
      if( random8() < SPARKING ) {
        int y = random8(7);
        heat[strip][y] = qadd8( heat[strip][y], random8(160,255) );
      }

      // Step 4.  Map from heat cells to LED colors
      for( int j = 0; j < stripLen[strip]; j++) {
        CRGB color = HeatColor( heat[strip][j]);
          leds[j+stripStart[strip]] = color;
      }
  }
  LEDS.show();

  taskShowPattern.setInterval(lerp8by8(50,2,_speed));
} // end Fire2012



void rainbow() {
  static uint8_t hue = 0;
  for(int i = 0; i < 10; i++) {
    for(int j = 0; j < stripLen[i]; j++) {
      leds[stripStart[i] + j] = CHSV((32*i) + hue+j,192,255);
    }
  }

  hue++;
  LEDS.show();
}

// So awesome
void redDots2() {
  boolean glitchy = false ;
  fill_solid(leds, NUM_LEDS, CRGB::Black); // black

if( glitchy ){
  for( int i = taskShowPattern.getRunCounter() % beatsin8(30,0,10) ; i < NUM_LEDS ; i = i + 10 ) {
      leds[i] = CRGB::Gray;
  }
} else {
    for( int i = taskShowPattern.getRunCounter() % 10 ; i < NUM_LEDS ; i = i + 10 ) {
      leds[i] = CRGB::Gray;
  }

}

  // Add red static dots
  for( int i = 0 ; i < NUM_LEDS ; i ++ ) {
    if( i % 10 == 0 ) {
       leds[i] = CHSV(beat8(20), 255, 255);
    }
  }
  LEDS.show();

  taskShowPattern.setInterval(lerp8by8(50,1,_speed));
}

// ************************************************************************
//                              HELPER FUNCTIONS
// ************************************************************************


void setTaskCallback() {
  // Prevent weird shit with bright patterns at times
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  LEDS.show();
  DEBUG_PRINTLN(_ledMode);

  if ( strcmp(routines[_ledMode], "cylon") == 0  ) {
    taskShowPattern.setCallback(&cylon) ;
    taskShowPattern.setInterval(5);
  } else

  if ( strcmp(routines[_ledMode], "cylonBig") == 0  ) {
    taskShowPattern.setCallback(&cylonBig) ;
    taskShowPattern.setInterval(5);
  } else

  // if ( strcmp(routines[_ledMode], "strobeParts") == 0  ) {
  //   taskShowPattern.setCallback(&strobeParts) ;
  //   taskShowPattern.setInterval(50);
  // } else

  if ( strcmp(routines[_ledMode], "strobeAll") == 0  ) {
    taskShowPattern.setCallback(&strobeAll) ;
    taskShowPattern.setInterval(3);
  } else

  if ( strcmp(routines[_ledMode], "rainbow") == 0  ) {
    taskShowPattern.setCallback(&rainbow) ;
    taskShowPattern.setInterval(10);
  } else

  if ( strcmp(routines[_ledMode], "fadeGlitter") == 0  ) {
    taskShowPattern.setCallback(&fadeGlitter) ;
    taskShowPattern.setInterval(5);
  } else

  if ( strcmp(routines[_ledMode], "discoGlitter") == 0  ) {
    taskShowPattern.setCallback(&discoGlitter) ;
    taskShowPattern.setInterval(10);
  } else

  if ( strcmp(routines[_ledMode], "wipe") == 0  ) {
    taskShowPattern.setCallback(&wipe) ;
    taskShowPattern.setInterval(10);
  } else

  if ( strcmp(routines[_ledMode], "fire2012") == 0  ) {
    taskShowPattern.setCallback(&Fire2012) ;
    taskShowPattern.setInterval(50);
  } else

  if ( strcmp(routines[_ledMode], "redDots2") == 0  ) {
    taskShowPattern.setCallback(&redDots2) ;
    taskShowPattern.setInterval(10);
  } else

  if ( strcmp(routines[_ledMode], "noiselava") == 0  ) {
    taskShowPattern.setCallback(&fillnoise8lava) ;
    taskShowPattern.setInterval(10);
  } else

  if ( strcmp(routines[_ledMode], "noiseparty") == 0  ) {
    taskShowPattern.setCallback(&fillnoise8party) ;
    taskShowPattern.setInterval(10);
  }
}

void nextLedMode() {
    _ledMode++;
    if (_ledMode == NUMROUTINES ) {
    _ledMode = 0;
    }
    setTaskCallback() ;
}

void prevPattern() {
  if (_ledMode == 0 ) {
    _ledMode = NUMROUTINES;
  }
  _ledMode-- ;
  setTaskCallback() ;
}



#ifdef AUTOADVANCE
void autoAdvanceLedMode() {
  nextLedMode() ;
}
#endif

void checkButtonPress()
{
  static uint16_t lastBrightness ;
  static uint16_t lastSpeed ;

  nextPatternButton.read();

  if( nextPatternButton.wasPressed() ) {
    nextLedMode();
  }

  static uint32_t Pot1Old;
  static uint32_t Pot1EMA_S = 0;          //initialization of EMA S
  float Pot1EMA_A = 0.3;      //initialization of EMA alpha
  uint32_t Pot1Current = analogRead(POT_BRIGHTNESS_PIN);
  Pot1EMA_S = (Pot1EMA_A*Pot1Current) + ((1-Pot1EMA_A)*Pot1EMA_S);
  
  uint16_t calculatedBrightness;

  if( Pot1EMA_S != Pot1Old ) {
    Pot1Old = Pot1EMA_S ;
    calculatedBrightness = map(Pot1EMA_S,0,1023,3,255);
    if( calculatedBrightness != lastBrightness ) {
      lastBrightness = calculatedBrightness;
      LEDS.setBrightness(lastBrightness);
      DEBUG_PRINT("Brightness: ");
      DEBUG_PRINTLN(lastBrightness);
    }
  }

  static uint32_t Pot2Old;
  static uint32_t Pot2EMA_S = 0;          //initialization of EMA S
  float Pot2EMA_A = 0.3;      //initialization of EMA alpha
  uint32_t Pot2Current = analogRead(POT_SPEED_PIN);
  Pot2EMA_S = (Pot2EMA_A*Pot2Current) + ((1-Pot2EMA_A)*Pot2EMA_S);
  
  uint16_t calculatedSpeed;

  if( Pot2EMA_S != Pot2Old ) {
    Pot2Old = Pot2EMA_S ;
    calculatedSpeed = map(Pot2EMA_S,0,1023,3,255);
    if( calculatedSpeed != lastSpeed ) {
      lastSpeed = calculatedSpeed;
      _speed = lastSpeed;
      DEBUG_PRINT("Speed: ");
      DEBUG_PRINTLN(lastSpeed);
    }
  }
}