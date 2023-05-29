#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

class SafeQueue {
private:
    std::queue<void*> q;
    mutable std::mutex mtx;
    std::condition_variable cv;

public:
    // Add an element to the queue.
    void enqueue(void* item);

    // Get the front element and remove it from the queue.
    // If the queue is empty, this function will block until an element is available.
    void* dequeue();

    // Check if the queue is empty.
    bool empty() const;
};

#endif // SAFE_QUEUE_H
