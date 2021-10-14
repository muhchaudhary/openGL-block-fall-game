#include "GL/freeglut.h"
#include "GL/gl.h"
#include <iostream>
#include <string>
#include "GLwindow.h"


void display(){
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    RenderString(WIDTH/2-70, 25, GLUT_BITMAP_TIMES_ROMAN_24,"BLOCK FALL GAME");
    
    // draw grid for my player 
    // need to also add option for y offset
    // this is just a test to see if graphics are working proper
    for (int i = 1; i < 18; i++) {
        for(int j = 0; j < 12;j++) {
            drawCell(i,j,30,set_RGB(0,0,0),50);
        }
    }
    glFlush();
/*    while(true) {
        std::cout << glutGet(GLUT_ELAPSED_TIME) << std::endl;
    }*/
}

// subject to change drastically
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Biquadris");
    glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}