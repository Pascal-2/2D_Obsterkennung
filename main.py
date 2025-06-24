
#Bilder sortieren
import os
import shutil

with open("data.csv", "r", encoding="utf-8") as f:
    lines = [l.rstrip() for l in f]



m = [x.split(";") for x in lines]
counter = 0
image_files = [f for f in os.listdir("./images") if f.lower().endswith("jpg")]
for x in image_files:
    print(x)

for i in range(len(m[0])):
    folder_name = "./images/" + m[0][i].replace(" ", "-")
    os.makedirs(folder_name, exist_ok=True)
    for j in range(1, len(m)):
        if not m[j][i]:
            continue
        src_path = os.path.join(folder_name, image_files[counter])
        dst_path = os.path.join(folder_name, image_files[counter])
        shutil.move(src_path, dst_path)
        counter += 1



