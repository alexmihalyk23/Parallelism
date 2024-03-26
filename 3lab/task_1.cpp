#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <memory>

int count = 40;

double cpuSecond()
{
    using namespace std::chrono;
    system_clock::time_point now = system_clock::now();
    system_clock::duration tp = now.time_since_epoch();
    return duration_cast<duration<double>>(tp).count();
}


int LB(int i, int k, int n, int size) {

    int lb = 0;
    if (i < k) {
        // 4
        lb = i * ((size / n)+1);
    } else {
        lb = k * ((size / n)+1) + (i - k) * (size / n);
    }
    return lb;
}

void matrix_vector_product(double* a, double* b, double* c, int m, int n)
{
    for (int i = 0; i < m; i++)
    {
        c[i] = 0.0;
        for (int j = 0; j < n; j++)
            c[i] += a[i * n + j] * b[j];
    }
}

void matrix_vector_product_thread(double* a, double* b, double* c, int m, int n, int threadid, int items_per_thread)
{
        //  m= 11 ntrherads = 3
        // 11 % 3 = 2
        // threadid = 0
        // 1 2 3 11
        int lb = LB(threadid, items_per_thread, count, m);
        // lb = 0 
        // ub = 4 4
        // lb = 4 
        // ub = 8 4 
        // lb = 8
        // ub = 11 3
        int ub = LB(threadid + 1, items_per_thread, count, m);
        printf("Thread %d: LB %d UB %d\n", threadid, lb, ub);
    for (int i = lb; i <= ub; i++)
    {
        c[i] = 0.0;
        for (int j = 0; j < n; j++)
            c[i] += a[i * n + j] * b[j];
    }
}

void run_serial(size_t n, size_t m)
{
    std::unique_ptr<double[]> a(new double[m * n]);
    std::unique_ptr<double[]> b(new double[n]);
    std::unique_ptr<double[]> c(new double[m]);

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
            a[i * n + j] = i + j;
    }

    for (int j = 0; j < n; j++)
        b[j] = j;

    double t = cpuSecond();
    matrix_vector_product(a.get(), b.get(), c.get(), m, n);
    t = cpuSecond() - t;

    std::cout << "Elapsed time (serial): " << t << " sec." << std::endl;
}

void run_parallel(size_t n, size_t m)
{
    std::unique_ptr<double[]> a(new double[m * n]);
    std::unique_ptr<double[]> b(new double[n]);
    std::unique_ptr<double[]> c(new double[m]);

    // Заполнение a и b данными

    double t_start = cpuSecond(); // Замеряем время до запуска потоков

    std::vector<std::thread> threads;
    int items_per_thread = m % count;

    for (int i = 0; i < count; i++)
    {
        threads.push_back(std::thread(matrix_vector_product_thread, a.get(), b.get(), c.get(), m, n, i, items_per_thread));
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    double t_end = cpuSecond(); // Замеряем время после завершения всех потоков

    std::cout << "Elapsed time (parallel): " << t_end - t_start << " sec." << std::endl;
}

int main(int argc, char* argv[])
{
    size_t M = 1000;
    size_t N = 1000;
    if (argc > 1)
        M = atoi(argv[1]);
    if (argc > 2)
        N = atoi(argv[2]);
    if (argc > 3)
        count = atoi(argv[3]);

    run_serial(M, N);
    run_parallel(M, N);

    return 0;
}
