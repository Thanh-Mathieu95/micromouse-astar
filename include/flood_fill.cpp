#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iomanip>
using namespace std;

// Forward declarations
class Cell;
class Mouse;
class ParentMaze;

// Constants
const int MAZE_SIZE = 16;
const int MAX_DISTANCE = 1000;

// Global variables
ParentMaze* map;
Mouse* mouse;
vector<string> long_path;
bool time_flag = false;
bool require_flooding = false;

// Cell class definition
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
    void set_east_wall(bool wall) { east_wall = wall; }
    void set_west_wall(bool wall) { west_wall = wall; }
    void set_checked(bool check) { checked = check; }
    void set_step(int s) { step = s; }
};

// Mouse class definition
class Mouse {
public:
    int x, y;
    char look;
    bool sensor_ahead;
    bool sensor_left;
    bool sensor_right;

    Mouse() : x(0), y(0), look('n'), sensor_ahead(false), 
              sensor_left(false), sensor_right(false) {}

    void read_wall() {
        // Implementation depends on hardware sensors
        // Placeholder for actual sensor reading
    }

    void move_to_n() { 
        y++; 
        look = 'n';
    }
    
    void move_to_s() { 
        y--; 
        look = 's';
    }
    
    void move_to_e() { 
        x++; 
        look = 'e';
    }
    
    void move_to_w() { 
        x--; 
        look = 'w';
    }
};

// ParentMaze class definition
class ParentMaze {
public:
    Cell maze[MAZE_SIZE][MAZE_SIZE];
    vector<vector<int>> unvisited_stack;

    ParentMaze() {
        // Initialize maze with edge walls
        for (int i = 0; i < MAZE_SIZE; i++) {
            maze[i][0].set_south_wall(true);
            maze[0][i].set_west_wall(true);
            maze[i][MAZE_SIZE-1].set_north_wall(true);
            maze[MAZE_SIZE-1][i].set_east_wall(true);
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
            maze[i][MAZE_SIZE-1].set_north_wall(true);
            maze[MAZE_SIZE-1][i].set_east_wall(true);
        }
        maze[0][0].set_east_wall(true);
        maze[1][0].set_west_wall(true);
    }

    void print() {
        int step_width = 3; // Assuming step values up to 3 digits
        cout << "___________________________________________________________\n" << endl;
        cout << "+";
        for (int j = 0; j < MAZE_SIZE; j++) {
            cout << string(step_width + 2, '-') << "+";
        }
        cout << endl;
        
        for (int j = MAZE_SIZE - 1; j >= 0; j--) {
            cout << "|";
            for (int i = 0; i < MAZE_SIZE; i++) {
                int total_space = step_width + 2;
                string step_str = to_string(maze[i][j].step);
                int front = (total_space - step_str.length()) / 2;
                int back = total_space - step_str.length() - front;
                
                if (i == mouse->x && j == mouse->y) {
                    cout << string(front, ' ') << "xx" << string(back, ' ');
                } else {
                    cout << string(front, ' ') << step_str << string(back, ' ');
                }
                
                if (maze[i][j].east_wall) {
                    cout << "|";
                } else {
                    cout << " ";
                }
            }
            cout << endl << "+";
            
            for (int i = 0; i < MAZE_SIZE; i++) {
                if (maze[i][j].south_wall) {
                    cout << string(step_width + 2, '-') << "+";
                } else {
                    cout << string(step_width + 2, ' ') << "+";
                }
            }
            cout << endl;
        }
    }
};

// Function declarations
void read_wall();
void advance();
void modified_flood();
void initial_flood();
void init_back();
bool at_center();
void reset();
void creat();
void complete();
void upfile(int i, const vector<string>& commands);
void normalize_list();
string run_algorithm();

// Function implementations
Cell& cell(int x, int y) {
    return map->cell(x, y);
}

void maze_print() {
    map->print();
}

