#include <iostream>
#include <omp.h>
#include <vector>

using namespace std;
const double tau = 0.00001;

bool flag = false;

double ProductOfMatrix(double* A, double* x, int index, long long N) {
    double sum = 0;

    for (int k = 0; k < N; k++) {
        sum += A[index * N + k] * x[k];
    }

//    cout << sum << endl;

    return sum;
}

void linear_equation(double* A, double* b, double* x, long long N) {
    double sum_2 = 0;

    for (int i = 0; i < N; i++) {
        sum_2 += b[i] * b[i];
    }

    int index = 0;
    for (int k = 0; ; k++) {
        index++;
        double sum, sum_1 = 0;

        double *arg = new double[N];
        for (int j = 0; j < N; j++) {
            sum = ProductOfMatrix(A, x, j, N);
            sum_1 += (sum - b[j]) * (sum - b[j]);
            arg[j] = tau * (sum - b[j]);
        }

        for (int j = 0; j < N; j++) {
            x[j] -= arg[j];
        }

        if (sum_1 / sum_2 < 0.000000001) {
            break;
        }
    }

    cout << x[0] << endl;
}

void linear_equation_omp_1(double* A, double* b, double* x, long long N) {
    double sum_1;
    double sum_2 = 0;

#pragma omp parallel
    {
        double sum_22 = 0;
#pragma omp for schedule(guided, 100) nowait
        for (int j = 0; j < N; j++) {
            sum_22 += b[j] * b[j];
        }

#pragma omp atomic
        sum_2 += sum_22;
    }

    double *arr = new double[N];

    for (int k = 0; ; k++){
        for (int i = 0; i < N; i++) {
            arr[i] = 0;
        }

        sum_1 = 0;

#pragma omp parallel
        {
#pragma omp for schedule(guided, 100)
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    arr[i] += A[i * N + j] * x[j];
                }
            }

#pragma omp for schedule(guided, 100) reduction(+:sum_1)
            for (int i = 0; i < N; i++) {
                sum_1 += (arr[i] - b[i]) * (arr[i] - b[i]);
                x[i] -= tau * (arr[i] - b[i]);
            }
        }

        if (sum_1 / sum_2 < 0.000000001) {
            break;
        }
    }

    cout << x[0] << endl;
}

void linear_equation_omp_2(double* A, double* b, double* x, long long N) {
    double sum_1;
    double sum_2 = 0;
#pragma omp parallel
    {
#pragma omp for schedule(guided, 100) reduction(+:sum_2) nowait
        for (int j = 0; j < N; j++) {
            sum_2 += b[j] * b[j];
        }
    }

    double *arr = new double[N];

#pragma omp parallel
    {
        for (int k = 0; ; k++) {
#pragma omp for schedule(guided, 100)
            for (int i = 0; i < N; i++) {
                arr[i] = 0;
                for (int j = 0; j < N; j++) {
                    arr[i] += A[i * N + j] * x[j];
                }
            }

            sum_1 = 0;
#pragma omp for schedule(guided, 100) reduction(+:sum_1)
            for (int i = 0; i < N; i++) {
                sum_1 += (arr[i] - b[i]) * (arr[i] - b[i]);
                x[i] -= tau * (arr[i] - b[i]);
            }

            if (sum_1 / sum_2 < 0.000000001) {
                break;
            }
        }
    }

    cout << x[0] << endl;
}

void run_serial(long long N) {
    int A_size = N*N;
    double* A = new double[N*N];
    double* b = new double[N];
    double* x = new double[N];

    for (int i = 0; i < A_size; i++) {
        A[i] = ((i % N == i / N)? 2.0: 1.0);
    }

    for (int i = 0; i < N; i++) {
        b[i] = N + 1;
        x[i] = 0.0;
    }

    double t = omp_get_wtime();
    linear_equation(A, b, x, N);
    double t1 = omp_get_wtime();

    std::cout << "This was a serial version: " << t1 - t << std::endl;
}

void run_parallel(long long N) {
    int A_size = N*N;
    double* A = new double[N * N];
    double* b = new double[N];
    double* x = new double[N];

    for (int i = 0; i < A_size; i++) {
        A[i] = ((i % N == i / N)? 2.0: 1.0);
    }

    for (int i = 0; i < N; i++) {
        b[i] = N + 1;
        x[i] = 0.0;
    }

    double t = omp_get_wtime();
    linear_equation_omp_1(A, b, x, N);
    double t1 = omp_get_wtime();

    std::cout << "\nparallel version 1: " << t1 - t << '\n' << std::endl;

    for (int i = 0; i < N; i++) {
        x[i] = 0;
    }

    t = omp_get_wtime();
    linear_equation_omp_2(A, b, x, N);
    t1 = omp_get_wtime();

    std::cout << "\nparallel version 2: " << t1 - t << '\n' << std::endl;
}

int main(int argc, char **argv) {
    std::cout.precision(10);
    size_t SIZE = 1000;
    
    
    if (argc > 1)
        SIZE = atoi(argv[1]);

    run_parallel(SIZE);
    run_serial(SIZE);

    return 0;
}
