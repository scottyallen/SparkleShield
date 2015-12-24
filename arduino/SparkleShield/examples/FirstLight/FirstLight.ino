// Light up each LED, one after another, starting at LED1, ending at LED70.

#include <FastLED.h>
#include <SparkleShield.h>

#define BRIGHTNESS 64

SparkleShield sparkle;

void loop() {
  // Step through the rows
  for(int y = 0; y < 7; y++) {
    // Step through the columns
    for(int x = 0; x < 10; x++) {
      // Set the current LED to be white:
      //  - hue: doesn't matter
      //  - saturation: zero, so there's no color
      //  - value: full brightness
      sparkle.set(x, y, CHSV(0, 0, 255));

      // Send the colors to the LEDs.
      sparkle.show();

      // Wait 0.1 seconds, so we have time to see the LED light up.
      delay(100);

      // Turn the LED off again, so it doesn't stay lit when we move onto the next led.
      sparkle.set(x, y, CHSV(0, 0, 0));
    }
  }
}

void setup() {
  sparkle.setBrightness(BRIGHTNESS);
}
