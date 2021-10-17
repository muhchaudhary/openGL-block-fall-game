#include <vector>
#include "block.h"

std::vector<Point> TblockPoints = {Point{1, 3}, Point{0, 2}, Point{1, 2}, Point{2, 2},Point {0,3}};
std::vector<Point> IblockPoints = {Point{0, 3}, Point{1, 3}, Point{2, 3}, Point{3, 3},Point {0,3}};
std::vector<Point> ZblockPoints = {Point{0, 2}, Point{1, 2}, Point{1, 3}, Point{2, 3},Point {0,3}};
std::vector<Point> SblockPoints = {Point{0, 3}, Point{1, 3}, Point{1, 2}, Point{2, 2},Point {0,3}};
std::vector<Point> LblockPoints = {Point{0, 3}, Point{1, 3}, Point{2, 3}, Point{2, 2},Point {0,3}};
std::vector<Point> OblockPoints = {Point{0, 2}, Point{0, 3}, Point{1, 2}, Point{1, 3},Point {0,3}};
std::vector<Point> JblockPoints = {Point{0, 2}, Point{0, 3}, Point{1, 3}, Point{2, 3},Point {0,3}};
std::vector<Point> BblockPoints = {Point{5, 3},Point {0,3}};


int id = -1;


Point LowerLeft(std::vector<Point> blockPoints) {
    int minX = blockPoints[0].x;
    int maxY = blockPoints[0].y;;
    for (int i = 0; i < (int)blockPoints.size(); i++) {
        if (minX > blockPoints[i].x) minX = blockPoints[i].x;
        if (maxY < blockPoints[i].y) maxY = blockPoints[i].y;
    }
    return Point{minX,maxY};
}

bool notMember(std::vector<Point> currLocc, int x, int y) {
    for (int i = 0; i < (int)currLocc.size()-1; ++i) {
        if (currLocc[i].x == x && currLocc[i].y == y) {
            return false;
        }
    }
    return true;
}

bool validBoundsRot(std::vector<Point> currLoc, 
                    std::vector<Point> newPoints,
                    std::vector<std::vector<Cell>>& grid) {
    for (int i = 0; i < (int)currLoc.size(); i++) { // check each point
        if (newPoints[i].x < 0 || newPoints[i].x > numCols - 1) {
            return false;
        }
        if ((newPoints[i].y < 0 || newPoints[i].y > numRows - 1)) {
            return false;
        }
        else if ((grid[newPoints[i].y][newPoints[i].x].getType() != 'n')
                 && notMember(currLoc, newPoints[i].x, newPoints[i].y)) {
            return false;
        }
    }
    return true;
}

bool validBounds(std::vector<Point> currLoc, int x, int y, 
                 std::vector<std::vector<Cell>>& grid) {
    for (int i = 0; i < (int)currLoc.size()-1; i++) { // check each point
        if (currLoc[i].x + x < 0 || currLoc[i].x + x > numCols - 1) {
            return false;
        }
        if ((currLoc[i].y + y < 0 || currLoc[i].y + y > numRows - 1)) {
            return false;
        }
        else if ((grid[currLoc[i].y + y][currLoc[i].x + x].getType() != 'n') 
                    && notMember(currLoc, currLoc[i].x + x, currLoc[i].y + y)) {
            if ((grid[currLoc[i].y + y][currLoc[i].x + x].getType() == 'p')) {
                return true;
            }
            return false;
        }
    }
    return true;
}

