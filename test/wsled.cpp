#include "wsled.h"


// Which pins are connected to the PSP mini joystick
#define Y_AXIS A9
#define X_AXIS A8

// Prototype Methods
void strobeAll();
void strobeParts();
void strobePart(int startP, int numLeds, int delayTime );
void cylon();
void solid_red();
void fadeGlitter();
void identify();
void rainbow();
void black() ;
void discoGlitter();
void addGlitter( fract8 chanceOfGlitter);
void fadeall(uint8_t fade_all_speed);
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

Scheduler runner;
Task taskShowPattern( 10, TASK_FOREVER, &cylon);
Task taskMonitorJoystick( 100, TASK_FOREVER, &monitorJoystick);

#define DEFAULT_BRIGHTNESS 20
#define MIN_BRIGHTNESS 1
#define MAX_BRIGHTNESS 180

uint8_t brightness = DEFAULT_BRIGHTNESS ;

CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

#define NUM_LEDS NUM_STRIPS * NUM_LEDS_PER_STRIP

ArduinoTapTempo tapTempo;
int ledMode = 2 ;

const char *routines[] = {
  "cylon",
  "strobeParts",
  "strobeAll",
  "rainbow",
  "fadeGlitter",
  "discoGlitter",
  "fire2012",
  "fire2012x4",
  "fire2012all",
  "noiselava",
  "noiseparty"
};

#define NUMROUTINES (sizeof(routines)/sizeof(char *)) //array size

// Pin layouts on the teensy 3:
// OctoWS2811: 2,14,7,8,6,20,21,5
// https://www.pjrc.com/store/octo28_adaptor.html

void setup() {
  delay(2000);
  LEDS.addLeds<OCTOWS2811>(leds, NUM_LEDS_PER_STRIP);
  fill_solid(leds, NUM_LEDS, CRGB::Black);  // cleanup any LEDs stuck on from previous boot
  LEDS.show();
  LEDS.setBrightness(brightness);

  LEDS.setMaxPowerInVoltsAndMilliamps(5,500); // sets max amperage to 1500 milliamps (1.5 amps)
  set_max_power_indicator_LED(11);

  Serial.begin(115200);

  runner.init();
  runner.addTask(taskShowPattern);
  taskShowPattern.enable() ;
  runner.addTask(taskMonitorJoystick);
  taskMonitorJoystick.enable() ;
  // Serial.print("Numroutines: ");
  // Serial.print(NUMROUTINES);
  setTaskCallback() ; // Set the callback according to ledMode
}

void loop() {
  runner.execute() ;
}

void nextPattern() {
  ledMode++ ;
  if (ledMode == NUMROUTINES ) {
    ledMode = 0;
  }
  setTaskCallback() ;
}

void prevPattern() {
  if (ledMode == 0 ) {
    ledMode = NUMROUTINES;
  }
  ledMode-- ;
  setTaskCallback() ;
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


void strobeAll() {
  if ( beat8(240) > 245 ) {
    fill_solid(leds, NUM_LEDS, CRGB::White );
  } else {
    fill_solid(leds, NUM_LEDS, CRGB::Black); // black
  }
  LEDS.show();
}

bool last = false ;
void strobeParts() {
  static int iteration = 0 ;
  if( iteration % 4 == 0 ) {
    strobePart(HEAD_START, HEAD, 500) ;
    taskShowPattern.setInterval(50);
  } else
  if( iteration % 4 == 1 ) {
    strobePart(ARMS_START, ARMS, 500) ;
  } else
  if( iteration % 4 == 2 ) {
    strobePart(TORSO_START, TORSO, 500) ;
  } else
  if( iteration % 4 == 3 ) {
    strobePart(LEGS_START, LEGS, 500) ;
    last = true ;
  }
  LEDS.show();
  taskShowPattern.setCallback(&black) ;
  iteration++ ;
}

void black() {
 fill_solid(leds, NUM_LEDS, CRGB::Black );
 LEDS.show();
 if( last ) {
   taskShowPattern.setInterval(800);
   last = false ;
 }
 taskShowPattern.setCallback(&strobeParts) ;
}

void strobePart(int startP, int numLeds, int delayTime ) {
  fill_solid(leds + startP, numLeds, CHSV(random8(),255,255) );
  LEDS.show();
}

void cylon() {
  uint8_t bpm = 30 ;
  uint8_t hue = beat8( 2 );
  leds[beatsin8( bpm, HEAD_START,  HEAD_END  )] = CHSV(hue, 255, 255) ;
  leds[beatsin8( bpm, TORSO_START, TORSO_END )] = CHSV(hue, 255, 255) ;
  leds[beatsin8( bpm, ARMS_START,  ARMS_END  )] = CHSV(hue, 255, 255) ;
  leds[beatsin8( bpm, LEGS_START,  LEGS_END  )] = CHSV(hue, 255, 255) ;
  LEDS.show();
  fadeall(250);
}

void fadeGlitter() {
  addGlitter(110);
  LEDS.show();
  fadeall(246);
}

void identify() {
  // Set the first n leds on each strip to show which strip it is
  for(int i = 0; i < NUM_STRIPS; i++) {
    for(int j = 0; j <= i; j++) {
      leds[(i*NUM_LEDS_PER_STRIP) + j] = CRGB::Red;
    }
  }

  LEDS.show();
  LEDS.delay(500);
}

void rainbow() {
  static uint8_t hue = 0;
  for(int i = 0; i < NUM_STRIPS; i++) {
    for(int j = 0; j < NUM_LEDS_PER_STRIP; j++) {
      leds[(i*NUM_LEDS_PER_STRIP) + j] = CHSV((32*i) + hue+j,192,255);
    }
  }

  hue++;

  LEDS.show();
}


void discoGlitter() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  addGlitter( 255 ) ;
  FastLED.show();
}

