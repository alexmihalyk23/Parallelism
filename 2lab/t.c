#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>


double cpuSecond()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ((double)ts.tv_sec + (double)ts.tv_nsec * 1.e-9);
}

/*
 * matrix_vector_product: Compute matrix-vector product c[m] = a[m][n] * b[n]
 */
void matrix_vector_product(double *a, double *b, double *c, int m, int n)
{
    double sum = 0.0;
    for (int i = 0; i < m; i++)
    {
        c[i] = 0.0;
        for (int j = 0; j < n; j++){
            c[i] += a[i * n + j] * b[j];
            sum += c[i];
        }
    }
    printf("serial %lf\n", sum);
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

double matrix_vector_product_omp(double *a, double *b, double *c, int m, int n) {
    double sum = 0.0;
#pragma omp parallel // Используем механизм редукции для корректного подсчета общей суммы
    {
        int nthreads = omp_get_num_threads();
        int threadid = omp_get_thread_num();
        int k = m % nthreads;
        int lb = LB(threadid, k, nthreads, m);
        int ub = LB(threadid + 1, k, nthreads, m);

        printf("Thread %d: LB %d UB %d\n", threadid, lb, ub);

        double local_sum = 0.0;
        for (int i = lb; i < ub; i++) {
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



void run_serial(size_t n, size_t m)
{
    double *a, *b, *c;
    a = (double*)malloc(sizeof(*a) * m * n);
    b = (double*)malloc(sizeof(*b) * n);
    c = (double*)malloc(sizeof(*c) * m);

    if (a == NULL || b == NULL || c == NULL)
    {
        free(a);
        free(b);
        free(c);
        printf("Error allocate memory!\n");
        exit(1);
    }

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
            a[i * n + j] = i + j;
    }

    for (int j = 0; j < n; j++)
        b[j] = j;

    double t = cpuSecond();
    matrix_vector_product(a, b, c, m, n);
    t = cpuSecond() - t;

    printf("Elapsed time (serial): %.6f sec.\n", t);
    free(a);
    free(b);
    free(c);
}

void run_parallel(size_t n, size_t m)
{
    double *a, *b, *c;

    a = (double*)malloc(sizeof(*a) * m * n);
    b = (double*)malloc(sizeof(*b) * n);
    c = (double*)malloc(sizeof(*c) * m);

    if (a == NULL || b == NULL || c == NULL)
    {
        free(a);
        free(b);
        free(c);
        printf("Error allocate memory!\n");
        exit(1);
    }

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
            a[i * n + j] = i + j;
    }

    for (int j = 0; j < n; j++)
        b[j] = j;
    

    double t = cpuSecond();
    matrix_vector_product_omp(a, b, c, m, n);
    t = cpuSecond() - t;

    printf("Elapsed time (parallel): %.6f sec.\n", t);
    free(a);
    free(b);
    free(c);
}

int main(int argc, char *argv[])
{
    size_t M = 1000;
    size_t N = 1000;
    if (argc > 1)
        M = atoi(argv[1]);
    if (argc > 2)
        N = atoi(argv[2]);
    run_serial(M, N);
    run_parallel(M, N);
    return 0;
}