pair<int, int> cur_pos() {
    return make_pair(mouse->x, mouse->y);
}

void read_wall() {
    bool curr_north_wall = false;
    bool curr_south_wall = false;
    bool curr_west_wall = false;
    bool curr_east_wall = false;
    
    mouse->read_wall();

    if (mouse->look == 'n') {
        curr_north_wall = mouse->sensor_ahead;
        curr_west_wall = mouse->sensor_left;
        curr_east_wall = mouse->sensor_right;
    } else if (mouse->look == 'w') {
        curr_north_wall = mouse->sensor_right;
        curr_south_wall = mouse->sensor_left;
        curr_west_wall = mouse->sensor_ahead;
    } else if (mouse->look == 'e') {
        curr_north_wall = mouse->sensor_left;
        curr_south_wall = mouse->sensor_right;
        curr_east_wall = mouse->sensor_ahead;
    } else if (mouse->look == 's') {
        curr_south_wall = mouse->sensor_ahead;
        curr_west_wall = mouse->sensor_right;
        curr_east_wall = mouse->sensor_left;
    }

    require_flooding = false;

    if (cell(mouse->x, mouse->y).north_wall != curr_north_wall) {
        require_flooding = true;
        cell(mouse->x, mouse->y).set_north_wall(curr_north_wall);
        if (mouse->y < MAZE_SIZE - 1) {
            cell(mouse->x, mouse->y + 1).set_south_wall(curr_north_wall);
        }
    }

    if (cell(mouse->x, mouse->y).south_wall != curr_south_wall) {
        require_flooding = true;
        cell(mouse->x, mouse->y).set_south_wall(curr_south_wall);
        if (mouse->y > 0) {
            cell(mouse->x, mouse->y - 1).set_north_wall(curr_south_wall);
        }
    }

    if (cell(mouse->x, mouse->y).west_wall != curr_west_wall) {
        require_flooding = true;
        cell(mouse->x, mouse->y).set_west_wall(curr_west_wall);
        if (mouse->x > 0) {
            cell(mouse->x - 1, mouse->y).set_east_wall(curr_west_wall);
        }
    }

    if (cell(mouse->x, mouse->y).east_wall != curr_east_wall) {
        require_flooding = true;
        cell(mouse->x, mouse->y).set_east_wall(curr_east_wall);
        if (mouse->x < MAZE_SIZE - 1) {
            cell(mouse->x + 1, mouse->y).set_west_wall(curr_east_wall);
        }
    }
}

void advance() {
    int shortest = MAX_DISTANCE;
    string path = "";
    
    if (mouse->y < MAZE_SIZE - 1 && !cell(mouse->x, mouse->y).north_wall && 
        cell(mouse->x, mouse->y + 1).step < shortest) {
        shortest = cell(mouse->x, mouse->y + 1).step;
        path = "n";
    }
    
    if (mouse->x > 0 && !cell(mouse->x, mouse->y).west_wall && 
        cell(mouse->x - 1, mouse->y).step < shortest) {
        shortest = cell(mouse->x - 1, mouse->y).step;
        path = "w";
    }
    
    if (mouse->x < MAZE_SIZE - 1 && !cell(mouse->x, mouse->y).east_wall && 
        cell(mouse->x + 1, mouse->y).step < shortest) {
        shortest = cell(mouse->x + 1, mouse->y).step;
        path = "e";
    }
    
    if (mouse->y > 0 && !cell(mouse->x, mouse->y).south_wall && 
        cell(mouse->x, mouse->y - 1).step < shortest) {
        shortest = cell(mouse->x, mouse->y - 1).step;
        path = "s";
    }
    
    if (path == "n") {
        mouse->move_to_n();
    } else if (path == "s") {
        mouse->move_to_s();
    } else if (path == "w") {
        mouse->move_to_w();
    } else {
        mouse->move_to_e();
    }
    
    cell(mouse->x, mouse->y).set_checked(true);
    long_path.push_back(path);
}

