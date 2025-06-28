#include <stdlib.h>
#include <stdio.h>
#include "texture.h"

#include <dirent.h>
#include <cJSON.h>

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

unsigned long distance_GLCM_optimized(const unsigned char m1[256][256], const unsigned char m2[256][256]){
    unsigned long res = 0;

    for(int i = 0; i < 256; i++){
        for(int j = 0; j < 256; j++){
            res += abs(m1[i][j] - m2[i][j]);
        }
    }
    return res;
}

unsigned long distance_LBP_optimized(const unsigned char l1[256], const unsigned char l2[256]){
    unsigned long res = 0;

    for(int i = 0; i < 256; i++){
        long el = l1[i] - l2[i];
        res += el * el;
    }
    return res;
}

typedef struct {
    char *category;
    long distance;
} Neighbor;

typedef struct {
    Neighbor *heap; //array with capacity k
    size_t size; //current size in heap
    size_t capacity;    //=k
} MaxHeap;

MaxHeap heap_init(int k) {
    MaxHeap h;
    h.size     = 0;
    h.capacity = k;
    h.heap     = malloc(k * sizeof *h.heap);
    if (!h.heap) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    return h;
}
void heap_insert(MaxHeap *h, Neighbor n){

    if(h->size < h->capacity){
        h->heap[h->size++] = n;
        shift_up(h, h->size - 1);
    } else if(n.distance < h->heap[0].distance){
        free(h->heap[0].category);
        h->heap[0] = n; //replace root with n
        shift_down(h, 0); //heap down
    } else {
        free(n.category);
    }
}

char *k_nearest_neighbor_optimized(int k, const *el, const char *dataset_name){

    //open json file
    char path[512];
    snprintf(path, sizeof(path),
         "datasets/%s.json",
         dataset_name);
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        return NULL;
    }

    //read file contents into a string
   /* char buffer[1024];
    int len = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);*/

    //alternative from chat
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *json = malloc(sz + 1);
    fread(json, 1, sz, fp);
    json[sz] = '\0';
    fclose(fp);

    //parse the JSON data
    cJSON *root = cJSON_Parse(json);
    if(root == NULL){
        const char *error_ptr = cJSON_GetErrorPtr();
        if(error_ptr != NULL){
            printf("Error: %s\n", error_ptr);
        }
        cJSON_Delete(root);
        return 1;
    }

    //access it
    cJSON *category = root->child;
    MaxHeap h = heap_init(k);

    while (category){
        const char *key = category->string;

        cJSON *array = category;
        int array_size = cJSON_GetArraySize(array);

        for(int i = 0; i < array_size; i++){
            cJSON *data = cJSON_GetArrayItem(array, i);
            long d = strcmp(dataset_name, "lbp") == 0
             ? distance_LBP_optimized(el, data)
             : distance_GLCM_optimized(el, data);
            Neighbor nb = {strdup(key), d};
            heap_insert(&h, nb);
        }
    }

    cJSON_Delete(root);

    const char *best = heap_majority_category(&h);

    //cleanup
    for(size_t i = 0; i < h.size; i++) free(h.heap[i].category);
    free(h.heap);

    return best;
}


/*
unsigned double *get_accuracy_optimized(int k, compa){
    struct dirent *category;
    DIR *categories;

    categories = opendir("images");
    if(categories == NULL){
        printf("Failed to open images");
        return 1;
    }

    while ((category = readdir(categories) != NULL){
        if (strcmp(category->d_name, "background") == 0){
            continue;
        }

        char test_dir[PATH_MAX];
        DIR *validation_data = opendir(test_dir);


    }


    closedir(categories);
    return accuracy;
}

*/