bool shift(int x, int y,int offset,int level, 
           std::vector<Point>& blockPoints, 
           std::vector<std::vector<Cell>>& grid) {
    if (validBounds(blockPoints,x,y,grid)) {
        char blockType = grid[blockPoints[0].y][blockPoints[0].x].getType();
        for (int i = 0; i < (int)blockPoints.size()-1; i++) {
            grid[blockPoints[i].y][blockPoints[i].x].setType('n',-1, -1); // turn off
            grid[blockPoints[i].y][blockPoints[i].x].draw(offset);
            blockPoints[i].x += x;
            blockPoints[i].y += y;
        }
        for(int i = 0; i < (int)blockPoints.size()-1; i++) {
            grid[blockPoints[i].y][blockPoints[i].x].setType(blockType,id,level);
            grid[blockPoints[i].y][blockPoints[i].x].draw(offset);
        }
        // lower left point stored in last blockPoint
        blockPoints[4].x += x;
        blockPoints[4].y += y;
        return true;
    }
    else {
        return false;
    }
}

bool rotate(int rotation, int offset, int level,
            std::vector<Point>& blockPoints, 
            std::vector<std::vector<Cell>>& grid) {
    std::vector<Point> newPoints = {Point{0, 0}, Point{0, 0}, Point{0, 0}, Point{0, 0}};
    Point lowerLeft = LowerLeft(blockPoints);
    for (int i = 0; i < (int)blockPoints.size()-1;i++) {
        int xNew = lowerLeft.x + (rotation * lowerLeft.y) - (rotation * blockPoints[i].y);
        int yNew = lowerLeft.y - (rotation * lowerLeft.x) + (rotation * blockPoints[i].x);
        newPoints[i].x = xNew;
        newPoints[i].y = yNew;
    }
    Point newLowerLeft = LowerLeft(newPoints);
    for(int i = 0; i < (int)blockPoints.size()-1; i++) {
        newPoints[i].x += (lowerLeft.x - newLowerLeft.x);
        newPoints[i].y += (lowerLeft.y - newLowerLeft.y);
    }
    if (validBoundsRot(blockPoints, newPoints,grid)) {
        char blockType = grid[blockPoints[1].y][blockPoints[1].x].getType();
        for (int i = 0; i < (int)blockPoints.size()-1; i++) {
            grid[blockPoints[i].y][blockPoints[i].x].setType('n',-1, -1);
            grid[blockPoints[i].y][blockPoints[i].x].draw(offset);
        }
        for(int i = 0; i < (int)blockPoints.size()-1; i++) {
            blockPoints[i].x = newPoints[i].x;
            blockPoints[i].y = newPoints[i].y;
            grid[blockPoints[i].y][blockPoints[i].x].setType(blockType,id,level);
            grid[blockPoints[i].y][blockPoints[i].x].draw(offset);
        }   
        blockPoints[4] = LowerLeft(blockPoints);
        return true;
    }
    else {
        return false;
    }
}

bool drop(int offset, int level,
            std::vector<Point>& blockPoints, 
            std::vector<std::vector<Cell>>& grid) {
    int xaxis = 0;
    int yaxis = 1;
    int runtime = 0;
    while (validBounds(blockPoints, xaxis, yaxis,grid)) {
        for (int i = 0; i < (int)blockPoints.size()-1; i++) {
            char blockType = grid[blockPoints[1].y][blockPoints[1].x].getType();
            grid[blockPoints[i].y][blockPoints[i].x].setType('n',-1, -1); // turn off
            if(runtime == 0) { // only undraw once
                grid[blockPoints[i].y][blockPoints[i].x].draw(offset);
            }
            blockPoints[i].x += xaxis;
            blockPoints[i].y += yaxis;
        for (int j = 0; j < (int)blockPoints.size()-1; j++) { 
            grid[blockPoints[j].y][blockPoints[j].x].setType(blockType,id,level);
            }
        }
        ++runtime;
    }
    int minY = blockPoints[0].y;;
    for (int i = 0; i < (int)blockPoints.size()-1; i++) {
        if (minY > blockPoints[i].y) minY = blockPoints[i].y;
    }
    if (minY < 3) {
        return false;
    }
    if (runtime != 0) { // no need to draw if nothing changed
        for (int i = 0; i < (int)blockPoints.size()-1; i++) {
            grid[blockPoints[i].y][blockPoints[i].x].draw(offset);
        }
    }
    return true;
}

