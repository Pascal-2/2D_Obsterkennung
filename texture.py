import cv2
from c_optimized.example import remove_bg
import matplotlib
matplotlib.use("TkAgg")
import matplotlib.pyplot as plt


# Gray Level Co-occurrence Matrix (GLCM)
def get_GLCM_matrix(img):
    m = [[0] * 256 for _ in range(256)]

    nb = [(-1, 0), (-1, 1), (0, 1), (1, 1), (1, 0), (1, -1), (0, -1), (-1, -1)]

    for i in range(len(img)):
        for j in range(len(img[0])):
            if img[i][j] == 255:
                continue
            for di, dj in nb:
                ni = i + di
                nj = j + dj
                if 0 <= ni < len(img) and 0 <= nj < len(img[0]) and not img[ni][nj] == 255:
                    m[img[i][j]][img[ni][nj]] += 1
    
    return m

# Local Binary Patterns (LBP)

def get_LBP(img):
    pattern = [0] * 256

    nb = [(-1, 0), (-1, 1), (0, 1), (1, 1), (1, 0), (1, -1), (0, -1), (-1, -1)]

    for i in range(len(img)):
        for j in range(len(img[0])):
            if img[i][j] == 255:
                continue
            number = []
            for di, dj in nb:
                ni = i + di
                nj = j + dj
                if 0 <= ni < len(img) and 0 <= nj < len(img[0]) and not img[ni][nj] == 255:
                    if img[ni][nj] < img[i][j]:
                        number.append("1")
                    else:
                        number.append("0")
            if number:
                pattern[int("".join(number), 2)] += 1

    return pattern

path = "images/Mandarinen/PICT_20250604_164120.JPG"
out_path = "temp/testToDel.png"
#test = cv2.imread(path)

remove_bg(path, out_path)

test = cv2.imread(out_path, cv2.IMREAD_GRAYSCALE)

g_m = get_GLCM_matrix(test)

g_p = get_LBP(test)

plt.imshow(g_m, cmap="gray")
plt.show()

plt.bar(list(range(256)), g_p)
plt.show()

