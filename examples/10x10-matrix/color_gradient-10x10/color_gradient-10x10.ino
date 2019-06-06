#include <RGBLEDMatrix.h>
#include <RGBImage.h>
#include <Glyph.h>
#include <RGBAnimation.h>
#include <RGBAnimationSequence.h>
#include <TimerAction.h>

//
// This program will cause a diagonal rainbow gradient to cycle through on the LED matrix
// Will work in either 6-bit or 24-bit color. 
//
#define LOOP_COUNTER_MAX 8000

const int ROW_COLOR_LIST_SIZE = 19;
RGBColorType rowColors[ROW_COLOR_LIST_SIZE];

RGBColorType currentColor = RED_COLOR;

RGBColorType redIncrement = 1 << RED_BIT_SHIFT;
RGBColorType greenIncrement = 1 << GREEN_BIT_SHIFT;
RGBColorType blueIncrement = 1 << BLUE_BIT_SHIFT;

const int SEQUENCE_LENGTH = 7;

// This is the list of main colors we want to cycle through. Agradient will be calculated between them.
RGBColorType colorSequence[SEQUENCE_LENGTH] = { 
    RED_COLOR,
    RED_COLOR+GREEN_COLOR,
    GREEN_COLOR,
    GREEN_COLOR+BLUE_COLOR,
    BLUE_COLOR,
    RED_COLOR+BLUE_COLOR,
    RED_COLOR+GREEN_COLOR+BLUE_COLOR,
};

// This is the list of step values needed to transition to the next color.
unsigned int colorIncrements[SEQUENCE_LENGTH] = {
    greenIncrement,
    redIncrement,
    blueIncrement,
    greenIncrement,
    redIncrement,
    greenIncrement,
    greenIncrement+blueIncrement
  };

// since RGBColorType is an unsigned value, we need to keep track whether we are
// adding or subtracting the increment.
RGBColorType incrementType[SEQUENCE_LENGTH] = {
    true,
    false,
    true,
    false,
    true,
    true,
    false   
  };

int sequenceIdx = 0;
int loopCounter = 0;

int getCurrentIdx() {
  return sequenceIdx;
}

int getNextIdx() {
  if (sequenceIdx == (SEQUENCE_LENGTH-1)) {
    return 0;
  }
  else  {
    return sequenceIdx+1;
  }
}

RGBColorType getNextColor() {
  if ( currentColor == colorSequence[getNextIdx()] ) {
    sequenceIdx++;
    if (sequenceIdx == SEQUENCE_LENGTH) {
      sequenceIdx = 0;
    }
  }

  if (incrementType[getCurrentIdx()]) {
    currentColor += colorIncrements[getCurrentIdx()];
  }
  else {
    currentColor -= colorIncrements[getCurrentIdx()];
  }

  return currentColor;
}

RGBLEDMatrix* leds;

void setup() {
  // put your setup code here, to run once:

  for (int i = 0; i < ROW_COLOR_LIST_SIZE; i++) {
    rowColors[i] = getNextColor();
  }

  leds = new RGBLEDMatrix(10,10);
  leds->setup();
  leds->image().paintColor(BLACK_COLOR);
  leds->startScanning();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (loopCounter == LOOP_COUNTER_MAX) {
    for (int i = 0; i < ROW_COLOR_LIST_SIZE - 1; ++i) {
      rowColors[i] = rowColors[i+1];
    }
    
    rowColors[ROW_COLOR_LIST_SIZE-1] = getNextColor();  
    
    leds->startDrawing();    
    for (int i = 0; i < ROW_COLOR_LIST_SIZE; i++ ) {
      // determine if we are in the upper or lower diagonal
      if (i/leds->rows() == 0) {
        leds->image().drawLine(i,0,0,i,rowColors[i]);
      }
      else {
        leds->image().drawLine(leds->rows()-1,i%leds->rows() + 1,i%leds->rows() + 1,leds->columns()-1,rowColors[i]);
      }
    }   
    leds->stopDrawing();
    
    loopCounter = 0;
  }
  else {
    loopCounter++;
  }

  leds->loop();
}
