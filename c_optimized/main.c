#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "remove_bg.h"
#include "color.h"
#include "texture.h"

int main(int argc, char **argv) {
    srand(time(NULL));
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <id> <parameters>\n", argv[0]);
        return 1;
    }

    int id = atoi(argv[1]);
    

    switch (id) {
        case 0: {
            int width, height, channels;
            unsigned char* img = stbi_load(argv[2], &width, &height, &channels, 3);

            if (!img) {
                fprintf(stderr, "Failed to load image: %s\n", argv[2]);
                return 1;
            }
            remove_background(img, width, height);
            if (!stbi_write_jpg(argv[3], width, height, 3, img, 90)) {
                fprintf(stderr, "Failed to write output image\n");
                stbi_image_free(img);
                return 1;
            }

            stbi_image_free(img);
            break;
        }
        case 1: {
            int k = atoi(argv[2]);
            int width, height, channels;
            unsigned char* img = stbi_load(argv[3], &width, &height, &channels, 3);

            if (!img) {
                fprintf(stderr, "Failed to load image: %s\n", argv[3]);
                return 1;
            }
            remove_background(img, width, height);

            color *colors = NULL;

            for (int i = 0; i < width * height * 3; i+=3) {
                if (img[i] != 255 && img[i+1] != 255 && img[i+2] != 255) {
                    color c = {{img[i], img[i+1], img[i+2]}};
                    arrput(colors, c);
                }
                
            }
            
            color *k_colors = get_k_dominant_colors(k, colors);
            for (int i = 0; i < k; i++) {
                printf("%d, %d, %d\n", k_colors[i].data[0], k_colors[i].data[1], k_colors[i].data[2]);
            }

            stbi_image_free(img);
            break;
        }

        case 2: {
            int width, height, channels;
            unsigned char* img = stbi_load(argv[2], &width, &height, &channels, 3);
            unsigned char* bw_img = make_gray(img, width, height);
            float **m = get_GLCM_matrix(bw_img, width, height);

            for (int i = 0; i < 256; i++) {
                for (int j = 0; j < 255; j++) {
                    printf("%f;", m[i][j]);
                }
                printf("%f", m[i][255]);
                printf("\n");
            }
            free(img);
            free(bw_img);
            for (int i = 0; i < 256; i++) {
                free(m[i]);
            }
            free(m);
            break;
        }

        case 3: {
            int width, height, channels;
            unsigned char* img = stbi_load(argv[2], &width, &height, &channels, 3);
            unsigned char* bw_img = make_gray(img, width, height);
            float *hist = get_LBP(bw_img, width, height);

            for (int j = 0; j < 255; j++) {
                printf("%f;", hist[j]);
            }
            printf("%f", hist[255]);
            free(hist);
            free(img);
            free(bw_img);
            break;
        }
            
        default:
            fprintf(stderr, "Unsupported operation id: %d\n", id);
            return 1;
    }

    
    return 0;
}

