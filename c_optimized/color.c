#include "stb_ds.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#include "color.h"

static int compare_by_count(const void *a, const void *b) {
    color_idx *c1 = (color_idx *)a;
    color_idx *c2 = (color_idx *)b;
    return c2->count - c1->count;
}

color *get_k_dominant_colors(int k, color *colors) {
    int n = arrlen(colors);
    if (k > n / 10) {
        fprintf(stderr, "Error: Requested too many colors: %d. Maximum suggested: %d\n", k, n / 10);
        return NULL;
    }
    color *res_colors = NULL;
    srand(time(NULL));
    int rand_idx[k];
    for (int i = 0; i < k; i++) {
        while (1) {
            int r = rand() % n;
            bool present = false;
            for (int j = 0; j < i; j++) {
                if (r == rand_idx[j]) {
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
        color temp1 = {{colors[rand_idx[i]].data[0], colors[rand_idx[i]].data[1], colors[rand_idx[i]].data[2]}};
        arrput(res_colors, temp1);
    }

    double eps = 1;
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

        for (int i = 0; i < n; i++) {
            color c = {{colors[i].data[0], colors[i].data[1], colors[i].data[2]}};
            int min_dist_color_idx = 0;
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
                colors_diff += abs((int)new_res_colors[i][j] - res_colors[i].data[j]);
                res_colors[i].data[j] = new_res_colors[i][j];
            }
        }
        
        if (colors_diff < eps || counter > 20) {
            
            color_idx temp_res[k];
            for (int i = 0; i < k; i++) {
                for (int j = 0; j < 3; j++)  {
                    temp_res[i].c.data[j] = res_colors[i].data[j];
                }
                temp_res[i].count = (int) arrlen(clusters[i]);
                temp_res[i].idx = i;
            }
            qsort(temp_res, k, sizeof(color_idx), compare_by_count);

            for (int i = 0; i < k; i++) {
                for (int j = 0; j < 3; j++)  {
                    res_colors[i].data[j] = temp_res[i].c.data[j];
                }
            }

            for (int i = 0; i < k; i++) {
                arrfree(clusters[i]);
            }
            arrfree(clusters);
            break;
        }
        for (int i = 0; i < k; i++) {
            arrfree(clusters[i]);
        }
        arrfree(clusters);
        
    }



    return res_colors;
}