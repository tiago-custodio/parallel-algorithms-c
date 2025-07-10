#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>   // Biblioteca OpenMP
#include <time.h>  // Para medição de tempo

int gcd(int u, int v) {
	if (v == 0)
		return u;
	return gcd(v, u % v);
}

void friendly_numbers(long int start, long int end) {
	double start_time = omp_get_wtime();  // Início do tempo

	long int last = end - start + 1;

	long int *the_num = (long int*) malloc(sizeof(long int) * last);
	long int *num     = (long int*) malloc(sizeof(long int) * last);
	long int *den     = (long int*) malloc(sizeof(long int) * last);

	long int i, j;

	#pragma omp parallel for private(i) shared(the_num, num, den)
	for (i = start; i <= end; i++) {
		long int ii = i - start;
		long int sum = 1 + i;
		long int factor = 2;
		long int done = i;

		while (factor < done) {
			if ((i % factor) == 0) {
				sum += (factor + (i / factor));
				if ((done = i / factor) == factor)
					sum -= factor;
			}
			factor++;
		}

		the_num[ii] = i;
		num[ii] = sum;
		den[ii] = i;
		int n = gcd(num[ii], den[ii]);
		num[ii] /= n;
		den[ii] /= n;
	}

	#pragma omp parallel for private(i, j) shared(the_num, num, den)
	for (i = 0; i < last; i++) {
		for (j = i + 1; j < last; j++) {
			if ((num[i] == num[j]) && (den[i] == den[j])) {
				#pragma omp critical
				printf("%ld and %ld are FRIENDLY\n", the_num[i], the_num[j]);
			}
		}
	}

	free(the_num);
	free(num);
	free(den);

	double end_time = omp_get_wtime();  // Fim do tempo
	printf("Tempo de execução: %.6f segundos\n", end_time - start_time);
}

int main(int argc, char **argv) {
	long int start, end;

	while (1) {
		scanf("%ld %ld", &start, &end);
		if (start == 0 && end == 0)
			break;
		printf("Number %ld to %ld\n", start, end);
		friendly_numbers(start, end);
	}

	return EXIT_SUCCESS;
}
