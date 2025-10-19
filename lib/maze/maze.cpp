#include "lib/maze/maze.h"
#include <vector>
#include <cassert>
#include <algorithm>
#include <queue>
#include <cmath>
#include <fstream>
#include <string>
#include <iostream>

// --- Cell Implementations ---
Cell::Cell() : step(65535), north_wall(false), south_wall(false), east_wall(false), west_wall(false), checked(false), run_visited(false) {}
void Cell::set_north_wall(bool wall) { north_wall = wall; }
void Cell::set_south_wall(bool wall) { south_wall = wall; }
void Cell::set_east_wall(bool wall)  { east_wall = wall; }
void Cell::set_west_wall(bool wall)  { west_wall = wall; }
void Cell::set_checked(bool check)   { checked = check; }
void Cell::set_step(int s)           { step = s; }
void Cell::reset() { step = 65535; north_wall = false; south_wall = false; east_wall = false; west_wall = false; checked = false; run_visited = false; }

// --- ParentMaze Implementations ---
ParentMaze::ParentMaze() {
    for (int i = 0; i < MAZE_SIZE; i++) {
        maze[i][0].set_south_wall(true);
        maze[0][i].set_west_wall(true);
        maze[i][MAZE_SIZE - 1].set_north_wall(true);
        maze[MAZE_SIZE - 1][i].set_east_wall(true);
    }
}
Cell& ParentMaze::cell(int x, int y) { assert(x >= 0 && x < MAZE_SIZE && y >= 0 && y < MAZE_SIZE); return maze[x][y]; }
void ParentMaze::clear_mem() { for (int i = 0; i < MAZE_SIZE; i++) for (int j = 0; j < MAZE_SIZE; j++) maze[i][j].reset(); /* ... re-add boundary walls ... */ }

void ParentMaze::floodfill_update(int goal_x, int goal_y, bool use_penalty, bool require_visited) {
    const int PENALTY = 30;
    const int QUEUE_SIZE = MAZE_SIZE * MAZE_SIZE; Point queue[QUEUE_SIZE];
    int head = 0, tail = 0;
    for (int y = 0; y < MAZE_SIZE; ++y) for (int x = 0; x < MAZE_SIZE; ++x) maze[x][y].set_step(65535);
    Point destination = {(int8_t)goal_x, (int8_t)goal_y};
    maze[destination.x][destination.y].set_step(0);
    queue[tail] = destination; tail = (tail + 1) % QUEUE_SIZE;
    while (head != tail) {
        Point current = queue[head]; head = (head + 1) % QUEUE_SIZE;
        int current_step = maze[current.x][current.y].step;
        Point neighbors[4] = {{current.x, (int8_t)(current.y + 1)}, {(int8_t)(current.x + 1), current.y}, {current.x, (int8_t)(current.y - 1)}, {(int8_t)(current.x - 1), current.y}};
        bool walls[4] = {maze[current.x][current.y].north_wall, maze[current.x][current.y].east_wall, maze[current.x][current.y].south_wall, maze[current.x][current.y].west_wall};
        for (int i = 0; i < 4; ++i) {
            if (!walls[i]) {
                Point n = neighbors[i];
                if (n.x >= 0 && n.x < MAZE_SIZE && n.y >= 0 && n.y < MAZE_SIZE) {
                    // Check if the cell is allowed at all
                    if (!require_visited || maze[n.x][n.y].run_visited) {
                        // Calculate cost
                        int cost = 1;
                        if (use_penalty && !maze[n.x][n.y].run_visited) {
                            cost += PENALTY;
                        }

                        if (maze[n.x][n.y].step > current_step + cost) {
                            maze[n.x][n.y].set_step(current_step + cost);
                            queue[tail] = n; tail = (tail + 1) % QUEUE_SIZE;
                        }
                    }
                }
            }
        }
    }
}

Point ParentMaze::get_next_move(int current_x, int current_y) {
    int min_step = 65535; Point best_move = {(int8_t)current_x, (int8_t)current_y};
    Point neighbors[4] = {{(int8_t)current_x, (int8_t)(current_y + 1)}, {(int8_t)(current_x + 1), (int8_t)current_y}, {(int8_t)current_x, (int8_t)(current_y - 1)}, {(int8_t)(current_x - 1), (int8_t)current_y}};
    bool walls[4] = {maze[current_x][current_y].north_wall, maze[current_x][current_y].east_wall, maze[current_x][current_y].south_wall, maze[current_x][current_y].west_wall};
    for (int i = 0; i < 4; ++i) {
        if (!walls[i]) {
            Point n = neighbors[i];
            if (n.x >= 0 && n.x < MAZE_SIZE && n.y >= 0 && n.y < MAZE_SIZE) {
                if (maze[n.x][n.y].step < min_step) { min_step = maze[n.x][n.y].step; best_move = n; }
            }
        }
    }
    return best_move;
}

std::vector<Point> ParentMaze::find_path_dfs(Point start, Point end) { /* ... implementation from previous steps ... */ return {}; }

