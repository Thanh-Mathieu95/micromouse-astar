#include <iostream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <thread>
#include <string>
#include <fstream>
#include <random>
#include <algorithm>
#include "lib/maze/maze.h"

enum RobotDirection { NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3 };

// --- CÁC HÀM TIỆN ÍCH ---

bool load_maze_from_file(ParentMaze& maze, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) { std::cerr << "Error: Could not open maze file: " << filename << std::endl; return false; }
    std::vector<std::string> lines; std::string line;
    while (std::getline(file, line)) lines.push_back(line);
    if (lines.size() != (MAZE_SIZE * 2 + 1)) { std::cerr << "Error: Maze file has incorrect number of lines." << std::endl; return false; }
    for (int y = 0; y < MAZE_SIZE; ++y) {
        int line_idx_horz = (MAZE_SIZE - 1 - y) * 2;
        int line_idx_vert = line_idx_horz + 1;
        for (int x = 0; x < MAZE_SIZE; ++x) {
            if (lines[line_idx_horz].substr(x * 4 + 1, 3) == "---") maze.cell(x, y).set_north_wall(true);
            if (lines[line_idx_vert][x * 4] == '|') maze.cell(x, y).set_west_wall(true);
        }
    }
    for (int y = 0; y < MAZE_SIZE; ++y) {
        for (int x = 0; x < MAZE_SIZE; ++x) {
            if (x < MAZE_SIZE - 1) { if (maze.cell(x + 1, y).west_wall) maze.cell(x, y).set_east_wall(true); } else { maze.cell(x, y).set_east_wall(true); }
            if (y > 0) { if (maze.cell(x, y - 1).north_wall) maze.cell(x, y).set_south_wall(true); } else { maze.cell(x, y).set_south_wall(true); }
        }
    }
    return true;
}

void print_side_by_side_mazes(ParentMaze& true_maze, ParentMaze& learned_maze, Point robot_pos, const std::vector<Point>& path = {}) {
    const std::string color_blue = "\033[94m"; const std::string color_green = "\033[32m"; const std::string color_reset = "\033[0m";
    std::cout << std::left << std::setw(MAZE_SIZE * 4 + 5) << "--- Ground Truth Maze ---" << "|   -- Robot's Knowledge ---" << std::endl;
    for (int y = MAZE_SIZE - 1; y >= 0; --y) {
        for (int x = 0; x < MAZE_SIZE; ++x) std::cout << "+" << (true_maze.cell(x, y).north_wall ? "---" : "   ");
        std::cout << "+   |   ";
        for (int x = 0; x < MAZE_SIZE; ++x) std::cout << "+" << (learned_maze.cell(x, y).north_wall ? "---" : "   ");
        std::cout << "+" << std::endl;
        for (int x = 0; x < MAZE_SIZE; ++x) std::cout << (true_maze.cell(x, y).west_wall ? "|" : " ") << "   ";
        std::cout << "|   |   ";
        for (int x = 0; x < MAZE_SIZE; ++x) {
            std::cout << (learned_maze.cell(x, y).west_wall ? "|" : " ");
            char path_char = ' ';
            for (size_t i = 0; i < path.size(); ++i) {
                if (path[i].x == x && path[i].y == y) {
                    if (i + 1 < path.size()) {
                        Point next_p = path[i+1];
                        if (next_p.x > x) path_char = '>'; else if (next_p.x < x) path_char = '<';
                        else if (next_p.y > y) path_char = '^'; else path_char = 'v';
                    } else path_char = 'G';
                    break;
                }
            }
            if (x == robot_pos.x && y == robot_pos.y) std::cout << color_blue << " R " << color_reset;
            else if (path_char != ' ') std::cout << color_green << " " << path_char << " " << color_reset;
            else if (learned_maze.cell(x,y).run_visited) std::cout << " . ";
            else std::cout << "   ";
        }
        std::cout << "|" << std::endl;
    }
    for (int i = 0; i < 2; ++i) {
        for (int x = 0; x < MAZE_SIZE; ++x) std::cout << "+---";
        std::cout << "+";
        if (i == 0) std::cout << "   |   ";
    }
    std::cout << std::endl;
}

