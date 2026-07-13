#include "audio/AudioManager.h"
#include <iostream>
#include "miniaudio.h"

namespace audio {

AudioManager::AudioManager() = default;

AudioManager::~AudioManager() {
    shutdown();
}

bool AudioManager::init() {
    engine_ = std::make_unique<ma_engine>();
    ma_result result = ma_engine_init(NULL, engine_.get());
    if (result != MA_SUCCESS) {
        std::cerr << "AudioManager: Failed to initialize audio engine. Error: " << result << "\n";
        return false;
    }

    music_ = std::make_unique<ma_sound>();
    initialized_ = true;
    std::cout << "AudioManager initialized.\n";
    return true;
}

void AudioManager::shutdown() {
    if (!initialized_) return;

    if (music_ && ma_sound_is_playing(music_.get())) {
        ma_sound_stop(music_.get());
        ma_sound_uninit(music_.get());
    }

    for (auto& pair : sounds_) {
        ma_sound_uninit(pair.second.get());
    }
    sounds_.clear();

    if (engine_) {
        ma_engine_uninit(engine_.get());
        engine_.reset();
    }
    
    initialized_ = false;
}

bool AudioManager::loadSound(const std::string& name, const std::string& filepath) {
    if (!initialized_) return false;

    auto sound = std::make_unique<ma_sound>();
    ma_result result = ma_sound_init_from_file(engine_.get(), filepath.c_str(), 0, NULL, NULL, sound.get());
    if (result != MA_SUCCESS) {
        std::cerr << "AudioManager: Failed to load sound: " << filepath << "\n";
        return false;
    }

    sounds_[name] = std::move(sound);
    return true;
}

void AudioManager::playSound(const std::string& name) {
    if (!initialized_) return;

    auto it = sounds_.find(name);
    if (it != sounds_.end()) {
        ma_sound_start(it->second.get());
    } else {
        std::cerr << "AudioManager: Sound not found: " << name << "\n";
    }
}

void AudioManager::playMusic(const std::string& filepath, bool loop) {
    if (!initialized_) return;

    if (ma_sound_is_playing(music_.get())) {
        ma_sound_stop(music_.get());
        ma_sound_uninit(music_.get());
    }

    ma_result result = ma_sound_init_from_file(engine_.get(), filepath.c_str(), MA_SOUND_FLAG_STREAM, NULL, NULL, music_.get());
    if (result != MA_SUCCESS) {
        std::cerr << "AudioManager: Failed to load music: " << filepath << "\n";
        return;
    }

    ma_sound_set_looping(music_.get(), loop ? MA_TRUE : MA_FALSE);
    ma_sound_start(music_.get());
}

void AudioManager::stopMusic() {
    if (!initialized_) return;

    if (ma_sound_is_playing(music_.get())) {
        ma_sound_stop(music_.get());
    }
}

} // namespace audio
