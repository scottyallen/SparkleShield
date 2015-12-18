/*
  SparkleShield.cpp - Library for controlling the Sparkle Shield LED display.
  Created by Scotty Allen, December 18, 2015.
  Released under a GPL v2 license.
*/

#include "Arduino.h"
#include "SparkleShield.h"
#include <FastLED.h>

// TODO(scotty): Add ability to change pin and support chaining

CRGB SparkleShield::leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* SparkleShield::leds( leds_plus_safety_pixel + 1);

SparkleShield::SparkleShield() {
  FastLED.addLeds<CHIPSET, DEFAULT_LED_PIN, COLOR_ORDER>(SparkleShield::leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( DEFAULT_BRIGHTNESS );
}

void SparkleShield::show() {
  FastLED.show();
}

void SparkleShield::setBrightness(uint8_t brightness) {
  FastLED.setBrightness(brightness);
}

// Helper functions for an two-dimensional XY matrix of pixels.
//
//     XY(x,y) takes x and y coordinates and returns an LED index number,
//             for use like this:  leds[ XY(x,y) ] == CRGB::Red;
//             No error checking is performed on the ranges of x and y.
//
//     XYsafe(x,y) takes x and y coordinates and returns an LED index number,
//             for use like this:  leds[ XY(x,y) ] == CRGB::Red;
//             Error checking IS performed on the ranges of x and y, and an
//             index of "-1" is returned.  Special instructions below
//             explain how to use this without having to do your own error
//             checking every time you use this function.
//             This is a slightly more advanced technique, and
//             it REQUIRES SPECIAL ADDITIONAL setup, described below.


// This function will return the right 'led index number' for
// a given set of X and Y coordinates on your matrix.
// IT DOES NOT CHECK THE COORDINATE BOUNDARIES.
// That's up to you.  Don't pass it bogus values.
//
// Use the "XY" function like this:
//
//    for( uint8_t x = 0; x < kMatrixWidth; x++) {
//      for( uint8_t y = 0; y < kMatrixHeight; y++) {
//
//        // Here's the x, y to 'led index' in action:
//        leds[ XY( x, y) ] = CHSV( random8(), 255, 255);
//
//      }
//    }
//
//
uint16_t SparkleShield::XY( uint8_t x, uint8_t y) {
  return (y * kMatrixWidth) + x;
}

// Here's a helpful technique that can be tricky to set up, but
// then helps you avoid the needs for sprinkling array-bound-checking
// throughout your code.
//
// It requires a careful attention to get it set up correctly, but
// can potentially make your code smaller and faster.
//
// Suppose you have an 8 x 5 matrix of 40 LEDs.  Normally, you'd
// delcare your leds array like this:
//    CRGB leds[40];
// But instead of that, declare an LED buffer with one extra pixel in
// it, "leds_plus_safety_pixel".  Then declare "leds" as a pointer to
// that array, but starting with the 2nd element (id=1) of that array:
//    CRGB leds_with_safety_pixel[41];
//    const CRGB* leds( leds_plus_safety_pixel + 1);
// Then you use the "leds" array as you normally would.
// Now "leds[0..N]" are aliases for "leds_plus_safety_pixel[1..(N+1)]",
// AND leds[-1] is now a legitimate and safe alias for leds_plus_safety_pixel[0].
// leds_plus_safety_pixel[0] aka leds[-1] is now your "safety pixel".
//
// Now instead of using the XY function above, use the one below, "XYsafe".
//
// If the X and Y values are 'in bounds', this function will return an index
// into the visible led array, same as "XY" does.
// HOWEVER -- and this is the trick -- if the X or Y values
// are out of bounds, this function will return an index of -1.
// And since leds[-1] is actually just an alias for leds_plus_safety_pixel[0],
// it's a totally safe and legal place to access.  And since the 'safety pixel'
// falls 'outside' the visible part of the LED array, anything you write
// there is hidden from view automatically.
// Thus, this line of code is totally safe, regardless of the actual size of
// your matrix:
//    leds[ XYsafe( random8(), random8() ) ] = CHSV( random8(), 255, 255);
//
// The only catch here is that while this makes it safe to read from and
// write to 'any pixel', there's really only ONE 'safety pixel'.  No matter
// what out-of-bounds coordinates you write to, you'll really be writing to
// that one safety pixel.  And if you try to READ from the safety pixel,
// you'll read whatever was written there last, reglardless of what coordinates
// were supplied.

uint16_t SparkleShield::XYsafe( uint8_t x, uint8_t y) {
  if ( x >= kMatrixWidth) return -1;
  if ( y >= kMatrixHeight) return -1;
  return XY(x, y);
}

void SparkleShield::set(uint8_t x, uint8_t y, CHSV color) {
  SparkleShield::leds[SparkleShield::XYsafe(x, y)] = color;
}
