#define MINIAUDIO_IMPLEMENTATION
#include "AudioManager.h"
#include "Colors.h"
#include <algorithm>

AudioManager* AudioManager::instance = nullptr;

AudioManager::AudioManager() {
    initialized = false;
    musicLoaded = false;
    musicPlaying = false;
}

AudioManager::~AudioManager() {
    cleanup();
}

AudioManager& AudioManager::getInstance() {
    if (!instance) {
        instance = new AudioManager();
    }
    return *instance;
}

bool AudioManager::initialize() {
    if (initialized) {
        return true;
    }
    
    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        Colors::printError("?"\);
        return false;
    }
    
    initialized = true;
    Colors::printSuccess("");
    return true;
}

bool AudioManager::playBackgroundMusic(const string& filePath) {
    if (!initialized) {
        Colors::printWarning("");
        return false;
    }
    
    // ?    if (musicLoaded.load()) {
        ma_sound_stop(&backgroundMusic);
        ma_sound_uninit(&backgroundMusic);
        musicLoaded = false;
        musicPlaying = false;
    }
    
    ma_result result = ma_sound_init_from_file(&engine, filePath.c_str(), 
                                              MA_SOUND_FLAG_STREAM, NULL, NULL, &backgroundMusic);
    if (result != MA_SUCCESS) {
        Colors::printError(": " + filePath);
        return false;
    }
    
    // 
    ma_sound_set_looping(&backgroundMusic, MA_TRUE);
    
    //  ()
    ma_sound_set_volume(&backgroundMusic, 0.6f);
    
    result = ma_sound_start(&backgroundMusic);
    if (result != MA_SUCCESS) {
        Colors::printError("");
        ma_sound_uninit(&backgroundMusic);
        return false;
    }
    
    musicLoaded = true;
    musicPlaying = true;
    Colors::printSuccess("?"\);
    return true;
}

void AudioManager::stopBackgroundMusic() {
    if (musicLoaded.load() && musicPlaying.load()) {
        ma_sound_stop(&backgroundMusic);
        musicPlaying = false;
        Colors::printInfo("?"\);
    }
}

bool AudioManager::playSoundEffect(const string& filePath) {
    if (!initialized) {
        return false;
    }
    
    // ?    ma_result result = ma_engine_play_sound(&engine, filePath.c_str(), NULL);
    return (result == MA_SUCCESS);
}

void AudioManager::setBackgroundMusicVolume(float volume) {
    if (musicLoaded.load()) {
        // 
        volume = std::clamp(volume, 0.0f, 1.0f);
        ma_sound_set_volume(&backgroundMusic, volume);
    }
}

void AudioManager::setSoundEffectVolume(float volume) {
    if (initialized) {
        // ?        volume = std::clamp(volume, 0.0f, 1.0f);
        ma_engine_set_volume(&engine, volume);
    }
}

void AudioManager::cleanup() {
    if (musicLoaded.load()) {
        ma_sound_stop(&backgroundMusic);
        ma_sound_uninit(&backgroundMusic);
        musicLoaded = false;
        musicPlaying = false;
    }
    
    if (initialized) {
        ma_engine_uninit(&engine);
        initialized = false;
    }
}

