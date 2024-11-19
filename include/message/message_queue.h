// cute-giggle@outlook.com

#ifndef MESSAGE_MESSAGE_QUEUE_H_
#define MESSAGE_MESSAGE_QUEUE_H_

#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>

#include "message/message.h"

namespace lisa {

namespace message {

class MessageQueue final {
public:
    MessageQueue() = default;
    ~MessageQueue() { shutdown(); }

    void push(std::shared_ptr<Message> msg) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push_back(msg);
        }
        cv_.notify_one();
    }

    std::shared_ptr<Message> wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !running_ || !queue_.empty(); });
        if (!running_ || queue_.empty()) {
            return nullptr;
        }
        auto msg = queue_.front();
        queue_.pop_front();
        return msg;
    }

    std::shared_ptr<Message> wait_for(uint64_t timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!cv_.wait_for(lock, std::chrono::milliseconds(timeout), [this] { return !running_ || !queue_.empty(); })) {
            return nullptr;
        }
        if (!running_ || queue_.empty()) {
            return nullptr;
        }
        auto msg = queue_.front();
        queue_.pop_front();
        return msg;
    }

    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.clear();
            running_ = false;
        }
        cv_.notify_all();
    }

private:
    bool running_{true};
    std::mutex mutex_{};
    std::condition_variable cv_{};
    std::deque<std::shared_ptr<Message>> queue_{};

private:
    MessageQueue(const MessageQueue &) = delete;
    MessageQueue &operator=(const MessageQueue &) = delete;
};

} // namespace message

} // namespace lisa

#endif