#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include "color.h"

color *get_k_dominant_colors(int k, unsigned char *img, int width, int height) {
    if (k > width * height / 10) {
        fprintf(stderr, "Error: Requested too many colors: %d. Maximum suggested: %d\n", k, width * height / 10);
        return NULL;
    }
    color *res_colors = NULL;
    srand(time(NULL));
    int rand_idx[k];
    for (int i = 0; i < k; i++) {
        while (1) {
            int r = rand() % (width * height);
            bool present = false;
            for (int j = 0; j < i; j++) {
                if (r == rand_idx) {
                    present = true;
                    break;
                }
            }
            if (present) {
                continue;
            }
            rand_idx[i] = r;
            break;
        }
    }

    for (int i = 0; i < k; i++) {
        int t_idx1 = rand_idx[i]*3;
        color temp1 = {img[t_idx1], img[t_idx1+1], img[t_idx1+2]};
        arrput(res_colors, temp1);
    }

    double eps = 1e-3;
    int counter = 0;
    while (1) {
        counter += 1;
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
                for (int l = 0; l < 3; l++) {
                    dist += pow(c.data[l] - res_colors[j].data[l], 2);
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
                colors_diff += fabs(new_res_colors[i][j] - (double)res_colors[i].data[j]);
                res_colors[i].data[j] = new_res_colors[i][j];
            }
        }
        for (int i = 0; i < k; i++) {
            arrfree(clusters[i]);
        }
        arrfree(clusters);
        if (colors_diff < eps || counter > 1000) {
            break;
        }
        
    }
    return res_colors;
}