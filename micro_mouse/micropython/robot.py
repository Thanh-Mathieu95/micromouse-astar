from control import run_one_cell,turn_around,turn_left,turn_right
from ir_sensor import is_wall_ahead,is_wall_left,is_wall_right
class Mouse:
    def __init__(self):
        self.x = 0
        self.y = 0
    #   n 
    # w   e
    #   s
        self.look = 'n'

        self.sensor_ahead = 0
        self.sensor_right = 1
        self.sensor_left = 1
    
    def read_wall(self):    
        self.sensor_left = is_wall_left()
        self.sensor_ahead = is_wall_ahead()
        self.sensor_right = is_wall_right()

    def move_to_n(self):
        """ Di chuyển lên phía Bắc """
        if self.look == 'n':
            run_one_cell()
        elif self.look == 'e':
            turn_left()
        elif self.look == 'w':
            turn_right()
        elif self.look == 's':
            turn_around()
        self.look = 'n'
        self.y += 1  # Đi lên thì y tăng

    def move_to_s(self):
        """ Di chuyển xuống phía Nam """
        if self.look == 's':
            run_one_cell()
        elif self.look == 'e':
            turn_right()
        elif self.look == 'w':
            turn_left()
        elif self.look == 'n':
            turn_around()
        self.look = 's'
        self.y -= 1  # Đi xuống thì y giảm

    def move_to_e(self):
        """ Di chuyển sang phải (phía Đông) """
        if self.look == 'e':
            run_one_cell()
        elif self.look == 'n':
            turn_right()
        elif self.look == 's':
            turn_left()
        elif self.look == 'w':
            turn_around()
        self.look = 'e'
        self.x += 1  # Đi sang phải thì x tăng

    def move_to_w(self):
        """ Di chuyển sang trái (phía Tây) """
        if self.look == 'w':
            run_one_cell()
        elif self.look == 'n':
            turn_left()
        elif self.look == 's':
            turn_right()
        elif self.look == 'e':
            turn_around()
        self.look = 'w'
        self.x -= 1  # Đi sang trái thì x giảm    

#//
mouse = Mouse()