struct AStarNode { Point point; int f_cost; bool operator>(const AStarNode& other) const { return f_cost > other.f_cost; } };
int heuristic(Point a, Point b) { return std::abs(a.x - b.x) + std::abs(a.y - b.y); }
std::vector<Point> ParentMaze::find_path_astar(Point start, Point end, int unvisited_modifier) {
    const int TURN_PENALTY = 10; // Penalty for making a turn
    const int UNVISITED_PENALTY = 50; // Penalty for entering unvisited cells

    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> open_set;
    int g_cost[MAZE_SIZE][MAZE_SIZE];
    Point predecessors[MAZE_SIZE][MAZE_SIZE];

    for (int i = 0; i < MAZE_SIZE; ++i) for (int j = 0; j < MAZE_SIZE; ++j) g_cost[i][j] = 65535;
    
    g_cost[start.x][start.y] = 0;
    open_set.push({start, heuristic(start, end)});

    while (!open_set.empty()) {
        Point current = open_set.top().point; 
        open_set.pop();

        if (current.x == end.x && current.y == end.y) {
            std::vector<Point> path; Point p = end;
            while (p.x != start.x || p.y != start.y) { path.push_back(p); p = predecessors[p.x][p.y]; }
            path.push_back(start); 
            std::reverse(path.begin(), path.end()); 
            return path;
        }

        Point neighbors[4] = {{current.x, (int8_t)(current.y + 1)}, {(int8_t)(current.x + 1), current.y}, {current.x, (int8_t)(current.y - 1)}, {(int8_t)(current.x - 1), current.y}};
        bool walls[4] = {maze[current.x][current.y].north_wall, maze[current.x][current.y].east_wall, maze[current.x][current.y].south_wall, maze[current.x][current.y].west_wall};

        for (int i = 0; i < 4; ++i) {
            if (!walls[i]) {
                Point neighbor = neighbors[i];
                if (neighbor.x >= 0 && neighbor.x < MAZE_SIZE && neighbor.y >= 0 && neighbor.y < MAZE_SIZE) {
                    
                    int cost = 1; // Base movement cost

                    // Apply modifier for unvisited cells (can be penalty or bonus)
                    if (!maze[neighbor.x][neighbor.y].run_visited) {
                        cost += unvisited_modifier;
                    }

                    // Turn penalty logic
                    if (!(current.x == start.x && current.y == start.y)) {
                        Point predecessor = predecessors[current.x][current.y];
                        int dx1 = current.x - predecessor.x;
                        int dy1 = current.y - predecessor.y;
                        int dx2 = neighbor.x - current.x;
                        int dy2 = neighbor.y - current.y;
                        if (dx1 != dx2 || dy1 != dy2) {
                            cost += TURN_PENALTY;
                        }
                    }

                    int tentative_g_cost = g_cost[current.x][current.y] + cost;
                    if (tentative_g_cost < g_cost[neighbor.x][neighbor.y]) {
                        predecessors[neighbor.x][neighbor.y] = current;
                        g_cost[neighbor.x][neighbor.y] = tentative_g_cost;
                        open_set.push({neighbor, tentative_g_cost + heuristic(neighbor, end)});
                    }
                }
            }
        }
    }
    return {};
}

bool ParentMaze::load_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) { std::cerr << "Error: Could not open maze file " << filename << std::endl; return false; }
    std::vector<std::string> lines; std::string line;
    while (std::getline(file, line)) lines.push_back(line);
    file.close();
    if (lines.size() < MAZE_SIZE * 2 + 1) { std::cerr << "Error: Maze file has insufficient rows." << std::endl; return false; }
    for (int y = 0; y < MAZE_SIZE; ++y) {
        int line_y_top = (MAZE_SIZE - 1 - y) * 2; int line_y_mid = line_y_top + 1;
        for (int x = 0; x < MAZE_SIZE; ++x) {
            if (lines[line_y_top][x * 4 + 1] == '-') cell(x, y).set_north_wall(true); else cell(x, y).set_north_wall(false);
            if (lines[line_y_mid][x * 4] == '|') cell(x, y).set_west_wall(true); else cell(x, y).set_west_wall(false);
        }
    }
    for (int y = 0; y < MAZE_SIZE; y++) {
        if (lines[(MAZE_SIZE - 1 - y) * 2 + 1][MAZE_SIZE * 4] == '|') cell(MAZE_SIZE - 1, y).set_east_wall(true);
        else cell(MAZE_SIZE - 1, y).set_east_wall(false);
    }
    for (int x = 0; x < MAZE_SIZE; x++) {
        if (lines[MAZE_SIZE * 2][x * 4 + 1] == '-') cell(x, 0).set_south_wall(true);
        else cell(x, 0).set_south_wall(false);
    }
    return true;
}

Point ParentMaze::find_nearest_unvisited(Point start) {
    std::queue<Point> q; bool visited[MAZE_SIZE][MAZE_SIZE] = {false};
    q.push(start); visited[start.x][start.y] = true;
    while(!q.empty()){
        Point current = q.front(); q.pop();
        if(!cell(current.x, current.y).run_visited) return current;
        Point neighbors[4] = {{current.x, (int8_t)(current.y + 1)}, {(int8_t)(current.x + 1), current.y}, {current.x, (int8_t)(current.y - 1)}, {(int8_t)(current.x - 1), current.y}};
        bool walls[4] = {maze[current.x][current.y].north_wall, maze[current.x][current.y].east_wall, maze[current.x][current.y].south_wall, maze[current.x][current.y].west_wall};
        for(int i=0; i<4; ++i){
            if(!walls[i]){
                Point n = neighbors[i];
                if(n.x >= 0 && n.x < MAZE_SIZE && n.y >= 0 && n.y < MAZE_SIZE && !visited[n.x][n.y]){
                    visited[n.x][n.y] = true; q.push(n);
                }
            }
        }
    }
    return {-1, -1};
}