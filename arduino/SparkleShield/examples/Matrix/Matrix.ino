#include <FastLED.h>
#include <SparkleShield.h>
#include <math.h>

#define BRIGHTNESS 64 

SparkleShield sparkle;

void loop() {
  DrawOneFrame(millis());
  sparkle.show();
}

void setup() {
  sparkle.setBrightness(BRIGHTNESS);
}

void DrawOneFrame(unsigned long ms) {
  sparkle.clear();
  drawMatrixColumn(ms, 0, 14.0, 300.0);
  drawMatrixColumn(ms, 1, 14.0, 400.0);
  drawMatrixColumn(ms, 2, 14.0, 200.0);
  drawMatrixColumn(ms, 3, 14.0, 500.0);
  drawMatrixColumn(ms, 4, 14.0, 375.0);
  drawMatrixColumn(ms, 5, 14.0, 600.0);
  drawMatrixColumn(ms, 6, 14.0, 342.0);
  drawMatrixColumn(ms, 7, 14.0, 250.0);
  drawMatrixColumn(ms, 8, 14.0, 475.0);
  drawMatrixColumn(ms, 9, 14.0, 215.0);
}

// x - which column?

// animation cycle - How long before it repeats?  corresponds to the number of rows it falls (including below the edge of the
// display)

// speed - Speed of the falling - bigger is slower
void drawMatrixColumn(unsigned long ms, int x, float animation_cycle, float speed) {

  // The y position of the head of the falling dot.  This is a float so that we can smoothly fade it down
  // between two pixel (rasterization).
  float dotY = 6.0 - fmod(ms / speed, animation_cycle);

  // Draw the head of the dot as a blend between two pixels, so it falls smoothly
  drawRasterDot(x, dotY, CHSV(96, 255, 255));

  // Draw the tail
  for (int y = 0; y < 7; y++) {
    int brightness = max(0, 7 - y) * 10;
    sparkle.leds[sparkle.XYsafe(x, y + dotY)] |= CHSV(96, 255, brightness);
  }
}

void drawRasterDot(float x, float y, CHSV color) {
  sparkle.set(int(x), floor(y), color % (abs(1 - (y - floor(y))) * 255));
  sparkle.set(int(x), ceil(y), color % (abs(1 - (ceil(y) - y)) * 255));
}

