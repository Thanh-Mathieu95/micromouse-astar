#pragma once
#include <Arduino.h>
#include <vector>
using namespace std;
struct 
Cell
{   uint8_t x,y;
    int step = INFINITY;
    bool north_wall, south_wall, east_wall, west_wall = false;
    bool checked = false;
};
class Map{
    public:
    static constexpr int SIZE = 16;
    Cell cell[SIZE][SIZE];


    Map() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                cell[i][j].x = i;
                cell[i][j].y = j;
            }
        }
    }
// Map m;
// auto c = m.getCell(4, 5);
// std::cout << "Cell tại (" << c.x << "," << c.y << ")\n";

    Cell& getCell(int x, int y) {
        return cell[x][y];
    }
};
