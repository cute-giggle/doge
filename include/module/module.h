// cute-giggle@outlook.com

#ifndef MODULE_MODULE_H_
#define MODULE_MODULE_H_

#include <atomic>
#include <string>
#include <thread>

#include "message/message_center.h"

namespace lisa {

namespace module {

class Module {
public:
    explicit Module(const std::string &name) : name_(name) {}

    virtual ~Module() {
        stop();
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    void start() {
        if (running_) {
            return;
        }
        initialize();
        running_ = true;
        thread_ = std::thread(&Module::run, this);
    }

    void stop() {
        if (!running_) {
            return;
        }
        running_ = false;
        finalize();
    }

    const std::string &name() const { return name_; }

    bool running() const { return running_; }

protected:
    virtual void initialize() = 0;
    virtual void finalize() = 0;
    virtual void loop() = 0;

private:
    void run() {
        while (running_) {
            loop();
        }
    }

    Module(const Module &) = delete;
    Module &operator=(const Module &) = delete;

private:
    std::string name_{};
    std::thread thread_{};
    std::atomic<bool> running_{false};
};

} // namespace module

} // namespace lisa

#endif
