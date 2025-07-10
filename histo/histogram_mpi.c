#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

void printHistogram ( int *hist, int n );

int main(int argc, char *argv[]) {
   int i, j;
   int inputValue;
   int *hist = NULL;
   int rank, size;
   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &size);

   if (rank == 0) {
      printf("Input the amount of values: \n");  
      scanf("%d", &inputValue);
      hist = malloc(sizeof(int) * inputValue);
      printf("Input the values between 0 and 255 (separated by space): \n");
      for (i = 0; i < inputValue; ++i) {
           scanf("%d", &hist[i]); 
      }
   }
   MPI_Bcast(&inputValue, 1, MPI_INT, 0, MPI_COMM_WORLD);
   if (rank != 0) hist = malloc(sizeof(int) * inputValue);
   MPI_Bcast(hist, inputValue, MPI_INT, 0, MPI_COMM_WORLD);

   int local_start = (255 * rank) / size;
   int local_end = (255 * (rank + 1)) / size;
   int local_results[255] = {0};
   struct timespec start, end;
   if (rank == 0) clock_gettime(CLOCK_MONOTONIC, &start);
   for (i = local_start; i < local_end; ++i) {
        for(j = 0; j < inputValue; j++) {
            if (hist[j] == i) 
                local_results[i]++;
        } 
   }
   int results[255] = {0};
   MPI_Reduce(local_results, results, 255, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
   if (rank == 0) {
      clock_gettime(CLOCK_MONOTONIC, &end);
      double tempo = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
      printHistogram(results, 255);
      printf("Tempo de execução (MPI): %.6f segundos\n", tempo);
   }
   free(hist);
   MPI_Finalize();
   return 0;
}

void printHistogram(int *hist, int n) {
      int i;
      for (i = 0; i < n; i++) {
           printf("[%d] - [%d]\n", i, hist[i]);
      }
}
