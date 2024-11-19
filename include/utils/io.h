// cute-giggle@outlook.com

#ifndef INCLUDE_UTILS_IO_H_
#define INCLUDE_UTILS_IO_H_

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "sndfile.h"
#include "spdlog/spdlog.h"

namespace lisa {

namespace utils {

template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T>, void> * = nullptr>
std::vector<T> read_txt(const std::string &path, const std::string &seps = " ") {
    if (!std::filesystem::exists(path)) {
        spdlog::error("File not exists! target path: {}", path);
        return {};
    }

    std::ifstream ifs(path, std::ios_base::in);
    if (!ifs.is_open()) {
        spdlog::error("Open file failed! target path: {}", path);
        return {};
    }

    std::vector<T> result;
    std::string line_buffer;
    while (std::getline(ifs, line_buffer)) {
        for (auto sep : seps) {
            std::replace(line_buffer.begin(), line_buffer.end(), sep, ' ');
        }
        std::stringstream ss(line_buffer);
        std::copy(std::istream_iterator<T>{ss}, std::istream_iterator<T>{}, std::back_inserter(result));
    }

    return result;
}

template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T>, void> * = nullptr>
bool save_txt(const std::string &path, const T *buffer, size_t size, const char sep = ' ') {
    std::ofstream ofs(path, std::ios_base::out);
    if (!ofs.is_open()) {
        spdlog::error("Open file failed! target path: {}", path);
        return false;
    }

    for (auto i = 0U; i < size; ++i) {
        ofs << buffer[i] << sep;
    }

    return ofs.good();
}

template <typename T> struct WavSaveMapping;

template <> struct WavSaveMapping<float> {
    static constexpr auto format = SF_FORMAT_FLOAT;
    static constexpr auto function = sf_write_float;
};

template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T>, void> * = nullptr>
bool save_wav(const std::string &path, const T *data, size_t size, uint32_t sample_rate, uint32_t channels) {
    SF_INFO sfinfo = {static_cast<int64_t>(size / channels),
                      static_cast<int>(sample_rate),
                      static_cast<int>(channels),
                      SF_FORMAT_WAV | WavSaveMapping<T>::format,
                      0,
                      0};

    SNDFILE *outfile = sf_open(path.c_str(), SFM_WRITE, &sfinfo);
    if (!outfile) {
        spdlog::error("Open file failed! target path: {}", path);
        return false;
    }

    auto count = WavSaveMapping<T>::function(outfile, data, size);
    sf_close(outfile);

    return count == size;
}

} // namespace utils

} // namespace lisa

#endif
