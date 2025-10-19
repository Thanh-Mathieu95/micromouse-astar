#include <Arduino.h>
#include <vector>
#include <LittleFS.h>
#include "maze.h"
#include "robot_control.h"
#include "ir_sensor.h"
#include "encoder_motor.h"

// --- QUẢN LÝ TRẠNG THÁI NHIỆM VỤ ---
enum MissionStage {
    WAITING_FOR_START,
    STAGE_1_DFS_EXPLORE,
    STAGE_2_RETURN_HOME_1,
    STAGE_3_RUN_TO_GOAL,
    STAGE_4_RETURN_HOME_2,
    STAGE_5_RETURN_HOME_3,
    STAGE_6_CALCULATE_FINAL_PATH,
    STAGE_7_FINAL_RUN,
    MISSION_COMPLETE
};
MissionStage current_stage = WAITING_FOR_START;

// --- TRẠNG THÁI MÊ CUNG & ROBOT ---
ParentMaze maze;
Point robotPosition = {0, 0};
RobotDirection robotDirection = NORTH_DIR;
Point start_pos = {0, 0};
Point found_goal_pos = {-1, -1};
Point goal_center[4] = {{7,7}, {7,8}, {8,7}, {8,8}};

// --- BIẾN HỖ TRỢ THUẬT TOÁN ---
std::vector<Point> path_history;
std::vector<Point> final_path;

// --- CÁC HÀM LƯU/TẢI & ĐIỀU KHIỂN ---

bool save_maze_to_file(const char* filename) {
    File file = LittleFS.open(filename, "w");
    if (!file) {
        Serial.println("Loi: Khong the mo file de ghi.");
        return false;
    }
    for (int y = MAZE_SIZE - 1; y >= 0; --y) {
        for (int x = 0; x < MAZE_SIZE; ++x) { file.print("+"); file.print(maze.cell(x, y).north_wall ? "---" : "   "); }
        file.println("+");
        for (int x = 0; x < MAZE_SIZE; ++x) { file.print(maze.cell(x, y).west_wall ? "|" : " "); file.print("   "); }
        file.println("|");
    }
    for (int x = 0; x < MAZE_SIZE; ++x) { file.print("+---"); }
    file.println("+");
    file.close();
    Serial.println("Da luu ban do.");
    return true;
}

bool load_maze_from_file(const char* filename) {
    File file = LittleFS.open(filename, "r");
    if (!file) { Serial.println("Loi: Khong the mo file de doc."); return false; }
    std::vector<String> lines;
    while (file.available()) { lines.push_back(file.readStringUntil('\n')); }
    file.close();
    if (lines.size() < (MAZE_SIZE * 2 + 1)) { Serial.println("Loi: File ban do khong dung dinh dang."); return false; }
    for (int y = 0; y < MAZE_SIZE; ++y) {
        int line_idx_horz = (MAZE_SIZE - 1 - y) * 2;
        int line_idx_vert = line_idx_horz + 1;
        for (int x = 0; x < MAZE_SIZE; ++x) {
            if (lines[line_idx_horz].substring(x * 4 + 1, x * 4 + 4) == "---") maze.cell(x, y).set_north_wall(true);
            if (lines[line_idx_vert].charAt(x * 4) == '|') maze.cell(x, y).set_west_wall(true);
        }
    }
    for (int y = 0; y < MAZE_SIZE; ++y) {
        for (int x = 0; x < MAZE_SIZE; ++x) {
            if (x < MAZE_SIZE - 1) { if (maze.cell(x + 1, y).west_wall) maze.cell(x, y).set_east_wall(true); } else { maze.cell(x, y).set_east_wall(true); }
            if (y > 0) { if (maze.cell(x, y - 1).north_wall) maze.cell(x, y).set_south_wall(true); } else { maze.cell(x, y).set_south_wall(true); }
            if(maze.cell(x,y).north_wall || maze.cell(x,y).south_wall || maze.cell(x,y).east_wall || maze.cell(x,y).west_wall) {
                maze.cell(x,y).run_visited = true;
            }
        }
    }
    return true;
}

void handle_serial_commands() {
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        if (cmd == "delete") {
            Serial.println("Nhan lenh 'delete'. Dang xoa ban do...");
            if (LittleFS.remove("/map.txt")) Serial.println("Da xoa file.");
            else Serial.println("Loi: Khong the xoa file.");
        } else if (current_stage == WAITING_FOR_START) {
            Serial.println("Bat dau Giai doan 1: Kham pha DFS...");
            path_history.push_back(robotPosition);
            maze.cell(robotPosition.x, robotPosition.y).run_visited = true;
            current_stage = STAGE_1_DFS_EXPLORE;
        }
    }
}

