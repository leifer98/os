#include "safe_queue.hpp"

void SafeQueue::enqueue(void* item) {
    std::lock_guard<std::mutex> lock(mtx);
    q.push(item);
    cv.notify_one(); // Notify a waiting thread, if any.
}

void* SafeQueue::dequeue() {
    std::unique_lock<std::mutex> lock(mtx);
    while (q.empty()) {
        cv.wait(lock); // Release the lock and wait until notified.
    }
    void* val = q.front();
    q.pop();
    return val;
}

bool SafeQueue::empty() const {
    std::lock_guard<std::mutex> lock(mtx);
    return q.empty();
}
