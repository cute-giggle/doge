// cute-giggle@outlook.com

#include "language/normalize.h"

#include "language/number.h"
#include "spdlog/spdlog.h"
#include "utils/string.h"
#include "utils/time.h"

namespace lisa {

const std::unordered_set<wchar_t> SentenceNormalizer::supported_punctuations_ = {
    L',', L'.', L'!', L'?', L':', L';',
};

const std::unordered_map<wchar_t, wchar_t> SentenceNormalizer::punctuation_mapper_ = {
    {L'，', L','}, {L'。', L'.'}, {L'！', L'!'}, {L'？', L'?'}, {L'：', L':'},
    {L'；', L';'}, {L'、', L','}, {L'…', L'.'},  {L'-', L' '},  {L'—', L' '},
};

std::unordered_set<char> SentenceNormalizer::get_supported_punctuations() {
    return std::unordered_set<char>(supported_punctuations_.begin(), supported_punctuations_.end());
}

std::string SentenceNormalizer::normalize(const std::string &sentence) {
    const auto start = lisa::utils::current_ts_us();
    std::wstring wsentence = utils::utf8_to_wstring(sentence);
    wsentence = replace_number(wsentence);
    wsentence = remap_punctuation(wsentence);
    wsentence = remove_unrecognized_chars(wsentence);
    wsentence = remove_continuous_punctuation(wsentence);
    wsentence = strip_punctuation_begin_and_end(wsentence);
    wsentence = replace_space_between_chinese_and_chinese(wsentence);
    wsentence = append_space_after_inner_punctuation(wsentence);
    wsentence = append_space_between_english_lower_and_upper(wsentence);
    const auto result = utils::wstring_to_utf8(wsentence);
    spdlog::debug("Normalize sentence done, time cost: {} us", lisa::utils::current_ts_us() - start);
    spdlog::debug("Original sentence is: [{}]", sentence);
    spdlog::debug("Normalized sentence is: [{}]", result);
    return result;
}

std::wstring SentenceNormalizer::replace_number(const std::wstring &sentence) {
    return NumberHelper::number_to_chinese(sentence);
}

std::wstring SentenceNormalizer::remap_punctuation(const std::wstring &sentence) {
    std::wstring result;
    for (const auto &ch : sentence) {
        const auto iter = punctuation_mapper_.find(ch);
        result.push_back(iter == punctuation_mapper_.end() ? ch : iter->second);
    }
    return result;
}

std::wstring SentenceNormalizer::remove_unrecognized_chars(const std::wstring &sentence) {
    std::wstring result;
    for (const auto &ch : sentence) {
        if (!is_chinese(ch) && !iswalpha(ch) && ch != L' ' && !is_supported_punctuation(ch)) {
            continue;
        }
        result.push_back(ch);
    }
    return result;
}

std::wstring SentenceNormalizer::remove_continuous_punctuation(const std::wstring &sentence) {
    std::wstring result;
    for (auto i = 0U; i < sentence.size(); ++i) {
        result.push_back(sentence[i]);
        if (sentence[i] == L' ' || is_supported_punctuation(sentence[i])) {
            while (i + 1 < sentence.size() && (sentence[i + 1] == L' ' || is_supported_punctuation(sentence[i + 1]))) {
                if (result.back() == L' ' && sentence[i + 1] != L' ') {
                    result.back() = sentence[i + 1];
                }
                ++i;
            }
        }
    }
    return result;
}

std::wstring SentenceNormalizer::strip_punctuation_begin_and_end(const std::wstring &sentence) {
    std::wstring symbols(supported_punctuations_.begin(), supported_punctuations_.end());
    symbols.push_back(L' ');
    auto result = lisa::utils::string_strip(sentence, symbols);
    if (!result.empty() && result.back() != L'.') {
        result.push_back(L'.');
    }
    return result;
}

std::wstring SentenceNormalizer::replace_space_between_chinese_and_chinese(const std::wstring &sentence) {
    std::wstring result = sentence;
    for (auto i = 1U; i + 1 < result.size(); ++i) {
        if (result[i] == L' ' && is_chinese(result[i - 1]) && is_chinese(result[i + 1])) {
            result[i] = L',';
        }
    }
    return result;
}

std::wstring SentenceNormalizer::append_space_after_inner_punctuation(const std::wstring &sentence) {
    std::wstring result;
    for (auto i = 0U; i < sentence.size(); ++i) {
        result.push_back(sentence[i]);
        if (is_supported_punctuation(sentence[i]) && i + 1 < sentence.size() && sentence[i + 1] != L' ') {
            result.push_back(L' ');
        }
    }
    return result;
}

std::wstring SentenceNormalizer::append_space_between_english_lower_and_upper(const std::wstring &sentence) {
    std::wstring result;
    for (auto i = 0U; i < sentence.size(); ++i) {
        result.push_back(sentence[i]);
        if (iswlower(sentence[i]) && i + 1 < sentence.size() && iswupper(sentence[i + 1])) {
            result.push_back(L' ');
        }
    }
    return result;
}

} // namespace doge