// --- CÁC HÀM CẢM BIẾN & DI CHUYỂN ---
bool is_in_goal(Point pos) { for(int i=0; i<4; ++i) { if (pos.x == goal_center[i].x && pos.y == goal_center[i].y) return true; } return false; }

void update_walls_from_sensors() {
    update_ir();
    bool wall_front = sensorValues[1] > 1000; bool wall_left  = sensorValues[0] > 1000; bool wall_right = sensorValues[2] > 1000;
    Cell& currentCell = maze.cell(robotPosition.x, robotPosition.y);
    switch (robotDirection) {
        case NORTH_DIR: if (wall_front) { currentCell.set_north_wall(true); if (robotPosition.y < MAZE_SIZE - 1) maze.cell(robotPosition.x, robotPosition.y + 1).set_south_wall(true); } if (wall_left)  { currentCell.set_west_wall(true);  if (robotPosition.x > 0) maze.cell(robotPosition.x - 1, robotPosition.y).set_east_wall(true);  } if (wall_right) { currentCell.set_east_wall(true);  if (robotPosition.x < MAZE_SIZE - 1) maze.cell(robotPosition.x + 1, robotPosition.y).set_west_wall(true);  } break;
        case EAST_DIR:  if (wall_front) { currentCell.set_east_wall(true);  if (robotPosition.x < MAZE_SIZE - 1) maze.cell(robotPosition.x + 1, robotPosition.y).set_west_wall(true);  } if (wall_left)  { currentCell.set_north_wall(true); if (robotPosition.y < MAZE_SIZE - 1) maze.cell(robotPosition.x, robotPosition.y + 1).set_south_wall(true); } if (wall_right) { currentCell.set_south_wall(true); if (robotPosition.y > 0) maze.cell(robotPosition.x, robotPosition.y - 1).set_north_wall(true); } break;
        case SOUTH_DIR: if (wall_front) { currentCell.set_south_wall(true); if (robotPosition.y > 0) maze.cell(robotPosition.x, robotPosition.y - 1).set_north_wall(true); } if (wall_left)  { currentCell.set_east_wall(true);  if (robotPosition.x < MAZE_SIZE - 1) maze.cell(robotPosition.x + 1, robotPosition.y).set_west_wall(true);  } if (wall_right) { currentCell.set_west_wall(true);  if (robotPosition.x > 0) maze.cell(robotPosition.x - 1, robotPosition.y).set_east_wall(true);  } break;
        case WEST_DIR:  if (wall_front) { currentCell.set_west_wall(true);  if (robotPosition.x > 0) maze.cell(robotPosition.x - 1, robotPosition.y).set_east_wall(true);  } if (wall_left)  { currentCell.set_south_wall(true); if (robotPosition.y > 0) maze.cell(robotPosition.x, robotPosition.y - 1).set_north_wall(true); } if (wall_right) { currentCell.set_north_wall(true); if (robotPosition.y < MAZE_SIZE - 1) maze.cell(robotPosition.x, robotPosition.y + 1).set_south_wall(true); } break;
    }
}

// --- CHƯƠNG TRÌNH CHÍNH ---
void setup() {
    Serial.begin(115200); delay(2000);
    setup_robot_control();
    if (LittleFS.begin()) {
        Serial.println("LittleFS mounted.");
        if (LittleFS.exists("/map.txt")) {
            Serial.println("Phat hien ban do da luu, dang tai...");
            if (load_maze_from_file("/map.txt")) Serial.println("Tai ban do thanh cong.");
            else Serial.println("Loi: Khong the tai ban do.");
        } else Serial.println("Khong tim thay ban do da luu.");
    } else Serial.println("Loi: Khong the khoi dong LittleFS.");
    Serial.println("Robot san sang. Gui lenh 'delete' de xoa map, hoac ky tu bat ky de bat dau...");
}

