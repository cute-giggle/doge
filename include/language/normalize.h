// cute-giggle@outlook.com

#ifndef INCLUDE_LANGUAGE_NORMALIZE_H_
#define INCLUDE_LANGUAGE_NORMALIZE_H_

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace lisa {

class SentenceNormalizer final {
public:
    static std::unordered_set<char> get_supported_punctuations();

    static std::string normalize(const std::string &sentence);

private:
    static const std::unordered_set<wchar_t> supported_punctuations_;
    static const std::unordered_map<wchar_t, wchar_t> punctuation_mapper_;

private:
    static inline bool is_chinese(wchar_t ch) { return ch >= 0x4E00 && ch <= 0x9FA5; }

    static inline bool is_supported_punctuation(wchar_t ch) {
        return supported_punctuations_.find(ch) != supported_punctuations_.end();
    }

    static std::wstring replace_number(const std::wstring &sentence);

    static std::wstring remap_punctuation(const std::wstring &sentence);

    static std::wstring remove_unrecognized_chars(const std::wstring &sentence);

    static std::wstring remove_continuous_punctuation(const std::wstring &sentence);

    static std::wstring strip_punctuation_begin_and_end(const std::wstring &sentence);

    static std::wstring replace_space_between_chinese_and_chinese(const std::wstring &sentence);

    static std::wstring append_space_after_inner_punctuation(const std::wstring &sentence);

    static std::wstring append_space_between_english_lower_and_upper(const std::wstring &sentence);
};

} // namespace lisa

#endif
