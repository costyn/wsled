#ifndef WSLED_H
#define WSLED_H

#define USE_OCTOWS2811

#include <Arduino.h>
#include <FastLED.h>
#include <OctoWS2811.h>
#include <ArduinoTapTempo.h>
#include <TaskScheduler.h>
#include <OneButton.h>


#define NUM_LEDS_PER_STRIP 90
#define NUM_STRIPS 8

#define TORSO 42  // strip 1
#define ARMS 45 // strip 2
#define HEAD 47  // strip 3
#define LEGS 53  // strip 4

#define TORSO_START 0
#define ARMS_START NUM_LEDS_PER_STRIP
#define HEAD_START NUM_LEDS_PER_STRIP * 2
#define LEGS_START NUM_LEDS_PER_STRIP * 3

#define TORSO_END TORSO
#define ARMS_END ARMS_START + ARMS
#define HEAD_END HEAD_START + HEAD
#define LEGS_END LEGS_START + LEGS

class LEDman {
  public:
    void setup();
    void loop();
    void strobe_all();
    void strobe_parts();
    void cylon();
    void solid_red();
    void chasers();
    void identify();
    void rainbow();
    void discoGlitter();
    void addGlitter( fract8 chanceOfGlitter);
    void fadeall(uint8_t fade_all_speed);

    CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];
    ArduinoTapTempo tapTempo;
    OneButton button;
    int ledMode ;
    enum class Patterns {cylon, strobe_parts, strobe_all, rainbow, chasers, discoGlitter};
    Patterns pattern;
};

#endif