void loop() {
    handle_serial_commands();
    Point next_move = {-1, -1};
    std::vector<Point> path;
    switch (current_stage) {
        case WAITING_FOR_START:
            break;
        case STAGE_1_DFS_EXPLORE:
            update_walls_from_sensors();
            maze.cell(robotPosition.x, robotPosition.y).run_visited = true;
            if (is_in_goal(robotPosition)) {
                found_goal_pos = robotPosition;
                Serial.println("\n*** DA TIM THAY DICH! ***"); save_maze_to_file("/map.txt");
                Serial.println("Chuyen sang Giai doan 2: Quay ve lan 1...");
                current_stage = STAGE_2_RETURN_HOME_1; delay(2000);
                return;
            }
            {
                Cell& learned_cell = maze.cell(robotPosition.x, robotPosition.y);
                Point neighbors[4] = {{robotPosition.x, (int8_t)(robotPosition.y + 1)}, {(int8_t)(robotPosition.x + 1), robotPosition.y}, {robotPosition.x, (int8_t)(robotPosition.y - 1)}, {(int8_t)(robotPosition.x - 1), robotPosition.y}};
                bool walls[4] = {learned_cell.north_wall, learned_cell.east_wall, learned_cell.south_wall, learned_cell.west_wall};
                for(int i=0; i<4; ++i) {
                    if(!walls[i]) {
                        Point n = neighbors[i];
                        if(n.x >= 0 && n.x < MAZE_SIZE && n.y >= 0 && n.y < MAZE_SIZE && !maze.cell(n.x, n.y).run_visited) { next_move = n; break; }
                    }
                }
            }
            if (next_move.x != -1) { path_history.push_back(next_move); } 
            else { 
                if (path_history.size() <= 1) { current_stage = MISSION_COMPLETE; return; }
                path_history.pop_back();
                next_move = path_history.back();
            }
            execute_move(next_move, &robotPosition, &robotDirection);
            break;
        case STAGE_2_RETURN_HOME_1: case STAGE_4_RETURN_HOME_2: case STAGE_5_RETURN_HOME_3:
            if (robotPosition.x == start_pos.x && robotPosition.y == start_pos.y) {
                Serial.printf("\nDa quay ve diem xuat phat (Ket thuc Giai doan %d).\n", (int)current_stage);
                save_maze_to_file("/map.txt");
                current_stage = (MissionStage)((int)current_stage + 1);
                Serial.printf("Chuan bi Giai doan tiep theo...\n");
                delay(2000);
                return;
            }
            update_walls_from_sensors(); maze.cell(robotPosition.x, robotPosition.y).run_visited = true;
            path = maze.find_path_astar(robotPosition, start_pos, -5);
            if (path.size() < 2) { current_stage = MISSION_COMPLETE; return; }
            execute_move(path[1], &robotPosition, &robotDirection);
            break;
        case STAGE_3_RUN_TO_GOAL:
            if (is_in_goal(robotPosition)) {
                Serial.println("\nDa den dich. Ket thuc Giai doan 3.");
                save_maze_to_file("/map.txt");
                current_stage = STAGE_4_RETURN_HOME_2;
                Serial.println("Chuan bi Giai doan 4: Quay ve lan 2...");
                delay(2000);
                return;
            }
            update_walls_from_sensors(); maze.cell(robotPosition.x, robotPosition.y).run_visited = true;
            path = maze.find_path_astar(robotPosition, found_goal_pos, 0);
            if (path.size() < 2) { current_stage = MISSION_COMPLETE; return; }
            execute_move(path[1], &robotPosition, &robotDirection);
            break;
        case STAGE_6_CALCULATE_FINAL_PATH:
            Serial.println("\nTat ca da hoan tat. Tinh toan duong di cuoi cung...");
            final_path = maze.find_path_astar(start_pos, found_goal_pos, 50);
            if (final_path.empty()) {
                Serial.println("LOI: Khong the tinh duong di cuoi cung.");
                current_stage = MISSION_COMPLETE;
            } else {
                Serial.printf("Duong di an toan ngan nhat: %d buoc.\n", final_path.size() - 1);
                Serial.println("Chuan bi chay toc do!");
                delay(3000);
                current_stage = STAGE_7_FINAL_RUN;
            }
            break;
        case STAGE_7_FINAL_RUN:
            Serial.println("--- CHAY TOC DO ---");
            for (size_t i = 1; i < final_path.size(); ++i) {
                execute_move(final_path[i], &robotPosition, &robotDirection);
            }
            Serial.println("\n*** HOAN THANH NHIEM VU! ***");
            current_stage = MISSION_COMPLETE;
            break;
        case MISSION_COMPLETE:
            controlMotor(0, 0);
            delay(100);
            break;
    }
    delay(50);
}
