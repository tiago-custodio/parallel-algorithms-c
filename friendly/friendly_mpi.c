#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int gcd(int u, int v) {
	if (v == 0)
		return u;
	return gcd(v, u % v);
}

void compute_ratios(long int start, long int end, long int *the_num, long int *num, long int *den, int rank, int size) {
	long int i, ii, sum, done, factor, n;
	for (i = start + rank; i <= end; i += size) {
		ii = i - start;
		sum = 1 + i;
		done = i;
		factor = 2;
		while (factor < done) {
			if (i % factor == 0) {
				sum += factor + (i / factor);
				if ((done = i / factor) == factor)
					sum -= factor;
			}
			factor++;
		}
		the_num[ii] = i;
		num[ii] = sum;
		den[ii] = i;
		n = gcd(num[ii], den[ii]);
		num[ii] /= n;
		den[ii] /= n;
	}
}

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	long int start, end;
	if (rank == 0) {
		scanf("%ld %ld", &start, &end);
	}

	MPI_Bcast(&start, 1, MPI_LONG, 0, MPI_COMM_WORLD);
	MPI_Bcast(&end, 1, MPI_LONG, 0, MPI_COMM_WORLD);

	if (start == 0 && end == 0) {
		MPI_Finalize();
		return 0;
	}

	if (rank == 0)
		printf("Number %ld to %ld\n", start, end);

	double t0 = MPI_Wtime();

	long int last = end - start + 1;
	long int *the_num = (long int*) calloc(last, sizeof(long int));
	long int *num     = (long int*) calloc(last, sizeof(long int));
	long int *den     = (long int*) calloc(last, sizeof(long int));

	compute_ratios(start, end, the_num, num, den, rank, size);

	// Buffer para receber no processo 0
	long int *the_num_total = NULL, *num_total = NULL, *den_total = NULL;
	if (rank == 0) {
		the_num_total = (long int*) malloc(sizeof(long int) * last);
		num_total     = (long int*) malloc(sizeof(long int) * last);
		den_total     = (long int*) malloc(sizeof(long int) * last);
	}

	MPI_Reduce(the_num, the_num_total, last, MPI_LONG, MPI_MAX, 0, MPI_COMM_WORLD);
	MPI_Reduce(num,     num_total,     last, MPI_LONG, MPI_MAX, 0, MPI_COMM_WORLD);
	MPI_Reduce(den,     den_total,     last, MPI_LONG, MPI_MAX, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		for (long int i = 0; i < last; i++) {
			for (long int j = i + 1; j < last; j++) {
				if ((num_total[i] == num_total[j]) && (den_total[i] == den_total[j]))
					printf("%ld and %ld are FRIENDLY\n", the_num_total[i], the_num_total[j]);
			}
		}

		double t1 = MPI_Wtime();
		printf("Tempo de execução: %.6f segundos\n", t1 - t0);

		free(the_num_total);
		free(num_total);
		free(den_total);
	}

	free(the_num);
	free(num);
	free(den);

	MPI_Finalize();
	return 0;
}
