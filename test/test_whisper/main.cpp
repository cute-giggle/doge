#include <cstdio>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <vector>

#include "sndfile.h"
#include "whisper.h"

int main() {
    std::vector<float> pcmf32;
    constexpr auto wav_path = "result_16000.wav";
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
        pcmf32.resize(20000);
    }

    struct whisper_context_params cparams = whisper_context_default_params();
    cparams.use_gpu = false;
    cparams.flash_attn = false;
    cparams.dtw_token_timestamps = true;
    cparams.dtw_aheads_preset = WHISPER_AHEADS_TINY;

    const auto current_path = std::filesystem::path(__FILE__).parent_path().string();
    const auto model_path = current_path + "/../../model/whisper/ggml-tiny.en.bin";
    struct whisper_context *ctx = whisper_init_from_file_with_params(model_path.c_str(), cparams);
    if (ctx == nullptr) {
        fprintf(stderr, "failed to initialize whisper context\n");
        return 2;
    }

    whisper_ctx_init_openvino_encoder(ctx, nullptr, "CPU", nullptr);

    whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    wparams.language = "en";

    if (whisper_full_parallel(ctx, wparams, pcmf32.data(), pcmf32.size(), 1) != 0) {
        fprintf(stderr, "failed to process audio\n");
        return 10;
    }

    whisper_print_timings(ctx);

    const int n_segments = whisper_full_n_segments(ctx);

    std::cout << n_segments << " segments" << std::endl;

    const int s0 = 0;

    if (s0 == 0) {
        printf("\n");
    }

    for (int i = s0; i < n_segments; i++) {
        const char *text = whisper_full_get_segment_text(ctx, i);
        printf("%s\n", text);
        fflush(stdout);
    }

    whisper_free(ctx);

    return 0;
}