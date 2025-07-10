#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <time.h>

void printHistogram ( int *hist, int n );

int main(int argc, char *argv[]) {   /* histogram_seq.c   */
   int i, j;
   int inputValue;

   printf("Input the amount of values: \n");  
   scanf("%d", &inputValue);
   int hist[inputValue];

   printf("Input the values between 0 and 255 (separated by space): \n");
   for (i = 0; i < inputValue; ++i) {
        scanf("%d", &hist[i]); 
        //printf("%d ", hist[i]); 
   }

   int results[255] = {0};
   struct timespec start, end;
   clock_gettime(CLOCK_MONOTONIC, &start);
    // Processing data to compute histogram, see 5.17
    for (i = 0; i < 255; ++i) {   
         for(j = 0; j < inputValue; j++) {
             if (hist[j] == i) 
                 results[i]++;
         } 
    }
   clock_gettime(CLOCK_MONOTONIC, &end);
   double tempo = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
   printHistogram(results, 255);
   printf("Tempo de execução: %.6f segundos\n", tempo);
   return 0;
}

void printHistogram(int *hist, int n) {
      int i, j;
      for (i = 0; i < n; i++) {
           printf("[%d] - [%d]\n", i, hist[i]);
           }
}
