#include <LEDMatrix.h>
#include <RGBLEDMatrix.h>
#include <RGBImage.h>
#include <Glyph.h>
#include <RGBAnimation.h>
#include <RGBAnimationSequence.h>
#include <TimerAction.h>

//
// This is an toroidal array implementation of Conway's gqme of life. 
//
// Colors indicate:
//   green = cell is newly born
//   blue = cell is alive
//   red = cell is dying
//   black = cell is dead
// 
// A random set of cells are born on the first generration, then the game commences
// according to the standard rules.
//

class CellUniverse : public TimerAction {
public:

  typedef byte LifeState;

  const static LifeState ALIVE = 1;
  const static LifeState BORN = 2;
  const static LifeState DYING = 3;  
  const static LifeState DEAD = 0;
  const static LifeState GAME_OVER = 4;
  
private:


  RGBLEDMatrix& _leds;

  LifeState* _cells;
  LifeState* _nextCells;

  bool  _isReseting;
  
  RGBColorType getColorForLifeState( LifeState state ) const;

  
protected:
  virtual void action();

public:

  CellUniverse(
    RGBLEDMatrix& matrix,
    unsigned long updateMicros
  );

  void setCellStatus(unsigned int row, unsigned int column, LifeState cellStatus);
  LifeState getCellStatus(int row, int column) const;
  void createRandomState();
  
  bool isAlive(int row, int column) const;
  int countAliveNeighbors(int row, int column) const;

  void drawToScreen();
};

CellUniverse::CellUniverse(
    RGBLEDMatrix& matrix,
    unsigned long updateMicros
) :   TimerAction(updateMicros),
      _leds(matrix),
      _cells(new LifeState[matrix.rows()*matrix.columns()]),
      _nextCells(new LifeState[matrix.rows()*matrix.columns()]),
      _isReseting(false)
{
  memset(_cells,DEAD,matrix.rows()*matrix.columns()*sizeof(LifeState));
  memset(_nextCells,DEAD,matrix.rows()*matrix.columns()*sizeof(LifeState));
}

void CellUniverse::setCellStatus(unsigned int row, unsigned int column, LifeState cellStatus) {
  if (row >= _leds.rows() || column >= _leds.columns()) {
    return;
  }

  unsigned int idx = row*_leds.columns() + column;

  _cells[idx] = cellStatus;
}

CellUniverse::LifeState CellUniverse::getCellStatus(int row, int column) const {
  // this causes the matrix to be a toroidal array
  unsigned int r = row < 0 ? row + _leds.rows() : ( row >= _leds.rows() ? row - _leds.rows() : row );
  unsigned int c = column < 0 ? column + _leds.columns() : ( column >= _leds.columns() ? column - _leds.columns() : column );

  // double check just to be sure
  if (r >= _leds.rows() || c >= _leds.columns()) {
    return CellUniverse::DEAD;
  }

  unsigned int idx = r*_leds.columns() + c;

  return _cells[idx];
}

bool CellUniverse::isAlive(int row, int column) const {
  return (this->getCellStatus(row, column) == CellUniverse::ALIVE || this->getCellStatus(row, column) == CellUniverse::BORN);
}

int CellUniverse::countAliveNeighbors(int row, int column) const {
    int aliveCount = 0;

    for (int x = column - 1; x <= column+1; x++) {
      for (int y = row - 1; y <= row + 1; y++ ) {
        if (this->isAlive(y, x) && !(x == column && y == row)) {
          aliveCount++;
        }
      }
    }

    return aliveCount;
}

