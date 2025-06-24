#include <stdlib.h>
#include <stdio.h>
#include "texture.h"

unsigned char *make_gray(unsigned char *img, int width, int height) {
    unsigned char *res = malloc(sizeof(unsigned char) * width * height);
    if (!res) {
        perror("error using malloc");
        return NULL;
    }

    for (int i = 0; i < width * height; i++) {
        int x = img[i * 3];
        int y = img[i * 3 + 1];
        int z = img[i * 3 + 2];
        res[i] = (int) ((x + y + z) / 3.);
    }
    return res;
}

float **get_GLCM_matrix(unsigned char *img, int width, int height) {
    float **m = malloc(sizeof(float *) * 256);
    if (!m) {
        perror("error using malloc");
        return NULL;
    }

    for (int i = 0; i < 256; i++) {
        m[i] = malloc(sizeof(float) * 256);
        if (!m[i]) {
            for (int j = 0; j < i; j++) {
                free(m[j]);
            }
            free(m);
            perror("error using malloc");
            return NULL;
        }
    }

    float nb[8][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};

    int px_count = 0;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (img[i * width + j] == 255) {
                continue;
            }
            px_count += 1;
            for (int k = 0; k < 8; k++) {
                int ni = i + nb[k][0];
                int nj = j + nb[k][1];
                if (0 <= ni && ni < height && 0 <= nj && nj < width && !(img[ni * width + nj] == 255)) {
                    m[img[i * width + j]][img[ni * width + nj]] += 1.;
                }
            }
        }
    }

    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            m[i][j] /= (float) px_count;
        }
    }

    return m;
}

//______
// unsigned char *img, int width, int height
float* get_LBP(unsigned char *img, int width, int height) {
    float *pattern = (float*) malloc(256 * sizeof(float));
    if(!pattern) {
        perror("error using malloc");
        return NULL;
    }
    float nb[8][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};

    //len(img) = height   len(img[0]) = width
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (img[i * width + j] == 255) {
                continue;
            }
            int number = 0;
            int c = 0;
            for (int k = 0; k < 8; k++) {
                int ni = i + nb[k][0];
                int nj = j + nb[k][1];
                if (0 <= ni && ni < height && 0 <= nj && nj < width && !(img[ni * width + nj] == 255)) {
                    if (img[ni * width + nj] <= img[i * width + j]) {
                        number |= 1 << (7 - k);
                    }
                    c += 1;
                }
            }
            if (c == 8) {
                pattern[number] += 1;
            }
        }
    }

    float total_px = 0;

    for (int i = 0; i < 256; i++) {
        total_px += pattern[i];
    }

    for (int i = 0; i < 256; i++) {
        pattern[i] /= total_px;
    }
    return pattern;
}

