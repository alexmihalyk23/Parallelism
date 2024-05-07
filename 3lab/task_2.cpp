#include <iostream>
#include <future>
#include <thread>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <random>

#define Type double

using namespace std;

mutex mut;
condition_variable cv, cv2;

queue<pair<size_t, future<Type>>> tasks;
queue<pair<size_t, future<Type>>> get_tasks;

unordered_map<size_t, Type> results;

void server_thread(const stop_token& stoken)
{
    unique_lock<mutex> lock_res(mut, defer_lock);
    size_t id_task;
    future<Type> task;

    while (!stoken.stop_requested())
    {
        lock_res.lock();
        if (tasks.empty())
            cv.wait(lock_res);

        if (!tasks.empty()) {
            get_tasks.push({tasks.front().first, std::move(tasks.front().second)});

            tasks.pop();
        }

        lock_res.unlock();


        

        while (!get_tasks.empty()) {

            id_task = get_tasks.front().first;
            Type result = get_tasks.front().second.get();

            lock_res.lock();

            results.insert({id_task, result});

            get_tasks.pop();

            cv2.notify_all();

            lock_res.unlock();
        }


       
    }

    cout << "Server stop!\n";
}

template<typename T>
class Server{
public:
    void start() {
        cout << "Start\n";
        server = jthread(server_thread);
    };

    void stop() {
        unique_lock<mutex> lock_res(mut, defer_lock);
        lock_res.lock();
        cv.notify_all();
        lock_res.unlock();
        server.request_stop();
        server.join();

        cout << "End\n";
    };

    size_t add_task(future<T> task) {
        size_t id = rand();
        tasks.push({id, std::move(task)});
        // cv.notify_all();
        return id;
    };

    T request_result(size_t id_res) {
        T res = results[id_res];
        results.erase(id_res);
        return res;
    };

private:
    size_t rand() {
        static size_t counter = 0;
        return counter++;
    }

    jthread server;
};

Server<Type> server;

template<typename T>
T fun_sin() {
    static std::default_random_engine generator;
    static std::uniform_real_distribution<T> distribution(-3.14159, 3.14159);
    T x = distribution(generator);
    return sin(x);
}

template<typename T>
T fun_sqrt() {
    static std::default_random_engine generator;
    static std::uniform_real_distribution<T> distribution(1.0, 10.0);
    T value = distribution(generator);
    return std::sqrt(value);
}

template<typename T>
T fun_pow() {
    static std::default_random_engine generator;
    static std::uniform_real_distribution<T> distribution(1.0, 10.0);
    T value = distribution(generator);
    return std::pow(value, 2.0);
}

template<typename T>
void add_tasks() {
    unique_lock lock_res(mut, defer_lock);

    for (int i = 0; i < 10000; i++) {
        future<T> result;

        if (i % 3 == 0) {
            result = async(launch::deferred, [](){return fun_sin<T>();});
        } else if (i % 3 == 1) {
            result = async(launch::deferred, [](){return fun_sqrt<T>();});
        } else {
            result = async(launch::deferred, [](){return fun_pow<T>();});
        }

        lock_res.lock();

        size_t id = server.add_task(std::move(result));

        cv.notify_one();

        lock_res.unlock();

        lock_res.lock();

        while (results.find(id) == results.end()) {
            cv2.wait(lock_res);

            if (results.find(id) != results.end()) {
                // cout << "id is:" << id << ", task_thread result(sin):";
                cout << "id is: " << id << " task_thread result:\t" << server.request_result(id) << endl;

                break;
            }
        }

        lock_res.unlock();
    }

    cout << "Client work is done" << endl;
}

int main() {
    server.start();

    thread task1(add_tasks<Type>);
    thread task2(add_tasks<Type>);
    thread task3(add_tasks<Type>);

    task1.join();
    task2.join();
    task3.join();

    server.stop();
}
