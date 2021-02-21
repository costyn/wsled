#include <Arduino.h>
#include <FastLED.h>
#include <TaskScheduler.h>  // https://github.com/arkhipenko/TaskScheduler

NUM_LEDS = 1000;

int bpm = 30 ;
long patternInterval = round(60000/bpm);

CRGB leds[NUM_LEDS];

void cylon();

Scheduler runner;
Task taskShowPattern(patternInterval, TASK_FOREVER, &cylon);

void setup(){ 
    FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    runner.init();
    runner.addTask(taskShowPattern);
    taskShowPattern.enable();
}

void loop() {
    runner.execute();
}

void cylon(){
    static long ledPos = 0;
    leds[ledPos] = CHSV( beatsin8(5), 255, 255);
    FastLED.show();
    ledPos++ ;
    if( ledPos == NUM_LEDS )
    
    fadeToBlackBy(leds, NUM_LEDS, 10);
}