void print_final_path_on_maze(ParentMaze& maze, const std::vector<Point>& path) {
    const std::string color_green = "\033[32m";
    const std::string color_reset = "\033[0m";
    std::cout << "\nFinal learned map with shortest path (^>v<):" << std::endl;
    for (int y = MAZE_SIZE - 1; y >= 0; --y) {
        for (int x = 0; x < MAZE_SIZE; ++x) std::cout << "+" << (maze.cell(x, y).north_wall ? "---" : "   ");
        std::cout << "+" << std::endl;
        for (int x = 0; x < MAZE_SIZE; ++x) {
            std::cout << (maze.cell(x, y).west_wall ? "|" : " ");
            char path_char = ' ';
            for (size_t i = 0; i < path.size(); ++i) {
                if (path[i].x == x && path[i].y == y) {
                    if (i + 1 < path.size()) {
                        Point next_p = path[i+1];
                        if (next_p.x > x) path_char = '>';
                        else if (next_p.x < x) path_char = '<';
                        else if (next_p.y > y) path_char = '^';
                        else path_char = 'v';
                    } else {
                        path_char = 'G'; // Goal
                    }
                    if (i == 0) path_char = 'S'; // Start always overrides
                    break;
                }
            }
            if (path_char != ' ') std::cout << color_green << " " << path_char << " " << color_reset;
            else if (maze.cell(x,y).run_visited) std::cout << " . ";
            else std::cout << "   ";
        }
        std::cout << "|" << std::endl;
    }
    for (int x = 0; x < MAZE_SIZE; ++x) std::cout << "+---";
    std::cout << "+" << std::endl;
}

std::string generate_move_sequence(const std::vector<Point>& path) {
    if (path.size() < 2) return "";
    std::string move_sequence = ""; RobotDirection current_dir = NORTH;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        Point current_pos = path[i]; Point next_pos = path[i+1];
        RobotDirection target_dir;
        if (next_pos.y > current_pos.y) target_dir = NORTH; else if (next_pos.x > current_pos.x) target_dir = EAST;
        else if (next_pos.y < current_pos.y) target_dir = SOUTH; else target_dir = WEST;
        int direction_diff = (target_dir - current_dir + 4) % 4;
        if (direction_diff == 1) move_sequence += 'R'; else if (direction_diff == 2) move_sequence += 'B'; else if (direction_diff == 3) move_sequence += 'L';
        move_sequence += 'F'; current_dir = target_dir;
    }
    return move_sequence;
}

Point find_goal(ParentMaze& learned_maze) {
    for (int y = 0; y < MAZE_SIZE - 1; ++y) {
        for (int x = 0; x < MAZE_SIZE - 1; ++x) {
            Cell& bl = learned_maze.cell(x, y); Cell& br = learned_maze.cell(x + 1, y); 
            Cell& tl = learned_maze.cell(x, y + 1); Cell& tr = learned_maze.cell(x + 1, y + 1);
            if (bl.north_wall || br.north_wall || tl.south_wall || tr.south_wall || bl.east_wall || tl.east_wall || br.west_wall || tr.west_wall) continue;
            int entrances = 0;
            if (!bl.west_wall) entrances++; if (!tl.west_wall) entrances++;
            if (!br.east_wall) entrances++; if (!tr.east_wall) entrances++;
            if (!bl.south_wall) entrances++; if (!br.south_wall) entrances++;
            if (!tl.north_wall) entrances++; if (!tr.north_wall) entrances++;
            if (entrances == 1) return {(int8_t)x, (int8_t)y};
        }
    }
    return {-1, -1};
}

void simulate_sensors(ParentMaze& true_maze, ParentMaze& learned_maze, Point robot_pos) {
    Cell& true_cell = true_maze.cell(robot_pos.x, robot_pos.y);
    Cell& learned_cell = learned_maze.cell(robot_pos.x, robot_pos.y);
    if (true_cell.north_wall) { learned_cell.set_north_wall(true); if (robot_pos.y < MAZE_SIZE - 1) learned_maze.cell(robot_pos.x, robot_pos.y + 1).set_south_wall(true); }
    if (true_cell.east_wall) { learned_cell.set_east_wall(true); if (robot_pos.x < MAZE_SIZE - 1) learned_maze.cell(robot_pos.x + 1, robot_pos.y).set_west_wall(true); }
    if (true_cell.south_wall) { learned_cell.set_south_wall(true); if (robot_pos.y > 0) learned_maze.cell(robot_pos.x, robot_pos.y - 1).set_north_wall(true); }
    if (true_cell.west_wall) { learned_cell.set_west_wall(true); if (robot_pos.x > 0) learned_maze.cell(robot_pos.x - 1, robot_pos.y).set_east_wall(true); }
}

