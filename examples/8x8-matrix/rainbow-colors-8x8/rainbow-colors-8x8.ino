#include <RGBImage.h>
#include <RGBLEDMatrix.h>

RGBLEDMatrix leds(8,8);

void setup() {
  leds.setup();
  
  leds.image().drawLine(0,0,0,7,RED_COLOR);
  leds.image().drawLine(1,0,1,7,ORANGE_COLOR);
  leds.image().drawLine(2,0,2,7,YELLOW_COLOR);
  leds.image().drawLine(3,0,3,7,GREEN_COLOR);
  leds.image().drawLine(4,0,4,7,BLUE_COLOR);
  leds.image().drawLine(5,0,5,7,PURPLE_COLOR);
  leds.image().drawLine(6,0,6,7,VIOLET_COLOR);
  leds.image().drawLine(7,0,7,7,WHITE_COLOR);

  leds.startScanning();
}

void loop() {
  leds.loop();
}
