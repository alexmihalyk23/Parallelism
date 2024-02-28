
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <malloc.h>

double e = 0.00001;

int m = 4, n = 4;  

char end = 'f';  
double t = 0.1;  

double loss = 0;
int parallel_loop = 0;

double cpuSecond()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ((double)ts.tv_sec + (double)ts.tv_nsec * 1.e-9);
}

int t_flag = 0;

void test(std::vector<double>x, int numThread, double bhg, double bhgv) {
    {
        int sm = 0;
        double r = sqrt(bhg) / sqrt(bhgv);
        if (loss < r && loss != 0 && loss > 0.999999999)
        {
            t = t / 10;
            t_flag = 1;
            // printf("first %f %f\n", loss, t);
            loss = 0;
            x.assign(x.size(), 0);
        }
        else {
            if (abs(loss - r) < 0.00001 && sm == 0) {
                t = t * -1;
                sm = 1;
            }
            // printf(" second %f %f\n", loss, t);
            loss = r;
            if (r < e) end = 't';
        }
    }
}



std::vector<double> matrix_vector_product_omp(std::vector<std::vector<double>>A, std::vector<double>b, std::vector<double>x, int numThread, double bhgv)
{
    std::vector<double>x_predict(n, 0);
    double bhg = 0;
#pragma omp parallel num_threads(numThread)
    {
        double aaaaaa = 0;
        #pragma omp for
        for (int i = 0; i <m; i++) {
            x_predict[i] = 0;
            for (int j = 0; j < n; j++) {
                x_predict[i] = x_predict[i] + A[i][j] * x[j];
            }
            x_predict[i] = x_predict[i] - b[i];
            aaaaaa += x_predict[i] * x_predict[i];
            x_predict[i] = x[i] - t * x_predict[i];
        }
#pragma omp atomic
        bhg += aaaaaa;
    }
    //printf("%f\n", x_predict[0]);
    test(x, numThread, bhg, bhgv);
    if (t_flag == 1) {
        x_predict = x;
        t_flag = 0;
    }
    return x_predict;
}


std::vector<double> matrix_vector_product_omp_second(std::vector<std::vector<double>>A, std::vector<double>b, std::vector<double>x, int numThread, double bhgv)
{
    std::vector<double>x_predict(n, 0);
    double bhg = 0;
#pragma omp parallel num_threads(numThread)
    {

#pragma omp for schedule(dynamic, int(m/(numThread*3))) nowait reduction(+:bhg)
        for (int i = 0; i < m; i++) {
            x_predict[i] = 0;
            for (int j = 0; j < n; j++) {
                x_predict[i] = x_predict[i] + A[i][j] * x[j];
            }
            x_predict[i] = x_predict[i] - b[i];
            bhg += x_predict[i] * x_predict[i];
            x_predict[i] = x[i] - t * x_predict[i];
        }
    }

    test(x, numThread, bhg, bhgv);
    if (t_flag == 1) {
        x_predict = x;
        t_flag = 0;
    }
    return x_predict;
}

void run_parallel(std::vector<std::vector<double>>A, std::vector<double>b, int numThread)
{
    std::vector<double>x(n, 0);
    double bhgv = 0;

    double time = cpuSecond();
#pragma omp parallel reduction num_threads(numThread)
    {

        #pragma omp for
        for (int i = 0; i <m; i++) {
            A[i][i] = 2;
            //b[i] = n+1;
            x[i] = b[i] / A[i][i];
// #pragma omp atomic
            bhgv += b[i] * b[i];
        }
    }
    if (parallel_loop == 0) {
        while (end == 'f') {
            x = matrix_vector_product_omp(A, b, x, numThread, bhgv);
        }
    }
    else {
        while (end == 'f') {
            x = matrix_vector_product_omp_second(A, b, x, numThread, bhgv);
        }
    }

    //for (int i = 0; i < n; i++) {
    //    printf("%f ", x[i]);    
    //}
    time = cpuSecond() - time; 

    printf("Elapsed time (parallel): %.6f sec.\n", time);
}

int main(int argc, char** argv) {
    int numThread = 2;
    if (argc > 1)
        numThread = atoi(argv[1]);
    if (argc > 2) {
        m = atoi(argv[2]);
        n = atoi(argv[2]);
    }
    if (argc > 3) {
        parallel_loop = atoi(argv[3]);
    }
    std::vector<std::vector<double>>A(m, std::vector<double>(n, 1));
    std::vector<double>b(n, n + 1);
    // run_parallel(A, b, 1);
    run_parallel(A, b, numThread);
}