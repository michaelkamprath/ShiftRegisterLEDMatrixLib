/***
 * NOTICE
 * 
 * This example only works with a 4x4 LED matrix
 */
#include <RGBImage.h>
#include <Screen.h>
#include <TimerAction.h>

Screen leds(4,4);

void setup() {
  leds.pixel(0,0) = WHITE_COLOR;
  leds.pixel(0,1) = RED_COLOR;
  leds.pixel(0,2) = GREEN_COLOR;
  leds.pixel(0,3) = BLUE_COLOR;
  leds.pixel(1,0) = YELLOW_COLOR;
  leds.pixel(1,1) = AQUA_COLOR;
  leds.pixel(1,2) = MAGENTA_COLOR;
  leds.pixel(1,3) = PINK_COLOR;
  leds.pixel(2,0) = ORANGE_COLOR;
  leds.pixel(2,1) = CORAL_COLOR;
  leds.pixel(2,2) = TURQUOISE_COLOR;
  leds.pixel(2,3) = VIOLET_COLOR;
  leds.pixel(3,0) = GRAY_COLOR;
  leds.pixel(3,1) = DARK_GRAY_COLOR;
  leds.pixel(3,2) = LIME_COLOR;
  leds.pixel(3,3) = SLATE_BLUE_COLOR;

  leds.startScanning();
}

void loop() {
  leds.loop();
}
