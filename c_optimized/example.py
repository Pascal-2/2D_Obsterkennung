import subprocess

"""
files needed:

remove_bg.c
stb_ds.h
stb_image_write.h
stb_image.h

compilation command:
gcc -Ofast -march=native -flto=auto -funroll-loops -fno-math-errno -Wall -Wextra -Werror -Wpedantic -std=gnu17 -o main main.c remove_bg.c color.c -lm
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
