#include <RGBLEDMatrix.h>
#include <RGBColor.h>

RGBLEDMatrix leds(8,8);

#define MAX_DISTANCE (leds.columns() - 1)

void setup() {
	leds.setup();
	leds.startDrawing();
	for (int y = 0; y < leds.rows(); y++ ) {
		for (int x = 0; x < leds.columns(); x++ ) {
			float dx = leds.columns() - x - 1;
			float dy = leds.rows()-y - 1;
			float dmy = leds.rows()/2.0-y;

			float d_tl = max(1.0-sqrt(x*x + y*y)/MAX_DISTANCE, 0);
			float d_tr = max(1.0-sqrt(dx*dx + y*y)/MAX_DISTANCE, 0);
			float d_bl = max(1.0-sqrt(x*x + (leds.rows()-y)*(leds.rows()))/MAX_DISTANCE, 0);
			float d_br = max(1.0-sqrt(dx*dx + dy*dy)/MAX_DISTANCE, 0);
			float d_rm = max(1.0-sqrt(dx*dx + dmy*dmy)/(MAX_DISTANCE*3.0/4.0), 0);

			RGBColorType c = RGBColor::fromRGB( d_tl*255, d_br*194, d_bl*255);
			leds.writePixel(x, y, c);
		} 
	}
	leds.stopDrawing();
	leds.loop();
	leds.startScanning();
}

void loop() {
	leds.loop();
}
