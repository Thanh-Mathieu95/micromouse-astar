from maze import map
from robot import mouse
from funs import beep
import file
def cell(x, y):
    #""" For easy access """
    return map.cell(x, y)

def maze_print():
    #""" For debugging """
    map.print()


def cur_pos():
    #""" Return the current position of the mouse """
    return mouse.x, mouse.y



require_flooding = False  
curr_north_wall = 0
curr_south_wall = 1
curr_west_wall = 1
curr_east_wall = 1



def read_wall():
    
    global curr_north_wall, curr_south_wall, curr_east_wall, curr_west_wall,require_flooding
    
    mouse.read_wall()

    if mouse.look == 'n':
        curr_north_wall = mouse.sensor_ahead
        #curr_south_wall = 0
        curr_west_wall = mouse.sensor_left
        curr_east_wall = mouse.sensor_right

    if mouse.look == 'w':
        curr_north_wall = mouse.sensor_right
        curr_south_wall = mouse.sensor_left
        curr_west_wall = mouse.sensor_ahead
        #curr_east_wall = 0

    if mouse.look == 'e':
        curr_north_wall = mouse.sensor_left
        curr_south_wall = mouse.sensor_right
        #curr_west_wall = 0
        curr_east_wall = mouse.sensor_ahead
    
    if mouse.look == 's':
        #curr_north_wall =  0
        curr_south_wall =  mouse.sensor_ahead
        curr_west_wall = mouse.sensor_right
        curr_east_wall = mouse.sensor_left

    require_flooding = False

    if cell(mouse.x, mouse.y).north_wall != curr_north_wall:
        require_flooding = True
        cell(mouse.x, mouse.y).set_north_wall(curr_north_wall)
        if mouse.y < 15:
            cell(mouse.x, mouse.y+1).set_south_wall(curr_north_wall)

    if cell(mouse.x, mouse.y).south_wall != curr_south_wall:
        require_flooding = True
        cell(mouse.x, mouse.y).set_south_wall(curr_south_wall)
        if mouse.y > 0:
            cell(mouse.x, mouse.y-1).set_north_wall(curr_south_wall)

    if cell(mouse.x, mouse.y).west_wall != curr_west_wall:
        require_flooding = True
        cell(mouse.x, mouse.y).set_west_wall(curr_west_wall)
        if mouse.x > 0:
            cell(mouse.x-1, mouse.y).set_east_wall(curr_west_wall)

    if cell(mouse.x, mouse.y).east_wall != curr_east_wall:
        require_flooding = True
        cell(mouse.x, mouse.y).set_east_wall(curr_east_wall)
        if mouse.x < 15:
            cell(mouse.x+1, mouse.y).set_west_wall(curr_east_wall)
            
    #if require_flooding:
        #modified_flood()  # Chạy thuật toán Flood Fill cập nhật các ô liên quan
long_path=[]
def advance():
    #""" Move to the next best path """
    #   n 
    # w   e
    #   s
    global curr_north_wall, curr_south_wall, curr_east_wall, curr_west_wall,require_flooding


    shortest = 1000
    path = ""
    if mouse.y < 15 and curr_north_wall == 0  and cell(mouse.x, mouse.y+1).step < shortest:
        shortest = cell(mouse.x, mouse.y+1).step
        path = "n"
        
    if mouse.x > 0 and curr_west_wall == 0 and cell(mouse.x-1, mouse.y).step < shortest:
        shortest = cell(mouse.x-1, mouse.y).step
        path = "w"
        
    if mouse.x < 15 and curr_east_wall == 0 and cell(mouse.x+1, mouse.y).step < shortest:
        shortest = cell(mouse.x+1, mouse.y).step
        path = "e"
        
        
    if mouse.y > 0 and curr_south_wall == 0 and cell(mouse.x, mouse.y-1).step < shortest:
        shortest = cell(mouse.x, mouse.y-1).step
        path = "s"
    
    if path == "n":  # advance north
        curr_north_wall = 0
        mouse.move_to_n()
        curr_south_wall = 0
        
        
    elif path == "s":  # advance south
        curr_south_wall =0
        mouse.move_to_s()
        curr_north_wall=0
        
    elif path == "w":  # advance west
        curr_west_wall=0
        mouse.move_to_w()
        curr_east_wall =0
        
    else:  # advance east
        curr_east_wall = 0
        mouse.move_to_e()
        curr_west_wall=0

    cell(mouse.x, mouse.y).set_checked(True)
    long_path.append(path)
    
