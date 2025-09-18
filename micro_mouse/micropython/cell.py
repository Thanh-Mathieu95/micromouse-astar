class Cell:
    def __init__(self):
        self.checked = False
        self.north_wall = 0
        self.south_wall = 0
        self.west_wall = 0
        self.east_wall = 0
        self.step = 1000
        self.int_var = 10
        self.bool_var = 10

    def set_checked(self, status: bool):
        """ Set this cell whether having been previously traversed or not """
        self.checked = status

    def set_north_wall(self, status: int):
        """ Set the status of north wall of this cell as existing or not """
        self.north_wall = status

    def set_south_wall(self, status: int):
        """ Set the status of south wall of this cell as existing or not """
        self.south_wall = status

    def set_west_wall(self, status: int):
        """ Set the status of west wall of this cell as existing or not """
        self.west_wall = status

    def set_east_wall(self, status: int):
        """ Set the status of east wall of this cell as existing or not """
        self.east_wall = status

    def set_step(self, val: int):
        """ Set the steps needed to get from the origin to this cell """
        self.step = val

    def set_int_var(self, val: int):
        """ Set an additional integer variable if needed """
        self.int_var = val

    def set_bool_var(self, status: int):
        """ Set an additional boolean variable if needed """
        self.bool_var = status