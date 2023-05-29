#include <functional>
#include <map>
#include <poll.h>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>

typedef std::function<void()> handler_t;

class Reactor
{
public:
    Reactor() : running(false) {}
    ~Reactor() { stopReactor(); }

    void *createReactor() { return this; }

    void addFD(int fd, handler_t handler)
    {
        fd_handlers[fd] = handler;
    }

    void startReactor()
    {
        running = true;
        reactor_thread = std::thread([this]()
                                     {
            while (running) {
                std::vector<struct pollfd> fds;
                for (const auto &p : fd_handlers) {
                    struct pollfd pollfd_item;
                    pollfd_item.fd = p.first;
                    pollfd_item.events = POLLIN;
                    fds.push_back(pollfd_item);
                }

                if (poll(fds.data(), fds.size(), -1) == -1) {
                    return;
                }

                for (const auto &p : fds) {
                    if (p.revents & POLLIN) {
                        fd_handlers[p.fd]();
                    }
                }
            } });
    }

    void stopReactor()
    {
        if (running)
        {
            running = false;
            if (reactor_thread.joinable())
            {
                reactor_thread.join();
            }
        }
    }

    void waitFor()
    {
        if (reactor_thread.joinable())
        {
            reactor_thread.join();
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }

    const std::map<int, handler_t> &getFDHandlers() const
    {
        return fd_handlers;
    }

private:
    std::map<int, handler_t> fd_handlers;
    std::thread reactor_thread;
    std::atomic<bool> running;
};
