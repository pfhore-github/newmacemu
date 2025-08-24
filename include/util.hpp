#ifndef UTIL_HPP_
#define UTIL_HPP_
#include <atomic>
class ThreadController {
    std::atomic<bool> stopping{false};

  public:
    ThreadController() {}
    void stop() {
        stopping.store(true);
    }
    void resume() {
        stopping.store(false);
        stopping.notify_all();
    }
    void wait() {
        while(stopping.load()) {
            stopping.wait(true);
        }
    }
};
#endif