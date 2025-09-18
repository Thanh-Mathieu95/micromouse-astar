#ifndef ROBOT_H
#define ROBOT_H

#include <iostream>
#include <maze.h>
// Hướng nhìn của robot
enum class Direction { North = 0, East = 1, South = 2, West = 3 };

class Robot {
public:
    int x, y;
    Direction dir;

    Robot(int start_x = 0, int start_y = 0, Direction start_dir = Direction::North);

    bool check_north();
    bool check_east();
    bool check_south();
    bool check_west();
    void print_status() const;
    bool is_goal();
    void check_wall();
    void move_next(Cell);
private:
    static char dir_to_char(Direction d);
};

#endif // ROBOT_H
