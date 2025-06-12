import cv2
import numpy as np
import time
test_img = cv2.imread("images/Aubergine/PICT_20250604_181550.JPG", cv2.COLOR_BGR2RGB)


"""
def remove_bg(img):
    for i in range(len(img)):
        for j in range(len(img[0])):
            if np.sum(img[i][j]) < 120 and abs(img[i][j][0] - img[i][j][1]) < 10 and abs(img[i][j][1] - img[i][j][2]) < 10:
                img[i][j] = [255, 255, 255]
"""


def remove_bg(img):
    # Berechne die Summe der RGB-Werte pro Pixel
    sum_rgb = img.sum(axis=2)

    # Berechne die absoluten Differenzen zwischen R-G und G-B
    diff_rg = np.abs(img[:, :, 0] - img[:, :, 1])
    diff_gb = np.abs(img[:, :, 1] - img[:, :, 2])

    # Maske der Hintergrundpixel
    mask = (sum_rgb < 120) & (diff_rg < 10) & (diff_gb < 10)

    # Setze diese Pixel auf Weiß
    img[mask] = [255, 255, 255]

    return img


"""
def remove_dots(img):
    visited = set()
    objects = []

    for i in range(len(img)):
        for j in range(len(img[0])):
            if (i, j) in visited:
                continue
            if tuple(img[i][j]) == (255, 255, 255):
                continue

            object_l = set()
            queue = [(i, j)]
            while queue:
                cur = queue.pop()
                if cur in visited:
                    continue
                visited.add(cur)
                ci, cj = cur
                if tuple(img[ci][cj]) == (255, 255, 255):
                    continue
                object_l.add(cur)

                for d in [(-1, 0), (0, 1), (1, 0), (0, -1)]:
                    ni, nj = ci + d[0], cj + d[1]
                    if 0 <= ni < len(img) and 0 <= nj < len(img[0]):
                        if (ni, nj) not in visited:
                            queue.append((ni, nj))

            if object_l:
                objects.append(object_l)

    for object_l2 in objects:
        if len(object_l2) > 1000:
            continue
        for i, j in object_l2:
            img[i][j] = [255, 255, 255]
"""
import numpy as np
from scipy.ndimage import label

def remove_dots(img):
    # Erstelle Maske für Nicht-Hintergrundpixel (also != weiß)
    mask = np.any(img != [255, 255, 255], axis=2)

    # Label alle zusammenhängenden Bereiche (4er-Nachbarschaft)
    labeled, num_features = label(mask)

    # Iteriere über alle Labels
    for label_idx in range(1, num_features + 1):
        coords = np.where(labeled == label_idx)
        if len(coords[0]) <= 1000:
            img[coords] = [255, 255, 255]  # Setze Punkte auf Weiß

    return img


start = time.time_ns()
remove_dots(test_img)
end = time.time_ns()
print((end - start) / 1_000_000_000)
start = time.time_ns()
remove_bg(test_img)
end = time.time_ns()
print((end - start) / 1_000_000_000)
cv2.imwrite("test/img4.jpeg", test_img)