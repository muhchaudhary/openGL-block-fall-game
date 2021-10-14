#include <vector>
#include <utility>
#include "Cell.h"
#include "GLwindow.h"

#ifndef BLOCK_H
#define BLOCK_H

class Point { // can be replaced with std::pair<int,int>
	public:
	int x;
	int y;
};

Point LowerLeft(std::vector<Point> blockPoints);
bool validBounds(std::vector<Point> currLoc, int x, int y, std::vector<std::vector<Cell>>& grid);
bool shift(int x, int y,int offset,int level, 
           std::vector<Point>& currBlockPoints, 
		   std::vector<std::vector<Cell>>& grid);
bool genBlocks(char blockType,int level, 
               std::vector<Point>& currBlockPoints,
               std::vector<std::vector<Cell>>& grid);
bool rotate(int rotation, int offset, int level,
            std::vector<Point>& blockPoints, 
            std::vector<std::vector<Cell>>& grid);
bool validBoundsRot(std::vector<Point> currLoc, 
                    std::vector<Point> newPoints,
                    std::vector<std::vector<Cell>>& grid);
#endif