# Micro Mouse - Maze Solving Robot

A Micromouse maze-solving robot built on **Raspberry Pi Pico**, implementing a multi-stage **A\* pathfinding algorithm** to navigate a 16×16 maze from start position (0,0) to the center goals. Includes both real hardware firmware and an interactive desktop simulation.

---

## Features

- **A\* pathfinding** with realistic turn cost modeling (90°, 180°, consecutive turns)
- **3-stage exploration strategy**: explore → return → fast run
- **4× Sharp IR sensors** for real-time wall detection
- **Dual DC motors** with PID-controlled encoders
- **Desktop simulation** for algorithm testing without hardware
- **Memory-efficient** implementation — no STL, static arrays only (MCU-safe)
- FreeRTOS-compatible interrupt-safe design

---

## Algorithm

The robot operates in three stages:

| Stage | Description |
|-------|-------------|
| **Stage 1 — Explore to Center** | A\* with bias toward unvisited cells; runs until any center goal is reached |
| **Stage 2 — Explore Return** | Returns to start while mapping remaining unvisited cells on the main corridor |
| **Stage 3 — Fast Run** | Executes the optimal known path at full speed (`forbid_unvisited = true`) |

**Pathfinding details:**
- Binary min-heap open set (no `std::priority_queue`)
- Manhattan distance heuristic
- Turn cost: 90° = 40, 180° = 200, consecutive turns = +50
- U-turns only allowed when forward path is blocked
- Max iterations capped to prevent MCU lockup

---

## Hardware

| Component | Details |
|-----------|---------|
| MCU | Raspberry Pi Pico |
| Motors | 2× DC motors with gear reduction |
| Encoders | Quadrature rotary encoders (1250 counts/cell) |
| IR Sensors | 4× Sharp IR (left, front-left, front-right, right) |
| Power | Min. **5V** input required |

### Pin Assignments

| Function | Pin |
|----------|-----|
| Motor A PWM / Direction | 6, AIN1, AIN2 |
| Motor B PWM / Direction | 0, BIN1, BIN2 |
| Encoder Left | 18, 19 |
| Encoder Right | 22, 23 |
| IR Sensors (ADC) | 26, 27, 28, 29 |
| IR LED | 9 |
| Buttons | 24, 25 |
| Buzzer | 7 |
| LED Indicator | 8 |
| I2C (SDA/SCL) | 20, 21 |

---

## Project Structure

```
micro_mouse_floodfill_14-3/
├── src/
│   └── main.cpp              # Hardware firmware entry point (Pico)
├── simulation/
│   ├── main_sim.cpp          # Interactive desktop simulation
│   └── old_sim.cpp
├── lib/
│   ├── maze/                 # Core A* pathfinding & maze data structures
│   ├── encoder_motor/        # PID motor control & movement primitives
│   ├── ir_sensor/            # IR wall detection
│   ├── Pin_and_init/         # Pin definitions, direction enum, initialization
│   └── map_storage/          # Maze persistence
├── map/
│   ├── custom_maze.txt       # Default maze layout
│   └── custom_maze_uet*.txt  # UET competition maze variants
├── build/                    # Compiled simulation binary
└── platformio.ini            # PlatformIO build config
```

---

## Getting Started

### Desktop Simulation

Requires a C++11 compiler (GCC/Clang/MSVC).

```bash
# Compile
g++ -std=c++11 -I. simulation/main_sim.cpp lib/maze/maze.cpp -o build/main_sim

# Run
./build/main_sim
```

The simulation menu lets you:
1. Input walls manually
2. Run full A\* exploration
3. Visualize the maze and path in the terminal
4. Generate the move sequence

### Hardware Deployment (PlatformIO)

1. Install [VS Code](https://code.visualstudio.com/) + [PlatformIO extension](https://platformio.org/)
2. Open this project folder
3. Calibrate IR thresholds in [lib/Pin_and_init/variables.h](lib/Pin_and_init/variables.h) if needed
4. **Build & Upload**: `PlatformIO: Build` → `PlatformIO: Upload`
5. Open Serial Monitor at **115200 baud**
6. Press the button (or send any character) to start the robot

> **Note:** The robot waits 3 seconds on startup before beginning exploration.

---

## Maze Format

Mazes are stored as ASCII text files:

```
+---+---+---+
|   |       |
+   +---+   +
|       |   |
+---+---+---+
```

- `+` = cell corners
- `-` = horizontal walls
- `|` = vertical walls
- Spaces = open passages

The goal is the 2×2 center region: cells **(7,7), (7,8), (8,7), (8,8)** in a 16×16 grid.

---

## Configuration

Key tunable parameters in [lib/Pin_and_init/variables.h](lib/Pin_and_init/variables.h):

| Parameter | Default | Description |
|-----------|---------|-------------|
| `IR_THRESHOLD` | ~100 | ADC level to detect a wall |
| `CELL_ENCODER_COUNT` | 1250 | Encoder counts per cell |
| `MOTOR_DEADZONE` | 20 | Minimum PWM to overcome stiction |
| `SPEED_SLOW/MED/FAST` | 50/60/60+ | PWM speed levels |

---

## Dependencies

- [PlatformIO](https://platformio.org/) — build system for embedded
- [FreeRTOS](https://freertos.org/) — RTOS on Pico (via PlatformIO framework)
- C++11 standard — simulation only

---

## License

This project is open-source. Feel free to use and adapt for your own Micromouse build.
