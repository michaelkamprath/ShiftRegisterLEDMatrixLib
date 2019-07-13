#include <RGBLEDMatrix.h>

RGBLEDMatrix leds(8,8);

void setup() {
  leds.setup();
  
  leds.startDrawing();
  leds.drawLine(0,0,0,7,RED_COLOR);
  leds.drawLine(1,0,1,7,ORANGE_COLOR);
  leds.drawLine(2,0,2,7,YELLOW_COLOR);
  leds.drawLine(3,0,3,7,GREEN_COLOR);
  leds.drawLine(4,0,4,7,BLUE_COLOR);
  leds.drawLine(5,0,5,7,PURPLE_COLOR);
  leds.drawLine(6,0,6,7,VIOLET_COLOR);
  leds.drawLine(7,0,7,7,WHITE_COLOR);
  leds.stopDrawing();

  leds.startScanning();
}

void loop() {
  leds.loop();
}
