#include "GLwindow.h"
#include <iostream>
#include <string>

RGB set_RGB(float r, float g, float b) {
    RGB rgb;
    rgb.r = r;
    rgb.g = g;
    rgb.b = b;
    return rgb;
}

void drawCell(int y, int x, int width, RGB rgb, int xOffset,int yOffset) {
    glColor3f(1,1,1);
    int x1 = xOffset + 0 + x*width;
    int y1 = yOffset + 0 + y*width;
    int y2 = yOffset + width+ y*width;
    int x2 = xOffset + width+ x*width;
    float border = 2.50;
    glBegin(GL_POLYGON);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
    glEnd();

    glColor3f(rgb.r,rgb.g,rgb.b); 
    
    glBegin(GL_POLYGON);
        glVertex2f(x1+border, y1+border);
        glVertex2f(x2-border, y1+border);
        glVertex2f(x2-border, y2-border);
        glVertex2f(x1+border, y2-border);
    glEnd();
    glFlush();
}

void RenderString(float x, float y, void *font, const char* string) {  
  //char *c;

  glColor3f(1,1,1); 
  glRasterPos2f(x, y);
 
  glutBitmapString(font, reinterpret_cast<const unsigned char*>(string));
}

void drawRect(int x1, int y1, int x2, int y2, RGB rgb) {
    glColor3f(rgb.r,rgb.g,rgb.b); 
    glBegin(GL_POLYGON);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
    glEnd(); 
}