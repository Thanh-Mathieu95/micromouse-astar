"""
Thư viện cho thuật toán Flood Fill, được tối ưu cho Micromouse.

Thuật toán này hoạt động theo hai bước:
1. `update_distance_map`: "Lũ lụt" mê cung từ ô đích để tính toán khoảng cách
   ngắn nhất từ mọi ô khác đến đích. Kết quả là một bản đồ khoảng cách.
2. `get_next_move`: Dựa trên bản đồ khoảng cách, robot chỉ cần di chuyển đến
   ô hàng xóm có giá trị khoảng cách nhỏ hơn, đảm bảo đường đi ngắn nhất.
"""

MAZE_WIDTH = 16 # Có thể thay đổi tùy theo kích thước mê cung
MAZE_HEIGHT = 16

def update_distance_map(maze, destinations):
    """
    Cập nhật bản đồ khoảng cách bằng thuật toán Flood Fill.

    Args:
        maze (dict): Dictionary đại diện cho mê cung. 
                     maze[(x, y)].walls = {'N': True, ...}
        destinations (list): Danh sách các ô đích, ví dụ: [(7, 7), (7, 8), (8, 7), (8, 8)]

    Returns:
        dict: Một dictionary chứa khoảng cách từ mỗi ô đến đích gần nhất.
              Ví dụ: distance_map[(x, y)] = 5
    """
    distance_map = {}
    # Khởi tạo tất cả khoảng cách là vô cực (hoặc một số lớn)
    for y in range(MAZE_HEIGHT):
        for x in range(MAZE_WIDTH):
            distance_map[(x, y)] = 255 # 255 được coi là vô cực

    queue = []

    # Đặt khoảng cách của các ô đích là 0 và thêm vào hàng đợi
    for dest in destinations:
        if dest in distance_map:
            distance_map[dest] = 0
            queue.append(dest)

    head = 0
    while head < len(queue):
        current_pos = queue[head]
        head += 1

        x, y = current_pos
        current_dist = distance_map[current_pos]

        # Khám phá các hàng xóm
        # 1. Hướng Bắc (y-1)
        neighbor = (x, y - 1)
        if not maze[current_pos].walls['N'] and neighbor in distance_map and distance_map[neighbor] == 255:
            distance_map[neighbor] = current_dist + 1
            queue.append(neighbor)

        # 2. Hướng Đông (x+1)
        neighbor = (x + 1, y)
        if not maze[current_pos].walls['E'] and neighbor in distance_map and distance_map[neighbor] == 255:
            distance_map[neighbor] = current_dist + 1
            queue.append(neighbor)

        # 3. Hướng Nam (y+1)
        neighbor = (x, y + 1)
        if not maze[current_pos].walls['S'] and neighbor in distance_map and distance_map[neighbor] == 255:
            distance_map[neighbor] = current_dist + 1
            queue.append(neighbor)

        # 4. Hướng Tây (x-1)
        neighbor = (x - 1, y)
        if not maze[current_pos].walls['W'] and neighbor in distance_map and distance_map[neighbor] == 255:
            distance_map[neighbor] = current_dist + 1
            queue.append(neighbor)
            
    return distance_map

def get_next_move(maze, distance_map, current_pos):
    """
    Tìm nước đi tiếp theo dựa trên bản đồ khoảng cách.

    Args:
        maze (dict): Dictionary đại diện cho mê cung.
        distance_map (dict): Bản đồ khoảng cách được tạo từ `update_distance_map`.
        current_pos (tuple): Vị trí hiện tại của robot (x, y).

    Returns:
        tuple: Vị trí (x, y) của ô tiếp theo cần di chuyển đến.
               Trả về None nếu bị kẹt.
    """
    x, y = current_pos
    current_dist = distance_map[current_pos]
    
    best_move = None
    min_dist = current_dist

    # Kiểm tra các ô hàng xóm và tìm ô có khoảng cách nhỏ nhất
    # 1. Hướng Bắc
    neighbor = (x, y - 1)
    if not maze[current_pos].walls['N'] and neighbor in distance_map and distance_map[neighbor] < min_dist:
        min_dist = distance_map[neighbor]
        best_move = neighbor

    # 2. Hướng Đông
    neighbor = (x + 1, y)
    if not maze[current_pos].walls['E'] and neighbor in distance_map and distance_map[neighbor] < min_dist:
        min_dist = distance_map[neighbor]
        best_move = neighbor

    # 3. Hướng Nam
    neighbor = (x, y + 1)
    if not maze[current_pos].walls['S'] and neighbor in distance_map and distance_map[neighbor] < min_dist:
        min_dist = distance_map[neighbor]
        best_move = neighbor

    # 4. Hướng Tây
    neighbor = (x - 1, y)
    if not maze[current_pos].walls['W'] and neighbor in distance_map and distance_map[neighbor] < min_dist:
        min_dist = distance_map[neighbor]
        best_move = neighbor

    # Nếu không tìm thấy nước đi tốt hơn (bị kẹt), robot cần phải tính toán lại
    # bản đồ khoảng cách (vì có thể vừa phát hiện ra tường mới)
    if best_move is None:
        # Trong trường hợp này, logic của robot nên gọi lại update_distance_map
        # và sau đó gọi lại get_next_move.
        pass

    return best_move
