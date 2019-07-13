#include <LEDMatrix.h>
#include <Fonts/Org_01.h>

//
// This code is set up for a simple monochrome LED matrix that uses two UDN2981 source drivers controlled by 
// 74HC595 shift registers to control the rows, and uses 2 DM13A sink drivers to contr0l the columns. The 
// Both the UDN2981 and DM13A are turned on with HIGH signals, so we configure the control bit settings 
// as such for both the rows and columns. UDN2981 source drivers have a slow turn off time, so we have to
// have a short blank time in between each  row scan to give the UDN2981 enough time to turn off. 
//

LEDMatrix leds(16,32, HIGH, HIGH, 3);

uint16_t loopCounter;
int16_t xpos = leds.columns() + 1;
uint16_t textWidth;
uint16_t textHeight;
const char* str = "Hello World!";

void setup() {  
	int16_t x, y;

	leds.setup();
	leds.setFont(&Org_01);
	leds.setCursor(0, leds.rows()-1);
	leds.setTextWrap(false);
	leds.getTextBounds(str, 0, 0, &x, &y, &textWidth, &textHeight); 
	textWidth += 8;

	leds.startDrawing();
	leds.fillScreen(LED_BLACK);
	leds.drawLine(0, 0, leds.columns(), leds.rows()/2-1, LED_WHITE);
	leds.stopDrawing();

	loopCounter = 0;

	leds.startScanning();
}

void loop() {
	leds.loop();
	loopCounter++;
	if (loopCounter >= 2000) {
		loopCounter = 0;
		leds.startDrawing();
		leds.fillRect(0, leds.rows()/2, leds.columns(), leds.rows()/2, LED_BLACK);
		xpos--;
		if (xpos < -((int16_t)textWidth)) {
			xpos = leds.columns() + 1;
		}
		leds.setCursor(xpos, leds.rows()/2 + textHeight + 1);
		leds.print(str);
		leds.stopDrawing();
	}
}
