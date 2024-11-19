// cute-giggle@outlook.com

#ifndef MESSAGE_MESSAGE_H_
#define MESSAGE_MESSAGE_H_

#include <cstdint>

#include "utils/time.h"

namespace lisa {

namespace message {

class Message {
public:
    Message() = default;
    virtual ~Message() = default;

    virtual void reset() { time_stamp_ = 0; }

    uint64_t time_stamp_{0};
};

} // namespace message

} // namespace lisa

#endif
