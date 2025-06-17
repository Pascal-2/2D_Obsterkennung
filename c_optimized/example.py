import subprocess

"""
files needed:

remove_bg.c
stb_ds.h
stb_image_write.h
stb_image.h

compilation command:
gcc -Ofast -march=native -flto=auto -funroll-loops -fno-math-errno -Wall -Wextra -Werror -Wpedantic -std=gnu17 -o remove_bg remove_bg.c -lm
"""

def remove_bg(img, out_img):
    try:
        subprocess.run(["./remove_bg", img, out_img], check=True)
        print("Image processed successfully.")
    except subprocess.CalledProcessError as e:
        print("Error during image processing:", e)

input_path = "PICT_20250604_171910.JPG"
output_path = "see.jpg"

remove_bg(input_path, output_path)