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
#include <cstdlib>

// global vars 
std::vector<std::vector<Cell>> board;
std::vector<Point> currBlockPoints;
std::vector<Point> nextBlockPoints;

bool move_down = false;
bool move_right = false;
bool move_left = false;
bool drop_down = false;
bool rotate_cc = false;
bool player_moved = true;
bool block_placed = false;

// basic block creator using rand()
char createBlock(){
    int block_type = rand() % 6; // generate random number with values from 0 to 5 (p = 1/6)
    char block = 0;
    switch (block_type) {
    case 0:
        if(rand()%2 == 0) { // (p = 1/2), for each option we have p = 1/12
            block = 'S'; // 1/6 chance of getting zero and 1 /2 chance of getting zero again  = 1/12
        } else {
            block = 'Z';
        }
        break;
    case 1:
        block = 'I';
        break;
    case 2:
        block = 'T';
        break;
    case 3:
        block = 'J';
        break;
    case 4:
        block = 'O';
        break;
    case 5:
        block = 'L';
        break;
    }
    return block;
}

void process_Normal_Keys(int key, int x, int y)  {
     switch (key) {
       case 27  : exit(0);     break;
       case 100 : move_left = true; ; ;  break;
       case 102 : move_right= true; ; ;  break;
       case 999 : drop_down = true; ; ;  break;
       case 103 : move_down = true; ; ;  break;
       case 101 : drop_down = true; ; ;  break;
    }
    std::cout << key << std::endl;
}

void char_keys(unsigned char key, int x, int y){
	switch (key){
		case 32:
            drop_down = true;
			break;
		case 'r':
            rotate_cc = true; 
		    break;

		default:
         break;
	} 
}

void key_movement(int now_runs) {
    player_moved = false;
    if(move_down) {
        block_placed = !shift(0,1,50,1,currBlockPoints,board);
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
        drop(50,1,currBlockPoints,board);
        block_placed = true;
        drop_down = false;
        player_moved = true;
    } else if (rotate_cc) {
        rotate(1,50,1,currBlockPoints,board);
        rotate_cc = false;
        player_moved = true;
    }

    if (block_placed) {
        checkClearRow(board);
        if (!genBlocks(createBlock(),1,currBlockPoints,board)){
            exit(0);
        }
        block_placed = false;
        glutSwapBuffers();
        glutPostRedisplay();

    }else if (player_moved) { //redraw only if play moves
        glutSwapBuffers();
        glutPostRedisplay();
    }
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

// set up first time disp
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

bool draw_block = true;
//get timing working first WORKs, now need to reset once player makes move
void drawFallingBlock(int value) {
    if (draw_block) {
        genBlocks(createBlock(),1,currBlockPoints,board);
        shift(0,0,50,1,currBlockPoints,board);
        draw_block = false;        
    }
    if (player_moved == true) {
        player_moved = false;
        glutTimerFunc(500, drawFallingBlock, 1);
        return;
    }
    block_placed = !shift(0,1,50,1,currBlockPoints,board);
    glutSwapBuffers();
    //glutPostRedisplay();
    glutTimerFunc(1000, drawFallingBlock, 0);
}


// subject to change drastically
int main(int argc, char **argv)
{
    srand (time(NULL));
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
    glutKeyboardFunc(char_keys);
    glutTimerFunc(0,key_movement,0);
    glutMainLoop();
    return 0;
}