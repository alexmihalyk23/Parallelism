#include <iostream>
#include <thread>
#include <list>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>
#include <random>
#include <cmath> // Для std::sin
#include <unordered_map>

template<typename T>
class Server {
public:
    Server() {
        //std::cout << "sozdan\n";
    }

    void start() {
        server_thread = std::jthread ([this] (std::stop_token stokenk) 
        {
            //std::cout << "help me\n";
            run(); 
        });
        //server_thread.join();
    }

    void stop() {
        server_thread.request_stop();
    }

    size_t add_task(std::function<T(void)> task) {
        tupoi_kostil = false;

        //как заблочить доступ в вектор из разных функций
        this->tasks.push(task);
        kolvo++;
        //std::cout << "tasks.size(): " << kolvo-1 << "\n";
        cv.notify_all();

        tupoi_kostil = true;
        tupoi_kostil2 = false;
        return kolvo-1; // Return index of the added task
    }

   T request_result(int idx) {
        if(tupoi_kostil2 == false){
            std::unique_lock<std::mutex> lockx(result_mutex);
            cv_2.wait(lockx);
        }
       return results[idx];
   }

private:

    std::jthread server_thread;
    std::queue<std::function<T(void)>> tasks;
    std::unordered_map<int, T> results;
    bool tupoi_kostil = false;
    bool tupoi_kostil2 = false;
    int idx_tekushi = 0;
    int kolvo = 0;

    std::mutex result_mutex;
    std::condition_variable cv;
    std::condition_variable cv_2;
    std::mutex queue_mutex;
    std::random_device random_device;

    void run() {
        //std :: cout << server_thread.get_stop_token().stop_requested() << "\n";
        while (server_thread.get_stop_token().stop_requested() == 0) {
            tupoi_kostil2 = false;
            while(tasks.size()!= 0){

            if(tupoi_kostil == false){
                std::unique_lock<std::mutex> lockk(queue_mutex);
                cv.wait(lockk);
            }

            std::function<T(void)> task;

            {
                //std::unique_lock<std::mutex> lock(queue_mutex);
                if (!this->tasks.empty()) {
                    task = tasks.front();
                    tasks.pop();
                }
            }

            std::mt19937 generator(random_device());
            std::uniform_int_distribution<> distribution(5, 6);
            double x = distribution(generator);

            T result = task();
            results.insert({idx_tekushi, result});
            idx_tekushi++;
            //std::cout << result <<"\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            //std::cout <<"done\n";
            }

        cv_2.notify_all();
        tupoi_kostil2 = true;
        }
    }
};

template<typename T>
class Client{
public:

    std:: list<int>arguments;
    std:: list<T>results;
    std::random_device random_device;

    void run_client(Server<T>& server, std::function<T(void)> func){
        std::mt19937 generator(random_device());
        std::uniform_int_distribution<> distribution(5, 20);
        int x = distribution(generator);

        for (int i = 0; i < x; i++){
            arguments.push_back(server.add_task(func));
        }
    }

    std:: list<T> client_to_result(Server<T>& server){
        while (!arguments.empty()){
            results.push_back(server.request_result(arguments.front()));
            //std::cout << results.back() << "\t";
            arguments.pop_front();
        }
        //std::cout << "\n";
        return results;
    }

};

template<typename T>
T fun_sin() {
    return std::sin(3.14); // Пример значения для sin
}

template<typename T>
T fun_sqrt() {
    return std::sqrt(3.0);
}

template<typename T>
T fun_pow() {
    return std::pow(2.0, 2.0);
}

int main() {
    Server<double> server; // Уточняем тип double
    server.start();
    
    Client<double> client1;
    Client<double> client2;
    Client<double> client3;

    client1.run_client(server, fun_sin<double>);
    client2.run_client(server, fun_sqrt<double>);
    client3.run_client(server, fun_pow<double>);

    std:: list<double> t1 = client1.client_to_result(server);
    std:: list<double> t2 = client2.client_to_result(server);
    std:: list<double> t3 = client3.client_to_result(server);

    std::cout << "t1 ";
    for (double n : t1)
        std::cout << n << ", ";
    std::cout << "fuck\n";

    std::cout << "t2 ";
    for (double n : t2)
        std::cout << n << ", ";
    std::cout << "dumb\n";

    std::cout << "t3 ";
    for (double n : t3)
        std::cout << n << ", ";
    std::cout << "bitch\n";

    server.stop();

    return 0;
}

