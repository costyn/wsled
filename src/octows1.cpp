#include "octows1.h"

// ************************************************************************
//                              GLOBAL CONFIG
// ************************************************************************

#define DEFAULT_BRIGHTNESS 80
#define DEFAULT_LEDMODE 10

// ************************************************************************
//                              DUMMY FUNCTIONS
// ************************************************************************

void checkPots();
void setTaskCallback();
void autoAdvanceLedMode();

// ************************************************************************
//                              GLOBAL VARIABLES
// ************************************************************************

CRGB leds[NUM_LEDS];
int _ledMode = DEFAULT_LEDMODE;
Scheduler runner;
Task taskShowPattern(10, TASK_FOREVER, &cylon);
Task taskCheckButtonPress(TASK_CHECK_BUTTON_PRESS_INTERVAL, TASK_FOREVER, &checkButtonPress);

Task taskAutoAdvanceLedMode(15 * TASK_SECOND, TASK_FOREVER, &autoAdvanceLedMode);

Button nextPatternButton(BUTTON_PIN);

uint8_t _speed;

// ************************************************************************
//                              SETUP
// ************************************************************************

void setup()
{
  delay(2000);
  LEDS.addLeds<OCTOWS2811>(leds, NUM_LEDS_PER_STRIP);
  fill_solid(leds, NUM_LEDS, CRGB::Black); // cleanup any LEDs stuck on from previous boot
  LEDS.setBrightness(DEFAULT_BRIGHTNESS);
  LEDS.show();
  Serial.begin(115200);
  runner.init();
  runner.addTask(taskShowPattern);
  taskShowPattern.enable();
  setTaskCallback(); // Set the callback according to _ledMode

  // Turn on LED
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  pinMode(POT_HIGH_PIN, OUTPUT);    // 3v source
  digitalWrite(POT_HIGH_PIN, HIGH); // 3v source
  pinMode(POT_BRIGHTNESS_PIN, INPUT);
  pinMode(POT_SPEED_PIN, INPUT);
  pinMode(LED_INDICATOR_PIN, OUTPUT);    // 3v source

  runner.addTask(taskAutoAdvanceLedMode);

  nextPatternButton.begin();
  runner.addTask(taskCheckButtonPress);
  taskCheckButtonPress.enable();
}

// ************************************************************************
//                              MAIN
// ************************************************************************

void loop()
{
  runner.execute();
}

// ************************************************************************
//                              PATTERNS
// ************************************************************************

void testOneStrip(int i)
{
  fill_solid(leds + stripStart[i], stripLen[i], CRGB::Red);
  LEDS.show();
}

void cylon()
{
  uint16_t bpm = map8(_speed, 5, 60);
  uint8_t hue = beat8(8);
  for (int i = 0; i < 10; i++)
  {
    cylon1(bpm, stripStart[i], stripEnd[i], hue);
  }
  LEDS.show();
}

void cylon1(uint16_t bpm, uint16_t startPos, uint16_t endPos, uint8_t hue)
{
  leds[beatsin88(bpm * 256, startPos, endPos - 1)] = CHSV(hue, 255, 255);
  // Serial.println(beatsin88( bpm*256, startPos, endPos - 1 ));
  fadeSome(245, startPos, endPos);
}

void cylonBig()
{
  uint16_t bpm = map8(_speed, 5, 60);
  // uint16_t bpm = 20 ;
  // uint8_t hue = beat8( 8 );
  uint8_t hue = map8(_speed, 3, 20);
  uint16_t wavePos = beatsin8(bpm, 0, 48 + 36);
  if (wavePos < 42)
  {
    leds[STRIP1_START + wavePos] = CHSV(hue, 255, 255);
    leds[STRIP2_START + wavePos] = CHSV(hue, 255, 255);
    leds[STRIP3_START + wavePos] = CHSV(hue, 255, 255);
    leds[STRIP8_START + wavePos] = CHSV(hue, 255, 255);
    leds[STRIP9_START + wavePos] = CHSV(hue, 255, 255);
    leds[STRIP10_START + wavePos] = CHSV(hue, 255, 255);
  }
  if (wavePos < 48)
  {
    leds[STRIP2_START + wavePos] = CHSV(hue, 255, 255);
    leds[STRIP3_START + wavePos] = CHSV(hue, 255, 255);
    leds[STRIP8_START + wavePos] = CHSV(hue, 255, 255);
    leds[STRIP9_START + wavePos] = CHSV(hue, 255, 255);
  }
  if (wavePos < 51)
  {
    leds[STRIP3_START + wavePos] = CHSV(hue, 255, 255);
    leds[STRIP8_START + wavePos] = CHSV(hue, 255, 255);
  }
  if (wavePos > 48)
  {
    leds[STRIP4_START + wavePos - 48] = CHSV(hue, 255, 255);
    leds[STRIP5_START + wavePos - 48] = CHSV(hue, 255, 255);
    leds[STRIP6_START + wavePos - 48] = CHSV(hue, 255, 255);
    leds[STRIP7_START + wavePos - 48] = CHSV(hue, 255, 255);
  }
  LEDS.show();
  fadeAll(map8(_speed, 240, 250));
}

