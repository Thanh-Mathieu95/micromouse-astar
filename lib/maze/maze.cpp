#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include "maze.h"

using namespace std;

// #                     n 
// #                   w   e
// #                     s

class Cell {
public:
    int step;
    bool north_wall;
    bool south_wall;
    bool east_wall;
    bool west_wall;
    bool checked;

    Cell() : step(256), north_wall(false), south_wall(false),
             east_wall(false), west_wall(false), checked(false) {}

    void set_north_wall(bool wall) { north_wall = wall; }
    void set_south_wall(bool wall) { south_wall = wall; }
    void set_east_wall(bool wall)  { east_wall = wall; }
    void set_west_wall(bool wall)  { west_wall = wall; }
    void set_checked(bool check)   { checked = check; }
    void set_step(int s)           { step = s; }
};

const int MAZE_SIZE = 16;
const int MAX_DISTANCE = 1000;

class ParentMaze {
public:
    Cell maze[MAZE_SIZE][MAZE_SIZE];
    vector<vector<int>> unvisited_stack; // chưa dùng

    ParentMaze() {
        // Initialize maze with edge walls
        for (int i = 0; i < MAZE_SIZE; i++) {
            maze[i][0].set_south_wall(true);
            maze[0][i].set_west_wall(true);
            maze[i][MAZE_SIZE - 1].set_north_wall(true);
            maze[MAZE_SIZE - 1][i].set_east_wall(true);
        }
        maze[0][0].set_east_wall(true);
        maze[1][0].set_west_wall(true);
    }

    Cell& cell(int x, int y) {
        return maze[x][y];
    }

    void clear_mem() {
        for (int i = 0; i < MAZE_SIZE; i++) {
            for (int j = 0; j < MAZE_SIZE; j++) {
                maze[i][j].set_north_wall(false);
                maze[i][j].set_south_wall(false);
                maze[i][j].set_west_wall(false);
                maze[i][j].set_east_wall(false);
                maze[i][j].set_checked(false);
                maze[i][j].step = 256;
            }
        }

        // Reset edge walls
        for (int i = 0; i < MAZE_SIZE; i++) {
            maze[i][0].set_south_wall(true);
            maze[0][i].set_west_wall(true);
            maze[i][MAZE_SIZE - 1].set_north_wall(true);
            maze[MAZE_SIZE - 1][i].set_east_wall(true);
        }
        maze[0][0].set_east_wall(true);
        maze[1][0].set_west_wall(true);
    }
}; // <-- thiếu ; ở đây
