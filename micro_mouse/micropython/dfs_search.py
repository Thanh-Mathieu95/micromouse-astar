"""
Thư viện cho thuật toán tìm kiếm theo chiều sâu (DFS - Depth-First Search).

Thuật toán này hữu ích cho việc khám phá mê cung để tìm một đường đi bất kỳ
từ điểm bắt đầu đến điểm kết thúc. Nó không đảm bảo tìm ra đường đi ngắn nhất.
"""

def find_path_dfs(maze, start_pos, end_pos):
    """
    Tìm một đường đi từ start_pos đến end_pos trong mê cung bằng DFS.

    Args:
        maze (dict): Một dictionary đại diện cho mê cung, trong đó key là tuple (x, y)
                     và value là một object/dictionary chứa thông tin về các bức tường.
                     Ví dụ: maze[(x, y)].walls = {'N': True, 'E': False, ...}
        start_pos (tuple): Toạ độ (x, y) của điểm bắt đầu.
        end_pos (tuple): Toạ độ (x, y) của điểm kết thúc.

    Returns:
        list: Một danh sách các tuple (x, y) tạo thành đường đi từ đầu đến cuối.
              Trả về một danh sách rỗng nếu không tìm thấy đường đi.
    """
    stack = []
    visited = set()
    path = {}

    stack.append(start_pos)
    visited.add(start_pos)

    while stack:
        current_pos = stack.pop()

        if current_pos == end_pos:
            # Truy vết ngược lại để xây dựng đường đi
            final_path = []
            temp = end_pos
            while temp != start_pos:
                final_path.append(temp)
                temp = path[temp]
            final_path.append(start_pos)
            final_path.reverse()
            return final_path

        x, y = current_pos
        
        # Khám phá các hướng: Bắc, Đông, Nam, Tây (ưu tiên có thể thay đổi)
        # 1. Hướng Bắc (y-1)
        neighbor = (x, y - 1)
        if not maze[current_pos].walls['N'] and neighbor in maze and neighbor not in visited:
            visited.add(neighbor)
            stack.append(neighbor)
            path[neighbor] = current_pos

        # 2. Hướng Đông (x+1)
        neighbor = (x + 1, y)
        if not maze[current_pos].walls['E'] and neighbor in maze and neighbor not in visited:
            visited.add(neighbor)
            stack.append(neighbor)
            path[neighbor] = current_pos

        # 3. Hướng Nam (y+1)
        neighbor = (x, y + 1)
        if not maze[current_pos].walls['S'] and neighbor in maze and neighbor not in visited:
            visited.add(neighbor)
            stack.append(neighbor)
            path[neighbor] = current_pos

        # 4. Hướng Tây (x-1)
        neighbor = (x - 1, y)
        if not maze[current_pos].walls['W'] and neighbor in maze and neighbor not in visited:
            visited.add(neighbor)
            stack.append(neighbor)
            path[neighbor] = current_pos
            
    return [] # Không tìm thấy đường đi

# --- Ví dụ cách sử dụng ---
# if __name__ == '__main__':
#     # Giả lập một class Cell đơn giản
#     class Cell:
#         def __init__(self):
#             # Ban đầu giả sử có tường ở mọi phía
#             self.walls = {'N': True, 'E': True, 'S': True, 'W': True}

#     # Tạo một mê cung 3x3 đơn giản
#     # S E
#     # |
#     # . . F
#     #   |
#     # . . .
#     maze_data = {}
#     for y in range(3):
#         for x in range(3):
#             maze_data[(x, y)] = Cell()

#     # Gỡ bỏ các bức tường để tạo đường đi
#     maze_data[(0, 0)].walls['E'] = False
#     maze_data[(1, 0)].walls['W'] = False
#     maze_data[(1, 0)].walls['S'] = False
#     maze_data[(1, 1)].walls['N'] = False
#     maze_data[(1, 1)].walls['E'] = False
#     maze_data[(2, 1)].walls['W'] = False

#     start = (0, 0)
#     end = (2, 1)

#     path_found = find_path_dfs(maze_data, start, end)
    
#     if path_found:
#         print(f"Đường đi từ {start} đến {end}: {path_found}")
#         # Output: Đường đi từ (0, 0) đến (2, 1): [(0, 0), (1, 0), (1, 1), (2, 1)]
#     else:
#         print(f"Không tìm thấy đường đi từ {start} đến {end}.")

