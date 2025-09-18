import os


file_path = "map/myfile.txt"

run_time=4

def delete_all():
    try:
        files = os.listdir("map")  # Liệt kê tất cả các mục trong thư mục
        for file in files:
            file_path = f"map/{file}"  # Đường dẫn đầy đủ của tệp
            if os.stat(file_path)[0] & 0x4000:  # Nếu là thư mục con
                print(f"Bỏ qua thư mục: {file_path}")
            else:
                os.remove(file_path)  # Xóa tệp
                print(f"Đã xóa tệp: {file_path}")
    except OSError as e:
        print(f"Lỗi: {e}")

def create_file( filename, content=""):
    try:
        # Kiểm tra và tạo thư mục nếu chưa tồn tại
        if not "map" in os.listdir():
            os.mkdir("map")
        # Đường dẫn đầy đủ của tệp
        file_path = f"map/{filename}"
        # Tạo và ghi nội dung vào tệp
        with open(file_path, "a") as file:
            file.write(str(content)+"\n")
    except OSError as e:
        print(f"Lỗi: {e}")


def upfile(i,comand):
    for com in comand:
        create_file(f"run_{i}.txt", com)
