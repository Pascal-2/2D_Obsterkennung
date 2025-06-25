from c_optimized.optimized_lib import remove_bg
import cv2
import math

img_path = "images/Orange/PICT_20250604_163420_702.JPG"
t_path = "temp/todel.png"

remove_bg(img_path, t_path)
img = cv2.imread(t_path)

flaeche = 0
mean_color = [0, 0, 0]
umfang = 0

d = [(-1, 0), (-1, 1), (0, 1), (1, 1), (1, 0), (1, -1), (0, -1), (-1, -1)]
for i in range(len(img)):
    for j in range(len(img[0])):
        if img[i][j][0] == 255 and img[i][j][1] == 255 and img[i][j][2] == 255:
            continue
        flaeche += 1
        for k in range(3):
            mean_color[k] += int(img[i][j][k])
        for di, dj in d:
            ni = i + di
            nj = j + dj
            if 0 <= ni < len(img) and 0 <= nj < len(img[0]):
                if img[ni][nj][0] == 255 and img[ni][nj][1] == 255 and img[ni][nj][2] == 255:
                    img[i][j][0] = 0
                    img[i][j][1] = 255
                    img[i][j][2] = 0
                    umfang += 1
        
for i in range(3):
    mean_color[i] /= flaeche
print(img_path)
weight = int(img_path[img_path.rfind("_") + 1:img_path.rfind(".")])

kompaktheit = (flaeche / (umfang ** 2))
rundheit = (4 * math.pi) * kompaktheit

print((flaeche, weight, *mean_color, umfang, rundheit, kompaktheit))
cv2.imwrite("temp/see.png", img)