# Vi du 1: Phép toán hình thái học + hiển thị có viền cell
import numpy as np
import cv2
import matplotlib.pyplot as plt

# Ma trận X
X = np.array([
    [0,1,1,0,0,1,0,0],
    [1,1,0,0,0,1,1,0],
    [0,1,1,0,1,1,1,1],
    [1,1,0,1,1,1,1,1],
    [1,0,1,1,1,1,0,0],
    [1,0,0,1,0,1,1,1],
    [1,0,1,1,1,0,1,0],
    [1,0,0,0,1,1,1,1]
], dtype=np.uint8)

# Kernel B (structuring element)
B = np.array([
    [0,1,0],
    [1,1,1],
    [0,1,0]
], dtype=np.uint8)

# Thực hiện các phép toán hình thái học với OpenCV
erosion = cv2.erode(X, B, iterations=1)
dilation = cv2.dilate(X, B, iterations=1)
opening = cv2.morphologyEx(X, cv2.MORPH_OPEN, B)
closing = cv2.morphologyEx(X, cv2.MORPH_CLOSE, B)

# Hiển thị kết quả
titles = ['Original (X)', 'Erosion (Co)', 'Dilation (Dãn)', 'Opening (Mở)', 'Closing (Đóng)']
images = [X, erosion, dilation, opening, closing]

plt.figure(figsize=(12, 6))
for i in range(5):
    plt.subplot(2, 3, i+1)
    plt.imshow(images[i], cmap='gray', interpolation='nearest')

    # Thêm lưới (viền nhỏ giữa các cell)
    plt.title(titles[i])
    plt.xticks(np.arange(-0.5, X.shape[1], 1), [])
    plt.yticks(np.arange(-0.5, X.shape[0], 1), [])
    plt.grid(color='black', linestyle='-', linewidth=0.5)

plt.tight_layout()
plt.show()

# In kết quả dạng ma trận
print("Erosion:\n", erosion)
print("Dilation:\n", dilation)
print("Opening:\n", opening)
print("Closing:\n", closing)
