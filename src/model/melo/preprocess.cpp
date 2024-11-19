// cute-giggle@outlook.com

#include "model/melo/preprocess.h"

#include <algorithm>
#include <cstring>
#include <unordered_set>

#include "language/language.h"
#include "language/normalize.h"
#include "spdlog/spdlog.h"
#include "utils/string.h"

namespace lisa::model {

const std::unordered_map<std::string, uint32_t> MeloPreprocessor::symbol_id_mapper_ = {
    {" ", 0},     {"AA", 1},   {"E", 2},    {"EE", 3},    {"En", 4},   {"N", 5},    {"OO", 6},    {"V", 7},   {"a", 8},
    {"a:", 9},    {"aa", 10},  {"ae", 11},  {"ah", 12},   {"ai", 13},  {"an", 14},  {"ang", 15},  {"ao", 16}, {"aw", 17},
    {"ay", 18},   {"b", 19},   {"by", 20},  {"c", 21},    {"ch", 22},  {"d", 23},   {"dh", 24},   {"dy", 25}, {"e", 26},
    {"e:", 27},   {"eh", 28},  {"ei", 29},  {"en", 30},   {"eng", 31}, {"er", 32},  {"ey", 33},   {"f", 34},  {"g", 35},
    {"gy", 36},   {"h", 37},   {"hh", 38},  {"hy", 39},   {"i", 40},   {"i0", 41},  {"i:", 42},   {"ia", 43}, {"ian", 44},
    {"iang", 45}, {"iao", 46}, {"ie", 47},  {"ih", 48},   {"in", 49},  {"ing", 50}, {"iong", 51}, {"ir", 52}, {"iu", 53},
    {"iy", 54},   {"j", 55},   {"jh", 56},  {"k", 57},    {"ky", 58},  {"l", 59},   {"m", 60},    {"my", 61}, {"n", 62},
    {"ng", 63},   {"ny", 64},  {"o", 65},   {"o:", 66},   {"ong", 67}, {"ou", 68},  {"ow", 69},   {"oy", 70}, {"p", 71},
    {"py", 72},   {"q", 73},   {"r", 74},   {"ry", 75},   {"s", 76},   {"sh", 77},  {"t", 78},    {"th", 79}, {"ts", 80},
    {"ty", 81},   {"u", 82},   {"u:", 83},  {"ua", 84},   {"uai", 85}, {"uan", 86}, {"uang", 87}, {"uh", 88}, {"ui", 89},
    {"un", 90},   {"uo", 91},  {"uw", 92},  {"v", 7},     {"van", 94}, {"ve", 95},  {"vn", 96},   {"w", 97},  {"x", 98},
    {"y", 99},    {"z", 100},  {"zh", 101}, {"zy", 102},  {"!", 103},  {"?", 104},  {"…", 105},   {",", 106}, {".", 107},
    {"'", 108},   {"-", 109},  {"SP", 110}, {"UNK", 111},
};

std::vector<std::string> MeloPreprocessor::split(const std::string &sentence, size_t min_len) const {
    std::string temp;
    std::vector<std::string> result;
    const auto norm_sentence = lisa::SentenceNormalizer::normalize(sentence);
    static const std::unordered_set<char> sep_set = lisa::SentenceNormalizer::get_supported_punctuations();

    for (const auto &c : norm_sentence) {
        if (c == ' ' && temp.empty()) {
            continue;
        }
        temp.push_back(c);
        if (sep_set.find(c) == sep_set.end()) {
            continue;
        }
        if (temp.size() >= min_len) {
            result.emplace_back(std::move(temp));
            temp.clear();
        }
    }

    if (!temp.empty()) {
        if (temp.size() < 2 && !result.empty()) {
            result.back().append(temp);
        } else {
            result.emplace_back(std::move(temp));
        }
    }

    return result;
}

lisa::model::InputData MeloPreprocessor::preprocess(const std::string &text, float speed) const {
    std::string temp = " " + text + " ";
    const auto pronounces = lisa::language::LanguageHelper::instance().pronounce(temp);

    std::vector<int64_t> phones;
    std::vector<int64_t> tones;
    std::vector<int64_t> langs;

    for (auto [phone, tone, lang] : pronounces) {
        langs.push_back(ZH_LANG_ID);
        phones.push_back(get_symbol_id(phone));
        tones.push_back(lang == lisa::language::Language::EN ? tone + EN_TONE_START : tone + ZH_TONE_START);
    }

    tones = add_blank(tones);
    langs = add_blank(langs);
    phones = add_blank(phones);

    std::vector<uint8_t> phones_buffer(phones.size() * sizeof(int64_t));
    std::copy(phones.begin(), phones.end(), reinterpret_cast<int64_t *>(phones_buffer.data()));
    std::vector<uint8_t> tones_buffer(tones.size() * sizeof(int64_t));
    std::copy(tones.begin(), tones.end(), reinterpret_cast<int64_t *>(tones_buffer.data()));
    std::vector<uint8_t> langs_buffer(langs.size() * sizeof(int64_t));
    std::copy(langs.begin(), langs.end(), reinterpret_cast<int64_t *>(langs_buffer.data()));

    lisa::model::InputData result;
    const size_t seq_len = phones.size();

    result["phones"] = lisa::model::Data{std::move(phones_buffer), {1, seq_len}};
    result["tones"] = lisa::model::Data{std::move(tones_buffer), {1, seq_len}};
    result["langs"] = lisa::model::Data{std::move(langs_buffer), {1, seq_len}};

    std::vector<uint8_t> speed_buffer(sizeof(float));
    *reinterpret_cast<float *>(speed_buffer.data()) = speed;
    std::vector<uint8_t> speakers(sizeof(int64_t));
    *reinterpret_cast<int64_t *>(speakers.data()) = SPEAKERS_ID;
    std::vector<uint8_t> phones_len(sizeof(int64_t));
    *reinterpret_cast<int64_t *>(phones_len.data()) = seq_len;

    result["speed"] = lisa::model::Data{std::move(speed_buffer), {1}};
    result["speakers"] = lisa::model::Data{std::move(speakers), {1}};
    result["phones_len"] = lisa::model::Data{std::move(phones_len), {1}};

    std::vector<uint8_t> bert1(1 * 1024 * seq_len * sizeof(float), 0);
    std::vector<uint8_t> bert2(1 * 768 * seq_len * sizeof(float), 0);

    result["bert1"] = lisa::model::Data{std::move(bert1), {1, 1024, seq_len}};
    result["bert2"] = lisa::model::Data{std::move(bert2), {1, 768, seq_len}};

    return result;
}

uint32_t MeloPreprocessor::get_symbol_id(const std::string &symbol) const {
    auto it = symbol_id_mapper_.find(symbol);
    if (it == symbol_id_mapper_.end()) {
        spdlog::warn("Melo preprocess unknown symbol: {}", symbol);
        return symbol_id_mapper_.at("UNK");
    }
    return it->second;
}

} // namespace doge::model
