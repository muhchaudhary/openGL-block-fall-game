#include <vector>
#include "block.h"
#include "Cell.h"
std::vector<Point> TblockPoints = {Point{1,3}, Point{0,2}, Point{1,2}, Point{2,2}, Point{0,3}};
std::vector<Point> IblockPoints = {Point{0, 3}, Point{1, 3}, Point{2, 3}, Point{3, 3},Point{0,3}};
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
            std::cout << blockPoints[i].y << " " << blockPoints[i].x << std::endl;
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
        for (int i = 0; i < (int)blockPoints.size(); i++) {
            char blockType = grid[blockPoints[1].y][blockPoints[1].x].getType();
            grid[blockPoints[i].y][blockPoints[i].x].setType('n',-1, -1); // turn off
            if(runtime == 0) { // only undraw once
                grid[blockPoints[i].y][blockPoints[i].x].draw(offset);
            }
            blockPoints[i].x += xaxis;
            blockPoints[i].y += yaxis;
        for (int i = 0; i < (int)blockPoints.size(); i++) { 
            grid[blockPoints[i].y][blockPoints[i].x].setType(blockType,id,level);
            }
        }
        ++runtime;
    }
    int minY = blockPoints[0].y;;
    for (int i = 0; i < (int)blockPoints.size(); i++) {
        if (minY > blockPoints[i].y) minY = blockPoints[i].y;
    }
    if (minY < 3) {
        return false;
    }
    if (runtime != 0) { // no need to draw if nothing changed
        for (int i = 0; i < (int)blockPoints.size(); i++) {
            grid[blockPoints[i].y][blockPoints[i].x].draw(offset);
        }
    }
    return true;
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