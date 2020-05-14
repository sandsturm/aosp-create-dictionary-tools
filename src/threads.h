#ifndef THREADS_H
#define THREADS_H

#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>


class Threads{
  private:
    // Create the message queue
    std::set<std::string> queue;

    // Create thread pool
    std::vector<std::thread> workers;

    const int m_Max_Threads = 2;
    std::mutex mutex;
    bool run = true;

    Threads();
    ~Threads();

  public:
    void add_work(std::string task);
    void stop();
};

#endif // THREADS_H ///:~
