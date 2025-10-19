#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <cassert>
#include <cstdint> // Thêm header này
#include <string> // Thêm header này

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
    bool run_visited; // Thêm thuộc tính này
    bool known = false;

    Cell();

    void set_north_wall(bool wall);
    void set_south_wall(bool wall);
    void set_east_wall(bool wall);
    void set_west_wall(bool wall);
    void set_checked(bool check);
    void set_step(int s);
    void reset();
};

struct Point {
    int8_t x;
    int8_t y;
};

class ParentMaze {
public:
    Cell maze[MAZE_SIZE][MAZE_SIZE];
    std::vector<std::vector<int>> unvisited_stack;

    ParentMaze();

    Cell& cell(int x, int y);
    void clear_mem();

    // --- Thêm các hàm thuật toán ---
    std::vector<Point> find_path_astar(Point start, Point end, int unvisited_modifier = 0);
    Point get_next_move(int current_x, int current_y);
    std::vector<Point> find_path_dfs(Point start, Point end);
    void floodfill_update(int goal_x, int goal_y, bool use_penalty, bool require_visited);
    bool load_from_file(const std::string& filename);
    Point find_nearest_unvisited(Point start);
    bool load_maze_from_file(const std::string& file_path);
};

#endif // MAZE_H
