// cute-giggle@outlook.com

#ifndef MESSAGE_MESSAGE_POOL_H_
#define MESSAGE_MESSAGE_POOL_H_

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>

namespace lisa {

namespace message {

template <typename T> class MessagePool {
public:
    static constexpr uint32_t MIN_MSG_COUNT = 1;
    static constexpr uint32_t MAX_MSG_COUNT = 64;

    template <typename... Args> explicit MessagePool(uint32_t msg_count, Args &&...args) {
        msg_count_ = std::clamp(msg_count, MIN_MSG_COUNT, MAX_MSG_COUNT);
        for (auto i = 0U; i < msg_count_; ++i) {
            pool_.push_back(new T(std::forward<Args>(args)...));
        }
    }

    ~MessagePool() {
        for (auto &msg : pool_) {
            delete msg;
        }
    }

    std::shared_ptr<T> get() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pool_.empty()) {
            return nullptr;
        }
        auto msg = pool_.front();
        pool_.pop_front();
        auto deleter = [this](T *msg) {
            assert(msg != nullptr);
            msg->reset();
            std::lock_guard<std::mutex> lock(mutex_);
            pool_.push_back(msg);
        };
        return std::shared_ptr<T>(msg, deleter);
    }

private:
    std::mutex mutex_{};
    uint32_t msg_count_{};
    std::deque<T *> pool_{};
};

} // namespace message

} // namespace lisa

#endif
