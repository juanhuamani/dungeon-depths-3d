#pragma once

#include <string>
#include <unordered_map>
#include <memory>

// Forward declaration of miniaudio types to avoid including miniaudio.h in the header
struct ma_engine;
struct ma_sound;

namespace audio {

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool init();
    void shutdown();

    bool loadSound(const std::string& name, const std::string& filepath);
    void playSound(const std::string& name);

    void playMusic(const std::string& filepath, bool loop = true);
    void stopMusic();

private:
    std::unique_ptr<ma_engine> engine_;
    std::unique_ptr<ma_sound> music_;
    std::unordered_map<std::string, std::unique_ptr<ma_sound>> sounds_;
    
    bool initialized_ = false;
};

} // namespace audio
