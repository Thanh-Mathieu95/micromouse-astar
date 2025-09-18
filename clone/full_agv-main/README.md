# AMR tutorial

## 1.ROS2
Những phần lý thuyết quan trọng:
1. Hiểu và tạo [workspace](https://docs.ros.org/en/humble/Tutorials/Beginner-Client-Libraries/Creating-A-Workspace/Creating-A-Workspace.html), [package](https://docs.ros.org/en/humble/Tutorials/Beginner-Client-Libraries/Creating-Your-First-ROS2-Package.html)
2. Hiểu tác dụng của nodes, topic, service, action:
    * nodes: chứa công việc thực thi của robot
    * topic: kênh truyền data các node
    * service: node khi có yêu cầu thì sẽ hoạt động
    * action: tương tự như service nhưng có phản hồi data lại
3. Cách viết file .launch: vì hầu hết các thuật toán,hành động robot đều có package thực hiện rồi nên chỉ cần file launch để chạy các package cùng 1 lúc. [Tutorial](https://docs.ros.org/en/foxy/Tutorials/Intermediate/Launch/Launch-Main.html)
4. Khai báo các folder tạo mới:
    * Package build CMake: dùng nhiều, [Tutorial](https://docs.ros.org/en/foxy/How-To-Guides/Ament-CMake-Documentation.html). Cơ bản khá dài nhưng thường tập trung 3 thứ: file package.xml, folder tạo mới và tên executor của file .cpp
    * Package build Python: [Tutorial](https://docs.ros.org/en/foxy/Tutorials/Beginner-Client-Libraries/Creating-Your-First-ROS2-Package.html). Ít khi dùng và không có hướng dẫn quá chi tiết nhưng tập trung vào khai báo các executor trong setup.py
    * Mix CMake và python: thường xuyên dùng :))), cơ bản build package là ament_cmake, code cho robot là python, ngoại trừ file launch thì MỌI FILE PYTHON CÒN LẠI PHẢI THÊM `#! /usr/bin/env python3` dòng đầu tiên của code và phải có 1 folder tên giống y hệt tên package trong folder đấy thêm file __init__.py để trống. Có thể đọc thêm [Tutorial](https://docs.ros.org/en/foxy/How-To-Guides/Ament-CMake-Python-Documentation.html)
        * Đọc thêm về [executor](https://docs.ros.org/en/foxy/Concepts/About-Executors.html)
5. TF2: về cơ bản là chia từng bộ phận robot và môi trường(map) thành các hệ tọa độ (giống như hệ tọa độ cho cánh tay robot, cái này thì đ** có luật). Ý nghĩa giúp ta mô hình hóa chuyển động robot lên Rviz. Cái này khá khoai gồm 2 loại tĩnh([static](https://docs.ros.org/en/foxy/Tutorials/Intermediate/Tf2/Writing-A-Tf2-Static-Broadcaster-Cpp.html)) và động([dynamic](https://docs.ros.org/en/foxy/Tutorials/Intermediate/Tf2/Writing-A-Tf2-Broadcaster-Cpp.html))
6. Kết nối nhiều thiết bị ROS2 với nhau [ROS_DOMAIN_ID](https://docs.ros.org/en/foxy/Concepts/About-Domain-ID.html)

Tổng kết: Cơ bản như vậy là bay cmn 2 tháng để HIỂU ROS2, thôi cứ cày [ROS2 Document](https://docs.ros.org/en/foxy/index.html) thật kỹ là dc
## 2. SLAM(trong đồ án này)
* Sử dụng thuật toán GMapping vẽ map + định vị
* RF2O có vai trò cung cấp odometry cho GMapping


## 3.Nav2
Không có quá nhiều thứ để nói về cái này, cơ bản là *Give me the fu***** SLAM and encoder, i will do it*
Để lựa chọn thuật toán hay plugin phù hợp cho loại robot nên đọc [tuning guide](https://docs.nav2.org/tuning/index.html)
