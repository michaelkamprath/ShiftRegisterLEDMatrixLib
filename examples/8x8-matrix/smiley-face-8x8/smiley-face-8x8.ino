#include <RGBImage.h>
#include <RGBLEDMatrix.h>
#include <TimerAction.h>

RGBLEDMatrix leds(8,8);

void setup() {
  leds.setup();
  // draw face
  // outer Circle
  leds.image().drawLine( 0, 2, 0, 5, ORANGE_COLOR);
  leds.image().drawLine( 0, 5, 2, 7, ORANGE_COLOR);
  leds.image().drawLine( 2, 7, 5, 7, ORANGE_COLOR);
  leds.image().drawLine( 5, 7, 7, 5, ORANGE_COLOR);
  leds.image().drawLine( 7, 5, 7, 2, ORANGE_COLOR);
  leds.image().drawLine( 7, 2, 5, 0, ORANGE_COLOR);
  leds.image().drawLine( 5, 0, 2, 0, ORANGE_COLOR);
  leds.image().drawLine( 2, 0, 0, 2, ORANGE_COLOR);

  //  eyes

  leds.image().drawRectangle( 2,1,3,2, BLUE_COLOR);
  leds.image().drawRectangle( 2,5,3,6, BLUE_COLOR);
  leds.image().pixel(3,2) = WHITE_COLOR;
  leds.image().pixel(3,5) = WHITE_COLOR;

  // mouth
  leds.image().pixel(5,2) = RED_COLOR;
  leds.image().pixel(6,3) = RED_COLOR;
  leds.image().pixel(6,4) = RED_COLOR;
  leds.image().pixel(5,5) = RED_COLOR;  

  leds.startScanning();
}

void loop() {
  // put your main code here, to run repeatedly:
  leds.loop();
}
