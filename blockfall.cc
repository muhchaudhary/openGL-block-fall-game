#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <stdlib.h>
#include <time.h>
#include "font.h"
#include "Cell.h"
#include "block.h"
#include "GLwindow.h"

// ── global game state ───────────────────────────────────────────────────────
std::vector<std::vector<Cell>> board;
std::vector<Point> currBlockPoints;
std::vector<Point> nextBlockPoints;
char nextBlock;
char currBlock;

int  offset       = 100;
bool move_down    = false;
bool move_right   = false;
bool move_left    = false;
bool drop_down    = false;
bool rotate_cc    = false;
bool rotate_ccw   = false;

bool player_moved = true;
bool block_placed = false;
int  rowsCleared  = 0;
int  score        = 0;
int  fallSpeed    = 1000;

GameState gameState = PLAYING;

bool isFlashing      = false;
std::vector<int>   flashingRows;
bool lockPulseActive = false;
std::vector<Point> lockPulseCells;

// ── level system ─────────────────────────────────────────────────────────────
int level = 1;

// ── DAS (Delayed Auto Shift) ──────────────────────────────────────────────────
bool left_held    = false, right_held    = false;
int  das_left_ms  = 0,     das_right_ms  = 0;
static const int DAS_DELAY  = 170;
static const int DAS_REPEAT = 50;

// ── lock delay ────────────────────────────────────────────────────────────────
bool lockDelayActive = false;
int  lockDelayGen    = 0;
bool hardDrop        = false;

// ── floating score popups ─────────────────────────────────────────────────────
struct FloatText { char text[16]; float x, y; int ttl; };
std::vector<FloatText> floatTexts;

// ── tetris flash ──────────────────────────────────────────────────────────────
bool tetrisFlash = false;

// ── cascade animation ─────────────────────────────────────────────────────────
float cascadeOffset = 0.0f;
float cascadeStep   = 0.0f;

// ── settings state ───────────────────────────────────────────────────────────
int  settingsSelected = 0;   // 0=speed, 1=ghost, 2=resume, 3=quit
int  speedIndex       = 1;   // 0=Slow 1=Normal 2=Fast 3=Insane
bool ghostEnabled     = true;

static const int   speedValues[] = { 1500, 1000, 600, 300 };
static const char* speedNames[]  = { "SLOW", "NORMAL", "FAST", "INSANE" };
static const int   scoreTable[]  = { 0, 100, 300, 500, 800 };

// ── helpers ──────────────────────────────────────────────────────────────────
static void glString(float x, float y, void* font, const char* s) {
    glRasterPos2f(x, y);
    glutBitmapString(font, reinterpret_cast<const unsigned char*>(s));
}

static void drawFilledRect(float x1, float y1, float x2, float y2,
                           float r, float g, float b) {
    glColor3f(r, g, b);
    glRectf(x1, y1, x2, y2);
}

static void drawBorderRect(float x1, float y1, float x2, float y2,
                           float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x1, y1); glVertex2f(x2, y1);
        glVertex2f(x2, y2); glVertex2f(x1, y2);
    glEnd();
}

