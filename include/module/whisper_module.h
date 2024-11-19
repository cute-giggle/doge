// cute-giggle@outlook.com

#ifndef MODULE_WHISPER_MODULE_H_
#define MODULE_WHISPER_MODULE_H_

#include <cassert>
#include <filesystem>
#include <string>

#include "message/audio_message.h"
#include "message/message_center.h"
#include "module/module.h"
#include "whisper.h"

namespace lisa {

namespace module {

class WhisperModule final : public Module {
public:
    explicit WhisperModule(const std::string &module_name, const std::string &model_path,
                           const whisper_context_params &ctx_params, const whisper_full_params &full_params)
        : Module(module_name), model_path_(model_path), ctx_params_(ctx_params), full_params_(full_params) {}

private:
    void initialize() override {
        assert(!running());
        assert(ctx_ == nullptr);
        assert(std::filesystem::exists(model_path_));
        ctx_ = whisper_init_from_file_with_params(model_path_.c_str(), ctx_params_);
        assert(ctx_ != nullptr);
        queue_ = message::MessageCenter::instance().get_queue(name());
        assert(queue_ != nullptr);
    }

    void finalize() override {
        assert(!running());
        assert(ctx_ != nullptr);
        whisper_free(ctx_);
        ctx_ = nullptr;
    }

    void loop() override {
        auto msg = queue_->wait();
        if (!running()) {
            return;
        }
        if (!msg) {
            static constexpr auto time_sleep = std::chrono::milliseconds(100);
            std::this_thread::sleep_for(time_sleep);
        }
        auto audio_msg = std::dynamic_pointer_cast<message::AudioMessage>(msg);
        assert(audio_msg);

        static constexpr auto processor_count = 1;
        if (whisper_full_parallel(ctx_, full_params_, audio_msg->data_.data(), audio_msg->data_.size(), processor_count) != 0) {
            fprintf(stderr, "failed to process audio\n");
            return;
        }

        std::string text{};
        const int n_segments = whisper_full_n_segments(ctx_);
        for (int i = 0; i < n_segments; ++i) {
            text += whisper_full_get_segment_text(ctx_, i);
            text.push_back('.');
        }
        printf("%s\n", text.c_str());

        whisper_print_timings(ctx_);
    }

private:
    std::string model_path_{};
    whisper_context_params ctx_params_{};
    whisper_context *ctx_{};
    whisper_full_params full_params_{};
    std::shared_ptr<message::MessageQueue> queue_{};
};

} // namespace module

} // namespace lisa

#endif
