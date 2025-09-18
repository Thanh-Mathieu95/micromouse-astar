from cell import Cell
from robot import mouse
class ParentMaze:
    """ Class to store mazes of different perspectives for the robot"""

    maze = [[Cell() for n in range(16)] for m in range(16)]
    # Maze has edge walls
    for i in range(16):
        maze[i][0].set_south_wall(1)
        maze[0][i].set_west_wall(1)
        maze[i][15].set_north_wall(1)
        maze[15][i].set_east_wall(1)
    maze[0][0].set_east_wall(1)
    maze[1][0].set_west_wall(1)

    unvisited_stack = []

    #   n 
    # w   e
    #   s
    
    @classmethod
    def cell(cls, x: int, y: int):
        """Return the cell of robot's virtual maze at the given coordinate"""
        return cls.maze[x][y]

    @classmethod
    def available_neighbor(cls, x: int, y: int):
        """ For use with creating the actual maze
        Return whether there is an available neighbor """
        if y < 15 and not cls.maze[x][y+1].checked:
            cls.unvisited_stack.append([x, y+1])
            return True
        if y > 0 and not cls.maze[x][y-1].checked:
            cls.unvisited_stack.append([x, y-1])
            return True
        if x < 15 and not cls.maze[x+1][y].checked:
            cls.unvisited_stack.append([x+1, y])
            return True
        if x > 0 and not cls.maze[x-1][y].checked:
            cls.unvisited_stack.append([x-1, y])
            return True
        return False

    @classmethod
    def clear_mem(cls):
        """ Reset the mouse's memory of the virtual maze """
        for i in range(16):
            for j in range(16):
                cls.maze[i][j].set_north_wall(0)
                cls.maze[i][j].set_south_wall(0)
                cls.maze[i][j].set_west_wall(0)
                cls.maze[i][j].set_east_wall(0)
                cls.maze[i][j].set_checked(0)
                cls.maze[i][j].step = 256
        for i in range(16):
            cls.maze[i][0].set_south_wall(1)
            cls.maze[0][i].set_west_wall(1)
            cls.maze[i][15].set_north_wall(1)
            cls.maze[15][i].set_east_wall(1)
        cls.maze[0][0].set_east_wall(1)
        cls.maze[1][0].set_west_wall(1)
    @classmethod
    def print(cls):
        """ Print out robot's virtual maze in its current state to the console
            (This function overrides the print function from Mazemod, not always
            necessary for every algorithm) """
        step_width = len(str(cls.maze[0][0].step))
        print("___________________________________________________________\n")
        print("+", end="")
        for j in range(16):
            print("-" * (step_width + 2) + "+", end="")
        print()
        for j in reversed(range(16)):
            print("|", end="")
            for i in range(16):
                total_space = step_width - len(str(cls.maze[i][j].step)) + 2
                front = int(total_space/2)
                back = total_space - front
                
                if i == mouse.x and j== mouse.y :
                    print(" " * front + "xx"+ " " * back, end="")
                else:
                    print(" " * front + str(cls.maze[i][j].step)+ " " * back, end="")
                    
                if cls.maze[i][j].east_wall == 1:
                    print("|", end="")
                else:
                    print(" ", end="")
            print()
            print("+", end="")
            for i in range(16):
                if cls.maze[i][j].south_wall == 1:
                    print("-" * (step_width + 2) + "+", end="")
                else:
                    print(" " * (step_width + 2) + "+", end="")
            print()
map = ParentMaze()