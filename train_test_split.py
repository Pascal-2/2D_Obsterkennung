import os
import random
import shutil

folder_path = 'images'

for x in os.listdir('images'):
    print(x)

def split(percentage_train_data):

    for folder in os.listdir('images'):
        if folder == "background":
            continue
        img_path = "images/" + folder
        train_path = img_path + "/train"
        test_path = img_path + "/test"
        if os.path.exists(train_path):
            shutil.rmtree(train_path)
        if os.path.exists(test_path):
            shutil.rmtree(test_path)
        os.mkdir(train_path)

        os.mkdir(test_path)

        imgs = [x for x in os.listdir(img_path) if "." in x]
        train_imgs = random.sample(imgs, k=round(len(imgs)*percentage_train_data))
        test_imgs = [x for x in imgs if x not in train_imgs]
        for img in test_imgs:
            shutil.copy(img_path + "/" + img, test_path + "/" + img)
        for img in train_imgs:
            shutil.copy(img_path + "/" + img, train_path + "/" + img)

split(0.8)