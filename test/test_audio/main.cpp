#include "module/whisper_module.h"
#include "sndfile.h"
#include "message/message_pool.h"

#include <iostream>

int main() {
    std::vector<float> pcmf32;
    constexpr auto wav_path = "jfk.wav";
    SF_INFO sfinfo;
    SNDFILE *sf = sf_open(wav_path, SFM_READ, &sfinfo);
    if (sf == nullptr) {
        fprintf(stderr, "failed to open '%s'\n", wav_path);
        return 1;
    }
    pcmf32.resize(sfinfo.frames * sfinfo.channels);
    sf_readf_float(sf, pcmf32.data(), pcmf32.size());
    sf_close(sf);
    printf("Read %lu samples\n", pcmf32.size());
    if (pcmf32.size() < 16000) {
        pcmf32.resize(32000);
    }
    struct whisper_context_params cparams = whisper_context_default_params();
    cparams.use_gpu = false;
    cparams.flash_attn = false;
    cparams.dtw_token_timestamps = false;
    cparams.dtw_aheads_preset = WHISPER_AHEADS_TINY;
    whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    wparams.language = "en";
    lisa::module::WhisperModule whisper_module("whisper", "../model/whisper/ggml-tiny.en.bin", cparams, wparams);
    whisper_module.start();
    auto queue = lisa::message::MessageCenter::instance().get_queue(whisper_module.name());
    auto audio_msg_pool = lisa::message::MessagePool<lisa::message::AudioMessage>(10);
    for (int i = 0; i < 5; ++i) {
        auto audio_msg = audio_msg_pool.get();
        audio_msg->channels_ = 1;
        audio_msg->sample_rate_ = 16000;
        audio_msg->data_ = pcmf32;
        queue->push(audio_msg);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    whisper_module.stop();
    lisa::message::MessageCenter::instance().shutdown();

    return 0;
}