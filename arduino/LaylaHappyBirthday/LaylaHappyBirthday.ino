#include <FastLED.h>
#include <SparkleShield.h>

#define BRIGHTNESS 32

#define MESSAGE "Happy Birthday Layla! From China with love"
#define TEXT_COLOR CHSV(70, 255, 255)
#define BACKGROUND_COLOR CHSV(0, 255, 255)

SparkleShield sparkle;

void loop() {
  uint32_t ms = millis();
  int length = strlen(MESSAGE) * 6 + 20;
  if (ms / 100 <= length) {
    sparkle.clear(BACKGROUND_COLOR);
    sparkle.scroll_text(MESSAGE, TEXT_COLOR);
  } else {
    int32_t yHueDelta32 = ((int32_t)cos16(ms * (27 / 1)) * (350 / kMatrixWidth));
    int32_t xHueDelta32 = ((int32_t)cos16(ms * (39 / 1)) * (310 / kMatrixHeight));
    DrawRainbowOneFrame(ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
  }
  sparkle.show();
}

void setup() {
  sparkle.setBrightness(BRIGHTNESS);
}

void DrawRainbowOneFrame(byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8) {
  byte lineStartHue = startHue8;
  for (byte y = 0; y < kMatrixHeight; y++) {
    lineStartHue += yHueDelta8;
    byte pixelHue = lineStartHue;
    for (byte x = 0; x < kMatrixWidth; x++) {
      pixelHue += xHueDelta8;
      sparkle.set(x, y, CHSV(pixelHue, 255, 255));
    }
  }
}
