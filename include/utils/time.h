// cute-giggle@outlook.com

#ifndef INCLUDE_UTILS_TIME_H_
#define INCLUDE_UTILS_TIME_H_

#include <chrono>
#include <string>

namespace lisa {

namespace utils {

template <typename T> uint64_t current_ts() {
    const auto now = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<T>(now).count();
}

static inline uint64_t current_ts_sc() { return current_ts<std::chrono::seconds>(); }
static inline uint64_t current_ts_ns() { return current_ts<std::chrono::nanoseconds>(); }
static inline uint64_t current_ts_us() { return current_ts<std::chrono::microseconds>(); }
static inline uint64_t current_ts_ms() { return current_ts<std::chrono::milliseconds>(); }

} // namespace utils

} // namespace lisa

#endif