// --- HÀM MAIN ---
int main() {
    ParentMaze true_maze;
    if (!load_maze_from_file(true_maze, "/home/dwduong74/Documents/micro_mouse/map/custom_maze.txt")) return 1;
    
    ParentMaze robot_maze_knowledge;
    Point robot_pos = {0, 0};
    Point start_pos = {0, 0};
    Point found_goal_pos = {-1, -1};

    system("clear");
    std::cout << "--- Micromouse Simulation: DFS + Flood Fill ---" << std::endl;
    std::cout << "Starting..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // --- STAGE 1: DFS Exploration with Path History Backtracking ---
    std::vector<Point> path_history;
    path_history.push_back(robot_pos);
    int step_count = 0;

    while(step_count < 4000) { // Safety break
        step_count++;
        robot_maze_knowledge.cell(robot_pos.x, robot_pos.y).run_visited = true;
        simulate_sensors(true_maze, robot_maze_knowledge, robot_pos);

        // Check for goal
        if (found_goal_pos.x == -1) {
            found_goal_pos = find_goal(robot_maze_knowledge);
            if (found_goal_pos.x != -1) {
                std::cout << "\n*** GOAL FOUND at (" << (int)found_goal_pos.x << "," << (int)found_goal_pos.y << ")! ***" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                break; // End of Stage 1
            }
        }

        system("clear");
        std::cout << "Stage 1: Exploring (DFS)... Step: " << step_count << " | Position: (" << (int)robot_pos.x << "," << (int)robot_pos.y << ")" << std::endl;
        print_side_by_side_mazes(true_maze, robot_maze_knowledge, robot_pos);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        // Find unvisited neighbors
        Point next_move = {-1, -1};
        Cell& learned_cell = robot_maze_knowledge.cell(robot_pos.x, robot_pos.y);
        Point neighbors[4] = {{robot_pos.x, (int8_t)(robot_pos.y + 1)}, {(int8_t)(robot_pos.x + 1), robot_pos.y}, {robot_pos.x, (int8_t)(robot_pos.y - 1)}, {(int8_t)(robot_pos.x - 1), robot_pos.y}};
        bool walls[4] = {learned_cell.north_wall, learned_cell.east_wall, learned_cell.south_wall, learned_cell.west_wall};

        for(int i=0; i<4; ++i) {
            if(!walls[i]) {
                Point n = neighbors[i];
                if(n.x >= 0 && n.x < MAZE_SIZE && n.y >= 0 && n.y < MAZE_SIZE && !robot_maze_knowledge.cell(n.x, n.y).run_visited) {
                    next_move = n;
                    break; // Take the first available unvisited path
                }
            }
        }

        if (next_move.x != -1) {
            // Move forward
            robot_pos = next_move;
            path_history.push_back(robot_pos);
        } else {
            // Backtrack by reversing path
            if (path_history.size() <= 1) {
                std::cout << "\nRobot is trapped at the start." << std::endl;
                break;
            }
            path_history.pop_back();
            robot_pos = path_history.back();
        }
    }

    // --- STAGE 2: Return to Start using Dynamic A* (Run 1) ---
    if (found_goal_pos.x != -1) {
        std::cout << "\nStage 2: Returning to Start (A* with penalty)..." << std::endl; std::this_thread::sleep_for(std::chrono::seconds(2));
        while(robot_pos.x != start_pos.x || robot_pos.y != start_pos.y) {
            step_count++; if(step_count > 8000) {found_goal_pos.x = -1; break;}
            robot_maze_knowledge.cell(robot_pos.x, robot_pos.y).run_visited = true; simulate_sensors(true_maze, robot_maze_knowledge, robot_pos);
            std::vector<Point> path = robot_maze_knowledge.find_path_astar(robot_pos, start_pos, true);
            if (path.size() < 2) { found_goal_pos = {-1,-1}; break; }
            robot_pos = path[1];
            system("clear"); std::cout << "Stage 2... Pos: (" << (int)robot_pos.x << "," << (int)robot_pos.y << ")" << std::endl; print_side_by_side_mazes(true_maze, robot_maze_knowledge, robot_pos, path); std::this_thread::sleep_for(std::chrono::milliseconds(75));
        }
    }

    // --- STAGE 3: Dynamic A* Run to Goal ---
    if (found_goal_pos.x != -1) {
        std::cout << "\nStage 3: Dynamic optimistic A* run to goal..." << std::endl; std::this_thread::sleep_for(std::chrono::seconds(2));
        while(robot_pos.x != found_goal_pos.x || robot_pos.y != found_goal_pos.y) {
            step_count++; if(step_count > 8000) {found_goal_pos.x = -1; break;}
            robot_maze_knowledge.cell(robot_pos.x, robot_pos.y).run_visited = true; simulate_sensors(true_maze, robot_maze_knowledge, robot_pos);
            std::vector<Point> path = robot_maze_knowledge.find_path_astar(robot_pos, found_goal_pos, true);
            if (path.size() < 2) { found_goal_pos = {-1,-1}; break; }
            robot_pos = path[1];
            system("clear"); std::cout << "Stage 3... Pos: (" << (int)robot_pos.x << "," << (int)robot_pos.y << ")" << std::endl; print_side_by_side_mazes(true_maze, robot_maze_knowledge, robot_pos, path); std::this_thread::sleep_for(std::chrono::milliseconds(75));
        }
    }

    // --- STAGE 4: Return to Start using Dynamic A* (Run 2) ---
    if (found_goal_pos.x != -1) {
        std::cout << "\nStage 4: Returning to Start again (A* with penalty)..." << std::endl; std::this_thread::sleep_for(std::chrono::seconds(2));
        while(robot_pos.x != start_pos.x || robot_pos.y != start_pos.y) {
            step_count++; if(step_count > 8000) {found_goal_pos.x = -1; break;}
            robot_maze_knowledge.cell(robot_pos.x, robot_pos.y).run_visited = true; simulate_sensors(true_maze, robot_maze_knowledge, robot_pos);
            std::vector<Point> path = robot_maze_knowledge.find_path_astar(robot_pos, start_pos, true);
            if (path.size() < 2) { found_goal_pos = {-1,-1}; break; }
            robot_pos = path[1];
            system("clear"); std::cout << "Stage 4... Pos: (" << (int)robot_pos.x << "," << (int)robot_pos.y << ")" << std::endl; print_side_by_side_mazes(true_maze, robot_maze_knowledge, robot_pos, path); std::this_thread::sleep_for(std::chrono::milliseconds(75));
        }
    }

    // --- STAGE 5: Return to Start using Dynamic A* (Run 3) ---
    if (found_goal_pos.x != -1) {
        std::cout << "\nStage 5: Returning to Start a third time (A* with penalty)..." << std::endl; std::this_thread::sleep_for(std::chrono::seconds(2));
        while(robot_pos.x != start_pos.x || robot_pos.y != start_pos.y) {
            step_count++; if(step_count > 8000) {found_goal_pos.x = -1; break;}
            robot_maze_knowledge.cell(robot_pos.x, robot_pos.y).run_visited = true; simulate_sensors(true_maze, robot_maze_knowledge, robot_pos);
            std::vector<Point> path = robot_maze_knowledge.find_path_astar(robot_pos, start_pos, true);
            if (path.size() < 2) { found_goal_pos = {-1,-1}; break; }
            robot_pos = path[1];
            system("clear"); std::cout << "Stage 5... Pos: (" << (int)robot_pos.x << "," << (int)robot_pos.y << ")" << std::endl; print_side_by_side_mazes(true_maze, robot_maze_knowledge, robot_pos, path); std::this_thread::sleep_for(std::chrono::milliseconds(75));
        }
    }

    // --- STAGE 6: Calculate and Display Final Path ---
    if (found_goal_pos.x != -1) {
        std::cout << "\nAll learning complete. Calculating final safe path..." << std::endl;
        std::vector<Point> final_path = robot_maze_knowledge.find_path_astar(start_pos, found_goal_pos, true);
        system("clear");
        std::cout << "--- FINAL RESULT ---" << std::endl;
        if (!final_path.empty()) {
            std::string race_moves = generate_move_sequence(final_path);
            std::cout << "Shortest safe path length: " << final_path.size() - 1 << " moves." << std::endl;
            std::cout << "\nReusable Move Sequence for Robot:\n" << race_moves << std::endl;
            print_final_path_on_maze(robot_maze_knowledge, final_path);
        } else {
            std::cout << "Could not calculate a final safe path." << std::endl;
        }
    }
    else {
        std::cout << "\n--- MISSION FAILED ---" << std::endl;
        std::cout << "Robot failed to find a verifiable path." << std::endl;
    }

    return 0;
}