void addGlitter( fract8 chanceOfGlitter)
{
  for ( uint8_t i = 0 ; i < 5 ; i++ ) {
    if ( random8() < chanceOfGlitter) {
      leds[ random16(NUM_LEDS) ] += CRGB::White;
    }
  }
}

void fadeall(uint8_t fade_all_speed) {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(fade_all_speed);
  }
}


// Fire2012 by Mark Kriegsman, July 2012

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
#define COOLING  75
// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 70

void Fire2012()
{
  // Array of temperature readings at each simulation cell
    static byte heat[NUM_LEDS_PER_STRIP];

    // Step 1.  Cool down every cell a little
      for( int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
        heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS_PER_STRIP) + 2));
      }

      // Step 2.  Heat from each cell drifts 'up' and diffuses a little
      for( int k= NUM_LEDS_PER_STRIP - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
      }

      // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
      if( random8() < SPARKING ) {
        int y = random8(7);
        heat[y] = qadd8( heat[y], random8(160,255) );
      }

      // Step 4.  Map from heat cells to LED colors
      for( int j = 0; j < NUM_LEDS_PER_STRIP; j++) {
        CRGB color = HeatColor( heat[j]);
        leds[j+HEAD_START] = color;
        leds[j+ARMS_START] = color;
        leds[j+TORSO_START] = color;
        leds[j+LEGS_START] = color;
      }
  LEDS.show();
} // end Fire2012


void Fire2012x4()
{
  // Array of temperature readings at each simulation cell
    static byte heat1[HEAD];
    static byte heat2[ARMS];
    static byte heat3[TORSO];
    static byte heat4[LEGS];

    // Step 1.  Cool down every cell a little
      for( int i = 0; i < HEAD; i++) {
        heat1[i] = qsub8( heat1[i],  random8(0, ((COOLING * 10) / HEAD) + 2));
      }
      for( int i = 0; i < ARMS; i++) {
        heat2[i] = qsub8( heat2[i],  random8(0, ((COOLING * 10) / ARMS) + 2));
      }
      for( int i = 0; i < TORSO; i++) {
        heat3[i] = qsub8( heat3[i],  random8(0, ((COOLING * 10) / TORSO) + 2));
      }
      for( int i = 0; i < LEGS; i++) {
        heat4[i] = qsub8( heat4[i],  random8(0, ((COOLING * 10) / LEGS) + 2));
      }

      // Step 2.  Heat from each cell drifts 'up' and diffuses a little
      for( int k= HEAD - 1; k >= 2; k--) {
        heat1[k] = (heat1[k - 1] + heat1[k - 2] + heat1[k - 2] ) / 3;
      }
      for( int k= ARMS - 1; k >= 2; k--) {
        heat2[k] = (heat2[k - 1] + heat2[k - 2] + heat2[k - 2] ) / 3;
      }
      for( int k= TORSO - 1; k >= 2; k--) {
        heat3[k] = (heat3[k - 1] + heat3[k - 2] + heat3[k - 2] ) / 3;
      }
      for( int k= LEGS - 1; k >= 2; k--) {
        heat4[k] = (heat4[k - 1] + heat4[k - 2] + heat4[k - 2] ) / 3;
      }

      // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
      if( random8() < SPARKING ) {
        int y = random8(7);
        heat1[y] = qadd8( heat1[y], random8(160,255) );
      }
      if( random8() < SPARKING ) {
        int y = random8(7);
        heat2[y] = qadd8( heat2[y], random8(160,255) );
      }
      if( random8() < SPARKING ) {
        int y = random8(7);
        heat3[y] = qadd8( heat3[y], random8(160,255) );
      }
      if( random8() < SPARKING ) {
        int y = random8(7);
        heat4[y] = qadd8( heat4[y], random8(160,255) );
      }

      // Step 4.  Map from heat cells to LED colors
      for( int j = 0; j < HEAD; j++) {
        leds[HEAD_END-1-j]  = HeatColor( heat1[j]);
        leds[HEAD_START+j]  = HeatColor( heat1[j]);
      }
      for( int j = 0; j < ARMS; j++) {
        leds[ARMS_END-1-j]  = HeatColor( heat2[j]);
      }
      for( int j = 0; j < TORSO; j++) {
        leds[TORSO_END-1-j] = HeatColor( heat3[j]);
      }
      for( int j = 0; j < LEGS; j++) {
        leds[LEGS_END-1-j]  = HeatColor( heat4[j]);
      }
  LEDS.show();
} // end Fire2012

