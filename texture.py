import time

import cv2
from c_optimized.optimized_lib import remove_bg
import matplotlib
matplotlib.use("TkAgg")
import matplotlib.pyplot as plt
import os
import json
from c_optimized.optimized_lib import get_GLCM_matrix_optimized, get_LBP_optimized, build_dataset_general, k_nearest_neighbour, k_nearest_neighbor_optimized

# Gray Level Co-occurrence Matrix (GLCM)
def get_GLCM_matrix(img):
    m = [[0] * 256 for _ in range(256)]

    nb = [(-1, 0), (-1, 1), (0, 1), (1, 1), (1, 0), (1, -1), (0, -1), (-1, -1)]

    px_count = 0

    for i in range(len(img)):
        for j in range(len(img[0])):
            if img[i][j] == 255:
                continue
            px_count += 1
            for di, dj in nb:
                ni = i + di
                nj = j + dj
                if 0 <= ni < len(img) and 0 <= nj < len(img[0]) and not img[ni][nj] == 255:
                    m[img[i][j]][img[ni][nj]] += 1
    for i in range(256):
        for j in range(256):
            m[i][j] = m[i][j] / px_count
    return m

def compare_GLCM(m1, m2):
    res = 0
    for i in range(256):
        for j in range(256):
            res += abs(m1[i][j] - m2[i][j])
    return res

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
                    if img[ni][nj] <= img[i][j]:
                        number.append("1")
                    else:
                        number.append("0")
                else:
                    number.append("0")
            if len(number) == 8:
                pattern[int("".join(number), 2)] += 1
    total_px = sum(pattern)
    for i in range(256):
        pattern[i] = pattern[i] / total_px
    return pattern



def diff_between_LBP(l1, l2):
    res = 0
    for i in range(256):
        el = l1[i] - l2[i]
        res += el * el
    return res


path = "images/Mandarinen/PICT_20250604_164120.JPG"
out_path = "temp/testToDel.png"
#test = cv2.imread(path)

remove_bg(path, out_path)

#test = cv2.imread(out_path, cv2.IMREAD_GRAYSCALE)

#g_m = get_GLCM_matrix(test)
#g_p = get_LBP(test)



#plt.imshow(g_m, cmap="gray")
#plt.show()

#fig, ax = plt.subplots(figsize=(16, 9))
#ax.bar(range(256), g_p)
#fig.savefig("temp/lpb_hist.png", dpi=100, bbox_inches="tight")


#build_dataset_general(get_GLCM_matrix_optimized, "glcm")
#build_dataset_general(get_LBP_optimized, "lbp")

#res = k_nearest_neighbour_optimized(5, g_m, compare_GLCM, "glcm")
#res = k_nearest_neighbour(1, g_p, diff_between_LBP, "lbp")


def get_accurary(k):

    categories = os.listdir("images")
    correct_count = 0
    total = 0
    for category in categories:

        if category == "background":
            continue
        l_path1 = "images/" + category + "/test"
        test_data_paths = os.listdir(l_path1)
        for img_path in test_data_paths:
            l_path2 = l_path1 + "/" + img_path
            lbc = get_GLCM_matrix_optimized(l_path2)
            #res = k_nearest_neighbor_optimized(k, lbc, "lbp")
            res = k_nearest_neighbor_optimized(k, lbc, "glcm")
            #res = k_nearest_neighbour(k, lbc, compare_GLCM, "glcm")
            if res == category:
                correct_count += 1
                print(correct_count, total)
            else:
                print("expected: ", category, "but was: ", res, )
            total += 1

    print(k, correct_count / total)

timer = time.time()
get_accurary(5)
print(time.time() - timer)
