#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <cstdlib>
#include <string>
#include <stdlib.h>
#include <time.h>
#include "font.h"
#include "Cell.h"
#include "block.h"
#include "GLwindow.h"
// global vars 
std::vector<std::vector<Cell>> board;
std::vector<Point> currBlockPoints;
std::vector<Point> nextBlockPoints;
char nextBlock;
char currBlock;

int offset = 100;
bool move_down = false;
bool move_right = false;
bool move_left = false;
bool drop_down = false;
bool rotate_cc = false;
bool rotate_ccw = false;

bool player_moved = true;
bool block_placed = false;
int rowsCleared = 0;
int fallSpeed = 1000;

double calculate_frames(int frames) {
    std::cout << 1/(double)frames * 1000 <<" ms" << std::endl;
    return 1/(double)frames * 1000;
}

// basic block creator using rand()
char createBlock(){
    int block_type = random() % 8; // generate random number with values from 0 to 7 (p = 1/8)
    char block = 0;
    switch (block_type) {
    case 0:
        block = 'S'; 
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
    case 6:
        block = 'Z';
        break;
    case 7:
        return createBlock();
    }
    return block;
}

void process_Normal_Keys(int key, int x, int y)  {
     switch (key) {
       case 27  : exit(0);     break;
       case 100 : move_left = true;engine->play2D("media/BONG.ogg", false); ; ;  break;
       case 102 : move_right= true;engine->play2D("media/BONG.ogg", false); ; ;  break;
       case 999 : drop_down = true;engine->play2D("media/BONG.ogg", false);; ;  break;
       case 103 : move_down = true;engine->play2D("media/BONG.ogg", false);; ;  break;
       case 101 : drop_down = true;engine->play2D("media/poop.ogg", false); ; ;  break;
    }
}

void char_keys(unsigned char key, int x, int y){
	switch (key){
		case 32: // space
            drop_down = true;
			break;
		case 'r':
            rotate_cc = true; 
		    break;
        case 'w':
            rotate_ccw = true; 
		    break;
        case 'p':
            exit(0);
		default:
         break;
	} 
}

void fps(int frames) {
    // 60 fps = 16.6666667
    glutSwapBuffers();
    glutPostRedisplay();
    glutTimerFunc(frames, fps, frames);
}


int dropped = 1;
void timedDrop(int doDrop) {
    dropped = 0;
    if (shift(0,1,offset,1,currBlockPoints,board)) {
        glutTimerFunc(doDrop,timedDrop,doDrop);
    } else {
        block_placed = true;
        dropped = 1;
    }
}

void key_movement(int now_runs) {
    player_moved = false;
    if(move_down) {
        block_placed = !shift(0,1,offset,1,currBlockPoints,board);
        move_down = false;
        player_moved = true;
    } else if(move_right) {
        shift(1,0,offset,1,currBlockPoints,board);
        move_right = false;
        player_moved = true;
    } else if (move_left) {
        shift(-1,0,offset,1,currBlockPoints,board);
        move_left = false;
        player_moved = true;
    } else if (drop_down) {
        glutTimerFunc(15,timedDrop,15);
        //drop(offset,1,currBlockPoints,board);
        drop_down = false;
        player_moved = true;
    } else if (rotate_cc) {
        rotate(1,offset,1,currBlockPoints,board);
        rotate_cc = false;
        player_moved = true;
    } else if (rotate_ccw) {
        rotate(-1,offset,1,currBlockPoints,board);
        rotate_ccw = false;
        player_moved = true;
    }
    
    if (block_placed) {
        rowsCleared += checkClearRow(board);
        currBlock = nextBlock;
        nextBlock = createBlock();
        genBlocks(currBlock,1,currBlockPoints,board);
        if (!shift(4,0,offset,1,currBlockPoints,board)){
            exit(0);
        }
        block_placed = false;
    } 
    if (rowsCleared > 3) {
        fallSpeed = 500;
    }
    drawPreviewDrop(offset,1,currBlockPoints,board);
    glutTimerFunc(now_runs, key_movement, now_runs);
}

void display(){
    glClearColor(0.0, 1/(float)rand(), 0.0,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glColor3f(1,1,1); 
    /*----------------------------*/
    glRasterPos2i(WIDTH/2-150, 25);
    std::string str = "BLOCK FALL GAME";
    char str3[str.length()+1];
    strcpy(str3, str.c_str());
    printString(str3);
    /*----------------------------*/
    glRasterPos2i(560,200);
    std::string s = "SCORE:" + std::to_string(rowsCleared);
    char str2[s.length() + 1];
    strcpy(str2, s.c_str());
    printString(str2);
    /*----------------------------*/
    glRasterPos2i(560,300);
    std::string st = "NEXT";
    char st0[st.length() + 1];
    strcpy(st0, st.c_str());
    printString(st0);

    glRectf(100-5,100-5,(30*numCols+offset)+5,(30*numRows+100)+5);
    drawPreview(nextBlock,300,560);
    // draw grid for my player 
    // need to also add option for y offset
    // this is just a test to see if graphics are working proper
    for (int i = 0; i < numRows; i++) {
        for(int j = 0; j < numCols;j++) {
            board[i][j].draw(offset);
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
        currBlock = createBlock();
        nextBlock = createBlock();
        genBlocks(currBlock,1,currBlockPoints,board);
        shift(4,0,offset,1,currBlockPoints,board);    
        draw_block = false;    
    }
    if (player_moved == true || dropped == 0) {
        player_moved = false;
        glutTimerFunc(value, drawFallingBlock, value);
        return;
    }
    if (!draw_block) {
        block_placed = !shift(0,1,offset,1,currBlockPoints,board);
    }
    //glutSwapBuffers();
    //glutPostRedisplay();
    glutTimerFunc(fallSpeed, drawFallingBlock, fallSpeed);
}

// subject to change drastically
int main(int argc, char **argv) {
   struct timespec ts;
    if (timespec_get(&ts, TIME_UTC) == 0) {
        exit(1);
    }
    srandom(ts.tv_nsec ^ ts.tv_sec);  /* Seed the PRNG */
    setDisp();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("BLOCK FALL GAME");
    glShadeModel (GL_FLAT);
    makeRasterFont();
    glOrtho(0, WIDTH, HEIGHT, 0.2, -1, 1);
    glutDisplayFunc(display);
    glutTimerFunc(0,fps,calculate_frames(60));
    glutTimerFunc(0,drawFallingBlock,fallSpeed);
    glutSpecialFunc( process_Normal_Keys );
    glutKeyboardFunc(char_keys);
    glutTimerFunc(0,key_movement,calculate_frames(60));
    glutMainLoop();
    return 0;
}