void modified_flood() {
    vector<vector<int>> flood_list = {{mouse->x, mouse->y}};
    
    while (!flood_list.empty()) {
        int i = flood_list.back()[0];
        int j = flood_list.back()[1];
        flood_list.pop_back();
        
        int min_distance = MAX_DISTANCE;
        
        if (!cell(i, j).north_wall && cell(i, j + 1).step < min_distance) {
            min_distance = cell(i, j + 1).step;
        }
        
        if (!cell(i, j).south_wall && cell(i, j - 1).step < min_distance) {
            min_distance = cell(i, j - 1).step;
        }
        
        if (!cell(i, j).east_wall && cell(i + 1, j).step < min_distance) {
            min_distance = cell(i + 1, j).step;
        }
        
        if (!cell(i, j).west_wall && cell(i - 1, j).step < min_distance) {
            min_distance = cell(i - 1, j).step;
        }
        
        if (cell(i, j).step != min_distance + 1) {
            cell(i, j).set_step(min_distance + 1);
            
            if (j < MAZE_SIZE - 1 && cell(i, j + 1).step != 0 && 
                find(flood_list.begin(), flood_list.end(), vector<int>{i, j + 1}) == flood_list.end()) {
                flood_list.push_back({i, j + 1});
            }
            
            if (j > 0 && cell(i, j - 1).step != 0 && 
                find(flood_list.begin(), flood_list.end(), vector<int>{i, j - 1}) == flood_list.end()) {
                flood_list.push_back({i, j - 1});
            }
            
            if (i < MAZE_SIZE - 1 && cell(i + 1, j).step != 0 && 
                find(flood_list.begin(), flood_list.end(), vector<int>{i + 1, j}) == flood_list.end()) {
                flood_list.push_back({i + 1, j});
            }
            
            if (i > 0 && cell(i - 1, j).step != 0 && 
                find(flood_list.begin(), flood_list.end(), vector<int>{i - 1, j}) == flood_list.end()) {
                flood_list.push_back({i - 1, j});
            }
        }
    }
}

void initial_flood() {
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            if (i < 8 && j < 8) {
                cell(i, j).set_step(14 - i - j);
            } else if (i > 7 && j < 8) {
                cell(i, j).set_step(i - j - 1);
            } else if (i < 8 && j > 7) {
                cell(i, j).set_step(j - i - 1);
            } else {
                cell(i, j).set_step(i + j - 16);
            }
        }
    }
}

void init_back() {
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            cell(i, j).set_step(i + j);
        }
    }
    cell(7, 8).set_step(2000);
    cell(7, 7).set_step(2000);
    cell(8, 7).set_step(2000);
    cell(8, 8).set_step(2000);
}

bool at_center() {
    return cell(mouse->x, mouse->y).step == 0;
}

void reset() {
    map->clear_mem();
    mouse->x = 0;
    mouse->y = 0;
    require_flooding = false;
    cell(0, 0).set_checked(true);
    initial_flood();
    long_path.clear();
    time_flag = false;
}

void creat() {
    int i = 0;
    while (i < static_cast<int>(long_path.size()) - 1) {
        if ((long_path[i] == "n" && long_path[i+1] == "s") ||
            (long_path[i] == "s" && long_path[i+1] == "n") ||
            (long_path[i] == "w" && long_path[i+1] == "e") ||
            (long_path[i] == "e" && long_path[i+1] == "w")) {
            long_path.erase(long_path.begin() + i, long_path.begin() + i + 2);
            if (i > 0) {
                i--;
            }
        } else {
            i++;
        }
    }
}

void complete() {
    for (auto& direction : long_path) {
        if (direction == "n") direction = "s";
        else if (direction == "s") direction = "n";
        else if (direction == "w") direction = "e";
        else if (direction == "e") direction = "w";
    }
    reverse(long_path.begin(), long_path.end());
}

