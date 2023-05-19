#include <functional>
#include <map>
#include <poll.h>
#include <vector>
#include <thread>
#include <atomic>

// This is the type for the callbacks.
typedef std::function<void()> handler_t;

class Reactor {
public:
    Reactor() : running(false) {}
    ~Reactor() { stop(); }

    // Add a file descriptor and a callback.
    void addFD(int fd, handler_t handler) {
        fd_handlers[fd] = handler;
    }

    // Start the reactor.
    void start() {
        running = true;
        reactor_thread = std::thread([this]() {
            while (running) {
                // Prepare the array of pollfd structures.
                std::vector<struct pollfd> fds;
                for (const auto &p : fd_handlers) {
                    struct pollfd pollfd_item;
                    pollfd_item.fd = p.first;
                    pollfd_item.events = POLLIN;
                    fds.push_back(pollfd_item);
                }

                // Wait for an event on any of the file descriptors.
                if (poll(fds.data(), fds.size(), -1) == -1) {
                    perror("poll");
                    return;
                }

                // Check which file descriptors are ready.
                for (const auto &p : fds) {
                    if (p.revents & POLLIN) {
                        // Call the handler for this file descriptor.
                        fd_handlers[p.fd]();
                    }
                }
            }
        });
    }

    // Stop the reactor.
    void stop() {
        if (running) {
            running = false;
            if (reactor_thread.joinable()) {
                reactor_thread.join();
            }
        }
    }

private:
    std::map<int, handler_t> fd_handlers;
    std::thread reactor_thread;
    std::atomic<bool> running;
};