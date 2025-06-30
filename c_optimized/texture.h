#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct {
    char   *category;
    float    distance;
} Neighbor;

typedef struct {
    Neighbor *heap;    /* Array mit capacity Einträgen */
    size_t    size;    /* wie viele aktuell belegt sind */
    size_t    capacity;
} MaxHeap;


unsigned char *make_gray(unsigned char*, int, int);

float **get_GLCM_matrix(unsigned char*, int, int);

float *get_LBP(unsigned char*, int, int);

float distance_GLCM_optimized(float m1[256][256], float m2[256][256]);

float distance_LBP_optimized(float l1[256], float l2[256]);


MaxHeap heap_init(int k);

void heap_insert(MaxHeap *h, Neighbor n);

void shift_up(MaxHeap *h, size_t index);

void shift_down(MaxHeap *h, size_t index);

void swap_neighbor(Neighbor *a, Neighbor *b);

int compare_neighbors_by_category(const void *a, const void *b);

char *heap_majority_category(MaxHeap *h);

char *k_nearest_neighbor_optimized(int k, void *eval_data, const char *dataset_name);

#endif

