/* 
 *  Plasma
 *    
 *  This is a demonstration of the plasma algorithm displayed on an RGB LED matrix. 
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
#include <RGBImage.h>

/* Color Schemes
 *  
 *  1 - Full rainbow gradients in RGB
 *  2 - Red, Orange, white gradients
 *  3 - gray scale
 * 
 */
const int COLOR_SCHEME = 1;

const float SPACE_STRETCH_FACTOR = 4.0;
const float TIME_DILATION = 5.0;

RGBLEDMatrix leds(8,8);

int mapSineToRange( float sineValue, int rangeMax ) {
  return rangeMax*(sineValue+1.0)/2.0;
}
void drawPlasma( unsigned long counter ) {
  float utime = float(counter)/TIME_DILATION;
  
  leds.startDrawing();
  for (int col = 0; col < leds.columns(); col++ ) {
    float x = ((float)col/((float)leds.columns()*SPACE_STRETCH_FACTOR)) - 0.5;

    for (int row = 0; row < leds.rows(); row++ ) {
      float y = ((float)row/((float)leds.rows()*SPACE_STRETCH_FACTOR)) - 0.5;

      float v1 = sin(x*10.0+utime);
      float v2 = sin(10.0*(x*sin(utime/2.0) + y*cos(utime/3.0)) + utime);
      
      float cx = x + 0.5*sin(utime/5.0);
      float cy = y + 0.5*cos(utime/3.0);
      float v3 = sin( sqrt(100.0*(cx*cx + cy*cy) + 1.0) + utime );
      
      float v = v1+v2+v3;

      int r, g, b;
      switch (COLOR_SCHEME) {
        default:
        case 1:
          r = mapSineToRange(sin(v*PI), 255);
          g = mapSineToRange(sin(v*PI + 2.0*PI/3.0), 255);
          b = mapSineToRange(sin(v*PI + 4.0*PI/3.0), 255);
          break;
        case 2:
          r = 255;
          g = mapSineToRange(cos(v*PI), 255);
          b = mapSineToRange(sin(v*PI), 255);
          break;
        case 3:
          r = g = b = mapSineToRange(sin(v*5.0*PI), 255);
          break;
      }

      RGBColorType color = RGBColor::fromRGB(r, g, b);
                              
      leds.image().pixel(row, col) = color;
    }
  }
  leds.stopDrawing();
}

void setup() {
  leds.setup();
  drawPlasma(0);
  leds.startScanning();
}

unsigned long loopCounter = 0;
unsigned long timeCount = 0;

const unsigned long loopMod = 2000;

void loop() {
  leds.loop();
  loopCounter++;

  if (loopCounter == loopMod) {
    timeCount++;
    drawPlasma(timeCount);
    loopCounter = 0;
  }
}
