
#include <iostream>
#include <queue>
#include <future>
#include <thread>
#include <chrono>
#include <cmath>
#include <functional>
#include <mutex>

std::mutex mut;
std::condition_variable cv;


// Очередь задач
std::queue<std::pair<size_t, std::future<int>>> tasks;

// Результаты
std::unordered_map<int, int> results;

// задача
int pow_func(int x, int y)
{

    return std::pow(x, y);
}
int sin_func(int x)
{

    return std::sin(x);
}
int sqrt_func(int x)
{

    return std::sqrt(x);
}

// пример сервера обрабатывающего задачи из очереди
void server_thread(std::stop_token stoken)
{
    std::unique_lock lock_res{mut, std::defer_lock};
    size_t id_task;
    // пока не получили сигнал стоп
    while (!stoken.stop_requested())
    {
        lock_res.lock();
        // если очередь не пуста, то достаем и решаем задчу
        if (!tasks.empty())
        {
            id_task = tasks.back().first;
            results.insert({id_task, tasks.back().second.get()});
            tasks.pop();
        }
        cv.notify_one();
        lock_res.unlock();
        // спим, чтобы не занимать одно ядро целиком
        // можно использовать std::condition_variable
        // std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    std::cout << "Server stop!\n";
}

// Пример потока добавляющего задачи
// Обратите внимание на метод emplace() в std::queue, для конструирования на месте
void add_task_thread()
{
    // id задачи
    size_t id_task = 1;

    // блокировщик для работы с общими данными
    std::unique_lock lock_res{mut, std::defer_lock};

    // создаем задачу (ленивое выполнение)
    std::future<int> result = std::async(std::launch::deferred,
                                         []() -> int
                                         {
                                             return sqrt_func(100);
                                         });

    // добавляем задачу в очередь
    lock_res.lock();

    if(tasks.empty()){
        cv.wait(lock_res);
    }
    ++id_task;
    tasks.push({id_task, std::move(result)});
    lock_res.unlock();

    // cv.notify_one();


    // ожидаем завершения задачи и выходим
    bool ready_result = false;
    while (!ready_result)
    {
        lock_res.lock();
        // если есть результат задачи, c id_tasks, то печатаем?
        if (results.find(id_task) != results.end())
        {
            std::cout << "task_thread result:\t" << results[id_task] << '\n';
            results.erase(id_task);
            ready_result = true;
            // std::cout << "true" << '\n';
        }
        lock_res.unlock();


        // ready_result = true;
        
        // спим, чтобы не занимать одно ядро целиком
        // можно использовать std::condition_variable
        // std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main()
{
    std::cout << "Start\n";

    std::jthread server(server_thread);
    std::thread add_task(add_task_thread);

    add_task.join();
    server.request_stop();
    server.join();
    std::cout << "End\n";
}
