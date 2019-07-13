/***
 * NOTICE
 * 
 * This example only works with a 4x4 LED matrix
 */
#include <RGBLEDMatrix.h>

RGBLEDMatrix leds(4,4);

void setup() {
  leds.setup();

  leds.startDrawing();
  leds.writePixel(0,0, WHITE_COLOR);
  leds.writePixel(0,1, RED_COLOR);
  leds.writePixel(0,2, GREEN_COLOR);
  leds.writePixel(0,3, BLUE_COLOR);
  leds.writePixel(1,0, YELLOW_COLOR);
  leds.writePixel(1,1, AQUA_COLOR);
  leds.writePixel(1,2, MAGENTA_COLOR);
  leds.writePixel(1,3, PINK_COLOR);
  leds.writePixel(2,0, ORANGE_COLOR);
  leds.writePixel(2,1, CORAL_COLOR);
  leds.writePixel(2,2, TURQUOISE_COLOR);
  leds.writePixel(2,3, VIOLET_COLOR);
  leds.writePixel(3,0, GRAY_COLOR);
  leds.writePixel(3,1, DARK_GRAY_COLOR);
  leds.writePixel(3,2, LIME_COLOR);
  leds.writePixel(3,3, SLATE_BLUE_COLOR);
  leds.stopDrawing();

  leds.startScanning();
}

void loop() {
  leds.loop();
}
