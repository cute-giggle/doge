#include <filesystem>
#include <iostream>
#include <string>

#include "language/language.h"
#include "model/melo/preprocess.h"
#include "openvino/openvino.hpp"
#include "spdlog/spdlog.h"
#include "utils/io.h"
#include "utils/string.h"
#include "utils/time.h"

int main() {
    spdlog::set_level(spdlog::level::debug);

    ov::Core core;
    const auto current_path = std::filesystem::path(__FILE__).parent_path().string();
    const auto model_path = current_path + "/../../model/melo/melo-zh-en-openvino.xml";
    ov::CompiledModel compiled_model = core.compile_model(model_path, "AUTO");
    ov::InferRequest infer_request = compiled_model.create_infer_request();

    std::string text = "Lisa";
    auto preprocesser = lisa::model::MeloPreprocessor();
    auto sentences = preprocesser.split(text, 16);
    std::vector<float> result;

    for (auto i = 0U; i < sentences.size(); ++i) {
        auto model_input = preprocesser.preprocess(sentences[i], 1.1);
        auto inputs = compiled_model.inputs();
        for (const auto &input : inputs) {
            auto name = *input.get_names().begin();
            auto data = model_input[name].buffer.data();
            auto shape = model_input[name].shape;
            ov::Tensor tensor(input.get_element_type(), shape, data);
            infer_request.set_tensor(input, tensor);
        }

        const auto start = lisa::utils::current_ts_ms();
        infer_request.start_async();
        infer_request.wait();
        spdlog::debug("Infer request done, time cost: {} ms", lisa::utils::current_ts_ms() - start);

        auto outputs = compiled_model.outputs();
        auto data = infer_request.get_tensor("output");
        std::copy(data.data<float>(), data.data<float>() + data.get_size(), std::back_inserter(result));
    }
    const auto save_path = "result.wav";
    lisa::utils::save_wav(save_path, result.data(), result.size(), 44100, 1);

    return 0;
}