// cute-giggle@outlook.com

#include "language/number.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cwctype>

#include "utils/string.h"

namespace lisa {

const std::unordered_map<wchar_t, wchar_t> NumberHelper::number_chinese_mapper_ = {
    {L'0', L'零'}, {L'1', L'一'}, {L'2', L'二'}, {L'3', L'三'}, {L'4', L'四'},
    {L'5', L'五'}, {L'6', L'六'}, {L'7', L'七'}, {L'8', L'八'}, {L'9', L'九'},
};

std::wstring NumberHelper::number_to_chinese(const std::wstring &sentence) {
    auto number_and_other = split_number_and_other(sentence);
    for (auto i = 0U; i < number_and_other.size(); ++i) {
        auto &[is_number, number] = number_and_other[i];
        if (is_number) {
            assert(!number.empty());
            std::wstring chinese;
            if (number.front() == L'-') {
                chinese.push_back(L'负');
                number = number.substr(1);
            }
            if (number.back() == L'%') {
                chinese += L"百分之";
                number.pop_back();
            }
            assert(!number.empty());
            const auto pos = number.find_first_of(L'.');
            if (pos == std::wstring::npos) {
                if (i + 1 < number_and_other.size() && !number_and_other[i + 1].first &&
                    !number_and_other[i + 1].second.empty() && number_and_other[i + 1].second.front() == L'年') {
                    chinese += number_to_chinese_directly(number);
                } else {
                    chinese += number_to_chinese_complex(number);
                }
            } else {
                auto integer = number.substr(0, pos);
                if (integer.empty()) {
                    chinese.push_back(L'零');
                } else {
                    chinese += number_to_chinese_complex(integer);
                }
                auto decimal = number.substr(pos + 1);
                if (!decimal.empty()) {
                    chinese.push_back(L'点');
                    chinese += number_to_chinese_directly(number.substr(pos + 1));
                }
            }
            if (chinese == L"负零") {
                chinese = L"零";
            } else if (chinese.size() >= 2 && chinese[0] == L'二' && chinese[1] == L'千') {
                chinese[0] = L'两';
            }
            number = std::move(chinese);
        }
    }
    std::wstring result;
    for (const auto &[is_number, part] : number_and_other) {
        result += part;
    }
    return result;
}

std::wstring NumberHelper::number_to_chinese_directly(const std::wstring &sentence) {
    std::wstring result;
    for (const auto &ch : sentence) {
        if (ch == '.') {
            result.push_back(L'点');
            continue;
        }
        const auto iter = number_chinese_mapper_.find(ch);
        assert(iter != number_chinese_mapper_.end());
        result.push_back(iter->second);
    }
    return result;
}

std::wstring NumberHelper::number_to_chinese_complex(uint64_t number) {
    assert(number < 10000);
    std::wstring result;
    static const std::array<std::wstring, 4> units = {L"", L"十", L"百", L"千"};
    for (auto i = 0U; i < 4; ++i) {
        const auto digit = number % 10;
        if (digit == 0) {
            if (result.empty() || result[0] != L'零') {
                result = L'零' + result;
            }
        } else {
            result = number_chinese_mapper_.at(digit + L'0') + units[i] + result;
        }
        number /= 10;
    }
    result = lisa::utils::string_strip(result, L"零");
    if (result.size() >= 2 && result[0] == L'一' && result[1] == L'十') {
        result = result.substr(1);
    }
    return result;
}

std::wstring NumberHelper::number_to_chinese_complex(const std::wstring &sentence) {
    assert(!sentence.empty() && std::all_of(sentence.begin(), sentence.end(), iswdigit));
    static const std::array<std::wstring, 4> units = {L"", L"万", L"亿", L"兆"};

    uint64_t number = std::stoull(lisa::utils::wstring_to_utf8(sentence));
    assert(number < 10000000000000000ULL);

    std::wstring result;
    for (auto i = 0U; i < 4; ++i) {
        const auto part = number % 10000;
        if (part == 0) {
            if (result.empty() || result[0] != L'零') {
                result = L'零' + result;
            }
        } else {
            result = number_to_chinese_complex(part) + units[i] + result;
            if (part < 1000 && number >= 10000) {
                result = L'零' + result;
            }
        }
        number /= 10000;
    }
    result = lisa::utils::string_strip(result, L"零");
    return result.empty() ? L"零" : result;
}

std::vector<std::pair<bool, std::wstring>> NumberHelper::split_number_and_other(const std::wstring &sentence) {
    std::vector<std::pair<bool, std::wstring>> result;
    std::wstring current;
    bool is_number = false;
    for (auto i = 0U; i < sentence.size(); ++i) {
        const auto &ch = sentence[i];
        if (ch == L'-') {
            if (i + 1 < sentence.size() && iswdigit(sentence[i + 1])) {
                if (!current.empty()) {
                    result.emplace_back(is_number, std::move(current));
                    current.clear();
                }
                current.push_back(ch);
                is_number = true;
            } else {
                if (is_number) {
                    result.emplace_back(is_number, std::move(current));
                    current.clear();
                }
                current.push_back(ch);
                is_number = false;
            }
            continue;
        }
        if (ch == L'.') {
            if (is_number) {
                if (i + 1 < sentence.size() && iswdigit(sentence[i + 1])) {
                    current.push_back(ch);
                } else {
                    result.emplace_back(is_number, std::move(current));
                    current.clear();
                    current.push_back(ch);
                    is_number = false;
                }
            } else if (i + 1 < sentence.size() && iswdigit(sentence[i + 1])) {
                if (!current.empty()) {
                    result.emplace_back(is_number, std::move(current));
                    current.clear();
                }
                current.push_back(ch);
                is_number = true;
            } else {
                if (is_number) {
                    result.emplace_back(is_number, std::move(current));
                    current.clear();
                }
                current.push_back(ch);
                is_number = false;
            }
            continue;
        }
        if (ch == L'%') {
            if (is_number) {
                current.push_back(ch);
                result.emplace_back(is_number, std::move(current));
                current.clear();
                is_number = false;
            } else {
                current.push_back(ch);
            }
            continue;
        }
        if (iswdigit(ch)) {
            if (!is_number) {
                if (!current.empty()) {
                    result.emplace_back(is_number, std::move(current));
                    current.clear();
                }
                is_number = true;
            }
            current.push_back(ch);
            continue;
        }
        if (is_number) {
            result.emplace_back(is_number, std::move(current));
            current.clear();
        }
        current.push_back(ch);
        is_number = false;
    }
    if (!current.empty()) {
        result.emplace_back(is_number, std::move(current));
    }
    return result;
}

} // namespace doge