void wipe()
{
  uint16_t bpm = map8(_speed, 5, 60);
  uint8_t hue = map8(_speed, 3, 20);
  int i = beatsin8(bpm, 0, 9);
  fill_solid(leds + stripStart[i], stripLen[i], CHSV(hue, 255, 255));
  LEDS.show();
  fadeAll(map8(_speed, 240, 250));
}

void fadeSome(uint8_t fade_all_speed, uint16_t startPos, uint16_t endPos)
{
  for (uint16_t i = startPos; i < endPos; i++)
  {
    leds[i].nscale8(fade_all_speed);
  }
}

void fadeAll(uint8_t fade_all_speed)
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i].nscale8(fade_all_speed);
  }
  // DEBUG_PRINTLN("fadeAll");
}

void strobeAll()
{
  if (beat8(_speed) > 245)
  {
    fill_solid(leds, NUM_LEDS, CRGB::White);
  }
  else
  {
    fill_solid(leds, NUM_LEDS, CRGB::Black); // black
  }
  LEDS.show();
}

void fadeGlitter()
{
  addGlitter(110);
  LEDS.show();
  fadeAll(map8(_speed, 240, 250));
}

void discoGlitter()
{
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  addGlitter(255);
  LEDS.show();
}

void addGlitter(fract8 chanceOfGlitter)
{
  for (uint8_t i = 0; i < 5; i++)
  {
    if (random8() < chanceOfGlitter)
    {
      leds[random16(NUM_LEDS)] += CRGB::White;
    }
  }
}

void fillnoise8lava()
{
  fillnoise8(0, map8(_speed,0,24), 20, 1); // pallette, speed, scale, loop
}

void fillnoise8party()
{
  fillnoise8(1, map8(_speed,0,24), 20, 1); // pallette, speed, scale, loop
}

void fillnoise8rainbow()
{
  fillnoise8(2, map8(_speed,0,24), 10, 1); // pallette, speed, scale, loop
}

void fillnoise8(uint8_t currentPalette, uint8_t speed, uint8_t scale, boolean colorLoop)
{
  static uint16_t noise[NUM_LEDS];

  const CRGBPalette16 palettes[] = {LavaColors_p, PartyColors_p, RainbowColors_p};

  static uint16_t x = random16();
  static uint16_t y = random16();
  static uint16_t z = random16();

  // If we're runing at a low "speed", some 8-bit artifacts become visible
  // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
  // The amount of data smoothing we're doing depends on "speed".
  uint16_t dataSmoothing = 0;

  if (speed < 50)
  {
    dataSmoothing = 200 - (speed * 4);
  }

  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    uint16_t ioffset = scale * i;

    uint16_t data = inoise8(x + ioffset, y, z);

    // The range of the inoise8 function is roughly 16-238.
    // These two operations expand those values out to roughly 0..255
    // You can comment them out if you want the raw noise data.
    data = qsub8(data, 16);
    data = qadd8(data, scale8(data, 39));

    if (dataSmoothing)
    {
      uint16_t olddata = noise[i];
      uint16_t newdata = scale8(olddata, dataSmoothing) + scale8(data, 256 - dataSmoothing);
      data = newdata;
    }

    noise[i] = data;
  }

  z += speed;

  // apply slow drift to X and Y, just for visual variation.
  x += speed / 8;
  y -= speed / 16;

  static uint8_t ihue = 0;

  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    // We use the value at the i coordinate in the noise
    // array for our brightness, and a 'random' value from NUM_LEDS - 1
    // for our pixel's index into the color palette.

    uint16_t index = noise[i];
    uint16_t bri = noise[NUM_LEDS - i];
    // uint8_t bri =  sin(noise[NUM_LEDS - i]);  // more light/dark variation

    // if this palette is a 'loop', add a slowly-changing base value
    if (colorLoop)
    {
      index += ihue;
    }

    // brighten up, as the color palette itself often contains the
    // light/dark dynamic range desired
    if (bri > 127)
    {
      bri = 255;
    }
    else
    {
      bri = dim8_raw(bri * 2);
    }

    CRGB color = ColorFromPalette(palettes[currentPalette], index, bri);
    leds[i] = color;
  }
  ihue += 1;

  LEDS.show();
}

