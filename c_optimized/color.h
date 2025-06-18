#ifndef COLOR_H
#define COLOR_H

typedef struct {
    unsigned char data[3];
} color;

color *get_k_dominant_colors(int k, color *colors);

#endif // COLOR_H