#ifndef ACTIVE_OBJECT_HPP
#define ACTIVE_OBJECT_HPP

#include <thread>
#include <functional>
#include <memory>
#include "safe_queue.hpp"

class ActiveObject {
private:
    std::unique_ptr<SafeQueue> queue;
    std::thread worker;
    std::function<void(void*)> func;
    bool stopSignal;

public:
    ActiveObject();
    void setTask(std::function<void(void*)> func);
    void start();
    SafeQueue* getQueue();
    void stop();
    ~ActiveObject();
};

#endif  // ACTIVE_OBJECT_HPP