// Fire2012 by Mark Kriegsman, July 2012

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
#define COOLING 70
// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120

void Fire2012()
{
  // Array of temperature readings at each simulation cell
  static byte heat[10][NUM_LEDS_PER_STRIP];

  for (int strip = 0; strip < 10; strip++)
  {
    // Step 1.  Cool down every cell a little
    for (int i = 0; i < stripLen[strip]; i++)
    {
      heat[strip][i] = qsub8(heat[strip][i], random8(0, ((COOLING * 10) / stripLen[strip]) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = stripLen[strip] - 1; k >= 2; k--)
    {
      heat[strip][k] = (heat[strip][k - 1] + heat[strip][k - 2] + heat[strip][k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < SPARKING)
    {
      int y = random8(7);
      heat[strip][y] = qadd8(heat[strip][y], random8(160, 255));
    }

    // Step 4.  Map from heat cells to LED colors
    for (int j = 0; j < stripLen[strip]; j++)
    {
      CRGB color = HeatColor(heat[strip][j]);
      leds[j + stripStart[strip]] = color;
    }
  }
  LEDS.show();

  taskShowPattern.setInterval(lerp8by8(50, 2, _speed));
} // end Fire2012

void rainbow()
{
  static uint8_t startindex = 0;
  uint8_t incindex = 2;
  // fill_palette(leds, NUM_LEDS, startindex, incindex, RainbowColors_p, 255, LINEARBLEND);
  fill_palette(leds + STRIP1_START, STRIP1_LEN, startindex, incindex, RainbowColors_p, 255, LINEARBLEND);
  fill_palette(leds + STRIP2_START, STRIP2_LEN, startindex + STRIP1_LEN, incindex, RainbowColors_p, 255, LINEARBLEND);
  fill_palette(leds + STRIP3_START, STRIP3_LEN, startindex + STRIP2_LEN, incindex, RainbowColors_p, 255, LINEARBLEND);
  fill_palette(leds + STRIP4_START, STRIP4_LEN, startindex + STRIP3_LEN, incindex, RainbowColors_p, 255, LINEARBLEND);
  fill_palette(leds + STRIP5_START, STRIP5_LEN, startindex + STRIP4_LEN, incindex, RainbowColors_p, 255, LINEARBLEND);
  fill_palette(leds + STRIP6_START, STRIP6_LEN, startindex + STRIP5_LEN, incindex, RainbowColors_p, 255, LINEARBLEND);
  fill_palette(leds + STRIP7_START, STRIP7_LEN, startindex + STRIP6_LEN, incindex, RainbowColors_p, 255, LINEARBLEND);
  fill_palette(leds + STRIP8_START, STRIP8_LEN, startindex + STRIP7_LEN, incindex, RainbowColors_p, 255, LINEARBLEND);
  fill_palette(leds + STRIP9_START, STRIP9_LEN, startindex + STRIP8_LEN, incindex, RainbowColors_p, 255, LINEARBLEND);
  fill_palette(leds + STRIP10_START, STRIP10_LEN, startindex + STRIP9_LEN, incindex, RainbowColors_p, 255, LINEARBLEND);
  startindex -= map8(_speed,0,12);
  LEDS.show();
}

// So awesome
void redDots2()
{
  fill_solid(leds, NUM_LEDS, CRGB::Black);

  for (int i = taskShowPattern.getRunCounter() % 10; i < NUM_LEDS; i = i + 10)
  {
    leds[i] = CRGB::Gray;
  }

  // Add colored static dots
  for (int i = 0; i < NUM_LEDS; i++)
  {
    if (i % 10 == 0)
    {
      leds[i] = CHSV(beat8(20), 255, 255);
    }
  }
  LEDS.show();

  taskShowPattern.setInterval(lerp8by8(50, 1, _speed));
}

void longPressOk()
{
  fill_solid(leds, NUM_LEDS, CRGB::Green);
  LEDS.show();
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  LEDS.show();
  fill_solid(leds, NUM_LEDS, CRGB::Green);
  LEDS.show();
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  LEDS.show();
  setTaskCallback(); // Set pattern back to _ledMode
}

void moveOut(uint16_t bpm, uint16_t startPos, uint16_t endPos, uint8_t hue)
{
  leds[map(beat88(bpm * 256), 0, 65536, startPos, endPos - 1)] = CHSV(hue, 255, 255);
  fadeSome(245, startPos, endPos);
}


void radiate() {
  // DEBUG_PRINT(taskShowPattern.getRunCounter());
  // DEBUG_PRINT("   ");
  // DEBUG_PRINTLN(mod(taskShowPattern.getRunCounter(),1));
  static uint8_t stripNr = 0;
  // fill_solid(leds, NUM_LEDS, CRGB::Black);
  

 EVERY_N_MILLIS(500) {
    DEBUG_PRINTLN(stripNr);
    if( stripNr == 2 ) {
      fill_solid(leds + stripStart[0], stripLen[0], CRGB::Red);
      fill_solid(leds + stripStart[9], stripLen[9], CRGB::Red);
    }
    if( stripNr == 1 ) {
      fill_solid(leds + stripStart[1], stripLen[1], CRGB::Red);
      fill_solid(leds + stripStart[8], stripLen[8], CRGB::Red);
    }
    if( stripNr == 0 ) {
      fill_solid(leds + stripStart[2], stripLen[2], CRGB::Red);
      fill_solid(leds + stripStart[7], stripLen[7], CRGB::Red);
    }
    stripNr++ ;
    if( stripNr == 3 ) {
      stripNr = 0;
    }
  }

  uint16_t bpm = 25;
  uint8_t hue = CRGB::Red;
  for (int i = 3; i < 7; i++)
  {
    moveOut(bpm, stripStart[i], stripEnd[i], hue);
  }
  LEDS.show();

  fadeAll(250);
}

void radiate2()
{
  // fill_solid(leds, NUM_LEDS, CRGB::Black);

  // for (int i = taskShowPattern.getRunCounter() % 9; i < NUM_LEDS; i = i + 2)
  // {
  //   leds[i] = CRGB::Red;
  // }
  // for (int i = taskShowPattern.getRunCounter() % 9; i < NUM_LEDS; i = i + 4)
  // {
  //   leds[i] = CRGB::White;
  // }


  for( int i  = NUM_LEDS ; i > 0; i-- ) {
    leds[i] = leds[i-1] ;
  }

  if( taskShowPattern.getRunCounter() % 10 == 0 ) {
    leds[0] = CRGB::Red;
  } else {
    leds[0] = CRGB::Black;
  }

  // fadeAll(map8(_speed, 240, 250));
  LEDS.show();




  // for (uint8_t i = 0; i < numTwirlers; i++)
  // {
  //   if ((i % 2) == 0)
  //   {
  //     // pos = (int)(round(clockwiseFirst + round(NUM_LEDS / numTwirlers) * i)) % NUM_LEDS;
  //     if (leds[pos])
  //     { 
  //       // FALSE if currently BLACK - don't blend with black
  //       leds[pos] = blend(leds[pos], clockwiseColor, 128);
  //     }
  //     else
  //     {
  //       leds[pos] = clockwiseColor;
  //     }
  //   }
  //   else
  //   {
  //     pos = (int)(round(clockwiseFirst + round(NUM_LEDS / numTwirlers) * i)) % NUM_LEDS;
  //     if (leds[pos])
  //     { // FALSE if currently BLACK - don't blend with black
  //       leds[pos] = blend(leds[pos], antiClockwiseColor, 128);
  //     }
  //     else
  //     {
  //       leds[pos] = antiClockwiseColor;
  //     }
  //   }
  // }
}

void FillLEDsFromPaletteColors() {
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; /* motion speed */
  FillLEDsFromPaletteColors( startIndex);
  // DEBUG_PRINTLN(startIndex);
  LEDS.show();
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
  uint8_t brightness = 255;
  CRGBPalette16 currentPalette( RainbowStripeColors_p );
  
  for( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex + sin8(i*16), brightness);
    colorIndex += 3;
  }

}

void paletteBeat()
{
  PeriodicallyChooseNewColorPalettes(); // currently does nothing
  MixBeatPalette(); // mix up the new 'beat palette' to draw with
  DrawOneFrameUsingBeatPalette(); // draw a simple animation using the 'beat palette'
  LEDS.show();
}

const CRGBPalette16 palettes[] = { 
  RainbowColors_p,
  RainbowStripeColors_p,
  HeatColors_p,
  LavaColors_p,
  PartyColors_p,
} ;

CRGBPalette16 gCurrentPaletteA( CRGB::Black);
CRGBPalette16 gCurrentPaletteB( CRGB::Black);

CRGBPalette16 gTargetPaletteA( palettes[0]);
// CRGBPalette16 gTargetPaletteB( CRGB::Red );
CRGBPalette16 gTargetPaletteB( palettes[1]);

CRGBPalette16 gBeatPalette;

void MixBeatPalette()
{
  // uint8_t paletteBeatsPerMinute = map8(_speed, 30, 120);
  uint8_t paletteBeatsPerMinute = 60;

  uint8_t beat = beat8( paletteBeatsPerMinute); // repeats from 0..255 

  // 'cubicwave8' spends more time at each end than sin8, and less time
  // in the middle.  Try others: triwave8, quadwave8, sin8, cubicwave8
  uint8_t mixer = cubicwave8( beat);

  // Mix a new palette, gBeatPalette, with a varying amount of contribution
  // from gCurrentPaletteA and gCurrentPaletteB, depending on 'mixer'.
  // The 'beat palette' is then used to draw onto the LEDs.
  uint8_t palettesize = sizeof( gBeatPalette) / sizeof(gBeatPalette[0]); // = 16
  blend( gCurrentPaletteA, gCurrentPaletteB, gBeatPalette, palettesize, mixer);
}


// Sample draw function to draw some pixels using the colors in gBeatPalette
void DrawOneFrameUsingBeatPalette()
{
  static uint8_t startindex = 0;
  uint8_t incindex = 1;
  fill_palette(leds, NUM_LEDS, startindex, incindex, gBeatPalette, 255, LINEARBLEND);
  // fill_palette(leds, NUM_LEDS, startindex, incindex, RainbowStripeColors_p, 255, LINEARBLEND);
  startindex -= map8(_speed,0,4);
}


// If you wanted to change the two palettes from 'rainbow' and 'red'
// to something else, here's where you'd do it.
void PeriodicallyChooseNewColorPalettes()
{
  EVERY_N_SECONDS(11) {
    gTargetPaletteA = palettes[random8(5)];
  }

  EVERY_N_SECONDS(17) {
    gTargetPaletteB = palettes[random8(5)];
  }
  
  EVERY_N_MILLISECONDS(20) {
    nblendPaletteTowardPalette( gCurrentPaletteA, gTargetPaletteA);
    nblendPaletteTowardPalette( gCurrentPaletteB, gTargetPaletteB);
  }
}

// ************************************************************************
//                              HELPER FUNCTIONS
// ************************************************************************

void setTaskCallback()
{
  // Prevent weird shit with bright patterns at times
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  LEDS.show();
  DEBUG_PRINTLN(_ledMode);

  if (strcmp(routines[_ledMode], "cylon") == 0)
  {
    taskShowPattern.setCallback(&cylon);
    taskShowPattern.setInterval(5);
  }
  else if (strcmp(routines[_ledMode], "cylonBig") == 0)
  {
    taskShowPattern.setCallback(&cylonBig);
    taskShowPattern.setInterval(5);
  }
  else if (strcmp(routines[_ledMode], "strobeAll") == 0)
  {
    taskShowPattern.setCallback(&strobeAll);
    taskShowPattern.setInterval(3);
  }
  else if (strcmp(routines[_ledMode], "rainbow") == 0)
  {
    taskShowPattern.setCallback(&rainbow);
    taskShowPattern.setInterval(20);
  }
  else if (strcmp(routines[_ledMode], "fadeGlitter") == 0)
  {
    taskShowPattern.setCallback(&fadeGlitter);
    taskShowPattern.setInterval(5);
  }
  else if (strcmp(routines[_ledMode], "discoGlitter") == 0)
  {
    taskShowPattern.setCallback(&discoGlitter);
    taskShowPattern.setInterval(20);
  }
  else if (strcmp(routines[_ledMode], "wipe") == 0)
  {
    taskShowPattern.setCallback(&wipe);
    taskShowPattern.setInterval(10);
  }
  else if (strcmp(routines[_ledMode], "fire2012") == 0)
  {
    taskShowPattern.setCallback(&Fire2012);
    taskShowPattern.setInterval(50);
  }
  else if (strcmp(routines[_ledMode], "redDots2") == 0)
  {
    taskShowPattern.setCallback(&redDots2);
    taskShowPattern.setInterval(10);
  }
  else if (strcmp(routines[_ledMode], "noiselava") == 0)
  {
    taskShowPattern.setCallback(&fillnoise8lava);
    taskShowPattern.setInterval(10);
  }
  else if (strcmp(routines[_ledMode], "noiseparty") == 0)
  {
    taskShowPattern.setCallback(&fillnoise8party);
    taskShowPattern.setInterval(10);
  }
  else if (strcmp(routines[_ledMode], "noiserainbow") == 0)
  {
    taskShowPattern.setCallback(&fillnoise8rainbow);
    taskShowPattern.setInterval(10);
  }
  else if (strcmp(routines[_ledMode], "radiate") == 0)
  {
    taskShowPattern.setCallback(&radiate);
    taskShowPattern.setInterval(10);
  }
  else if (strcmp(routines[_ledMode], "palettebeat") == 0)
  {
    taskShowPattern.setCallback(&paletteBeat);
    taskShowPattern.setInterval(10);
  }
}

void nextLedMode()
{
  _ledMode++;
  if (_ledMode == NUMROUTINES)
  {
    _ledMode = 0;
  }
  setTaskCallback();
}

void prevPattern()
{
  if (_ledMode == 0)
  {
    _ledMode = NUMROUTINES;
  }
  _ledMode--;
  setTaskCallback();
}

void autoAdvanceLedMode()
{
  nextLedMode();
}

#define LONGPRESS 1000

void checkButtonPress()
{
  nextPatternButton.read();

  if (nextPatternButton.pressedFor(LONGPRESS))
  {
    DEBUG_PRINTLN("LONGPRESS detected");
    if (taskAutoAdvanceLedMode.isEnabled())
    {
      taskAutoAdvanceLedMode.disable();
      DEBUG_PRINTLN("autoAdvance: DISABLED");
    }
    else
    {
      taskAutoAdvanceLedMode.enable();
      DEBUG_PRINTLN("autoAdvance: ENABLED");
    }
    taskShowPattern.setCallback(&longPressOk);
  } 
  else if (nextPatternButton.wasReleased())
  {
    DEBUG_PRINTLN("SHORTpress detected");
    nextLedMode();
    digitalWrite(LED_INDICATOR_PIN,HIGH);
    digitalWrite(LED_INDICATOR_PIN,LOW);
  }

  checkPots();
}

// Pots have a 10nF ceramic capacitor between signal and ground.
// Further noise cancelling needed with an Exponential Moving Average
// https://www.norwegiancreations.com/2015/10/tutorial-potentiometers-with-arduino-and-filtering/
void checkPots()
{
  static uint16_t lastBrightness;
  static uint16_t lastSpeed;

  static uint32_t Pot1Old;
  static uint32_t Pot1EMA_S = 0; //initialization of EMA S
  float Pot1EMA_A = 0.20;        //initialization of EMA alpha
  uint32_t Pot1Current = analogRead(POT_BRIGHTNESS_PIN);
  Pot1EMA_S = (Pot1EMA_A * Pot1Current) + ((1 - Pot1EMA_A) * Pot1EMA_S);

  uint16_t calculatedBrightness;

  if (Pot1EMA_S != Pot1Old) {
    Pot1Old = Pot1EMA_S;
    DEBUG_PRINT("Pot1EMA_S: ");
    DEBUG_PRINT(Pot1EMA_S);
    calculatedBrightness = 255 - map(Pot1EMA_S, 0, 800, 0, 253);
    if (calculatedBrightness != lastBrightness)
    {
      lastBrightness = calculatedBrightness;
      LEDS.setBrightness(lastBrightness);
      DEBUG_PRINT("\tBrightness: ");
      DEBUG_PRINTLN(lastBrightness);
    }
  }

  static uint32_t Pot2Old;
  static uint32_t Pot2EMA_S = 0; //initialization of EMA S
  float Pot2EMA_A = 0.20;        //initialization of EMA alpha
  uint32_t Pot2Current = analogRead(POT_SPEED_PIN);
  Pot2EMA_S = (Pot2EMA_A * Pot2Current) + ((1 - Pot2EMA_A) * Pot2EMA_S);

  uint16_t calculatedSpeed;

  if (Pot2EMA_S != Pot2Old)
  {
    Pot2Old = Pot2EMA_S;
    calculatedSpeed = 256 - map(Pot2EMA_S, 0, 800, 3, 255);
    if (calculatedSpeed != lastSpeed)
    {
      lastSpeed = calculatedSpeed;
      _speed = lastSpeed;
      DEBUG_PRINT("Speed: ");
      DEBUG_PRINTLN(lastSpeed);
    }
  }
}