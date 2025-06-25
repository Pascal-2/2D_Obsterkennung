from c_optimized.optimized_lib import remove_bg, k_means_colors
import os
import json

def create_color_dataset(k):

    categories = os.listdir("images")

    color_dataset = dict()

    for category in categories:
        if category == "background":
            continue
        color_dataset[category] = []
        l_path1 = "images/" + category + "/train"
        test_data_paths = os.listdir(l_path1)
        for img_path in test_data_paths:
            l_path2 = l_path1 + "/" + img_path
            remove_bg(l_path2, "temp/todel.png")
            colors = k_means_colors(k, "temp/todel.png")
            colors = [[int(y) for y in x.split(", ")] for x in colors.strip().split("\n")]
            #print(colors)
            color_dataset[category].append(colors)
            os.remove("temp/todel.png")

    with open('datasets/color_data.json', 'w') as f:
        json.dump(color_dataset, f)

def read_color_dataset():
    with open('datasets/color_data.json', 'r') as f:
        color_dataset = json.load(f)

    return color_dataset

def color_dist(c1, c2):
    dist = 0
    weights = [70, 30]
    for i in range(len(c1)):
        for j in range(3):
            diff = c1[i][j] - c2[i][j]
            dist += (diff * diff) * (weights[i] / 100)
    return dist

def get_k_nearest(k, colors):
    color_dataset = read_color_dataset()

    ranking = []

    for key, v in color_dataset.items():
        for data_colors in v:
            ranking.append((key, color_dist(colors, data_colors), data_colors))
    
    ranking.sort(key=lambda x:x[1])
    res_d = dict()
    print(ranking[:k])
    for entry in ranking[:k]:
        res_d[entry[0]] = res_d.get(entry[0], 0) + 1


    return sorted(res_d.items(), key=lambda x: x[1])[0][0]



def get_accurary(k, lim):

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
            remove_bg(l_path2, "temp/todel.png")
            colors = k_means_colors(k, "temp/todel.png")
            colors = [[int(y) for y in x.split(", ")] for x in colors.strip().split("\n")]
            res = get_k_nearest(lim, colors)
            if res == category:
                correct_count += 1
            else:
                print("expected: ", category, "but was: ", res)
                print(colors)
            total += 1
            #print(correct_count, total)
            os.remove("temp/todel.png")
    
    print(k, correct_count / total)


#create_color_dataset(2)
get_accurary(2, 2)