void upfile(int i, const vector<string>& commands) {
    ofstream file("run_" + to_string(i) + ".txt");
    for (const auto& cmd : commands) {
        file << cmd << endl;
    }
    file.close();
}

void normalize_list() {
    int i = 0;
    while (i < static_cast<int>(long_path.size()) - 2) {
        if (long_path[i] == "n" && long_path[i+1] == "n" && long_path[i+2] == "n") {
            int count_n = 0;
            int start_pos = i;
            
            while (i < static_cast<int>(long_path.size()) && long_path[i] == "n") {
                count_n++;
                i++;
            }
            
            long_path.erase(long_path.begin() + start_pos, long_path.begin() + start_pos + count_n);
            long_path.insert(long_path.begin() + start_pos, "n");
            long_path.insert(long_path.begin() + start_pos + 1, to_string(count_n - 1));
            
            i = start_pos + 2;
        } else if (long_path[i] == "s" && long_path[i+1] == "s" && long_path[i+2] == "s") {
            int count_n = 0;
            int start_pos = i;
            
            while (i < static_cast<int>(long_path.size()) && long_path[i] == "s") {
                count_n++;
                i++;
            }
            
            long_path.erase(long_path.begin() + start_pos, long_path.begin() + start_pos + count_n);
            long_path.insert(long_path.begin() + start_pos, "s");
            long_path.insert(long_path.begin() + start_pos + 1, to_string(count_n - 1));
            
            i = start_pos + 2;
        } else if (long_path[i] == "e" && long_path[i+1] == "e" && long_path[i+2] == "e") {
            int count_n = 0;
            int start_pos = i;
            
            while (i < static_cast<int>(long_path.size()) && long_path[i] == "e") {
                count_n++;
                i++;
            }
            
            long_path.erase(long_path.begin() + start_pos, long_path.begin() + start_pos + count_n);
            long_path.insert(long_path.begin() + start_pos, "e");
            long_path.insert(long_path.begin() + start_pos + 1, to_string(count_n - 1));
            
            i = start_pos + 2;
        } else if (long_path[i] == "w" && long_path[i+1] == "w" && long_path[i+2] == "w") {
            int count_n = 0;
            int start_pos = i;
            
            while (i < static_cast<int>(long_path.size()) && long_path[i] == "w") {
                count_n++;
                i++;
            }
            
            long_path.erase(long_path.begin() + start_pos, long_path.begin() + start_pos + count_n);
            long_path.insert(long_path.begin() + start_pos, "w");
            long_path.insert(long_path.begin() + start_pos + 1, to_string(count_n - 1));
            
            i = start_pos + 2;
        } else {
            i++;
        }
    }
}

string run_algorithm() {
    advance();
    
    if (at_center()) {
        if (time_flag) {
            complete();
            normalize_list();
            upfile(2, long_path);
            return "done";
        }
        
        creat();
        normalize_list();
        upfile(1, long_path);
        init_back();
        
        long_path.clear();
        time_flag = true;
        
        if (mouse->look == 'n') {
            mouse->move_to_n();
            mouse->move_to_s();
        } else if (mouse->look == 's') {
            mouse->move_to_s();
            mouse->move_to_n();
        } else if (mouse->look == 'w') {
            mouse->move_to_w();
            mouse->move_to_e();
        } else if (mouse->look == 'e') {
            mouse->move_to_e();
            mouse->move_to_w();
        }
    }
    
    read_wall();
    
    if (require_flooding) {
        modified_flood();
    }
    
    return "not done";
}

// Main function
int main() {
    // Initialize global objects
    map = new ParentMaze();
    mouse = new Mouse();
    
    // Reset and run the algorithm
    reset();
    
    string status;
    do {
        status = run_algorithm();
    } while (status != "done");
    
    // Clean up
    delete map;
    delete mouse;
    
    return 0;
}