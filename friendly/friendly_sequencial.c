#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>  // para medir tempo

int gcd(int u, int v) {
	if (v == 0)
		return u;
	return gcd(v, u % v);
}

void friendly_numbers(long int start, long int end) {
	clock_t start_time = clock(); // início do tempo

	long int last = end - start + 1;

	long int *the_num;
	the_num = (long int*) malloc(sizeof(long int) * last);
	long int *num;
	num = (long int*) malloc(sizeof(long int) * last);
	long int *den;
	den = (long int*) malloc(sizeof(long int) * last);

	long int i, j, factor, ii, sum, done, n;

	for (i = start; i <= end; i++) {
		ii = i - start;
		sum = 1 + i;
		the_num[ii] = i;
		done = i;
		factor = 2;
		while (factor < done) {
			if ((i % factor) == 0) {
				sum += (factor + (i / factor));
				if ((done = i / factor) == factor)
					sum -= factor;
			}
			factor++;
		}
		num[ii] = sum;
		den[ii] = i;
		n = gcd(num[ii], den[ii]);
		num[ii] /= n;
		den[ii] /= n;
	}

	for (i = 0; i < last; i++) {
		for (j = i + 1; j < last; j++) {
			if ((num[i] == num[j]) && (den[i] == den[j]))
				printf("%ld and %ld are FRIENDLY\n", the_num[i], the_num[j]);
		}
	}

	free(the_num);
	free(num);
	free(den);

	clock_t end_time = clock(); // fim do tempo
	double seconds = (double)(end_time - start_time) / CLOCKS_PER_SEC;
	printf("Tempo de execução: %.6f segundos\n", seconds);
}

int main(int argc, char **argv) {
	long int start;
	long int end;

	while (1) {
		scanf("%ld %ld", &start, &end);
		if (start == 0 && end == 0)
			break;
		printf("Number %ld to %ld\n", start, end);
		friendly_numbers(start, end);
	}

	return EXIT_SUCCESS;
}
