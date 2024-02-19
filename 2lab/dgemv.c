#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>




/*
* matrix_vector_product: Compute matrix-vector product c[m] = a[m][n] * b[n]
*/
void matrix_vector_product(double *a, double *b, double *c, int m, int n)
{
	for (int i = 0; i < m; i++) {
		c[i] = 0.0;
		for (int j = 0; j < n; j++)
			c[i] += a[i * n + j] * b[j];
	}
}

double run_serial(int m, int n)
{
	double *a, *b, *c;
	a = malloc(sizeof(*a) * m * n);
	b = malloc(sizeof(*b) * n);
	c = malloc(sizeof(*c) * m);
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++)
			a[i * n + j] = i + j;
	}
	for (int j = 0; j < n; j++)
		b[j] = j;
	double t = omp_get_wtime();
	matrix_vector_product(a, b, c, m, n);
	t = omp_get_wtime() - t;
	printf("Elapsed time (serial): %.6f sec.\n", t);
	free(a);
	free(b);
	free(c);
	return t;
}


/* matrix_vector_product_omp: Compute matrix-vector product c[m] = a[m][n] * b[n] */
void matrix_vector_product_omp(double *a, double *b, double *c, int m, int n)
{
#pragma omp parallel
{
	int nthreads = omp_get_num_threads();
	int threadid = omp_get_thread_num();
	int items_per_thread = m / nthreads;
// N =c.size n= thred_per i = threadid
	// start = i * (N/n) + (N%n < i ? 1 : 0)

	// int additional_items = n % nthreads;

    //     int lb = threadid * items_per_thread + (threadid < additional_items ? threadid : additional_items);
    //     int ub = lb + items_per_thread + (threadid < additional_items ? 1 : 0) - 1;
	// int lb = threadid * (sizeof(c)/items_per_thread) + (sizeof(c) % items_per_thread < threadid ? 1 : 0);

	int lb = threadid * items_per_thread + (n % nthreads < threadid ? n % nthreads : 0);
        int ub = lb + ceil((double)items_per_thread / nthreads) - 1;
	// int ub = (threadid + 1) * items_per_thread - 1;
	// printf("test %d %d", lb, ub);
	for (int i = lb; i <= ub; i++) {
		c[i] = 0.0;
		for (int j = 0; j < n; j++)
			c[i] += a[i * n + j] * b[j];
	}
	// int lb = threadid * items_per_thread;
	// int ub = (threadid == nthreads - 1) ? (m - 1) : (lb + items_per_thread - 1);
}
}



double run_parallel(int m, int n)
{
double *a, *b, *c;
// Allocate memory for 2-d array a[m, n]
a = malloc(sizeof(*a) * m * n);
b = malloc(sizeof(*b) * n);
c = malloc(sizeof(*c) * m);
for (int i = 0; i < m; i++) {
for (int j = 0; j < n; j++)
a[i * n + j] = i + j;

}
for (int j = 0; j < n; j++)
b[j] = j;
double t = omp_get_wtime();
matrix_vector_product_omp(a, b, c, m, n);
t = omp_get_wtime() - t;
printf("Elapsed time (parallel): %.6f sec.\n", t);
free(a);
free(b);
free(c);
return t;
}


int main(int argc, char **argv)
{
	size_t m = 1000;
    size_t n = 1000;
	
	if (argc > 1)
        m = atoi(argv[1]);
    if (argc > 2)
        n = atoi(argv[2]);
	
printf("Matrix-vector product (c[m] = a[m, n] * b[n]; m = %ld, n = %ld)\n", m, n);
printf("Memory used: %"  PRIu64 " MiB\n", ((m * n + m + n) * sizeof(double)) >> 20);

double tserial = run_serial(m, n);
double tparallel = run_parallel(m,n);
printf("Speedup: %.2f\n", tserial / tparallel);
return 0;
}