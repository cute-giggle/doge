// cute-giggle@outlook.com

#ifndef INCLUDE_LANGUAGE_LANGUAGE_H_
#define INCLUDE_LANGUAGE_LANGUAGE_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace cppjieba {
class Jieba;
}

namespace lisa {

namespace language {

enum class Language : uint32_t {
    NO = 0,
    ZH = 1,
    EN = 2,
};

struct Pronounce {
    std::string phone{};
    uint32_t tone{};
    Language lang{};
};

using Token = std::pair<std::string, std::string>;

class LanguageHelper final {
public:
    static LanguageHelper &instance();

    std::vector<Token> tokenize(const std::string &sentence) const;

    std::vector<Pronounce> pronounce(const std::string &sentence) const;

private:
    static constexpr auto LANGUAGE_DATA_ROOT_ENV = "DOGE_LANGUAGE_DATA_ROOT";
    static constexpr auto CPPJIEBA_DICT_ROOT_ENV = "DOGE_CPPJIEBA_DICT_ROOT";

    static constexpr auto LANGUAGE_DATA_DEFAULT_RELATIVE_PATH = "/../../data/language";
    static constexpr auto CPPJIEBA_DICT_DEFAULT_RELATIVE_PATH = "/../../data/cppjieba";

    static constexpr auto UNICODE_PINYIN_FILE_NAME = "unicode.pinyin";
    static constexpr auto CHINESE_PINYIN_PHONE_MAPPER_FILE_NAME = "chinese_pinyin.phone";
    static constexpr auto CHINESE_WORDS_PINYIN_MAPPER_FILE_NAME = "chinese_words.pinyin";
    static constexpr auto TAGGING_WORDS_PINYIN_MAPPER_FILE_NAME = "tagging_words.pinyin";
    static constexpr auto ENGLISH_WORDS_PINYIN_MAPPER_FILE_NAME = "english_words.pinyin";

    static constexpr const char *CPPJIEBA_DICT_FILE_NAME_LIST[] = {
        "jieba.dict.utf8", "hmm_model.utf8", "user.dict.utf8", "idf.utf8", "stop_words.utf8",
    };

    static constexpr uint16_t CHINESE_UNICODE_BEGIN = 0x4E00;
    static constexpr uint16_t CHINESE_UNICODE_END = 0x9FA5;

private:
    std::string cppjieba_dict_root_{};
    static std::unique_ptr<cppjieba::Jieba> jieba_;

    std::string language_data_root_{};
    std::vector<std::string> unicode_pinyin_list_{};
    std::unordered_map<std::string, std::string> chinese_pinyin_phone_mapper_{};
    std::unordered_map<std::string, std::string> chinese_words_pinyin_mapper_{};
    std::unordered_map<std::string, std::string> tagging_words_pinyin_mapper_{};
    std::unordered_map<std::string, std::string> english_words_pinyin_mapper_{};

    LanguageHelper();
    LanguageHelper(const LanguageHelper &) = delete;
    LanguageHelper &operator=(const LanguageHelper &) = delete;

private:
    std::vector<std::string> chinese_pinyin(const Token &word) const;
    std::vector<std::string> english_pinyin(const Token &word) const;

    std::vector<Pronounce> chinese_pronounce(const std::string &pinyin) const;
    std::vector<Pronounce> english_pronounce(const std::string &pinyin) const;

    void get_jieba_dict_root();
    void initialize_jieba();

    void get_language_data_root();
    void get_unicode_pinyin_list();
    void get_chinese_pinyin_phone_mapper();
    void get_chinese_words_pinyin_mapper();
    void get_tagging_words_pinyin_mapper();
    void get_english_words_pinyin_mapper();
};

} // namespace language

} // namespace lisa

#endif
