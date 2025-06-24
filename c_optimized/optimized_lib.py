import subprocess
import os
import json

"""
files needed:

remove_bg.c
stb_ds.h
stb_image_write.h
stb_image.h

compilation command:
gcc -O3 -march=native -flto=auto -funroll-loops -fno-math-errno -Wall -Wextra -Werror -Wpedantic -std=gnu17 -o main main.c remove_bg.c color.c texture.c -lm
"""

def remove_bg(img_path, out_img_path):
    try:
        subprocess.run(["./c_optimized/main", "0", img_path, out_img_path], check=True)
    except subprocess.CalledProcessError as e:
        print("Error during image processing:", e)

def k_means_colors(k, img_path):
    try:
        res = subprocess.run(["./c_optimized/main", "1", str(k), img_path], stdout=subprocess.PIPE, text=True)
        return res.stdout
    except subprocess.CalledProcessError as e:
        return "Error during image processing:"

def test_bg():
    input_path = "PICT_20250604_171910.JPG"
    output_path = "see.jpg"

    remove_bg(input_path, output_path)


def test_k_means_colors():
    input_path = "PICT_20250604_171910.JPG"
    k = 5
    k_means_colors(k, input_path)

def get_GLCM_matrix_optimized(img_path):
    try:
        res = subprocess.run(["./c_optimized/main", "2", img_path], stdout=subprocess.PIPE, text=True)
        final = [[float(y) for y in x.split(";")] for x in res.stdout.strip().split("\n")]
        return final
    except subprocess.CalledProcessError as e:
        return None


def get_LBP_optimized(img_path):
    try:
        res = subprocess.run(["./c_optimized/main", "3", img_path], stdout=subprocess.PIPE, text=True)
        return [float(x) for x in res.stdout.split(";")]
    except subprocess.CalledProcessError as e:
        return None

def build_dataset_general(fun, dataset_name):
    categories = os.listdir("images")

    my_dataset = dict()
    counter = 0
    for category in categories:
        if category == "background":
            continue#
        my_dataset[category] = []
        l_path1 = "images/" + category + "/train"
        test_data_paths = os.listdir(l_path1)
        for img_path in test_data_paths:
            l_path2 = l_path1 + "/" + img_path
            data = fun(l_path2)
            my_dataset[category].append(data)
            print(counter)
            counter += 1

    with open(f'datasets/{dataset_name}.json', 'w') as f:
        json.dump(my_dataset, f)

def k_nearest_neighbour(k, el, compare_fun, dataset_name):
    with open(f'datasets/{dataset_name}.json', 'r') as f:
        dataset = json.load(f)

    ranking = []

    for key, v in dataset.items():
        for data in v:
            ranking.append((key, compare_fun(el, data)))

    ranking.sort(key=lambda x:x[1])
    res_d = dict()
    for entry in ranking[:k]:
        res_d[entry[0]] = res_d.get(entry[0], 0) + 1


    return sorted(res_d.items(), key=lambda x: x[1])[0][0]