#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define MAX 10

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

    for (int i = 0; i < n; i++) {
        int index = (array[i] * bucketCount) / (max + 1);
        buckets[index][bucketSizes[index]] = array[i];
        bucketSizes[index]++;
    }

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

int readFile(const char *filename, float **array) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return -1;
    }

    int n = 0;
    *array = NULL; // Inicializa o ponteiro para evitar acesso inválido
    while (1) {
        float temp;
        int res = fscanf(file, "%f", &temp);
        if (res != 1) break;
        float *new_array = realloc(*array, sizeof(float) * (n + 1));
        if (!new_array) {
            free(*array);
            fclose(file);
            printf("Erro de memória!\n");
            return -1;
        }
        *array = new_array;
        (*array)[n] = temp;
        n++;
    }
    fclose(file);
    return n;
}

void printArray(float *array, int n) {
    for (int i = 0; i < n; i++) {
        printf("%.2f ", array[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    float *full_array = NULL;
    int n = 0;

    if (rank == 0) {
        n = readFile("dados.txt", &full_array);
        if (n == -1) {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    // Enviar tamanho total para todos os processos
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular tamanho de cada pedaço
    int local_n = n / size;
    int resto = n % size;
    int *sendcounts = malloc(size * sizeof(int));
    int *displs = malloc(size * sizeof(int));
    int offset = 0;
    for (int i = 0; i < size; i++) {
        sendcounts[i] = local_n + (i < resto ? 1 : 0);
        displs[i] = offset;
        offset += sendcounts[i];
    }
    int my_n = sendcounts[rank];
    float *local_array = malloc(sizeof(float) * my_n);

    // Distribuir pedaços do array
    MPI_Scatterv(full_array, sendcounts, displs, MPI_FLOAT, local_array, my_n, MPI_FLOAT, 0, MPI_COMM_WORLD);

    struct timespec start, end;
    if (rank == 0) clock_gettime(CLOCK_MONOTONIC, &start);

    // Cada processo ordena seu pedaço
    bucketSort(local_array, my_n);

    // Coletar resultados
    MPI_Gatherv(local_array, my_n, MPI_FLOAT, full_array, sendcounts, displs, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Corrigir a ordenação total após reunir os pedaços
        bucketSort(full_array, n);

        clock_gettime(CLOCK_MONOTONIC, &end);
        double tempo = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

        printf("Array ordenado por MPI:\n");
        printArray(full_array, n);
        printf("Tempo de execu\u00e7\u00e3o com MPI: %.6f segundos\n", tempo);
        free(full_array);
    }

    free(local_array);
    free(sendcounts);
    free(displs);
    MPI_Finalize();
    return 0;
}