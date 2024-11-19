// cute-giggle@outlook.com

#ifndef INCLUDE_MODEL_MELO_PREPROCESS_H_
#define INCLUDE_MODEL_MELO_PREPROCESS_H_

#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "model/data.h"

namespace lisa {

namespace model {

class MeloPreprocessor {
public:
    static inline constexpr auto NO_LANG_ID = 0;
    static inline constexpr auto ZH_LANG_ID = 3;

    static inline constexpr auto ZH_TONE_START = 0;
    static inline constexpr auto EN_TONE_START = 7;

    static inline constexpr auto SPEAKERS_ID = 1;

    std::vector<std::string> split(const std::string &sentence, size_t max_len = 12) const;

    lisa::model::InputData preprocess(const std::string &text, float speed = 1.0) const;

private:
    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
    std::vector<T> add_blank(const std::vector<T> &data) const {
        std::vector<T> result = {0};
        for (const auto &d : data) {
            result.push_back(d);
            result.push_back(0);
        }
        return result;
    }

    static const std::unordered_map<std::string, uint32_t> symbol_id_mapper_;

    uint32_t get_symbol_id(const std::string &symbol) const;
};

} // namespace model

} // namespace lisa

#endif
