#ifndef SOUND_H
#define SOUND_H

#ifndef NOMINMAX
#define NOMINMAX
#endif
#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MA_NO_GENERATION
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_NODE_GRAPH
#define MA_NO_ENGINE
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <vector>
#include <mutex>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <algorithm>

enum SoundId {
    SND_CLICK,
    SND_PLACE,
    SND_SHOOT,
    SND_ENEMY_SHOOT,
    SND_HIT,
    SND_HURT,
    SND_DEATH,
    SND_COIN,
    SND_JUMP,
    SND_WIN,
    SND_LOSE,
    SND_COUNT
};

enum SoundWave { WAVE_SINE, WAVE_SQUARE, WAVE_SAW };

const int SFX_RATE = 44100;
const float SFX_VOLUME = 0.6f;
const size_t SFX_MAX_VOICES = 16;

struct SfxVoice {
    int id;
    size_t pos;
};

std::vector<float> sfx[SND_COUNT];
std::vector<SfxVoice> voices;
std::mutex voiceLock;
std::chrono::steady_clock::time_point sfxLast[SND_COUNT];
bool sfxReady = false;
bool muted = false;
ma_device sfxDevice;
#ifdef SFX_TEST_NULL_BACKEND
ma_context sfxContext;
#endif

void sfxNote(std::vector<float>& out, float freqStart, float freqEnd, float seconds, int wave, float volume, float noiseMix) {
    int n = (int)(seconds * SFX_RATE);
    float phase = 0.0f;
    uint32_t seed = 12345u;
    for (int i = 0; i < n; i++) {
        float t = (float)i / (float)n;
        float freq = freqStart + (freqEnd - freqStart) * t;
        phase += 6.2831853f * freq / SFX_RATE;
        if (phase > 6.2831853f) phase -= 6.2831853f;
        float tone;
        if (wave == WAVE_SQUARE) tone = phase < 3.1415926f ? 1.0f : -1.0f;
        else if (wave == WAVE_SAW) tone = phase / 3.1415926f - 1.0f;
        else tone = std::sin(phase);
        seed = seed * 1664525u + 1013904223u;
        float noise = (float)(seed >> 8) / 8388608.0f - 1.0f;
        float attack = std::min(1.0f, (float)i / (0.002f * SFX_RATE));
        float decay = (1.0f - t) * (1.0f - t);
        out.push_back(volume * attack * decay * (tone * (1.0f - noiseMix) + noise * noiseMix));
    }
}

void sfxBuild() {
    sfxNote(sfx[SND_CLICK], 1000.0f, 700.0f, 0.05f, WAVE_SQUARE, 0.25f, 0.0f);
    sfxNote(sfx[SND_PLACE], 350.0f, 700.0f, 0.10f, WAVE_SINE, 0.5f, 0.0f);
    sfxNote(sfx[SND_SHOOT], 1400.0f, 500.0f, 0.10f, WAVE_SQUARE, 0.22f, 0.0f);
    sfxNote(sfx[SND_ENEMY_SHOOT], 600.0f, 220.0f, 0.13f, WAVE_SAW, 0.25f, 0.0f);
    sfxNote(sfx[SND_HIT], 180.0f, 90.0f, 0.08f, WAVE_SINE, 0.5f, 0.5f);
    sfxNote(sfx[SND_HURT], 300.0f, 120.0f, 0.16f, WAVE_SAW, 0.35f, 0.2f);
    sfxNote(sfx[SND_DEATH], 500.0f, 50.0f, 0.30f, WAVE_SQUARE, 0.3f, 0.15f);
    sfxNote(sfx[SND_COIN], 988.0f, 988.0f, 0.06f, WAVE_SINE, 0.35f, 0.0f);
    sfxNote(sfx[SND_COIN], 1319.0f, 1319.0f, 0.16f, WAVE_SINE, 0.35f, 0.0f);
    sfxNote(sfx[SND_JUMP], 250.0f, 800.0f, 0.14f, WAVE_SINE, 0.35f, 0.0f);
    float winNotes[4] = { 523.0f, 659.0f, 784.0f, 1047.0f };
    for (int i = 0; i < 4; i++) sfxNote(sfx[SND_WIN], winNotes[i], winNotes[i], i == 3 ? 0.3f : 0.14f, WAVE_SQUARE, 0.25f, 0.0f);
    float loseNotes[4] = { 392.0f, 349.0f, 311.0f, 233.0f };
    for (int i = 0; i < 4; i++) sfxNote(sfx[SND_LOSE], loseNotes[i], loseNotes[i], i == 3 ? 0.4f : 0.2f, WAVE_SAW, 0.3f, 0.0f);
}

void sfxMix(float* out, unsigned int frames) {
    for (unsigned int i = 0; i < frames; i++) out[i] = 0.0f;
    std::lock_guard<std::mutex> lock(voiceLock);
    for (size_t v = 0; v < voices.size();) {
        const std::vector<float>& data = sfx[voices[v].id];
        size_t pos = voices[v].pos;
        size_t count = std::min((size_t)frames, data.size() - pos);
        for (size_t i = 0; i < count; i++) out[i] += data[pos + i] * SFX_VOLUME;
        voices[v].pos += count;
        if (voices[v].pos >= data.size()) voices.erase(voices.begin() + v);
        else ++v;
    }
    for (unsigned int i = 0; i < frames; i++) out[i] = std::max(-1.0f, std::min(1.0f, out[i]));
}

void sfxCallback(ma_device* device, void* output, const void* input, ma_uint32 frames) {
    sfxMix((float*)output, frames);
}

void sfxInit() {
    sfxBuild();
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = ma_format_f32;
    config.playback.channels = 1;
    config.sampleRate = SFX_RATE;
    config.dataCallback = sfxCallback;
#ifdef SFX_TEST_NULL_BACKEND
    ma_backend backends[1] = { ma_backend_null };
    if (ma_context_init(backends, 1, NULL, &sfxContext) != MA_SUCCESS) return;
    if (ma_device_init(&sfxContext, &config, &sfxDevice) != MA_SUCCESS) return;
#else
    if (ma_device_init(NULL, &config, &sfxDevice) != MA_SUCCESS) return;
#endif
    if (ma_device_start(&sfxDevice) != MA_SUCCESS) {
        ma_device_uninit(&sfxDevice);
        return;
    }
    sfxReady = true;
}

void sfxShutdown() {
    if (!sfxReady) return;
    ma_device_uninit(&sfxDevice);
#ifdef SFX_TEST_NULL_BACKEND
    ma_context_uninit(&sfxContext);
#endif
    sfxReady = false;
}

void playSound(int id) {
    if (!sfxReady || muted) return;
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    if (std::chrono::duration<float>(now - sfxLast[id]).count() < 0.06f) return;
    sfxLast[id] = now;
    std::lock_guard<std::mutex> lock(voiceLock);
    if (voices.size() < SFX_MAX_VOICES) voices.push_back({ id, 0 });
}

#endif