def modified_flood():
    #""" Specifically flood only relevant cells and not from center """
    flood_list = [[mouse.x, mouse.y]]
    while len(flood_list) != 0:
        i = flood_list[-1][0]
        j = flood_list[-1][1]
        flood_list.pop(-1)
        min_distance = 1000
        if not cell(i, j).north_wall and cell(i, j+1).step < min_distance:
            min_distance = cell(i, j+1).step

        if not cell(i, j).south_wall and cell(i, j-1).step < min_distance:
            min_distance = cell(i, j-1).step

        if not cell(i, j).east_wall and cell(i+1, j).step < min_distance:
            min_distance = cell(i+1, j).step

        if not cell(i, j).west_wall and cell(i-1, j).step < min_distance:
            min_distance = cell(i-1, j).step

        if cell(i, j).step != min_distance + 1:
            cell(i, j).set_step(min_distance + 1)
            if j < 15 and cell(i, j+1).step != 0 and [i, j+1] not in flood_list:
                flood_list.append([i, j+1])
            if j > 0 and cell(i, j-1).step != 0 and [i, j-1] not in flood_list:
                flood_list.append([i, j-1])
            if i < 15 and cell(i+1, j).step != 0 and [i+1, j] not in flood_list:
                flood_list.append([i+1, j])
            if i > 0 and cell(i-1, j).step != 0 and [i-1, j] not in flood_list:
                flood_list.append([i-1, j])





def initial_flood():
    #""" Initially flood the maze with distances from center """
    for i in range(16):
        for j in range(16):
            if i < 8 and j < 8:
                cell(i, j).set_step(14 - i - j)  # 7 - x + 7 - y
            elif i > 7 and j < 8:
                cell(i, j).set_step(i - j - 1)  # x - 8 + 7 - y
            elif i < 8 and j > 7:
                cell(i, j).set_step(j - i - 1)  # 7 - x + y - 8
            else:
                cell(i, j).set_step(i + j - 16)  # x - 8 + y - 8

def init_back():
    for i in range(16):
        for j in range(16):
            cell(i, j).set_step(i + j)
    cell(7, 8).set_step( 2000 )
    cell(7, 7).set_step( 2000 )
    cell(8, 7).set_step( 2000 )
    cell(8, 8).set_step( 2000 )


def at_center():
    #""" Determine if the robot has reached the center cells """
    return cell(mouse.x, mouse.y).step == 0


def reset():
    #""" Reset the memory before traversing the maze"""
    global x, y, curr_north_wall, curr_south_wall, curr_west_wall,\
        curr_east_wall, require_flooding
    map.clear_mem()
    mouse.x = 0
    mouse.y = 0
    curr_north_wall = 0
    curr_south_wall = 1
    curr_west_wall = 1
    curr_east_wall = 1
    require_flooding = False
    cell(0, 0).set_checked(True)
    initial_flood()


def creat():
    global long_path
    i = 0
    # Duyệt đến phần tử áp chót để có thể so sánh với phần tử tiếp theo
    while i < len(long_path) - 1:
        # Kiểm tra các cặp đối nghịch
        if (long_path[i] == 'n' and long_path[i+1] == 's') or \
           (long_path[i] == 's' and long_path[i+1] == 'n') or \
           (long_path[i] == 'w' and long_path[i+1] == 'e') or \
           (long_path[i] == 'e' and long_path[i+1] == 'w'):
            # Xóa 2 phần tử cùng lúc
            del long_path[i:i+2]
            # Sau khi xóa, nếu có phần tử phía trước, quay lại 1 bước để kiểm tra lại cặp mới
            if i > 0:
                i -= 1
        else:
            i += 1

def compl():
    global long_path
    for i in range(len(long_path)):
        if long_path[i] == 'n':
            long_path[i] = 's'
        elif long_path[i] == 's':
            long_path[i] = 'n'
        elif long_path[i] == 'w':
            long_path[i] = 'e'
        elif long_path[i] == 'e':
            long_path[i] = 'w'
    
    # Đảo ngược mảng
    long_path.reverse()

