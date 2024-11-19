// cute-giggle@outlook.com

#ifndef INCLUDE_UTILS_STRING_H_
#define INCLUDE_UTILS_STRING_H_

#include <algorithm>
#include <codecvt>
#include <locale>
#include <string>
#include <vector>

namespace lisa {

namespace utils {

static inline std::wstring utf8_to_wstring(const std::string &text) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(text);
}

static inline std::string wstring_to_utf8(const std::wstring &text) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(text);
}

std::vector<std::string> string_split_any(const std::string &str, const std::string &delim = " ");
std::vector<std::string> string_split_all(const std::string &str, const std::string &delim = " ");

std::wstring string_strip(const std::wstring &str, const std::wstring &chars = L" ");

std::wstring string_join(const std::vector<std::wstring> &strs, const std::wstring &delim = L"");

static inline std::string string_tolower(const std::string &str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

static inline std::string string_toupper(const std::string &str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

} // namespace utils

} // namespace lisa

#endif
