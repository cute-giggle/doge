// cute-giggle@outlook.com

#ifndef INCLUDE_UTILS_TYPE_H_
#define INCLUDE_UTILS_TYPE_H_

#include <type_traits>

namespace lisa {

namespace utils {

template <typename T> inline constexpr auto to_underlying(T t) noexcept { return static_cast<std::underlying_type_t<T>>(t); }

} // namespace utils

} // namespace lisa

#endif
