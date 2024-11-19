// cute-giggle@outlook.com

#ifndef MESSAGE_MESSAGE_CENTER_H_
#define MESSAGE_MESSAGE_CENTER_H_

#include <map>
#include <mutex>
#include <string>

#include "message/message_queue.h"

namespace lisa {

namespace message {

class MessageCenter final {
public:
    static MessageCenter &instance() {
        static MessageCenter instance;
        return instance;
    }

    const std::shared_ptr<MessageQueue> get_queue(const std::string &queue_name) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = queues_.find(queue_name);
        if (it != queues_.end()) {
            return it->second;
        }
        auto queue = std::make_shared<MessageQueue>();
        queues_[queue_name] = queue;
        return queue;
    }

    void shutdown() {
        for (auto &pair : queues_) {
            if (pair.second) {
                pair.second->shutdown();
            }
        }
    }

    ~MessageCenter() { shutdown(); }

private:
    MessageCenter() = default;
    MessageCenter(const MessageCenter &) = delete;
    MessageCenter &operator=(const MessageCenter &) = delete;

    std::mutex mutex_{};
    std::map<std::string, std::shared_ptr<MessageQueue>> queues_{};
};

} // namespace message

} // namespace lisa

#endif