time = False

best_comand = []
def upfile(i,comand):
    for com in comand:
        file.create_file(f"run_{i}.txt", com)

def normalize_list():
    global long_path  # list
    i = 0
    
    # Duyệt qua danh sách
    while i < len(long_path) - 2:
        # Kiểm tra chuỗi 3 phần tử "nnn"
        if long_path[i] == 'n' and long_path[i+1] == 'n' and long_path[i+2] == 'n':
            # Đếm số phần tử 'n' liên tiếp bắt đầu từ vị trí i
            count_n = 0
            start_pos = i
            
            while i < len(long_path) and long_path[i] == 'n':
                count_n += 1
                i += 1
            
            # Xóa tất cả các phần tử 'n' liên tiếp
            for _ in range(count_n):
                long_path.pop(start_pos)
                
            # Thêm 'n' và số lượng vào danh sách
            long_path.insert(start_pos, 'n')
            long_path.insert(start_pos + 1, count_n-1)
            
            # Cập nhật vị trí i
            i = start_pos + 2
        elif long_path[i] == 's' and long_path[i+1] == 's' and long_path[i+2] == 's':
            # Đếm số phần tử 'n' liên tiếp bắt đầu từ vị trí i
            count_n = 0
            start_pos = i
            
            while i < len(long_path) and long_path[i] == 's':
                count_n += 1
                i += 1
            
            # Xóa tất cả các phần tử 'n' liên tiếp
            for _ in range(count_n):
                long_path.pop(start_pos)
                
            # Thêm 'n' và số lượng vào danh sách
            long_path.insert(start_pos, 's')
            long_path.insert(start_pos + 1, count_n-1)
            
            # Cập nhật vị trí i
            i = start_pos + 2
        elif long_path[i] == 'e' and long_path[i+1] == 'e' and long_path[i+2] == 'e':
            # Đếm số phần tử 'n' liên tiếp bắt đầu từ vị trí i
            count_n = 0
            start_pos = i
            
            while i < len(long_path) and long_path[i] == 'e':
                count_n += 1
                i += 1
            
            # Xóa tất cả các phần tử 'n' liên tiếp
            for _ in range(count_n):
                long_path.pop(start_pos)
                
            # Thêm 'n' và số lượng vào danh sách
            long_path.insert(start_pos, 'e')
            long_path.insert(start_pos + 1, count_n-1)
            
            # Cập nhật vị trí i
            i = start_pos + 2
            
        elif long_path[i] == 'w' and long_path[i+1] == 'w' and long_path[i+2] == 'w':
            # Đếm số phần tử 'n' liên tiếp bắt đầu từ vị trí i
            count_n = 0
            start_pos = i
            
            while i < len(long_path) and long_path[i] == 'w':
                count_n += 1
                i += 1
            
            # Xóa tất cả các phần tử 'n' liên tiếp
            for _ in range(count_n):
                long_path.pop(start_pos)
                
            # Thêm 'n' và số lượng vào danh sách
            long_path.insert(start_pos, 'w')
            long_path.insert(start_pos + 1, count_n-1)
            
            # Cập nhật vị trí i
            i = start_pos + 2
            
        else:
            # Nếu không tìm thấy chuỗi "nnn", di chuyển đến phần tử tiếp theo
            i += 1

def run_algorithm():
    #""" The algorithm which is to be iterated until the center is reached """
    global require_flooding,long_path,time
    
    #maze_print()
    advance()

    if at_center():
        if time:
            compl()
            normalize_list()
            print(long_path)
            upfile(2,long_path)
            return "done"
        creat()
        normalize_list()
        print(long_path)
        upfile(1,long_path)
        init_back()
        
        long_path = []
        time = True
        
        if mouse.look =='n':
            mouse.move_to_n()
            mouse.move_to_s()
        elif mouse.look =='s':
            mouse.move_to_s()
            mouse.move_to_n()
        elif mouse.look =='w':
            mouse.move_to_w()
            mouse.move_to_e()
        elif mouse.look =='e':
            mouse.move_to_e()
            mouse.move_to_w()
        
    read_wall()

    if require_flooding:
        modified_flood()
    
    return "not done"


