#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
typedef struct {
    unsigned char data[3];
} color;

color *get_k_dominant_colors(int k, unsigned char *img, int width, int height) {
    if (k > width * height / 10) {
        perror("requested to many colors %d\n", k);
        return NULL;
    }
    color *res_colors = NULL;
    srand(time(NULL));
    int rand_idx[k];
    int rand_idx_size = 0;
    for (int i = 0; i < k; i++) {
        while (1) {
            int r = rand() % (width * height);
            bool present = false;
            for (int j = 0; j < rand_idx_size; j++) {
                if (r == rand_idx) {
                    present = true;
                    break;
                }
            }
            if (present) {
                continue;
            }
            rand_idx[i] = r;
            rand_idx_size += 1;
            break;
        }
    }

    for (int i = 0; i < k; i++) {
        int t_idx1 = rand_idx[i]*3;
        color temp1 = {img[t_idx1], img[t_idx1+1], img[t_idx1+2]};
        arrput(res_colors, temp1);
    }

    double eps = 1e-3;
    while (1) {
        color **clusters = NULL;
        for (int i = 0; i < k; i++) {
            color *row = NULL;
            arrput(clusters, row);
        }

        double new_res_colors[k][3];
        int counts[k];
        for (int i = 0; i < k; i++) {
            counts[i] = 0;
            for (int j = 0; j < 3; j++) {
                new_res_colors[i][j] = 0;
            }
        }

        for (int i = 0; i < width * height; i++) {
            int t_idx2 = i * 3;
            color c = {img[t_idx2], img[t_idx2+1], img[t_idx2+2]};
            int min_dist_color_idx;
            double min_dist = 1000;
            for (int j = 0; j < k; j++) {
                double dist = 0;
                for (int k = 0; k < 3; k++) {
                    dist += pow(c.data[k] - res_colors[j].data[k], 2);
                }
                dist = sqrt(dist);
                if (dist < min_dist) {
                    min_dist = dist;
                    min_dist_color_idx = j;
                }
            }
            arrput(clusters[min_dist_color_idx], c);
            for (int j = 0; j < 3; j++) {
                new_res_colors[min_dist_color_idx][j] += c.data[j];
            }
            counts[min_dist_color_idx] += 1;
            
        }

        for (int i = 0; i < k; i++) {
            for (int j = 0; j < 3; j++) {
                if (counts[i] != 0) {
                    new_res_colors[i][j] /= counts[i];
                }
            }
        }

        double colors_diff = 0;
        for (int i = 0; i < k; i++) {
            for (int j = 0; j < 3; j++) {
                colors_diff += abs(new_res_colors[i][j] - res_colors[i].data[j]);
                res_colors[i].data[j] = new_res_colors[i][j];
            }
        }
        for (int i = 0; i < k; i++) {
            arrfree(clusters[i]);
        }
        arrfree(clusters);
        if (colors_diff < eps) {
            break;
        }
        
    }
    return res_colors;
}