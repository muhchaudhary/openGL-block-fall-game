#include "GL/freeglut.h"
#include "GL/gl.h"
#include <iostream>
#include <string>
#include "GLwindow.h"
#include <vector>
#include <map>
#include <utility>
#include "Cell.h"
#include "block.h"

std::vector<std::vector<Cell>> board;
std::vector<Point> currBlockPoints;
std::vector<Point> nextBlockPoints;

bool move_down = false;
bool move_right = false;
bool move_left = false;
bool drop_down = false;
bool rotate_cc = false;
bool player_moved = true;
std::pair<int,int> currPoint = std::pair<int,int>(0,0);

void process_Normal_Keys(int key, int x, int y)  {
     switch (key) {
       case 27 :      break;
       case 100 : move_left = true; ; ;  break;
       case 102 : move_right= true; ; ;  break;
       case 999 : drop_down = true; ; ;  break;
       case 103 : move_down = true; ; ;  break;
       case 101 : rotate_cc = true; ; ;  break;
    }
}

void key_movement(int now_runs) {
    player_moved = false;
    if(move_down) {
        shift(0,1,50,1,currBlockPoints,board);
        move_down = false;
        player_moved = true;
    } else if(move_right) {
        shift(1,0,50,1,currBlockPoints,board);
        move_right = false;
        player_moved = true;
    } else if (move_left) {
        shift(-1,0,50,1,currBlockPoints,board);
        move_left = false;
        player_moved = true;
    } else if (drop_down) {
        shift(0,-1,50,1,currBlockPoints,board);
        drop_down = false;
        player_moved = true;
    } else if (rotate_cc) {
        rotate(1,50,1,currBlockPoints,board);
        rotate_cc = false;
        player_moved = true;
    }
    if (player_moved) {
        glutSwapBuffers();
        glutPostRedisplay();
    }
    //glFlush();
    glutTimerFunc(0, key_movement, now_runs);
}


void display(){
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    RenderString(WIDTH/2-70, 25, GLUT_BITMAP_TIMES_ROMAN_24,"BLOCK FALL GAME");
    
    // draw grid for my player 
    // need to also add option for y offset
    // this is just a test to see if graphics are working proper
    for (int i = 0; i < numRows; i++) {
        for(int j = 0; j < numCols;j++) {
            board[i][j].draw(50);
        }
    }
    glFlush();
}

void setDisp() {
    for (int i = 0; i < numRows; i++) {
        std::vector<Cell> row;
        for(int j = 0; j < numCols;j++) {
            row.emplace_back(Cell());
            row[j].setCoords(i, j);
        }
        board.emplace_back(row);
    }
}

void genBB() {
    genBlocks('L',1,currBlockPoints,board);
}

bool draw_block = true;
//get timing working first WORKs, now need to reset once player makes move
void drawFallingBlock(int value) {
    if (draw_block) {
        genBB();
        shift(0,0,50,1,currBlockPoints,board);
        draw_block = false;        
    }
    if (player_moved == true) {
        player_moved = false;
        glutTimerFunc(500, drawFallingBlock, 1);
        return;
    }
    shift(0,1,50,1,currBlockPoints,board);
    glutSwapBuffers();
    //glutPostRedisplay();
    glutTimerFunc(1000, drawFallingBlock, 0);
}


// subject to change drastically
int main(int argc, char **argv)
{
    setDisp();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Biquadris");
    glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
    glutDisplayFunc(display);
    glutTimerFunc(500,drawFallingBlock,0);
    glutSpecialFunc( process_Normal_Keys );
    glutTimerFunc(0,key_movement,0);
    glutMainLoop();
    return 0;
}