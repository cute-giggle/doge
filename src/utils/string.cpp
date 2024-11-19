// cute-giggle@outlook.com

#include "utils/string.h"

namespace lisa::utils {

std::vector<std::string> string_split_any(const std::string &str, const std::string &delim) {
    std::vector<std::string> result;
    std::string::size_type pos0 = 0;
    std::string::size_type pos1 = 0;
    while (pos1 != str.npos) {
        pos0 = str.find_first_not_of(delim, pos1);
        if (pos0 == str.npos) {
            break;
        }
        pos1 = str.find_first_of(delim, pos0);
        result.push_back(str.substr(pos0, pos1 - pos0));
    }
    return result;
}

std::vector<std::string> string_split_all(const std::string &str, const std::string &delim) {
    std::vector<std::string> result;
    std::string::size_type pos0 = 0;
    std::string::size_type pos1 = 0;
    while (pos1 != str.npos) {
        pos1 = str.find(delim, pos0);
        auto item = str.substr(pos0, pos1 - pos0);
        if (!item.empty()) {
            result.push_back(std::move(item));
        }
        pos0 = pos1 + delim.size();
    }
    return result;
}

std::wstring string_strip(const std::wstring &str, const std::wstring &chars) {
    std::wstring result = str;
    auto first = str.find_first_not_of(chars);
    if (first == std::wstring::npos) {
        return L"";
    }
    auto last = str.find_last_not_of(chars);
    return str.substr(first, last - first + 1);
}

std::wstring string_join(const std::vector<std::wstring> &strs, const std::wstring &delim) {
    std::wstring result;
    for (auto i = 0U; i < strs.size(); ++i) {
        result += strs[i];
        if (i + 1 < strs.size()) {
            result += delim;
        }
    }
    return result;
}

} // namespace doge::utils
