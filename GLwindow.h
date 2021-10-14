#ifndef _GLWINDOW_H_
#define _GLWINDOW_H_
#include "GL/freeglut.h"
#include "GL/gl.h"

const int WIDTH = 900;
const int HEIGHT = 750;

const int numRows = 18;
const int numCols = 11;


// simple class to hold my rgb values for each block
class RGB {
    public:
        float r;
        float g;
        float b;
};

//draws a cell with borders
void drawCell(int y, int x, int width, RGB rgb, int xOffset = 0, int yOffset = 0);
//simple string renderer
void RenderString(float x, float y, void *font, const char* string);
//draws a simple rectangle
void drawRect(int x1, int y1, int x2, int y2, RGB rgb);
RGB set_RGB(float r, float g, float b);

#endif
