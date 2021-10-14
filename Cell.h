#ifndef __CELL_H__
#define __CELL_H__
#include <iostream>
#include <vector>
#include <sstream>

class Cell {
    int x; 
    int y;
    int id = -1;
    int level = -1;
    char blockType = ' ';
    bool blinded = false;
    public:
        Cell();  // Default constructor

        char getType() const; // Returns the value of isOn.
        void setType(char t,int id=0, int lev=0);          // Explicitly sets me to on.
        void setCoords(int r, int c);  // Tells me my row and column number.
        int getCoordX() const;
        int getCoordY() const;
        void setBlind(bool blind);
        bool getBlind();
        void draw(int offSet);
        void setId(int newID);
        int getId();
        void setLev(int newLev);
        int getLev();

    };

#endif