bool checkRow(int i,std::vector<std::vector<Cell>>& grid) {
    for (int j = 0; j < 11; ++j) {
        if (grid[i][j].getType() == 'n') {
            return false;
        }
    }
    return true;
}

void clearRow(int i, std::vector<std::vector<Cell>>& grid) {
    for (int j = i; j != 0; --j) {
        if (j == i) {
            for (int i = 0; i < (int)grid[j].size();i++) {
                //blocks[blocks.find(grid[j][i].getId())->first] -= 1; // cell is removed
                //if (blocks[blocks.find(grid[j][i].getId())->first] == 0) {
                //    score += (grid[j][i].getLev() + 1) * (grid[j][i].getLev() + 1);
                //}
                grid[j][i].setType('n',-1,-1);
            }
            grid[j].clear();
        }
        grid[j] = grid[j-1];
        for (int i = 0; i < (int)grid[j].size();i++) {
            int x = grid[j][i].getCoordX();
            int y = grid[j][i].getCoordY();
            char tmpType = grid[j][i].getType();
            grid[j][i].setType('n',grid[j][i].getId(),grid[j][i].getLev());
            grid[j][i].setCoords(x+1,y);
            grid[j][i].setType(tmpType,grid[j][i].getId(),grid[j][i].getLev());
        }
    }
    for (int k = 0; k < 11; ++k) {
        grid[0][k].setType('n',-1,-1);
    }
}

int checkClearRow(std::vector<std::vector<Cell>>& grid) {
    int clearedRows = 0;
    for (int i = 0; i < numRows; ++i) {
        if (checkRow(i,grid)) {
            clearedRows += 1;
            clearRow(i,grid);
            i -= 1; // want to check new row at i since row would be moved down
        }
    }

    return clearedRows;
}


bool genBlocks(char blockType,int level, 
               std::vector<Point>& currBlockPoints,
               std::vector<std::vector<Cell>>& grid) {
    id++;
    switch (blockType) {
        case 'T':
            currBlockPoints = TblockPoints;
            break;
        case 'J':
            currBlockPoints = JblockPoints;
            break;
        case 'L':
            currBlockPoints = LblockPoints;
            break;
        case 'I':
            currBlockPoints = IblockPoints;
            break;
        case 'Z':
            currBlockPoints = ZblockPoints;
            break;
        case 'S':
            currBlockPoints = SblockPoints;
            break;
        case 'O':
            currBlockPoints = OblockPoints;
            break;
        case 'B':
            currBlockPoints = IblockPoints;
            break;
    }
    for (int i = 0; i <(int)currBlockPoints.size()-1; i++) { // check each point
        if (grid[currBlockPoints[i].y][currBlockPoints[i].x].getType() != 'n') {
        return false;
        }
    }
    for (int i = 0; i <(int)currBlockPoints.size()-1; i++) { // check each point
        grid[currBlockPoints[i].y][currBlockPoints[i].x].setId(id);
        grid[currBlockPoints[i].y][currBlockPoints[i].x].setType(blockType);
        grid[currBlockPoints[i].y][currBlockPoints[i].x].setLev(level);
    }
    return true;
}

