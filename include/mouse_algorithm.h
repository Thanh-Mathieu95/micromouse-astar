#pragma once
#include <Arduino.h>
#include <vector>
#include <maze.h>
using namespace std;
enum Direction {
    North = 0,
    East  = 1,
    South = 2,
    West  = 3
};
class Mouse {
    public:
    Map maze;
    vector<Cell> path;
    vector<vector<bool>> visited;

    bool is_goal(Cell isgoal){
        // +---+---+---+
        // |           |
        // +   +   +   +
        // |           |
        // +   +   +   +
        // |           |
        // +---+   +---+
        //              N (Bắc)
        //                 ↑
        //       W (Tây) ← ┼ → E (Đông)
        //                 ↓
        //              S (Nam)

        if ( maze.getCell(isgoal.x - 1, isgoal.y     ).checked && 
             maze.getCell(isgoal.x - 1, isgoal.y + 1 ).checked &&
             maze.getCell(isgoal.x    , isgoal.y + 1 ).checked )
            {
                if (( ! maze.getCell(isgoal.x - 1, isgoal.y     ).north_wall &&
                      ! maze.getCell(isgoal.x - 1, isgoal.y     ).east_wall)    
                    &&
                    ( ! maze.getCell(isgoal.x - 1, isgoal.y + 1 ).east_wall && 
                      ! maze.getCell(isgoal.x - 1, isgoal.y + 1 ).south_wall)
                    &&
                    ( ! maze.getCell(isgoal.x    , isgoal.y + 1 ).west_wall &&
                      ! maze.getCell(isgoal.x    , isgoal.y + 1 ).south_wall)
                    &&
                    ( ! maze.getCell(isgoal.x    , isgoal.y     ).north_wall &&
                      ! maze.getCell(isgoal.x    , isgoal.y     ).west_wall)){      return true;    }   }

        if ( maze.getCell(isgoal.x    , isgoal.y + 1 ).checked && 
             maze.getCell(isgoal.x + 1, isgoal.y + 1 ).checked &&
             maze.getCell(isgoal.x + 1 , isgoal.y    ).checked )
            {
                if (( ! maze.getCell(isgoal.x    , isgoal.y + 1 ).east_wall &&
                      ! maze.getCell(isgoal.x    , isgoal.y + 1 ).south_wall)    
                    &&
                    ( ! maze.getCell(isgoal.x + 1, isgoal.y + 1 ).west_wall && 
                      ! maze.getCell(isgoal.x + 1, isgoal.y + 1 ).south_wall)
                    &&
                    ( ! maze.getCell(isgoal.x + 1 , isgoal.y    ).north_wall &&
                      ! maze.getCell(isgoal.x + 1 , isgoal.y    ).west_wall)
                    &&
                    ( ! maze.getCell(isgoal.x    , isgoal.y     ).north_wall &&
                      ! maze.getCell(isgoal.x    , isgoal.y     ).east_wall)){      return true;    }   }

        if ( maze.getCell(isgoal.x    , isgoal.y - 1 ).checked && 
             maze.getCell(isgoal.x + 1, isgoal.y - 1 ).checked &&
             maze.getCell(isgoal.x + 1, isgoal.y     ).checked )
            {
                if (( ! maze.getCell(isgoal.x    , isgoal.y - 1 ).south_wall &&
                      ! maze.getCell(isgoal.x    , isgoal.y - 1 ).east_wall)    
                    &&
                    ( ! maze.getCell(isgoal.x + 1, isgoal.y - 1 ).west_wall && 
                      ! maze.getCell(isgoal.x + 1, isgoal.y - 1 ).north_wall)
                    &&
                    ( ! maze.getCell(isgoal.x + 1, isgoal.y     ).south_wall &&
                      ! maze.getCell(isgoal.x + 1, isgoal.y     ).west_wall)
                    &&
                    ( ! maze.getCell(isgoal.x    , isgoal.y     ).south_wall &&
                      ! maze.getCell(isgoal.x    , isgoal.y     ).east_wall)){      return true;    }   }

        if ( maze.getCell(isgoal.x - 1, isgoal.y     ).checked && 
             maze.getCell(isgoal.x - 1, isgoal.y - 1 ).checked &&
             maze.getCell(isgoal.x    , isgoal.y - 1 ).checked )
            {
                if (( ! maze.getCell(isgoal.x - 1, isgoal.y     ).east_wall &&
                      ! maze.getCell(isgoal.x - 1, isgoal.y     ).south_wall)    
                    &&
                    ( ! maze.getCell(isgoal.x - 1, isgoal.y - 1 ).east_wall && 
                      ! maze.getCell(isgoal.x - 1, isgoal.y - 1 ).north_wall)
                    &&
                    ( ! maze.getCell(isgoal.x    , isgoal.y - 1 ).north_wall &&
                      ! maze.getCell(isgoal.x    , isgoal.y - 1 ).west_wall)
                    &&
                    ( ! maze.getCell(isgoal.x    , isgoal.y     ).west_wall &&
                      ! maze.getCell(isgoal.x    , isgoal.y     ).south_wall)){      return true;    }   }
    return false;
    }
};