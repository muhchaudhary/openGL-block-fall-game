#include "Cell.h"
#include "GLwindow.h"
Cell::Cell()
  :x{0},
   y{0},
   blockType{'n'} 
{}

char Cell::getState() const { return blockType; }
int Cell::getCoordX() const { return x; }
int Cell::getCoordY() const { return y; }
bool Cell::getBlind() {return blinded;}
int Cell::getId() {return id;}
int Cell::getLev() {return level;}


void Cell::setType(char t) {blockType = t;this->draw(50);}
void Cell::setCoords(int x, int y) { this->x = x; this->y = y; this->draw(50);}
void Cell::setBlind(bool blind) { blinded = blind; }
void Cell::setId(int newid) {id = newid;}
void Cell::setLev(int newLev) {level = newLev;}

void Cell::draw(int offset){
    int color = 0;
    int yOFFSET = 100;
    if (blinded) {
      return;
    }
    switch (blockType) {
      case 'n':
        drawCell(x,y,30,set_RGB(0,0,0),offset,yOFFSET);
        break;
      case 'T':
        drawCell(x,y,30,set_RGB(0.2,0.1,1),offset,yOFFSET);
        break;
      case 'I':
        drawCell(x,y,30,set_RGB(0,0,1),offset,yOFFSET);
        break;
      case 'J':
        drawCell(x,y,30,set_RGB(1,0,0),offset,yOFFSET);
        break;
      case 'O':
        drawCell(x,y,30,set_RGB(0,1,0),offset,yOFFSET);
        break;
      case 'L':
        drawCell(x,y,30,set_RGB(1,0,1),offset,yOFFSET);
        break;
      case 'S':
        drawCell(x,y,30,set_RGB(0.2,0.5,0.2),offset,yOFFSET);
        break;
      case 'Z':
        drawCell(x,y,30,set_RGB(0.6,1,0),offset,yOFFSET);
        break;
      case 'B':
        drawCell(x,y,30,set_RGB(0.6,1,0.2),offset,yOFFSET);
        break; // forgot this
      default:
        drawCell(x,y,30,set_RGB(0,0,0),offset,yOFFSET);
        break;
    }
    glFlush();
}