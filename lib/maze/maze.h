#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <cassert>

const int MAZE_SIZE = 16;
const int MAX_DISTANCE = 1000;

class Cell {
public:
    int step;
    bool north_wall;
    bool south_wall;
    bool east_wall;
    bool west_wall;
    bool checked;

    Cell();

    void set_north_wall(bool wall);
    void set_south_wall(bool wall);
    void set_east_wall(bool wall);
    void set_west_wall(bool wall);
    void set_checked(bool check);
    void set_step(int s);
    void reset();
};

class ParentMaze {
public:
    Cell maze[MAZE_SIZE][MAZE_SIZE];
    std::vector<std::vector<int>> unvisited_stack;

    ParentMaze();

    Cell& cell(int x, int y);
    void clear_mem();
};

#endif // MAZE_H
