// cute-giggle@outlook.com

#include "language/language.h"

#include <filesystem>
#include <fstream>

#include "cppjieba/Jieba.hpp"
#include "spdlog/spdlog.h"
#include "utils/string.h"
#include "utils/time.h"
#include "utils/type.h"

namespace lisa::language {

namespace fs = std::filesystem;

std::unique_ptr<cppjieba::Jieba> LanguageHelper::jieba_ = nullptr;

LanguageHelper &LanguageHelper::instance() {
    static LanguageHelper instance;
    return instance;
}

LanguageHelper::LanguageHelper() {
    get_jieba_dict_root();
    initialize_jieba();

    get_language_data_root();
    get_unicode_pinyin_list();
    get_chinese_pinyin_phone_mapper();
    get_chinese_words_pinyin_mapper();
    get_tagging_words_pinyin_mapper();
    get_english_words_pinyin_mapper();
}

void LanguageHelper::get_jieba_dict_root() {
    if (getenv(CPPJIEBA_DICT_ROOT_ENV) == nullptr) {
        spdlog::warn("Environment variable [{}] not set, use default", CPPJIEBA_DICT_ROOT_ENV);
        const auto current_path = fs::path(__FILE__).parent_path().string();
        cppjieba_dict_root_ = current_path + CPPJIEBA_DICT_DEFAULT_RELATIVE_PATH;
    } else {
        cppjieba_dict_root_ = getenv(CPPJIEBA_DICT_ROOT_ENV);
    }
    cppjieba_dict_root_ = fs::canonical(fs::absolute(cppjieba_dict_root_)).string();
    spdlog::debug("Cppjieba dict root: [{}]", cppjieba_dict_root_);
    assert(fs::exists(cppjieba_dict_root_));
}

void LanguageHelper::initialize_jieba() {
    std::vector<std::string> dict_paths;
    for (const auto &file_name : CPPJIEBA_DICT_FILE_NAME_LIST) {
        dict_paths.push_back(cppjieba_dict_root_ + "/" + file_name);
    }
    for (const auto &path : dict_paths) {
        spdlog::debug("Cppjieba dict file path: [{}]", path);
        assert(fs::exists(path));
    }
    const auto &pts = dict_paths;
    const auto start = lisa::utils::current_ts_us();
    LanguageHelper::jieba_ = std::make_unique<cppjieba::Jieba>(pts[0], pts[1], pts[2], pts[3], pts[4]);
    spdlog::debug("Initialize cppjieba done, time cost: {} us", lisa::utils::current_ts_us() - start);
}

void LanguageHelper::get_language_data_root() {
    if (getenv(LANGUAGE_DATA_ROOT_ENV) == nullptr) {
        spdlog::warn("Environment variable [{}] not set, use default", LANGUAGE_DATA_ROOT_ENV);
        const auto current_path = fs::path(__FILE__).parent_path().string();
        language_data_root_ = current_path + LANGUAGE_DATA_DEFAULT_RELATIVE_PATH;
    } else {
        language_data_root_ = getenv(LANGUAGE_DATA_ROOT_ENV);
    }
    language_data_root_ = fs::canonical(fs::absolute(language_data_root_)).string();
    spdlog::debug("Language data root: [{}]", language_data_root_);
    assert(fs::exists(language_data_root_));
}

void LanguageHelper::get_unicode_pinyin_list() {
    auto unicode_pinyin_path = language_data_root_ + "/" + UNICODE_PINYIN_FILE_NAME;
    spdlog::debug("Unicode pinyin file path: [{}]", unicode_pinyin_path);
    assert(fs::exists(unicode_pinyin_path));
    std::ifstream ifs(unicode_pinyin_path, std::ios::in);
    assert(ifs.is_open());

    std::string line;
    const auto start = lisa::utils::current_ts_us();
    while (std::getline(ifs, line)) {
        unicode_pinyin_list_.push_back(line);
    }
    spdlog::debug("Reading unicode pinyin file done, time cost: {} us", lisa::utils::current_ts_us() - start);
}

void LanguageHelper::get_chinese_pinyin_phone_mapper() {
    auto chinese_pinyin_phone_mapper_path = language_data_root_ + "/" + CHINESE_PINYIN_PHONE_MAPPER_FILE_NAME;
    spdlog::debug("Chinese pinyin phone mapper file path: [{}]", chinese_pinyin_phone_mapper_path);
    assert(fs::exists(chinese_pinyin_phone_mapper_path));
    std::ifstream ifs(chinese_pinyin_phone_mapper_path, std::ios::in);
    assert(ifs.is_open());

    std::string line;
    const auto start = lisa::utils::current_ts_us();
    while (std::getline(ifs, line)) {
        const auto pos = line.find_first_of(' ');
        assert(pos != std::string::npos);
        chinese_pinyin_phone_mapper_[line.substr(0, pos)] = line.substr(pos + 1);
    }
    spdlog::debug("Reading Chinese pinyin phone mapper file done, time cost: {} us", lisa::utils::current_ts_us() - start);
}

void LanguageHelper::get_chinese_words_pinyin_mapper() {
    auto chinese_words_pinyin_mapper_path = language_data_root_ + "/" + CHINESE_WORDS_PINYIN_MAPPER_FILE_NAME;
    spdlog::debug("Chinese words pinyin mapper file path: [{}]", chinese_words_pinyin_mapper_path);
    assert(fs::exists(chinese_words_pinyin_mapper_path));
    std::ifstream ifs(chinese_words_pinyin_mapper_path, std::ios::in);
    assert(ifs.is_open());

    std::string line;
    const auto start = lisa::utils::current_ts_us();
    while (std::getline(ifs, line)) {
        const auto pos = line.find_first_of(' ');
        assert(pos != std::string::npos);
        chinese_words_pinyin_mapper_[line.substr(0, pos)] = line.substr(pos + 1);
    }
    spdlog::debug("Reading chinese words pinyin mapper file done, time cost: {} us", lisa::utils::current_ts_us() - start);
}

void LanguageHelper::get_tagging_words_pinyin_mapper() {
    auto tagging_words_pinyin_mapper_path = language_data_root_ + "/" + TAGGING_WORDS_PINYIN_MAPPER_FILE_NAME;
    spdlog::debug("Tagging words pinyin mapper file path: [{}]", tagging_words_pinyin_mapper_path);
    assert(fs::exists(tagging_words_pinyin_mapper_path));
    std::ifstream ifs(tagging_words_pinyin_mapper_path, std::ios::in);
    assert(ifs.is_open());

    std::string line;
    const auto start = lisa::utils::current_ts_us();
    while (std::getline(ifs, line)) {
        const auto pos = line.find_first_of(' ');
        assert(pos != std::string::npos);
        tagging_words_pinyin_mapper_[line.substr(0, pos)] = line.substr(pos + 1);
    }
    spdlog::debug("Reading tagging words pinyin mapper file done, time cost: {} us", lisa::utils::current_ts_us() - start);
}

void LanguageHelper::get_english_words_pinyin_mapper() {
    auto english_words_pinyin_mapper_path = language_data_root_ + "/" + ENGLISH_WORDS_PINYIN_MAPPER_FILE_NAME;
    spdlog::debug("English words pinyin mapper file path: [{}]", english_words_pinyin_mapper_path);
    assert(fs::exists(english_words_pinyin_mapper_path));
    std::ifstream ifs(english_words_pinyin_mapper_path, std::ios::in);
    assert(ifs.is_open());

    std::string line;
    const auto start = lisa::utils::current_ts_us();
    while (std::getline(ifs, line)) {
        const auto pos = line.find_first_of("  ");
        assert(pos != std::string::npos);
        english_words_pinyin_mapper_[line.substr(0, pos)] = line.substr(pos + 2);
    }
    spdlog::debug("Reading english words pinyin mapper file done, time cost: {} us", lisa::utils::current_ts_us() - start);
}

std::vector<Token> LanguageHelper::tokenize(const std::string &sentence) const {
    std::vector<Token> result;
    jieba_->Tag(sentence, result);

    for (auto &word : result) {
        if (word.second == "x" && word.first.size() == 1 && isalpha(word.first[0])) {
            word.second = "eng";
        }
    }

    if (result.size() >= 3) {
        std::vector<Token> temp{result.front()};
        for (auto i = 1U; i < result.size() - 1; ++i) {
            if (result[i].first == " " && result[i - 1].second == "eng" && result[i + 1].second == "eng") {
                continue;
            }
            temp.emplace_back(result[i]);
        }
        temp.emplace_back(result.back());
        std::swap(temp, result);
    }

    std::string logstr = "";
    for (const auto &pr : result) {
        logstr += spdlog::fmt_lib::format("({}|{})", pr.first, pr.second);
    }
    spdlog::debug("Tokenize result: [{}]", logstr);

    return result;
}

std::vector<Pronounce> LanguageHelper::pronounce(const std::string &sentence) const {
    std::vector<Pronounce> result;
    auto words = tokenize(sentence);

    std::vector<Token> adjusted_words;
    for (const auto &word : words) {
        if (word.second == "eng" && std::all_of(word.first.begin(), word.first.end(), isupper)) {
            for (const auto ch : word.first) {
                Token token{std::string(1, ch) + '.', "eng"};
                adjusted_words.push_back(std::move(token));
            }
            continue;
        }
        adjusted_words.push_back(word);
    }
    std::swap(words, adjusted_words);

    std::string pinyin_logstr = "";

    for (const auto &word : words) {
        // if (word.second == "x") {
        //     result.emplace_back(Pronounce{word.first, 0, Language::NO});
        //     continue;
        // }
        if (word.second == "eng") {
            auto pinyin_list = english_pinyin(word);
            if (!pinyin_list.empty()) {
                for (const auto &pinyin : pinyin_list) {
                    const auto pronounces = english_pronounce(pinyin);
                    result.insert(result.end(), pronounces.begin(), pronounces.end());
                }
                for (const auto &pinyin : pinyin_list) {
                    pinyin_logstr += spdlog::fmt_lib::format("({})", pinyin);
                }
                continue;
            }
        }
        const auto pinyin_list = chinese_pinyin(word);
        for (const auto &pinyin : pinyin_list) {
            pinyin_logstr += spdlog::fmt_lib::format("({})", pinyin);
        }
        for (const auto &pinyin : pinyin_list) {
            const auto pronounces = chinese_pronounce(pinyin);
            result.insert(result.end(), pronounces.begin(), pronounces.end());
        }
    }

    spdlog::debug("Pinyin result: [{}]", pinyin_logstr);
    std::string pronounce_logstr = "";
    for (const auto &[phone, tone, lang] : result) {
        pronounce_logstr += spdlog::fmt_lib::format("({}|{}|{})", phone, tone, lisa::utils::to_underlying(lang));
    }
    spdlog::debug("Pronounce result: [{}]", pronounce_logstr);

    return result;
}

std::vector<std::string> LanguageHelper::chinese_pinyin(const Token &word) const {
    const auto citer = chinese_words_pinyin_mapper_.find(word.first);
    if (citer != chinese_words_pinyin_mapper_.end()) {
        return lisa::utils::string_split_any(citer->second, " ");
    }
    const auto word_tag_str = word.first + "|" + word.second;
    const auto titer = tagging_words_pinyin_mapper_.find(word_tag_str);
    if (titer != tagging_words_pinyin_mapper_.end()) {
        return lisa::utils::string_split_any(titer->second, " ");
    }
    std::vector<std::string> result;
    const auto wword = utils::utf8_to_wstring(word.first);
    for (const auto &ch : wword) {
        if (ch < CHINESE_UNICODE_BEGIN || ch > CHINESE_UNICODE_END) {
            result.emplace_back(utils::wstring_to_utf8(std::wstring(1, ch) + L'0'));
            continue;
        }
        const auto index = ch - CHINESE_UNICODE_BEGIN;
        result.emplace_back(unicode_pinyin_list_[index]);
    }
    return result;
}

std::vector<std::string> LanguageHelper::english_pinyin(const Token &token) const {
    const auto word = lisa::utils::string_toupper(token.first);
    const auto iter = english_words_pinyin_mapper_.find(word);
    if (iter == english_words_pinyin_mapper_.end()) {
        return {};
    }
    return lisa::utils::string_split_all(iter->second, " - ");
}

std::vector<Pronounce> LanguageHelper::chinese_pronounce(const std::string &pinyin_str) const {
    const uint32_t tone = pinyin_str.back() - '0';
    const auto pinyin = pinyin_str.substr(0, pinyin_str.size() - 1);
    const auto iter = chinese_pinyin_phone_mapper_.find(pinyin);
    if (iter == chinese_pinyin_phone_mapper_.end()) {
        spdlog::trace("Pinyin [{}] not found in phone mapper", pinyin);
        return {Pronounce{pinyin, tone, Language::ZH}};
    }
    std::vector<Pronounce> result;
    const auto phone_str = iter->second;
    const auto pos = phone_str.find_first_of(' ');
    assert(pos != std::string::npos);
    result.emplace_back(Pronounce{phone_str.substr(0, pos), tone, Language::ZH});
    result.emplace_back(Pronounce{phone_str.substr(pos + 1), tone, Language::ZH});
    return result;
}

std::vector<Pronounce> LanguageHelper::english_pronounce(const std::string &pinyin) const {
    std::vector<Pronounce> result;
    auto phone_list = lisa::utils::string_split_any(pinyin, " ");
    for (auto &phone : phone_list) {
        uint32_t tone = 0;
        if (isdigit(phone.back())) {
            tone = phone.back() - '0' + 1;
            phone.pop_back();
        }
        phone = lisa::utils::string_tolower(phone);
        result.emplace_back(Pronounce{std::move(phone), tone, Language::EN});
    }
    return result;
}

} // namespace doge::language
