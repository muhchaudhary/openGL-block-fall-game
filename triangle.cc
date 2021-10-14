#include "GL/freeglut.h"
#include "GL/gl.h"
#include <iostream>
#include <string>
#include "GLwindow.h"
#include <vector>
#include <map>
#include <utility>
#include "Cell.h"

std::vector<std::vector<Cell>> board;

bool move_down = false;
bool move_right = false;
bool player_moved = true;
std::pair<int,int> currPoint = std::pair<int,int>(0,0);

void process_Normal_Keys(int key, int x, int y)  {
     switch (key) {
       case 27 :      break;
       case 100 : printf("GLUT_KEY_LEFT %d\n",key);   break;
       case 102: move_right = true;  ;  break;
       case 101   : printf("GLUT_KEY_UP %d\n",key);  ;  break;
       case 103 : move_down = true;  ;  break;
    }
}

void key_movement(int now_runs) {
    if(move_down == true) {
        //board[currPoint.second][currPoint.first].setType('n');
        currPoint.second+=1;
        board[currPoint.second][currPoint.first].setType('T');
        move_down = false;
        player_moved = true;
    }
    if(move_right == true) {
        //board[currPoint.second][currPoint.first].setType('n');
        currPoint.first+=1;
        board[currPoint.second][currPoint.first].setType('T');
        move_right = false;
        player_moved = true;
    }
    glFlush();
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
        std::vector<Cell> row;
        for(int j = 0; j < numCols;j++) {
            row.emplace_back(Cell());
            row[j].setCoords(i, j);
            //drawCell(i,j,30,set_RGB(0,0,0),50,100);
        }
        board.emplace_back(row);
    }
    glFlush();
}

void clearDisplay() {
    for (int i = 0; i < numRows; i++) {
        std::vector<Cell> row;
        for(int j = 0; j < numCols;j++) {
            row.emplace_back(Cell());
            row[j].setCoords(i, j);
            drawCell(i,j,30,set_RGB(0,0,0),50);
        }
        board.emplace_back(row);
    }
    glFlush();

}

//get timing working first WORKs, now need to reset once player makes move
void drawFallingBlock(int value) {
    if (player_moved == true) {
        player_moved = false;
        glutTimerFunc(1000, drawFallingBlock, 1);
        return;
    }
    if (currPoint.second+1 < numRows && currPoint.first +1 < numCols) {
        //board[y][x].setType('n');
        board[currPoint.second][currPoint.first].setType('T');
        //drawCell(y,x,30,set_RGB(0,0,0),50);
        //x +=1;
        currPoint.second +=1;
        board[currPoint.second][currPoint.first].setType('T');
        //drawCell(y,x,30,set_RGB(1,0.2,0.8),50);
        std::cout << "whats up " << value  << std::endl;
        glutTimerFunc(1000, drawFallingBlock, 0);
    }
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
    glutTimerFunc(1000,drawFallingBlock,0);
    glutSpecialFunc( process_Normal_Keys );
    glutTimerFunc(0,key_movement,0);
    glutMainLoop();
    return 0;
}