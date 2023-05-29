#include <thread>
#include <functional>
#include <memory>
#include "active_object.hpp"
#include <iostream>

using namespace std;

ActiveObject::ActiveObject() : stopSignal(false)
{
    queue = std::make_unique<SafeQueue>();
}

void ActiveObject::setTask(std::function<void(void *)> func)
{
    this->func = func;
}

void ActiveObject::start()
{
    worker = std::thread([this]
                         {
        while (!stopSignal) {
            if (!this->getQueue()->empty()) {
                void* task = this->queue->dequeue();
                this->func(task);
            }
        } });
}

SafeQueue *ActiveObject::getQueue()
{
    return queue.get();
}

void ActiveObject::stop()
{
    stopSignal = true;
    worker.join();
}

ActiveObject::~ActiveObject()
{
    if (worker.joinable())
    {
        stop();
    }
}