void CellUniverse::action() {
  if (_isReseting) {
    delay(5000);
    this->createRandomState();
    _isReseting = false;
    return;
  }
  
  _leds.startDrawing();
  bool isSame = true;

  for (unsigned int x = 0; x < _leds.columns(); x++) {
    for (unsigned int y = 0; y < _leds.rows(); y++ ) {
      LifeState newState = DEAD;
      LifeState currentState = this->getCellStatus(y, x);
      int count = this->countAliveNeighbors(y, x);
      
      switch (currentState) {
        case BORN:
        case ALIVE:
          if ( count < 2 || count > 3 ) {
            newState = DYING;
          }
          else {
            newState = ALIVE;
          }
          break;
        case DYING:
        case DEAD:
        default:
          if (count == 3) {
            newState = BORN;
          }
          break; 
      }
      if (currentState != newState) {
        isSame = false;
      }
  
      unsigned int idx = y*_leds.columns() + x;
      _nextCells[idx] = newState;

      RGBColorType cellColor = this->getColorForLifeState(newState);
      _leds.image().pixel(y, x) = cellColor;
    }
  }
  _leds.stopDrawing();

  // determine if life needs to start over
  if (isSame) {
    for (unsigned int x = 0; x < _leds.columns(); x++ ) {
      for (unsigned int y = 0; y < _leds.rows(); y++ ) {
        if (this->getCellStatus(y, x) == DEAD) {
          this->setCellStatus(y, x, GAME_OVER);
        }
      }
    }
    this->drawToScreen();
    _isReseting = true;
  }
  else {
    memcpy(_cells, _nextCells, _leds.rows()*_leds.columns()*sizeof(LifeState));
  }

}

void CellUniverse::createRandomState() {
  unsigned int numTotalCells = _leds.rows()*_leds.columns();

  for (unsigned int x = 0; x < _leds.columns(); x++ ) {
    for (unsigned int y = 0; y < _leds.rows(); y++ ) {
      this->setCellStatus(y, x, CellUniverse::DEAD);
    }
  }
  
  unsigned int countStartingCells = random(0.25*numTotalCells, 0.75*numTotalCells);

  for (unsigned int i = 0; i < countStartingCells; i++ ) {
    int randomRow = random(0,_leds.rows());
    int randomColumn = random(0,_leds.columns());

    this->setCellStatus(randomRow, randomColumn, CellUniverse::BORN);
  }
  
  this->drawToScreen();  
}

void CellUniverse::drawToScreen() {
  _leds.startDrawing();
  for (unsigned int x = 0; x < _leds.columns(); x++) {
    for (unsigned int y = 0; y < _leds.rows(); y++ ) {
      LifeState currentState = this->getCellStatus(y, x);
      RGBColorType cellColor = this->getColorForLifeState(currentState);
      _leds.image().pixel(y, x) = cellColor;
    }
  }
  _leds.stopDrawing();
}

RGBColorType CellUniverse::getColorForLifeState( LifeState state ) const {
  RGBColorType cellColor = BLACK_COLOR;
  switch (state) {
    case BORN:
      cellColor = GREEN_COLOR;
      break;
    case ALIVE:
      cellColor = BLUE_COLOR;
      break;
    case DYING:
      cellColor = RED_COLOR;
      break;
    case GAME_OVER:
      cellColor = BLACK_COLOR;
      break;
    case DEAD:
    default:
      cellColor = BLACK_COLOR;
      break;
  }

  return cellColor;
}

//
// PROGRAM BEGINS
//

RGBLEDMatrix leds(8,8);
CellUniverse uni(leds, 500000);

void setup() {
  leds.setup();
  // create starting life positions
  // first, pick a rando fraction between 0.25 and 0.75 of cells.
#ifdef RANDOM_REG32
  randomSeed(RANDOM_REG32);
#else
  randomSeed(analogRead(0));
#endif
  int numTotalCells = leds.rows()*leds.columns();
  int countStartingCells = random(0.25*numTotalCells, 0.75*numTotalCells);

  for (int i = 0; i < countStartingCells; i++ ) {
    int randomRow = random(0,leds.rows());
    int randomColumn = random(0,leds.columns());

    uni.setCellStatus(randomRow, randomColumn, CellUniverse::BORN);
  }
  
  uni.drawToScreen();
  leds.startScanning();
}

void loop() {
  uni.loop();
  leds.loop();
}