// ── settings screen ──────────────────────────────────────────────────────────
void displaySettings() {
    glClearColor(0.04f, 0.04f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float px1 = 220.0f, py1 = 185.0f;
    const float px2 = 680.0f, py2 = 578.0f;

    // Drop shadow
    drawFilledRect(px1+6, py1+6, px2+6, py2+6, 0.0f, 0.0f, 0.0f);

    // Panel background
    drawFilledRect(px1, py1, px2, py2, 0.09f, 0.09f, 0.17f);

    // Panel border (two-line glow)
    drawBorderRect(px1,   py1,   px2,   py2,   0.42f, 0.36f, 0.68f);
    drawBorderRect(px1+2, py1+2, px2-2, py2-2, 0.22f, 0.18f, 0.40f);

    // Title bar
    drawFilledRect(px1+2, py1+2, px2-2, py1+52, 0.14f, 0.11f, 0.26f);
    glColor3f(0.86f, 0.82f, 1.00f);
    glString(px1 + 138, py1 + 34, GLUT_BITMAP_HELVETICA_18,
             "S E T T I N G S");

    // Title divider
    glColor3f(0.28f, 0.24f, 0.45f);
    glBegin(GL_LINES);
        glVertex2f(px1+14, py1+53); glVertex2f(px2-14, py1+53);
    glEnd();

    char buf[80];
    float optX = px1 + 42.0f;
    float optY = py1 + 108.0f;

    // Draw row highlight when selected
    auto rowBg = [&](float y, bool sel, float r, float g, float b) {
        if (sel) drawFilledRect(px1+14, y-24, px2-14, y+10, r, g, b);
    };
    auto rowColor = [&](bool sel) {
        glColor3f(sel ? 1.00f : 0.70f,
                  sel ? 0.95f : 0.68f,
                  sel ? 0.30f : 0.86f);
    };

    // ── FALL SPEED ────────────────────────────────────────────────────────────
    bool s0 = (settingsSelected == 0);
    rowBg(optY, s0, 0.14f, 0.12f, 0.28f);
    rowColor(s0);
    snprintf(buf, sizeof(buf), "FALL SPEED     < %s >", speedNames[speedIndex]);
    glString(optX, optY, GLUT_BITMAP_HELVETICA_18, buf);

    // ── GHOST PIECE ───────────────────────────────────────────────────────────
    optY += 66.0f;
    bool s1 = (settingsSelected == 1);
    rowBg(optY, s1, 0.14f, 0.12f, 0.28f);
    rowColor(s1);
    snprintf(buf, sizeof(buf), "GHOST PIECE    < %s >", ghostEnabled ? "ON " : "OFF");
    glString(optX, optY, GLUT_BITMAP_HELVETICA_18, buf);

    // Section divider
    optY += 44.0f;
    glColor3f(0.20f, 0.17f, 0.33f);
    glBegin(GL_LINES);
        glVertex2f(px1+30, optY); glVertex2f(px2-30, optY);
    glEnd();
    optY += 32.0f;

    // ── RESUME ────────────────────────────────────────────────────────────────
    bool s2 = (settingsSelected == 2);
    if (s2) drawFilledRect(px1+14, optY-24, px2-14, optY+10, 0.08f, 0.18f, 0.10f);
    glColor3f(s2 ? 0.35f : 0.42f,
              s2 ? 1.00f : 0.72f,
              s2 ? 0.50f : 0.54f);
    glString(px1 + 158.0f, optY, GLUT_BITMAP_HELVETICA_18, "[ RESUME GAME ]");

    // ── QUIT ──────────────────────────────────────────────────────────────────
    optY += 56.0f;
    bool s3 = (settingsSelected == 3);
    if (s3) drawFilledRect(px1+14, optY-24, px2-14, optY+10, 0.20f, 0.07f, 0.07f);
    glColor3f(s3 ? 1.00f : 0.72f,
              s3 ? 0.32f : 0.38f,
              s3 ? 0.32f : 0.38f);
    glString(px1 + 170.0f, optY, GLUT_BITMAP_HELVETICA_18, "[    QUIT    ]");

    // Navigation hint
    glColor3f(0.32f, 0.30f, 0.48f);
    glString(px1 + 18.0f, py2 - 22.0f, GLUT_BITMAP_HELVETICA_12,
             "Arrows: navigate/change    Enter/Space: confirm    S/ESC: resume");

    glFlush();
}

// ── block logic ───────────────────────────────────────────────────────────────
static double calculate_frames(int frames) {
    return 1.0 / (double)frames * 1000.0;
}

char createBlock() {
    switch (random() % 7) {
        case 0: return 'S';
        case 1: return 'I';
        case 2: return 'T';
        case 3: return 'J';
        case 4: return 'O';
        case 5: return 'L';
        default: return 'Z';
    }
}

// ── animation / delay callbacks ───────────────────────────────────────────────
void endTetrisFlash(int) { tetrisFlash = false; }

void lockDelayFire(int gen) {
    if (gen != lockDelayGen || !lockDelayActive || gameState != PLAYING) return;
    lockDelayActive = false;
    block_placed    = true;
}

// ── block placement helpers ───────────────────────────────────────────────────
static bool spawnNextBlock() {
    currBlock = nextBlock;
    nextBlock = createBlock();
    genBlocks(currBlock, 1, currBlockPoints, board);
    if (!shift(4, 0, offset, 1, currBlockPoints, board)) {
        gameState = GAME_OVER;
        return false;
    }
    return true;
}

void endLockPulse(int) {
    lockPulseActive = false;
    lockPulseCells.clear();
}

void afterFlash(int) {
    isFlashing = false;

    // Start cascade: rows above the cleared area will animate falling down
    int lowestCleared = *std::max_element(flashingRows.begin(), flashingRows.end());

    int cleared = checkClearRow(board);
    rowsCleared += cleared;

    if (cleared >= 1 && cleared <= 4) {
        score += scoreTable[cleared];

        // Floating score popup in the center of the board
        FloatText ft;
        snprintf(ft.text, sizeof(ft.text), "+%d", scoreTable[cleared]);
        ft.x  = offset + numCols * 30 / 2.0f - 16.0f;
        ft.y  = 100.0f + lowestCleared * 30.0f - 10.0f;
        ft.ttl = 700;
        floatTexts.push_back(ft);

        // Tetris flash on 4-line clear
        if (cleared == 4) {
            tetrisFlash = true;
            glutTimerFunc(150, endTetrisFlash, 0);
        }

        // Level up every 10 lines
        int newLevel = rowsCleared / 10 + 1;
        if (newLevel != level) {
            level = newLevel;
            int base = speedValues[speedIndex];
            fallSpeed = std::max(100, base - (level - 1) * 60);
        }

        // Cascade animation
        cascadeOffset = -(float)(cleared * 30);
        cascadeStep   = (float)(cleared * 30) / 10.0f;
    }

    flashingRows.clear();
    spawnNextBlock();
}

// ── input ─────────────────────────────────────────────────────────────────────
void restartGame();  // forward declaration

void process_Normal_Keys(int key, int x, int y) {
    if (gameState == SETTINGS) {
        const int N = 4;
        switch (key) {
            case 101: settingsSelected = (settingsSelected - 1 + N) % N; break;
            case 103: settingsSelected = (settingsSelected + 1) % N;     break;
            case 100:
                if (settingsSelected == 0) {
                    speedIndex = (speedIndex - 1 + 4) % 4;
                    fallSpeed  = std::max(100, speedValues[speedIndex] - (level - 1) * 60);
                } else if (settingsSelected == 1) {
                    ghostEnabled = !ghostEnabled;
                }
                break;
            case 102:
                if (settingsSelected == 0) {
                    speedIndex = (speedIndex + 1) % 4;
                    fallSpeed  = std::max(100, speedValues[speedIndex] - (level - 1) * 60);
                } else if (settingsSelected == 1) {
                    ghostEnabled = !ghostEnabled;
                }
                break;
        }
        return;
    }
    switch (key) {
        case 100:  // left
            if (!left_held) move_left = true;
            left_held   = true;
            das_left_ms = DAS_DELAY;
            break;
        case 102:  // right
            if (!right_held) move_right = true;
            right_held   = true;
            das_right_ms = DAS_DELAY;
            break;
        case 103: move_down = true; break;
        case 101: drop_down = true; break;
    }
}

void special_up(int key, int x, int y) {
    if (key == 100) { left_held  = false; das_left_ms  = 0; }
    if (key == 102) { right_held = false; das_right_ms = 0; }
}

void char_keys(unsigned char key, int x, int y) {
    if (gameState == GAME_OVER) {
        switch (key) {
            case 'r': restartGame(); break;
            case 'q':
            case 'p': exit(0);       break;
        }
        return;
    }
    if (gameState == SETTINGS) {
        switch (key) {
            case 27:  // ESC
            case 's': gameState = PLAYING; break;
            case 13:  // Enter
            case 32:  // Space
                if      (settingsSelected == 2) { gameState = PLAYING; }
                else if (settingsSelected == 3) { exit(0); }
                else if (settingsSelected == 0) {
                    speedIndex = (speedIndex + 1) % 4;
                    fallSpeed  = speedValues[speedIndex];
                } else if (settingsSelected == 1) {
                    ghostEnabled = !ghostEnabled;
                }
                break;
        }
        return;
    }
    switch (key) {
        case 32:  drop_down  = true;  break;
        case 'r': rotate_cc  = true;  break;
        case 'w': rotate_ccw = true;  break;
        case 27:  // ESC
        case 's': gameState = SETTINGS; settingsSelected = 0; break;
        case 'p': exit(0); break;
    }
}

// ── timers ────────────────────────────────────────────────────────────────────
void fps(int frames) {
    // Tick cascade animation
    if (cascadeOffset < 0.0f) {
        cascadeOffset += cascadeStep;
        if (cascadeOffset > 0.0f) cascadeOffset = 0.0f;
    }
    // Tick floating score texts
    for (auto it = floatTexts.begin(); it != floatTexts.end(); ) {
        it->y   -= 0.45f;
        it->ttl -= frames;
        if (it->ttl <= 0) it = floatTexts.erase(it);
        else              ++it;
    }
    glutSwapBuffers();
    glutPostRedisplay();
    glutTimerFunc(frames, fps, frames);
}

int dropped = 1;
void timedDrop(int doDrop) {
    if (gameState != PLAYING || isFlashing) {
        glutTimerFunc(doDrop, timedDrop, doDrop);
        return;
    }
    dropped = 0;
    if (shift(0, 1, offset, 1, currBlockPoints, board)) {
        if (lockDelayActive) { lockDelayActive = false; lockDelayGen++; }
        glutTimerFunc(doDrop, timedDrop, doDrop);
    } else {
        dropped = 1;
        if (hardDrop) {
            // Hard drop lands: place block and let this timer chain die
            hardDrop        = false;
            lockDelayActive = false;
            block_placed    = true;
        } else {
            if (!lockDelayActive) {
                lockDelayActive = true;
                lockDelayGen++;
                glutTimerFunc(500, lockDelayFire, lockDelayGen);
            }
            glutTimerFunc(doDrop, timedDrop, doDrop);
        }
    }
}

void key_movement(int now_runs) {
    if (gameState != PLAYING || isFlashing) {
        glutTimerFunc(now_runs, key_movement, now_runs);
        return;
    }

    // DAS auto-repeat
    if (left_held)  { das_left_ms  -= now_runs; if (das_left_ms  <= 0) { move_left  = true; das_left_ms  = DAS_REPEAT; } }
    if (right_held) { das_right_ms -= now_runs; if (das_right_ms <= 0) { move_right = true; das_right_ms = DAS_REPEAT; } }

    player_moved = false;
    if (move_down) {
        if (!shift(0, 1, offset, 1, currBlockPoints, board)) {
            if (!lockDelayActive) {
                lockDelayActive = true; lockDelayGen++;
                glutTimerFunc(150, lockDelayFire, lockDelayGen);
            }
        } else {
            if (lockDelayActive) { lockDelayActive = false; lockDelayGen++; }
        }
        move_down = false;
        player_moved = true;
    } else if (move_right) {
        shift(1, 0, offset, 1, currBlockPoints, board);
        if (lockDelayActive) {
            if (validBounds(currBlockPoints, 0, 1, board)) { lockDelayActive = false; lockDelayGen++; }
            else { lockDelayGen++; glutTimerFunc(500, lockDelayFire, lockDelayGen); }
        }
        move_right = false;
        player_moved = true;
    } else if (move_left) {
        shift(-1, 0, offset, 1, currBlockPoints, board);
        if (lockDelayActive) {
            if (validBounds(currBlockPoints, 0, 1, board)) { lockDelayActive = false; lockDelayGen++; }
            else { lockDelayGen++; glutTimerFunc(500, lockDelayFire, lockDelayGen); }
        }
        move_left = false;
        player_moved = true;
    } else if (drop_down) {
        hardDrop = true;
        lockDelayActive = false; lockDelayGen++;
        glutTimerFunc(15, timedDrop, 15);
        drop_down = false;
        player_moved = true;
    } else if (rotate_cc) {
        rotate(1, offset, 1, currBlockPoints, board);
        if (lockDelayActive) { lockDelayGen++; glutTimerFunc(500, lockDelayFire, lockDelayGen); }
        rotate_cc = false;
        player_moved = true;
    } else if (rotate_ccw) {
        rotate(-1, offset, 1, currBlockPoints, board);
        if (lockDelayActive) { lockDelayGen++; glutTimerFunc(500, lockDelayFire, lockDelayGen); }
        rotate_ccw = false;
        player_moved = true;
    }

    if (block_placed) {
        std::vector<int> full = findFullRows(board);
        if (!full.empty()) {
            // Flash full rows white, then clear them after delay
            flashingRows = full;
            isFlashing   = true;
            glutTimerFunc(80, afterFlash, 0);
        } else {
            // Brief bright pulse on the locked cells
            lockPulseCells.assign(currBlockPoints.begin(), currBlockPoints.begin() + 4);
            lockPulseActive = true;
            glutTimerFunc(150, endLockPulse, 0);
            spawnNextBlock();
        }
        block_placed = false;
    }

    if (ghostEnabled) {
        drawPreviewDrop(offset, 1, currBlockPoints, board);
    }
    glutTimerFunc(now_runs, key_movement, now_runs);
}

bool draw_block = true;
void drawFallingBlock(int value) {
    if (gameState != PLAYING || isFlashing) {
        glutTimerFunc(value, drawFallingBlock, value);
        return;
    }
    if (draw_block) {
        currBlock = createBlock();
        nextBlock = createBlock();
        genBlocks(currBlock, 1, currBlockPoints, board);
        shift(4, 0, offset, 1, currBlockPoints, board);
        draw_block = false;
    }
    if (player_moved || dropped == 0) {
        player_moved = false;
        glutTimerFunc(value, drawFallingBlock, value);
        return;
    }
    if (!draw_block) {
        if (!shift(0, 1, offset, 1, currBlockPoints, board)) {
            if (!lockDelayActive) {
                lockDelayActive = true; lockDelayGen++;
                glutTimerFunc(500, lockDelayFire, lockDelayGen);
            }
        } else {
            if (lockDelayActive) { lockDelayActive = false; lockDelayGen++; }
        }
    }
    glutTimerFunc(fallSpeed, drawFallingBlock, fallSpeed);
}

// ── game over screen ─────────────────────────────────────────────────────────
void displayGameOver() {
    glClearColor(0.08f, 0.08f, 0.13f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < numRows; i++)
        for (int j = 0; j < numCols; j++)
            board[i][j].draw(offset);

    const float px1 = 160.0f, py1 = 255.0f;
    const float px2 = 540.0f, py2 = 510.0f;

    drawFilledRect(px1+6, py1+6, px2+6, py2+6, 0.0f, 0.0f, 0.0f);
    drawFilledRect(px1, py1, px2, py2, 0.10f, 0.06f, 0.06f);
    drawBorderRect(px1,   py1,   px2,   py2,   0.75f, 0.20f, 0.20f);
    drawBorderRect(px1+2, py1+2, px2-2, py2-2, 0.38f, 0.10f, 0.10f);

    drawFilledRect(px1+2, py1+2, px2-2, py1+52, 0.20f, 0.06f, 0.06f);
    glColor3f(1.0f, 0.36f, 0.36f);
    glString(px1 + 80, py1 + 34, GLUT_BITMAP_HELVETICA_18, "G A M E   O V E R");

    glColor3f(0.30f, 0.12f, 0.12f);
    glBegin(GL_LINES);
        glVertex2f(px1+14, py1+53); glVertex2f(px2-14, py1+53);
    glEnd();

    char buf[64];

    glColor3f(0.60f, 0.58f, 0.80f);
    glString(px1 + 48, py1 + 105, GLUT_BITMAP_HELVETICA_18, "SCORE");
    snprintf(buf, sizeof(buf), "%d", score);
    glColor3f(0.95f, 0.92f, 1.00f);
    glString(px1 + 48, py1 + 133, GLUT_BITMAP_HELVETICA_18, buf);

    glColor3f(0.60f, 0.58f, 0.80f);
    glString(px1 + 210, py1 + 105, GLUT_BITMAP_HELVETICA_18, "LINES");
    snprintf(buf, sizeof(buf), "%d", rowsCleared);
    glColor3f(0.95f, 0.92f, 1.00f);
    glString(px1 + 210, py1 + 133, GLUT_BITMAP_HELVETICA_18, buf);

    glColor3f(0.35f, 0.90f, 0.55f);
    glString(px1 + 52, py1 + 195, GLUT_BITMAP_HELVETICA_18, "[ R  -  RESTART ]");

    glColor3f(0.85f, 0.30f, 0.30f);
    glString(px1 + 72, py1 + 238, GLUT_BITMAP_HELVETICA_18, "[  Q  -  QUIT  ]");

    glColor3f(0.28f, 0.26f, 0.42f);
    glString(px1 + 14, py2 - 22, GLUT_BITMAP_HELVETICA_12,
             "R: restart    Q: quit");

    glFlush();
}

// ── display ───────────────────────────────────────────────────────────────────
void display() {
    if (gameState == SETTINGS) {
        displaySettings();
        return;
    }
    if (gameState == GAME_OVER) {
        displayGameOver();
        return;
    }

    // Tetris flash: brief bright background on 4-line clear
    if (tetrisFlash)
        glClearColor(0.22f, 0.16f, 0.45f, 1.0f);
    else
        glClearColor(0.08f, 0.08f, 0.13f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Title
    glColor3f(0.95f, 0.88f, 0.25f);
    glRasterPos2i(WIDTH / 2 - 95, 38);
    printString("BLOCK FALL GAME");

    // Board border
    glColor3f(0.35f, 0.32f, 0.55f);
    glRectf(95, 95, 30*numCols+offset+5, 30*numRows+100+5);

    // ── Right panel ───────────────────────────────────────────
    // Score
    glColor3f(0.60f, 0.58f, 0.80f);
    glRasterPos2i(558, 130); printString("SCORE");
    glColor3f(0.95f, 0.92f, 1.00f);
    glRasterPos2i(558, 158); printString(std::to_string(score));

    // Lines
    glColor3f(0.60f, 0.58f, 0.80f);
    glRasterPos2i(558, 192); printString("LINES");
    glColor3f(0.95f, 0.92f, 1.00f);
    glRasterPos2i(558, 219); printString(std::to_string(rowsCleared));

    // Level
    glColor3f(0.60f, 0.58f, 0.80f);
    glRasterPos2i(558, 253); printString("LEVEL");
    glColor3f(0.95f, 0.92f, 1.00f);
    glRasterPos2i(558, 280); printString(std::to_string(level));

    // Next piece box
    drawFilledRect(548, 305, 750, 450, 0.12f, 0.11f, 0.20f);
    drawBorderRect( 548, 305, 750, 450, 0.35f, 0.32f, 0.55f);
    glColor3f(0.60f, 0.58f, 0.80f);
    glRasterPos2i(616, 325); printString("NEXT");

    drawPreview(nextBlock, 325, 560);

    // Controls hint
    glColor3f(0.38f, 0.36f, 0.56f);
    glRasterPos2i(554, 470); printString("CONTROLS");
    glColor3f(0.32f, 0.30f, 0.48f);
    glRasterPos2i(554, 492); printString("< > v  :  move");
    glRasterPos2i(554, 512); printString("^  /  Space  :  drop");
    glRasterPos2i(554, 532); printString("R / W  :  rotate");
    glRasterPos2i(554, 552); printString("S / ESC  :  menu");

    // ── Draw board with cascade offset ────────────────────────
    {
        int yBase = 100 + (int)cascadeOffset;
        for (int i = 0; i < numRows; i++)
            for (int j = 0; j < numCols; j++)
                board[i][j].draw(offset, yBase);
    }

    // Line-clear flash: white overlay on completed rows
    if (isFlashing) {
        for (int row : flashingRows)
            for (int col = 0; col < numCols; col++)
                drawCell(row, col, 30, set_RGB(1.0f, 1.0f, 1.0f), offset, 100);
    }

    // Lock pulse: bright yellow-white on the just-placed cells
    if (lockPulseActive) {
        for (const auto& p : lockPulseCells)
            drawCell(p.y, p.x, 30, set_RGB(1.0f, 0.95f, 0.55f), offset, 100);
    }

    // Floating score popups
    if (!floatTexts.empty()) {
        for (const auto& ft : floatTexts) {
            float alpha = (float)ft.ttl / 700.0f;
            glColor3f(1.0f, alpha * 0.9f + 0.1f, alpha * 0.3f);
            glRasterPos2f(ft.x, ft.y);
            glutBitmapString(GLUT_BITMAP_HELVETICA_18,
                             reinterpret_cast<const unsigned char*>(ft.text));
        }
    }

    glFlush();
}

// ── init ──────────────────────────────────────────────────────────────────────
void setDisp() {
    for (int i = 0; i < numRows; i++) {
        std::vector<Cell> row;
        for (int j = 0; j < numCols; j++) {
            row.emplace_back(Cell());
            row[j].setCoords(i, j);
        }
        board.emplace_back(row);
    }
}

void restartGame() {
    board.clear();
    setDisp();
    currBlockPoints.clear();
    score        = 0;
    rowsCleared  = 0;
    level        = 1;
    block_placed = false;
    player_moved = true;
    dropped      = 1;
    hardDrop        = false;
    isFlashing      = false;
    lockPulseActive = false;
    lockDelayActive = false;
    lockDelayGen++;
    flashingRows.clear();
    lockPulseCells.clear();
    floatTexts.clear();
    tetrisFlash   = false;
    cascadeOffset = 0.0f;
    left_held  = false; right_held  = false;
    das_left_ms = 0;    das_right_ms = 0;
    move_left = move_right = move_down = drop_down = rotate_cc = rotate_ccw = false;
    fallSpeed  = speedValues[speedIndex];

    // Spawn the first block now so key_movement never sees empty currBlockPoints.
    // drawFallingBlock uses draw_block==true to do this at initial launch; on
    // restart we do it here directly so the 16ms key_movement timer can't fire
    // first and send drawPreviewDrop into an infinite loop on the empty vector.
    currBlock = createBlock();
    nextBlock = createBlock();
    genBlocks(currBlock, 1, currBlockPoints, board);
    shift(4, 0, offset, 1, currBlockPoints, board);
    draw_block = false;

    gameState  = PLAYING;
}

int main(int argc, char **argv) {
    struct timespec ts;
    if (timespec_get(&ts, TIME_UTC) == 0) exit(1);
    srandom(ts.tv_nsec ^ ts.tv_sec);

    setDisp();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("BLOCK FALL GAME");

    makeRasterFont();
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    int frame_ms = (int)calculate_frames(60);
    glutTimerFunc(0, fps,             frame_ms);
    glutTimerFunc(0, drawFallingBlock, fallSpeed);
    glutTimerFunc(0, key_movement,    frame_ms);
    glutSpecialFunc(process_Normal_Keys);
    glutSpecialUpFunc(special_up);
    glutKeyboardFunc(char_keys);

    glutMainLoop();
    return 0;
}
