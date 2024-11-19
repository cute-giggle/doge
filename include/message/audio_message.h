// cute-giggle@outlook.com

#ifndef MESSAGE_AUDIO_MESSAGE_H_
#define MESSAGE_AUDIO_MESSAGE_H_

#include <vector>

#include "message/message.h"

namespace lisa {

namespace message {

class AudioMessage : public Message {
public:
    AudioMessage() = default;
    ~AudioMessage() override = default;

    void reset() override {
        Message::reset();
        channels_ = 0;
        sample_rate_ = 0;
        data_.clear();
    }

    uint32_t channels_{};
    uint32_t sample_rate_{};
    std::vector<float> data_{};
};

} // namespace message

} // namespace lisa

#endif
