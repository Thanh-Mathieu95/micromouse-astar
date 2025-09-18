#include <iostream>
#include <vector>
#include <stack>
#include "robot.h"
#include "maze.h"
using namespace std;

Robot mouse(0, 0, Direction::North); 
ParentMaze map;
stack<Cell> need_check;

void dfs_search() {
    while (!mouse.is_goal() && !need_check.empty()) {

        Cell next = need_check.top();
        need_check.pop();

        mouse.move_next(next);
        
        mouse.check_wall();

        if (mouse.y < MAZE_SIZE - 1 && mouse.check_north() && map.cell(mouse.x, mouse.y + 1).checked) {need_check.push(map.cell(mouse.x, mouse.y + 1));}
        if (mouse.y > 0 && mouse.check_south()){need_check.push(map.cell(mouse.x , mouse.y - 1));}
        if (mouse.x < MAZE_SIZE - 1 && mouse.check_east()) {need_check.push(map.cell(mouse.x + 1 , mouse.y));}
        if (mouse.x > 0 && mouse.check_west()) {need_check.push(map.cell(mouse.x - 1 , mouse.y));}
    }   
}

int main() {
    need_check.push(map.cell(0, 1));
    dfs_search();

    return 0;
}
