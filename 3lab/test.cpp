#include <iostream>
#include <future>
#include <thread>
#include <math.h>
#include <queue>
#include <unordered_map>

#define Type double

using namespace std;
std::condition_variable cv;


mutex mut;

queue<pair<size_t, future<Type>>> tasks;

unordered_map<int, Type> results;

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

        // this_thread::sleep_for(chrono::milliseconds(50));
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
        size_t id = rand;
        tasks.push({id, std::move(task)});
        rand++;

        return id;
    };

    T request_result(size_t id_res) {
        T res = results[id_res];

        results.erase(id_res);

        return res;
    };

private:
    size_t rand = 0;
    jthread server;
};

Server<Type> server;

template<typename T>
T fun_sin(T arg) {
    return sin(arg);
}

template<typename T>
T fun_sqrt(T arg) {
    return sqrt(arg);
}

template<typename T>
T fun_pow(T x, T y) {
    return pow(x, y);
}

template<typename T>
void add_task_1() {
    bool ready_task = false;

    unique_lock lock_res(mut, defer_lock);

    for (int i = 0; i < 10000; i++) {
        future<T> result = async(launch::deferred, [](){return fun_sin<T>(numbers::pi/2);});

        lock_res.lock();

        if(tasks.empty()){
            cv.wait(lock_res);
        }
        size_t id = server.add_task(std::move(result));

        lock_res.unlock();

        while (!ready_task) {
            lock_res.lock();

            if (results.find(id) != results.end()) {
                // cout << "task_thread result:\t" << server.request_result(id) << endl;
                ready_task = true;
            }

            lock_res.unlock();
        }

        // this_thread::sleep_for(chrono::milliseconds(50));

        ready_task = false;
    }
}

template<typename T>
void add_task_2() {
    bool ready_task = false;

    unique_lock lock_res(mut, defer_lock);

    for (int i = 0; i < 10000; i++) {
        future<T> result = async(launch::deferred, [](){return fun_sqrt<T>(4);});

        lock_res.lock();
        if(tasks.empty()){
            cv.wait(lock_res);
        }

        size_t id = server.add_task(std::move(result));

        lock_res.unlock();

        while (!ready_task) {
            lock_res.lock();

            if (results.find(id) != results.end()) {
                // cout << "task_thread result:\t" << server.request_result(id) << endl;
                ready_task = true;
            }

            lock_res.unlock();
        }

        // this_thread::sleep_for(chrono::milliseconds(50));

        ready_task = false;
    }
}

template<typename T>
void add_task_3() {
    bool ready_task = false;

    unique_lock lock_res(mut, defer_lock);

    for (int i = 0; i < 10000; i++) {
        future<T> result = async(launch::deferred, [](){return fun_pow<T>(2, 3);});

        lock_res.lock();
        if(tasks.empty()){
            cv.wait(lock_res);
        }

        size_t id = server.add_task(std::move(result));

        lock_res.unlock();

        while (!ready_task) {
            lock_res.lock();

            if (results.find(id) != results.end()) {
                // cout << "task_thread result:\t" << server.request_result(id) << endl;
                ready_task = true;
            }

            lock_res.unlock();
        }

        // this_thread::sleep_for(chrono::milliseconds(50));

        ready_task = false;
    }
}

int main() {
    server.start();

    thread task_1(add_task_1<Type>);
    thread task_2(add_task_2<Type>);
    thread task_3(add_task_3<Type>);

    task_1.join();
    task_2.join();
    task_3.join();

    server.stop();
}