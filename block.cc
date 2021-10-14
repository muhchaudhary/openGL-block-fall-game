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
    for (int i = 0; i < (int)blockPoints.size()-1; i++) {
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

bool genBlocks(char blockType,int level, 
               std::vector<Point>& currBlockPoints,
               std::vector<std::vector<Cell>>& grid) {
    id++;
    bool dropped = true;
    ;
    switch (blockType) {
        case 'T':
            currBlockPoints = JblockPoints;
            for (int i = 0; i <(int)currBlockPoints.size()-1; i++) { // check each point
                grid[currBlockPoints[i].y][currBlockPoints[i].x].setId(id);
                grid[currBlockPoints[i].y][currBlockPoints[i].x].setType('T');
                //grid[currBlockPoints[i].y][currBlockPoints[i].x].draw(50);
                if (grid[currBlockPoints[i].y][currBlockPoints[i].x].getType() != 'n') {
                    dropped = false;
                break;
                }
            }
            break;
    }
    return dropped;
}