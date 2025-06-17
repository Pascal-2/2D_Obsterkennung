#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include "remove_bg.h"

int main(int argc, char **argv) {
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
            
        default:
            fprintf(stderr, "Unsupported operation id: %d\n", id);
            return 1;
    }

    
    return 0;
}
