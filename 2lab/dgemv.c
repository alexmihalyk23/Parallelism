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
	// printf("Matrix = ")
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

int LB(int i, int k, int n, int size) {
    int lb = 0;
    if (i < k) {
        lb = i * ((size / n)+1);
    } else {
        lb = k * ((size / n)+1) + (i - k) * (size / n);
    }
    return lb;
}




// 0-2 3-5 6-8 9-11

double matrix_vector_product_omp(double *a, double *b, double *c, int m, int n) {
    double sum = 0.0;
#pragma omp parallel // Используем механизм редукции для корректного подсчета общей суммы
    {
        int nthreads = omp_get_num_threads();
        int threadid = omp_get_thread_num();
        int k = m % nthreads;
        int lb = LB(threadid, k, nthreads, m);
        int ub = LB(threadid + 1, k, nthreads, m);

        // printf("Thread %d: LB %d UB %d\n", threadid, lb, ub);

        double local_sum = 0.0;
        // #pragma omp for
        for (int i = lb; i <= ub; i++) {
            c[i] = 0.0;
            for (int j = 0; j < n; j++) {
                c[i] += a[i * n + j] * b[j];
                // local_sum += c[i]; // Локальная сумма для каждого потока
            }
        }

        // sum += local_sum; // Обновляем общую сумму с использованием механизма редукции
    }
    // printf("\nTotal sum: %lf\n", sum); // Выводим общую сумму только один раз

    return sum;
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