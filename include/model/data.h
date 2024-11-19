// cute-giggle@outlook.com

#ifndef INCLUDE_MODEL_DATA_H_
#define INCLUDE_MODEL_DATA_H_

#include <map>
#include <string>
#include <vector>

namespace lisa {

namespace model {

using DataShape = std::vector<size_t>;
using DataBuffer = std::vector<uint8_t>;

struct Data {
    DataBuffer buffer;
    DataShape shape;
};

using InputData = std::map<std::string, Data>;
using OutputData = std::map<std::string, Data>;

} // namespace model

} // namespace lisa

#endif