// very stupid way of doing this
void drawPreview(char next, int offset, int yoffset) {
    //drawCell(yoffset-15,offset-15,150);
    if (next == 'T') {
        drawCell(1,0,30,set_RGB(0.666, 0.086, 0.035),yoffset,offset);
        drawCell(2,1,30,set_RGB(0.666, 0.086, 0.035),yoffset,offset);
        drawCell(1,1,30,set_RGB(0.666, 0.086, 0.035),yoffset,offset);
        drawCell(1,2,30,set_RGB(0.666, 0.086, 0.035),yoffset,offset);
    } else if (next == 'I') {
        drawCell(3,0,30,set_RGB(0.082, 0.082, 0.6),yoffset,offset);
        drawCell(3,1,30,set_RGB(0.082, 0.082, 0.6),yoffset,offset);
        drawCell(3,2,30,set_RGB(0.082, 0.082, 0.6),yoffset,offset);
        drawCell(3,3,30,set_RGB(0.082, 0.082, 0.6),yoffset,offset);
    } else if (next == 'Z') {
        drawCell(2,0,30,set_RGB(0.301, 0.501, 0),yoffset,offset);
        drawCell(2,1,30,set_RGB(0.301, 0.501, 0),yoffset,offset);
        drawCell(3,1,30,set_RGB(0.301, 0.501, 0),yoffset,offset);
        drawCell(3,2,30,set_RGB(0.301, 0.501, 0),yoffset,offset);
    } else if (next == 'S') {
        drawCell(3,0,30,set_RGB(0.2,0.5,0.2),yoffset,offset);
        drawCell(3,1,30,set_RGB(0.2,0.5,0.2),yoffset,offset);
        drawCell(2,1,30,set_RGB(0.2,0.5,0.2),yoffset,offset);
        drawCell(2,2,30,set_RGB(0.2,0.5,0.2),yoffset,offset);
    } else if (next == 'L') {
        drawCell(3,0,30,set_RGB(0.501, 0.2, 0.501),yoffset,offset);
        drawCell(3,1,30,set_RGB(0.501, 0.2, 0.501),yoffset,offset);
        drawCell(3,2,30,set_RGB(0.501, 0.2, 0.501),yoffset,offset);
        drawCell(2,2,30,set_RGB(0.501, 0.2, 0.501),yoffset,offset);
    } else if (next == 'O') {
        drawCell(2,0,30,set_RGB(0, 0.501, 0),yoffset,offset);
        drawCell(3,0,30,set_RGB(0, 0.501, 0),yoffset,offset);
        drawCell(2,1,30,set_RGB(0, 0.501, 0),yoffset,offset);
        drawCell(3,1,30,set_RGB(0, 0.501, 0),yoffset,offset);
    } else if (next == 'J') {
        drawCell(2,0,30,set_RGB(0.501, 0, 0),yoffset,offset);
        drawCell(3,0,30,set_RGB(0.501, 0, 0),yoffset,offset);
        drawCell(3,1,30,set_RGB(0.501, 0, 0),yoffset,offset);
        drawCell(3,2,30,set_RGB(0.501, 0, 0),yoffset,offset);
    }
}


bool drawPreviewDrop(int offset, int level,
            std::vector<Point> blockPoints, 
            std::vector<std::vector<Cell>> grid) {
    int xaxis = 0;
    int yaxis = 1;
    int runtime = 0;
    while (validBounds(blockPoints, xaxis, yaxis,grid)) {
        for (int i = 0; i < (int)blockPoints.size()-1; i++) {
            //grid[blockPoints[i].y][blockPoints[i].x].setType('n',-1, -1); // turn off
            if(runtime == 0) { // only undraw once
                grid[blockPoints[i].y][blockPoints[i].x].draw(offset);
            }
            blockPoints[i].x += xaxis;
            blockPoints[i].y += yaxis;
        for (int j = 0; j < (int)blockPoints.size()-1; j++) {
            if (grid[blockPoints[j].y][blockPoints[j].x].getType() == 'n') {
                grid[blockPoints[j].y][blockPoints[j].x].setType('p',id,level);
                }
            }
        }
        ++runtime;
    }
    int minY = blockPoints[0].y;;
    for (int i = 0; i < (int)blockPoints.size()-1; i++) {
        if (minY > blockPoints[i].y) minY = blockPoints[i].y;
    }
    if (minY < 3) {
        return false;
    }
    if (runtime != 0) { // no need to draw if nothing changed
        for (int i = 0; i < (int)blockPoints.size()-1; i++) {
            grid[blockPoints[i].y][blockPoints[i].x].draw(offset);
        }
    }
    return true;
}