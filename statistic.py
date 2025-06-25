# uv run -m debugpy --listen 5678 --wait-for-client statistic.py

from c_optimized.optimized_lib import build_dataset_general, remove_bg
import cv2
import os
import math
import decisionTree

def flaechenmasse(f, m):
    if f == 0:
        return -1
    else:
        return m / f

with open("data.csv", "r", encoding="utf-8") as f:
    lines = [l.rstrip().split(";") for l in f]
data = dict()

for i in range(len(lines[0])):
    row = []
    for j in range(1, len(lines)):
        row.append(lines[j][i])
    data[lines[0][i]] = row

for k in data:
    data[k] = [el for el in data[k] if el and el[0] != "-"]



def get_flaechenmasse_no_opt(img_path):
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
                        umfang += 1
            
    for i in range(3):
        mean_color[i] /= flaeche
    print(img_path)
    weight = int(img_path[img_path.rfind("_") + 1:img_path.rfind(".")])

    kompaktheit = (flaeche / (umfang ** 2))
    rundheit = (4 * math.pi) * kompaktheit
    
    print((flaeche, weight, *mean_color, umfang, rundheit, kompaktheit))
    return (flaeche, weight, *mean_color, umfang, rundheit, kompaktheit)


def add_weights():
    categories = os.listdir("images")
    categories.remove("background")
    for i, category in enumerate(categories):
        l_path1 = "images/" + category
        test_data_paths = os.listdir(l_path1)
        test_data_paths.remove("train")
        test_data_paths.remove("test")
        test_data_paths.remove("tuete")

        for j, img_path in enumerate(test_data_paths):
            l_path2 = l_path1 + "/" + img_path
            weight = data[category.replace("-", " ")][j]
            print(l_path2[:-4] + "_" + weight + ".JPG")
            os.rename(l_path2, l_path2[:-4] + "_" + weight + ".JPG")
            #os.rename(l_path2, l_path2[:l_path2.rfind("_")] + ".JPG")



#build_dataset_general(get_flaechenmasse_no_opt, "stat")


def get_accurary(tree):

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
            stat = get_flaechenmasse_no_opt(l_path2)
            prediction = decisionTree.predict_sample(tree, stat)
            if prediction == category:
                correct_count += 1
            else:
                print("expected: ", category, "but was: ", prediction)
            total += 1
            os.remove("temp/todel.png")
    
    print(correct_count / total)


dataset_name = "stat"
dataset = decisionTree.get_dataset(dataset_name)
root = decisionTree.build_tree(dataset, max_depth=5, min_samples_split=2)


get_accurary(root)