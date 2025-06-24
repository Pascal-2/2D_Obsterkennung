#ifndef TEXTURE_H
#define TEXTURE_H

unsigned char *make_gray(unsigned char*, int, int);

float **get_GLCM_matrix(unsigned char*, int, int);

float *get_LBP(unsigned char*, int, int);

#endif

