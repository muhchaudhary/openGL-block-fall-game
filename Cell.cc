#include "Cell.h"
#include "GLwindow.h"
#include <cstdlib>

Cell::Cell()
  :x{0},
   y{0},
   blockType{'n'} 
{}

char Cell::getType() const { return blockType; }
int Cell::getCoordX() const { return x; }
int Cell::getCoordY() const { return y; }
bool Cell::getBlind() {return blinded;}
int Cell::getId() {return id;}
int Cell::getLev() {return level;}
    

void Cell::setType(char t, int newId, int lev) {
  blockType = t;
  id = newId;
  level = lev;
}
void Cell::setCoords(int x, int y) { this->x = x; this->y = y;}
void Cell::setBlind(bool blind) { blinded = blind; }
void Cell::setId(int newid) {id = newid;}
void Cell::setLev(int newLev) {level = newLev;}

void Cell::draw(int offset){
  //srand (time(NULL));
  //double r = ((double) rand() / (RAND_MAX));
  //double g = ((double) rand() / (RAND_MAX));
  //double b = ((double) rand() / (RAND_MAX));

    int yOFFSET = 100;
    if (blinded) {
      return;
    }

    switch (blockType) {
      case 'n':
        drawCell(x,y,30,set_RGB(0,0,0),offset,yOFFSET);
        break;
      case 'T':
        drawCell(x,y,30,set_RGB(0.666, 0.086, 0.035),offset,yOFFSET);
        break;
      case 'I':
        drawCell(x,y,30,set_RGB(0.082, 0.082, 0.6),offset,yOFFSET);
        break;
      case 'J':
        drawCell(x,y,30,set_RGB(0.501, 0, 0),offset,yOFFSET);
        break;
      case 'O':
        drawCell(x,y,30,set_RGB(0, 0.501, 0),offset,yOFFSET);
        break;
      case 'L':
        drawCell(x,y,30,set_RGB(0.501, 0.2, 0.501),offset,yOFFSET);
        break;
      case 'S':
        drawCell(x,y,30,set_RGB(0.2,0.5,0.2),offset,yOFFSET);
        break;
      case 'Z':
        drawCell(x,y,30,set_RGB(0.301, 0.501, 0),offset,yOFFSET);
        break;
      case 'B':
        drawCell(x,y,30,set_RGB(0.525, 0.101, 0.301),offset,yOFFSET);
        break; // forgot this
      default:
        drawCell(x,y,30,set_RGB(0,0,0),offset,yOFFSET);
        break;
    }
    glFlush();
}