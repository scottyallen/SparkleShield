#include <FastLED.h>
#include <SparkleShield.h>

#define BRIGHTNESS 32

#define MESSAGE "Hello world!"
#define TEXT_COLOR CHSV(70, 255, 255)
#define BACKGROUND_COLOR CHSV(128, 255, 200)

SparkleShield sparkle;

void loop() {
  sparkle.clear(BACKGROUND_COLOR);
  sparkle.scroll_text(MESSAGE, TEXT_COLOR);
  sparkle.show();
}

void setup() {
  sparkle.setBrightness(BRIGHTNESS);
}
