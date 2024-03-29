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
condition_variable cv;

queue<pair<size_t, future<Type>>> tasks;
unordered_map<size_t, Type> results;

void server_thread(const stop_token& stoken)
{
    unique_lock lock_res{mut, defer_lock};
    size_t id_task;

    while (!stoken.stop_requested())
    {
        lock_res.lock();

        while (!tasks.empty()) {
            id_task = tasks.front().first;
            results.insert({id_task, tasks.front().second.get()});
            tasks.pop();
        }

        cv.notify_one();
        lock_res.unlock();
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
        server.request_stop();
        server.join();

        cout << "End\n";
    };

    size_t add_task(future<T> task) {
        size_t id = rand();
        tasks.push({id, std::move(task)});
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
void add_task() {
    bool ready_task = false;

    unique_lock lock_res(mut, defer_lock);

    for (int i = 0; i < 10000; i++) {
        int task_type = rand() % 3;
        future<T> result;

        if (task_type == 0) {
            result = async(launch::deferred, [](){return fun_sin<T>();});
        } else if (task_type == 1) {
            result = async(launch::deferred, [](){return fun_sqrt<T>();});
        } else {
            result = async(launch::deferred, [](){return fun_pow<T>();});
        }

        lock_res.lock();

        if(tasks.empty()){
            cv.wait(lock_res);
        }
        size_t id = server.add_task(std::move(result));

        

        lock_res.unlock();

        while (!ready_task) {
            lock_res.lock();

            if (results.find(id) != results.end()) {
                if (task_type == 0) {
                    cout << "task_thread result(sin):\t" << server.request_result(id) << endl;
                } else if (task_type == 1) {
                    cout << "task_thread result(sqrt):\t" << server.request_result(id) << endl;
                } else {
                    cout << "task_thread result(pow):\t" << server.request_result(id) << endl;
                }
                ready_task = true;
            }

            lock_res.unlock();
        }

        ready_task = false;
    }
}

int main() {
    server.start();

    thread task(add_task<Type>);

    task.join();

    server.stop();
}
