import math
import random
from bg_ai import remove_bg_vectorized, remove_dots_optimized
import cv2


def get_k_dominant_colors(k, colors):
    clusters = random.sample(colors, k=k)
    eps = 1e-3
    while True:

        res = [[] for _ in range(k)]

        for color in colors:
            dists = []
            for i, cluster in enumerate(clusters):
                dist = math.sqrt(sum([(color[j] - cluster[j]) ** 2 for j in range(3)]))
                dists.append((i, dist))
            dists.sort(key=lambda x: x[1])
            res[dists[0][0]].append(color)

        new_clusters = []
        for group in res:
            r = sum(c[0] for c in group) / len(group)
            g = sum(c[1] for c in group) / len(group)
            b = sum(c[2] for c in group) / len(group)
            new_clusters.append([r, g, b])

        cluster_diff = sum(
            math.sqrt(sum((new_clusters[i][j] - clusters[i][j]) ** 2 for j in range(3)))
            for i in range(k)
        )

        clusters = new_clusters
        if cluster_diff < eps:
            break

    return clusters


test_img = cv2.imread("images/Zucchini/PICT_20250604_155938.JPG", cv2.COLOR_BGR2RGB)

remove_bg_vectorized(test_img)
remove_dots_optimized(test_img)



colors = []

for row in test_img:
    for px in row:
        if px[0] != 255 and px[1] != 255 and px[2] != 255:
            colors.append([int(x) for x in px])

domi = get_k_dominant_colors(5, colors)

print(domi)

for x in domi:
    print([round(y) for y in x[::-1]])

