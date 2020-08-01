/* 
 *  Plasma
 *    
 *  This is a demonstration of the plasma algorithm displayed on an RGB LED matrix. `
 *  The COLOR_SCHEME global variable allows you to easily change between some pre-made
 *  color schemes. Using the patterm of the demonstration color schemes, it should be 
 *  easy to add others.
 *  
 *  The algorithm for this demo was sourced from here:
 *  
 *    https://www.bidouille.org/prog/plasma
 * 
 */
#include <RGBLEDMatrix.h>
#include <RGBColor.h>

/* Color Schemes
 *  
 *  1 - Full rainbow gradients in RGB
 *  2 - Red, Orange, white gradients
 *  3 - gray scale
 * 
 */
const int COLOR_SCHEME = 1;

const float VERT_SPACE_STRETCH_FACTOR = 3.0;
const float HORIZ_SPACE_STRETCH_FACTOR = 3.0;
const float TIME_DILATION = 20.0;

RGBLEDMatrix leds(16,16, RGBLEDMatrix::RGB_GROUPS_CPRG8, HIGH, LOW, 3);

int mapSineToRange( float sineValue, int rangeMax ) {
  return rangeMax*(sineValue+1.0)/2.0;
}
void drawPlasma( unsigned long counter ) {
  float utime = float(counter)/TIME_DILATION;
  
  leds.startDrawing();
  for (unsigned int col = 0; col < leds.columns(); col++ ) {
    float x = ((float)col/((float)leds.columns()*HORIZ_SPACE_STRETCH_FACTOR)) - 0.5;

    for (unsigned int row = 0; row < leds.rows(); row++ ) {
      float y = ((float)row/((float)leds.rows()*VERT_SPACE_STRETCH_FACTOR)) - 0.5;

      float v1 = sinf(x*10.0+utime);
      float v2 = sinf(10.0*(x*sinf(utime/2.0) + y*cosf(utime/3.0)) + utime);
      
      float cx = x + 0.5*sinf(utime/5.0);
      float cy = y + 0.5*cosf(utime/3.0);
      float v3 = sinf( sqrtf(100.0*(cx*cx + cy*cy) + 1.0) + utime );
      
      float v = v1+v2+v3;

      int r, g, b;
      switch (COLOR_SCHEME) {
        default:
        case 1:
          r = mapSineToRange(sinf(v*PI), 255);
          g = mapSineToRange(sinf(v*PI + 2.0*PI/3.0), 255);
          b = mapSineToRange(sinf(v*PI + 4.0*PI/3.0), 255);
          break;
        case 2:
          r = 255;
          g = mapSineToRange(cosf(v*PI), 255);
          b = mapSineToRange(sinf(v*PI), 255);
          break;
        case 3:
          r = g = b = mapSineToRange(sinf(v*5.0*PI), 255);
          break;
        case 4:
          r = mapSineToRange(cosf(v*PI), 255);
          g = mapSineToRange(sinf(v*PI + 3.0*PI/4.0), 255);
          b = 255;
          break;
      }

      RGBColorType color = RGBColor::fromRGB(r, g, b);
                              
      leds.writePixel(col, row, color);
    }
  }
  leds.stopDrawing();
}

void setup() {
  leds.setup();
  drawPlasma(0);
  leds.startScanning();
}

unsigned long timeCount = 0;
bool timeIncrement = true;

void loop() {
  leds.loop();

  // update frame every 40 milleseconds. AVR chips are slow enough to not need this delay.
  #if !defined(ARDUINO_ARCH_AVR)
  delay(40);
  #endif

  if (timeIncrement) {
    timeCount++;
  
    //
    // set a maximum to timeCount because floats only have
    // a max precision of 5 significant digits. Otherwise, when timeCount
    // gets too large, the animation will get choppy because calls to drawPlasma()
    // will not have a noticable change to timeCount/TIME_DILATION. for several
    // consecutive calls.
    //
    if (timeCount >= 1000*PI) {
      timeIncrement = false;
    }
  } else {
    timeCount--;
    if (timeCount == 0) {
      timeIncrement = true;
    }
  }
  drawPlasma(timeCount);
}
