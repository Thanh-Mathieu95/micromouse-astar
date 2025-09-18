from N20 import round_back , turn_to_right, turn_to_left, go_one_cell,stop
from ir_sensor import read_all
#def control_whell():
    #print(A_speed,"  ",B_speed)

def run_one_cell():
    go_one_cell()
    stop()
    read_all()

def turn_right():
    turn_to_right()
    stop()
    read_all()

def turn_left():
    turn_to_left()
    stop()
    read_all()

def turn_around():
    round_back()
    go_one_cell()
    stop()
    read_all()
