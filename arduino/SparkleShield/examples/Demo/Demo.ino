// Based on the LoLShield_Demo.ino
//
// Restart the arduino (or hit the reset button) to go to the next animation.

#include <EEPROM.h>
#include <FastLED.h>
#include <SparkleShield.h>

#define BRIGHTNESS 32

int toggleState;
int EEPROMaddress = 0;
SparkleShield sparkle;

// Plasma settings
float phase = 0.0;
float phaseIncrement = 0.01;  // Controls the speed of the moving points. Higher == faster. I like 0.08 .
float colorStretch = 0.02;    // Higher numbers will produce tighter color bands. I like 0.11 .

// Text settings
#define MESSAGE "I'm a Sparkle Shield!!1!"
#define TEXT_COLOR CHSV(70, 255, 255)
#define BACKGROUND_COLOR CHSV(128, 255, 200)


//Ups or resets the state counter
void upToggleState(){
  toggleState++;
  if (toggleState > 4) toggleState = 0;
  EEPROM.write(EEPROMaddress, toggleState);
}

// Rainbow

void RainbowDrawOneFrame(byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8) {
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

void rainbow() {
  uint32_t ms = millis();
  int32_t yHueDelta32 = ((int32_t)cos16(ms * (27 / 1) ) * (350 / kMatrixWidth));
  int32_t xHueDelta32 = ((int32_t)cos16(ms * (39 / 1) ) * (310 / kMatrixHeight));
  RainbowDrawOneFrame(ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
  sparkle.show();
}

// Plasma
struct Point {
  float x;
  float y;
};

void plasma() {
  phase += phaseIncrement;
  
  // The two points move along Lissajious curves, see: http://en.wikipedia.org/wiki/Lissajous_curve
  // We want values that fit the LED grid: x values between 0..13, y values between 0..8 .
  // The sin() function returns values in the range of -1.0..1.0, so scale these to our desired ranges.
  // The phase value is multiplied by various constants; I chose these semi-randomly, to produce a nice motion.
  Point p1 = { (sin(phase*1.000)+1.0) * 7.5, (sin(phase*1.310)+1.0) * 4.0 };
  Point p2 = { (sin(phase*1.770)+1.0) * 7.5, (sin(phase*2.865)+1.0) * 4.0 };
  
  byte row, col;
  
  // For each row...
  for( row=0; row<7; row++ ) {
    float row_f = float(row);  // Optimization: Keep a floating point value of the row number, instead of recasting it repeatedly.
    
    // For each column...
    for( col=0; col<10; col++ ) {
      float col_f = float(col);  // Optimization.
      
      // Calculate the distance between this LED, and p1.
      Point dist1 = { col_f - p1.x, row_f - p1.y };  // The vector from p1 to this LED.
      float distance = dist1.x*dist1.x + dist1.y*dist1.y;
      
      // Calculate the distance between this LED, and p2.
      Point dist2 = { col_f - p2.x, row_f - p2.y };  // The vector from p2 to this LED.
      // Multiply this with the other distance, this will create weird plasma values :)
      distance *= dist2.x*dist2.x + dist2.y*dist2.y;
      distance = sqrt(distance);
      
      // Warp the distance with a sin() function. As the distance value increases, the LEDs will get light,dark,light,dark,etc...
      // You can use a cos() for slightly different shading, or experiment with other functions. Go crazy!
      float color_f = (sin( distance * colorStretch ) + 1.0) * 0.5;  // range: 0.0...1.0
      
      // Square the color_f value to weight it towards 0. The image will be darker and have higher contrast.
      //color_f *= color_f;
      color_f *= color_f*color_f*color_f;  // Uncomment this line to make it even darker :)
 
      // Set the hue based on the color_f value, and gradually rotate over time.  Range 0..255
      byte hue = byte( round(color_f * 64.0) ) + byte( round(phase * 191.0) );

      // Set the intensity base don the color_f value, with a minimum of half brightness so that things aren't too dark.  Range 0..255
      byte intensity = byte( round(color_f * 128.0) ) + 127.0;

      // Actually set the pixel color.
      sparkle.set(col, row, CHSV(hue, 255, intensity) );
    }
  }
  
  sparkle.show();
}

// Matrix

void drawRasterDot(float x, float y, CHSV color) {
  sparkle.set(int(x), floor(y), color % (abs(1 - (y - floor(y))) * 255));
  sparkle.set(int(x), ceil(y), color % (abs(1 - (ceil(y) - y)) * 255));
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

void MatrixDrawOneFrame(unsigned long ms) {
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


void matrix() {
  MatrixDrawOneFrame(millis());
  sparkle.show();
}


// Text scrolling

void text() {
  sparkle.clear(BACKGROUND_COLOR);
  sparkle.scroll_text(MESSAGE, TEXT_COLOR);
  sparkle.show();
}

// Double Helix

/////////////////////////////////////////////////////////////////////////////
//
// You can tweak these values to create a custom DNA molecule :)
//

const float stretch = 0.44;           // The width of each sine wave. Smaller values create wider sine waves. I like 0.44 .

const float helixPhaseIncrement = 0.05;     // How fast the sines move. I like 0.1 .

// The phase of the "darker" sine wave will drift (relative to the "lighter" sine wave).
// This makes the DoubleHelix more organic/hypnotic .
const float driftIncrement = 0.019;   // The speed it drifts back and forth. Larger == faster. I like 0.019 .
const float driftForce = 0.4;         // The visual amount of drift. I like 0.4 .

// On every other column, light the LEDs between the sine waves, resembling the nucleotides of a DNA molecule.
// This looks good if we switch between lighting odd columns, then even columns -- the molecule appears to be moving.
const float barPhaseIncrement = 0.09; // Bar movement speed. Plz use values between 0..1 . I like 0.09 .

// Brightness values. Range is 0..7
const byte lightSineBrightness = 255;
const byte darkSineBrightness = 128;
const byte barBrightness = 64;

// (End tweak section)
/////////////////////////////////////////////////////////////////////////////



// These values change every frame:
float helixPhase = 0.0;              // This is how "far" we've travelled along the DNA.
float driftPhase = 0.0; 
float barPhase = 0.0;           

void helix() {
  helixPhase += helixPhaseIncrement;  // Move the sine waves forward.
  
  // The "darker" sine wave drifts (relative to the "lighter" sine wave).
  driftPhase += driftIncrement;
  
  // Increment the position of the bars.
  barPhase += barPhaseIncrement;
  if( barPhase > 1.0 ) barPhase -= 1.0;  // Wrap this value between 0..1 .
  
  // We'll hilite either the even columns, or odd columns, depending on the value of barPhase.
  boolean drawEvenBars = (barPhase < 0.5);
  
  byte row, col;
  
  // For each column of LEDs...
  for( col=0; col<10; col++ ) {
  
    // This is the "raw" value for the lighter sine wave. Range: -1.0...1.0
    float lightSineThisColumn = sin( helixPhase + float(col)*stretch );
    
    // Scale the "raw" value and round it off, so the range is 0..7 . This is the LED we're going to light in this column.
    int lightSine = int( round( lightSineThisColumn*3.0 ) ) + 3;
    
    // driftPhase controls the phase drift of the "darker" sine.
    // The drift amount is derived from this sin() function, so it will drift back and forth.
    // Orbit around 2.1, which is about 1/3 phase offset from the lighter sine wave (2*PI/3). Looks pretty good.
    float drift = 2.1 + (driftForce * sin( driftPhase ));
    
    // This is the LED we're going to light for the "dark" sine wave.
    // This is similar to computing the lightSine value, but it's compacted into one line :P
    int darkSine = int( round( sin(helixPhase+drift+float(col)*stretch)*3.0 ) ) + 3;

    byte hue = byte(helixPhase * 10.0);

    // For each LED within the column...
    for( row=0; row<7; row++ ) {
      
      // Does this LED belong to our light sine wave?      
      if( row==lightSine ) {
         sparkle.set( col, row, CHSV(hue, 255, lightSineBrightness) );  // The third argument is the brightness. Max bright == 7.
        
      // Does this LED belong to our dark sine wave?
      } else if( row==darkSine ) {
        sparkle.set( col, row, CHSV(hue, 255, darkSineBrightness) );  // The third argument is the brightness.
        
      } else {
        // This LED doesn't belong to either sine wave. So we'll turn it off, unless it belongs to a vertical bar.
        int color = 0;  // 0 == unlit
        
        // Alternate even/odd columns:
        //   If col is an odd number, (col & 0x1) evaluates to true. (Example: 13 == B1101, rightmost bit is 1, so it's odd!)
        //   The ^ operator is binary XOR. So this statement evaluates true if _one_ condition is met, but _not_ both.
        if( (col & 0x1) ^ (drawEvenBars) ) {
        
          // If lightSine is above this LED, and darkSine is below, then this LED belongs to a vertical bar.
          if( lightSine < darkSine ) {
            if( lightSine<row && row<darkSine ) {
              color = barBrightness;
            }
          
          // If darkSine is above, and lightSine is below, this LED belongs to a vertical bar.
          } else if( darkSine < lightSine ) {
            if( darkSine<row && row<lightSine ) {
              color = barBrightness;
            }
          }
        }
        
        sparkle.set( col, row, CHSV(byte(hue + 32.0), 255, color) );
      }
    }
  }
  
  sparkle.show();
}

void setup(){
  toggleState = EEPROM.read(EEPROMaddress);
  upToggleState();

  sparkle.setBrightness(BRIGHTNESS);
}

void loop(){
  switch(toggleState){
  case 0:
    rainbow();
    break;
  case 1:
    plasma();
    break;
  case 2:
    matrix();
    break;
  case 3:
    text();
    break;
  case 4:
    helix();
    break;
  default:
    EEPROM.write(EEPROMaddress, 0);
  }
}

