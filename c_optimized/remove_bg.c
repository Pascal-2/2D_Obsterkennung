#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void remove_background(unsigned char *in_image, int width, int height) {
    for (int i = 0; i < width * height * 3; i+=3) {
        int sum = 0;
        for (int j = 0; j < 3; j++) {
            sum += (int)in_image[i+j];
        }
        int diff1 = abs(((int)in_image[i]) - ((int)in_image[i+1]));
        int diff2 = abs(((int)in_image[i+1]) - ((int)in_image[i+2]));

        if (sum < 120 && diff1 < 10 && diff2 < 10) {
            for (int j = 0; j < 3; j++) {
                in_image[i+j] = 255;
            }
        }
    }

    typedef struct {
        int coord[2];
    } Coords;

    typedef struct {
        Coords key;
        bool value;
    } VisitedEntry;

    VisitedEntry* visited = NULL;
    hmdefault(visited, false);

    Coords** objects = NULL;

    Coords temp1;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            temp1.coord[0] = i;
            temp1.coord[1] = j;

            int idx = (i * width + j) * 3;
            if (in_image[idx] == 255 && in_image[idx + 1] == 255 && in_image[idx + 2] == 255) {
                continue;
            }
            if (hmget(visited, temp1)) {
                continue;
            }

            Coords* object_l = NULL;
            Coords* queue = NULL;

            arrput(queue, temp1);
            hmput(visited, temp1, true);

            while (arrlen(queue) > 0) {
                Coords cur = arrpop(queue);

                int current_pixel_idx = (cur.coord[0] * width + cur.coord[1]) * 3;
                if (in_image[current_pixel_idx] == 255 &&
                    in_image[current_pixel_idx + 1] == 255 &&
                    in_image[current_pixel_idx + 2] == 255) {
                    continue;
                }

                arrput(object_l, cur);

                int d[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

                for (int k = 0; k < 4; k++) {
                    int ni = cur.coord[0] + d[k][0];
                    int nj = cur.coord[1] + d[k][1];

                    if (0 <= ni && ni < height && 0 <= nj && nj < width) {
                        Coords temp2;
                        temp2.coord[0] = ni;
                        temp2.coord[1] = nj;

                        if (!hmget(visited, temp2)) {
                            int neighbor_idx = (ni * width + nj) * 3;
                            if (!(in_image[neighbor_idx] == 255 &&
                                  in_image[neighbor_idx + 1] == 255 &&
                                  in_image[neighbor_idx + 2] == 255))
                            {
                                hmput(visited, temp2, true);
                                arrput(queue, temp2);
                            }
                        }
                    }
                }
            }
            arrfree(queue);

            if (arrlen(object_l) > 0) {
                arrput(objects, object_l);
            } else {
                arrfree(object_l);
            }
        }
    }

    for (ptrdiff_t i = 0; i < arrlen(objects); i++) {
        Coords* current_object_list = objects[i];
        if (arrlen(current_object_list) > 1000) {
            continue;
        }
        for (ptrdiff_t j = 0; j < arrlen(current_object_list); j++) {
            Coords current_coord = current_object_list[j];
            int idx = (current_coord.coord[0] * width + current_coord.coord[1]) * 3;
            for (int k = 0; k < 3; k++) {
                in_image[idx+k] = 255;
            }
        }
    }

    hmfree(visited);

    for (ptrdiff_t i = 0; i < arrlen(objects); i++) {
        arrfree(objects[i]);
    }
    arrfree(objects);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        perror("wrong number of arguments \nusage ./main path_to_input_image.jpg path_to_output_image.jpg\n");
        return 1;
    }
    int width, height, channels;
    unsigned char* img;

    img = stbi_load(argv[1], &width, &height, &channels, 3);

    if (!img) {
        fprintf(stderr, "Failed to load image\n");
        return 1;
    }
    
    remove_background(img, width, height);

    stbi_write_jpg(argv[2], width, height, 3, img, 90);

    stbi_image_free(img);
    return 0;
}