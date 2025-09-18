#include "robot.h"

// Constructor
Robot::Robot(int start_x, int start_y, Direction start_dir)
    : x(start_x), y(start_y), dir(start_dir) {}



// In trạng thái
void Robot::print_status() const {
    std::cout << "Robot at (" << x << "," << y << ") facing "
              << dir_to_char(dir) << "\n";
}

// Chuyển hướng thành ký tự
char Robot::dir_to_char(Direction d) {
    switch (d) {
        case Direction::North: return 'N';
        case Direction::South: return 'S';
        case Direction::East:  return 'E';
        case Direction::West:  return 'W';
    }
    return '?';
}
bool Robot::is_goal(){
    return false;
}