#define FIRELEDS LEGS + ARMS + 23
void Fire2012all()
{
    static byte heat[FIRELEDS];
    int pixelnumber;

    // Step 1.  Cool down every cell a little
      for( int i = 0; i < FIRELEDS; i++) {
        heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / FIRELEDS) + 2));
      }

      // Step 2.  Heat from each cell drifts 'up' and diffuses a little
      for( int k= FIRELEDS - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
      }

      // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
      if( random8() < SPARKING ) {
        int y = random8(7);
        heat[y] = qadd8( heat[y], random8(160,255) );
      }

      // Step 4.  Map from heat cells to LED colors
      int i = 0 ;
      for( int j = 0; j < LEGS; j++) {
        pixelnumber = (LEGS_END-1) - i;
        leds[pixelnumber] = HeatColor( heat[j]);
        i++ ;
      }
      i = 0 ;
      for( int j = LEGS; j < LEGS+TORSO; j++) {
        pixelnumber = (ARMS_END-1) - i ;
        leds[pixelnumber] = HeatColor( heat[j]);
        pixelnumber = (TORSO_END-1) - i;
        leds[pixelnumber] = HeatColor( heat[j]);
        i++ ;
      }
      i = 0 ;
      for( int j = LEGS+TORSO; j < LEGS+TORSO+23; j++) {
        leds[HEAD_END-1-i]  = HeatColor( heat[j]);
        leds[HEAD_START+i]  = HeatColor( heat[j]);
        i++ ;
      }

  LEDS.show();
} // end Fire2012



void twirl() {
  int pos = lerp8by8( 0, NUM_LEDS, beat8( 120 ));
  leds[pos] = CRGB::Red ;
  fadeall(250) ;
}


void fillnoise8lava() {
  fillnoise8( 0, 1, 30, 1); // pallette, speed, scale, loop
}

void fillnoise8party() {
  fillnoise8( 1, 1, 30, 1); // pallette, speed, scale, loop
}

void fillnoise8(uint8_t currentPalette, uint8_t speed, uint8_t scale, boolean colorLoop ) {
  static uint8_t noise[NUM_LEDS];

  const CRGBPalette16 palettes[] = { LavaColors_p, PartyColors_p, OceanColors_p } ;

  static uint16_t x = random16();
  static uint16_t y = random16();
  static uint16_t z = random16();

  // If we're runing at a low "speed", some 8-bit artifacts become visible
  // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
  // The amount of data smoothing we're doing depends on "speed".
  uint8_t dataSmoothing = 0;

  if ( speed < 50) {
    dataSmoothing = 200 - (speed * 4);
  }

  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    int ioffset = scale * i;

    uint8_t data = inoise8(x + ioffset, y, z);

    // The range of the inoise8 function is roughly 16-238.
    // These two operations expand those values out to roughly 0..255
    // You can comment them out if you want the raw noise data.
    data = qsub8(data, 16);
    data = qadd8(data, scale8(data, 39));

    if ( dataSmoothing ) {
      uint8_t olddata = noise[i];
      uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
      data = newdata;
    }

    noise[i] = data;
  }

  z += speed;

  // apply slow drift to X and Y, just for visual variation.
  x += speed / 8;
  y -= speed / 16;

  static uint8_t ihue = 0;

  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    // We use the value at the i coordinate in the noise
    // array for our brightness, and a 'random' value from NUM_LEDS - 1
    // for our pixel's index into the color palette.

    uint8_t index = noise[i];
    uint8_t bri =   noise[NUM_LEDS - i];
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

