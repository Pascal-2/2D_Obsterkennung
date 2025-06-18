#ifndef COLOR_H
#define COLOR_H

typedef struct {
    unsigned char data[3];
} color;

typedef struct {
    color c;
    int idx;
    int count;
} color_idx;

color *get_k_dominant_colors(int k, color *colors);

#endif // COLOR_H