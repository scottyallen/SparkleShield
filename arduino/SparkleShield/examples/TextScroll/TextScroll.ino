#include <FastLED.h>
#include <SparkleShield.h>

#define BRIGHTNESS 32

#define MESSAGE "Hello world!"
#define TEXT_COLOR CHSV(70, 255, 255)
#define BACKGROUND_COLOR CHSV(128, 255, 200)

SparkleShield sparkle;

void loop() {
  int length = strlen(MESSAGE) * 6 + 20;
  DrawTextOneFrame(millis() / 100 % length);
  sparkle.show();
}

void setup() {
  sparkle.setBrightness(BRIGHTNESS);
}

void DrawTextOneFrame(int xOffset) {
  sparkle.clear(BACKGROUND_COLOR);
  sparkle.draw_text(10 - xOffset, 0, MESSAGE, TEXT_COLOR);
}
