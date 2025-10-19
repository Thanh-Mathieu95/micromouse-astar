# Micromouse - Thuật toán và Mô phỏng

Dự án này chứa mã nguồn cho cả việc mô phỏng và triển khai trên robot thật của một thuật toán tìm đường cho Micromouse.

## Tính năng chính

- **Môi trường mô phỏng C++:** Cho phép kiểm thử và trực quan hóa thuật toán một cách nhanh chóng trên máy tính.
- **Mã nguồn cho robot (Arduino/PlatformIO):** Tệp `src/main.cpp` chứa logic hoàn chỉnh để nạp vào vi điều khiển (ví dụ: ESP32).
- **Thuật toán tìm đường đa giai đoạn:** Một chiến lược phức tạp và mạnh mẽ để robot có thể học hỏi và tối ưu hóa đường đi.
- **Lưu/Tải bản đồ:** Sử dụng hệ thống tệp `LittleFS` để robot có thể "ghi nhớ" mê cung đã học, ngay cả sau khi tắt nguồn.

## Thuật toán được triển khai

Chương trình sử dụng một thuật toán phức tạp gồm nhiều giai đoạn để đảm bảo robot vừa khám phá hiệu quả, vừa có thể chạy tốc độ một cách an toàn.

- **Giai đoạn 1: Khám phá (DFS)**
  - Robot dùng thuật toán Tìm kiếm theo chiều sâu (DFS) để khám phá mê cung cho đến khi tìm thấy ô đích.
  - Khi gặp ngõ cụt, nó sẽ đi ngược lại con đường đã đi để tìm lối rẽ mới.

- **Giai đoạn 2, 4, 5: Quay về (A* Thám hiểm)**
  - Sau khi đến đích hoặc hoàn thành một lượt chạy, robot sẽ quay về điểm xuất phát.
  - Trong các giai đoạn này, nó dùng thuật toán A* ở chế độ "lạc quan có thưởng", tức là nó sẽ **ưu tiên đi vào các ô chưa biết** để mở rộng kiến thức về mê cung.

- **Giai đoạn 3: Chạy đến đích (A* Lạc quan)**
  - Robot chạy từ điểm xuất phát đến đích, sử dụng A* "lạc quan trung tính" để tìm các lối tắt tiềm năng dựa trên kiến thức hiện có.

- **Giai đoạn 6 & 7: Chạy tốc độ (A* An toàn)**
  - Sau khi đã hoàn tất tất cả các lượt chạy học hỏi, robot sẽ có một bản đồ kiến thức rất đầy đủ.
  - Nó dùng A* "bi quan" (ưu tiên đường đã biết và có phí phạt cho khúc cua) để tính ra con đường an toàn và tối ưu nhất.
  - Cuối cùng, nó thực hiện một lượt "chạy tốc độ" (Speed Run) không học hỏi, chỉ đi theo con đường tốt nhất đã tính.

## Cách sử dụng

### 1. Chạy mô phỏng trên máy tính

Mở terminal và sử dụng các lệnh sau:

```bash
# Biên dịch chương trình mô phỏng
g++ -std=c++11 -I. simulation/main_sim.cpp lib/maze/maze.cpp -o build/main_sim

# Chạy chương trình mô phỏng
./build/main_sim
```

### 2. Triển khai trên Robot thật (PlatformIO)

1.  **Mở dự án:** Mở thư mục này bằng Visual Studio Code với tiện ích PlatformIO.
2.  **Hiệu chỉnh:** Mở tệp `src/main.cpp` và hiệu chỉnh các giá trị trong hàm `update_walls_from_sensors` cho phù hợp với cảm biến của bạn.
3.  **Tải lên:**
    -   Biên dịch và nạp chương trình vào robot (PlatformIO: Upload).
    -   Tải lên hệ thống tệp (PlatformIO: Upload Filesystem Image).
4.  **Điều khiển:**
    -   Mở Serial Monitor (tốc độ 115200).
    -   Gửi một ký tự bất kỳ để robot bắt đầu chạy.
    -   Gửi lệnh `delete` để xóa bản đồ đã lưu trong bộ nhớ của robot.

## Cấu trúc thư mục

-   `lib/`: Chứa các thư viện cốt lõi (maze, robot_control, ...).
-   `src/`: Chứa mã nguồn chính (`main.cpp`) để nạp cho robot.
-   `simulation/`: Chứa mã nguồn mô phỏng (`main_sim.cpp`) để chạy trên máy tính.
-   `map/`: Chứa các tệp văn bản định nghĩa mê cung cho môi trường mô phỏng.
-   `build/`: Chứa tệp thực thi của chương trình mô phỏng sau khi biên dịch.
