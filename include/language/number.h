// cute-giggle@outlook.com

#ifndef INCLUDE_LANGUAGE_NUMBER_H_
#define INCLUDE_LANGUAGE_NUMBER_H_

#include <string>
#include <unordered_map>
#include <vector>

namespace lisa {

class NumberHelper final {
public:
    static std::wstring number_to_chinese(const std::wstring &sentence);

private:
    static const std::unordered_map<wchar_t, wchar_t> number_chinese_mapper_;

    static std::wstring number_to_chinese_directly(const std::wstring &sentence);

    static std::wstring number_to_chinese_complex(uint64_t number);

    static std::wstring number_to_chinese_complex(const std::wstring &sentence);

    static std::vector<std::pair<bool, std::wstring>> split_number_and_other(const std::wstring &sentence);
};

} // namespace lisa

#endif
