#include <stdlib.h>
#include <stdio.h>
#include "texture.h"

#include <dirent.h>
#include "cJSON.h"
#include <string.h>
#include <math.h>

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

float distance_GLCM_optimized(float m1[256][256], float m2[256][256]){
    float res = 0;

    for(int i = 0; i < 256; i++){
        for(int j = 0; j < 256; j++){
            res += (float) fabsf(m1[i][j] - m2[i][j]);
        }
    }
    return res;
}

float distance_LBP_optimized(float l1[256], float l2[256]){
    float res = 0;

    for(int i = 0; i < 256; i++){
        float el_diff = l1[i] - l2[i];
        res += (float) (el_diff * el_diff);
    }
    return res;
}


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

void shift_up(MaxHeap *h, size_t index){
    while (index > 0){
        size_t parent = (index - 1) / 2;
        if(h->heap[index].distance <= h->heap[parent].distance) break;
        
        swap_neighbor(&h->heap[index], &h->heap[parent]);
        index = parent;
    }
}

void shift_down(MaxHeap *h, size_t idx){
    for (;;) {
        size_t left = 2*idx + 1;
        size_t right = 2*idx + 2;
        size_t largest = idx;

        if (left < h->size &&
            h->heap[left].distance > h->heap[largest].distance)
            largest = left;
        if (right < h->size &&
            h->heap[right].distance > h->heap[largest].distance)
            largest = right;

        if (largest == idx) break;
        swap_neighbor(&h->heap[idx], &h->heap[largest]);
        idx = largest;
    }
}

void swap_neighbor(Neighbor *a, Neighbor *b) {
    Neighbor tmp = *a;
    *a = *b;
    *b = tmp;
}

int compare_neighbors_by_category(const void *a, const void *b) {
    const Neighbor *n1 = (const Neighbor *)a;
    const Neighbor *n2 = (const Neighbor *)b;
    return strcmp(n1->category, n2->category);
}

char *heap_majority_category(MaxHeap *h) {
    if (h->size == 0) {
        return NULL; // Keine Nachbarn im Heap
    }

    // 1. Erstelle eine Kopie des Heaps, da qsort die Reihenfolge ändert
    // und wir den Heap für spätere Freigabe intakt halten wollen.
    // Oder: Wenn der Heap nach dieser Funktion nicht mehr benötigt wird,
    // kann man direkt auf h->heap arbeiten.
    // Aber da wir die Kategorie als strdup zurückgeben und der Heap später
    // freigegeben wird, ist eine Kopie sicherer, um nicht die Original-Zeiger zu überschreiben.
    Neighbor *temp_neighbors = malloc(h->size * sizeof(Neighbor));
    if (!temp_neighbors) {
        perror("malloc for temp_neighbors failed");
        return NULL;
    }
    // Kopiere die Strukturen. Wichtig: Dies kopiert die Pointer zu den Kategorien, nicht die Strings selbst.
    memcpy(temp_neighbors, h->heap, h->size * sizeof(Neighbor));


    // 2. Sortiere das temporäre Array nach der Kategorie
    qsort(temp_neighbors, h->size, sizeof(Neighbor), compare_neighbors_by_category);

    char *majority_category = NULL;
    int max_count           = 0;
    int current_count       = 0;
    char *current_category  = NULL;

    // 3. Iteriere durch das sortierte Array und zähle die aufeinanderfolgenden Kategorien
    for (size_t i = 0; i < h->size; i++) {
        if (current_category == NULL || strcmp(temp_neighbors[i].category, current_category) != 0) {
            // Neue Kategorie gefunden
            if (current_count > max_count) {
                max_count         = current_count;
                majority_category = current_category; // Speichere den Pointer zur Mehrheitskategorie
            }
            current_category = temp_neighbors[i].category;
            current_count    = 1;
        } else {
            // Gleiche Kategorie wie vorher
            current_count++;
        }
    }

    // 4. Überprüfe die letzte Gruppe von Elementen nach der Schleife
    if (current_count > max_count) {
        max_count         = current_count;
        majority_category = current_category;
    }

    // 5. Gib eine Kopie der Mehrheitskategorie zurück, da der Original-String
    // Teil des Heaps ist, der später freigegeben wird.
    char *result_category = NULL;
    if (majority_category != NULL) {
        result_category = strdup(majority_category);
        if (!result_category) {
            perror("strdup for result_category failed");
        }
    }
    
    // 6. Gib den temporär allokierten Speicher frei
    free(temp_neighbors);

    return result_category;
}



char *k_nearest_neighbor_optimized(int k, void *eval_data, const char *dataset_name){

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
        return NULL;
    }

    //access it
    cJSON *category = root->child;
    MaxHeap h = heap_init(k);

    while (category){
        const char *key = category->string;

        cJSON *array = category;
        int array_size = cJSON_GetArraySize(array);

       /* float eval_data[256];
        for(int i = 0; i < 256; i++) {
            eval_data[i] = (float)i / 1000.0f; // Beispielwerte für eval_data
        }*/

        for(int i = 0; i < array_size; i++){
            cJSON *data = cJSON_GetArrayItem(array, i);
            
            float d;
            if(strcmp(dataset_name, "lbp") == 0){
                float current[256];
                for (int j = 0; j < 256; j++) {
                    cJSON *data_item = cJSON_GetArrayItem(data, j);
                    current[j] = (float)data_item->valuedouble;
                }
                d = distance_LBP_optimized(eval_data, current);
            } else {
                float current[256][256];
                for (int j = 0; j < 256; j++) {
                    cJSON *row_array = cJSON_GetArrayItem(data, j);
                    for(int n = 0; n <256; n++){
                        cJSON *col_array = cJSON_GetArrayItem(row_array, n);
                        current[j][n] = (float)col_array->valuedouble;
                    }
                }
                d = distance_GLCM_optimized(eval_data, current);
            }
            //float *val = (float*) data->child;
            //long d = strcmp(dataset_name, "lbp") == 0
            // ? distance_LBP_optimized(eval_data, current)
            // : distance_GLCM_optimized(eval_data, current);
            Neighbor nb = {strdup(key), d};
            heap_insert(&h, nb);
        }
        category = category->next;
    }


    cJSON_Delete(root);

    char *best = heap_majority_category(&h);

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


