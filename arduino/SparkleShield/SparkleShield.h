/*
  SparkleShield.h - Library for controlling the Sparkle Shield LED display.
  Created by Scotty Allen, December 18, 2015.
  Released under a GPL v2 license.
*/

#ifndef SparkleShield_h
#define SparkleShield_h

#include "Arduino.h"
#include <FastLED.h>

#define COLOR_ORDER RGB
#define CHIPSET     WS2811
#define DEFAULT_LED_PIN  6
#define DEFAULT_BRIGHTNESS  32
const uint8_t kMatrixWidth = 10;
const uint8_t kMatrixHeight = 7;
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)

class SparkleShield {
  public:
    SparkleShield();
    void show();
    uint16_t XY( uint8_t x, uint8_t y);
    uint16_t XYsafe( uint8_t x, uint8_t y);
    void setBrightness(uint8_t brightness);
    void set(uint8_t x, uint8_t y, CHSV color);
    void scroll_text(char *text, CHSV color);
    void draw_text(int x, int y, char *text, CHSV color);
    void draw_sprite(int xOffset, int yOffset, unsigned char* sprite, int length, CHSV color);
    void clear();
    void clear(CHSV color);

    static CRGB leds_plus_safety_pixel[];
    static CRGB* leds;
};

#endif
