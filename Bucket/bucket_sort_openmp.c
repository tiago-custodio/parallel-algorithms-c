#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define MAX 10  // Número de elementos em cada bucket

float findMax(float array[], int n) {
    float max = array[0];
    for (int i = 1; i < n; i++) {
        if (array[i] > max) {
            max = array[i];
        }
    }
    return max;
}

void insertionSort(float bucket[], int n) {
    for (int i = 1; i < n; i++) {
        float key = bucket[i];
        int j = i - 1;
        while (j >= 0 && bucket[j] > key) {
            bucket[j + 1] = bucket[j];
            j--;
        }
        bucket[j + 1] = key;
    }
}

void bucketSort(float array[], int n) {
    float max = findMax(array, n);

    int bucketCount = MAX;
    float buckets[bucketCount][n];
    int bucketSizes[bucketCount];

    for (int i = 0; i < bucketCount; i++) {
        bucketSizes[i] = 0;
    }

    // Distribuir os elementos nos buckets (paralelizado)
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        int index = (array[i] * bucketCount) / (max + 1);
        #pragma omp critical
        {
            buckets[index][bucketSizes[index]] = array[i];
            bucketSizes[index]++;
        }
    }

    // Ordenar os buckets individualmente (paralelizado)
    #pragma omp parallel for
    for (int i = 0; i < bucketCount; i++) {
        if (bucketSizes[i] > 0) {
            insertionSort(buckets[i], bucketSizes[i]);
        }
    }

    int index = 0;
    for (int i = 0; i < bucketCount; i++) {
        for (int j = 0; j < bucketSizes[i]; j++) {
            array[index++] = buckets[i][j];
        }
    }
}

void printArray(float array[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%.2f ", array[i]);
    }
    printf("\n");
}

int readFile(const char *filename, float **array) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return -1;
    }

    int n = 0;
    while (fscanf(file, "%f", &(*array)[n]) == 1) {
        n++;
        *array = realloc(*array, sizeof(float) * (n + 1));
    }
    fclose(file);
    return n;
}

int main() {
    float *array = malloc(sizeof(float) * 1);
    const char *filename = "dados.txt";

    int n = readFile(filename, &array);
    if (n == -1) {
        return 1;
    }

    printf("Array original:\n");
    printArray(array, n);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    bucketSort(array, n);

    clock_gettime(CLOCK_MONOTONIC, &end);
    double tempo = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Array ordenado:\n");
    printArray(array, n);
    printf("Tempo de execu\u00e7\u00e3o com OpenMP: %.6f segundos\n", tempo);

    free(array);
    return 0;
}
