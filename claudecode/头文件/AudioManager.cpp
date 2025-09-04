#define MINIAUDIO_IMPLEMENTATION
#include "AudioManager.h"
#include "Colors.h"

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
        Colors::printError("无法初始化音频引擎");
        return false;
    }
    
    initialized = true;
    Colors::printSuccess("音频系统初始化成功！");
    return true;
}

bool AudioManager::playBackgroundMusic(const string& filePath) {
    if (!initialized) {
        Colors::printWarning("音频系统未初始化");
        return false;
    }
    
    // 如果已有背景音乐在播放，先停止
    if (musicLoaded.load()) {
        ma_sound_stop(&backgroundMusic);
        ma_sound_uninit(&backgroundMusic);
        musicLoaded = false;
        musicPlaying = false;
    }
    
    ma_result result = ma_sound_init_from_file(&engine, filePath.c_str(), 
                                              MA_SOUND_FLAG_STREAM, NULL, NULL, &backgroundMusic);
    if (result != MA_SUCCESS) {
        Colors::printError("无法加载背景音乐: " + filePath);
        return false;
    }
    
    // 设置循环播放
    ma_sound_set_looping(&backgroundMusic, MA_TRUE);
    
    // 设置音量 (稍微降低一点，避免太吵)
    ma_sound_set_volume(&backgroundMusic, 0.6f);
    
    result = ma_sound_start(&backgroundMusic);
    if (result != MA_SUCCESS) {
        Colors::printError("无法播放背景音乐");
        ma_sound_uninit(&backgroundMusic);
        return false;
    }
    
    musicLoaded = true;
    musicPlaying = true;
    Colors::printSuccess("背景音乐开始播放");
    return true;
}

void AudioManager::stopBackgroundMusic() {
    if (musicLoaded.load() && musicPlaying.load()) {
        ma_sound_stop(&backgroundMusic);
        musicPlaying = false;
        Colors::printInfo("背景音乐已停止");
    }
}

bool AudioManager::playSoundEffect(const string& filePath) {
    if (!initialized) {
        return false;
    }
    
    // 音效不需要保持引用，播放完自动清理
    ma_result result = ma_engine_play_sound(&engine, filePath.c_str(), NULL);
    return (result == MA_SUCCESS);
}

void AudioManager::setBackgroundMusicVolume(float volume) {
    if (musicLoaded.load()) {
        // 确保音量在有效范围内
        volume = max(0.0f, min(1.0f, volume));
        ma_sound_set_volume(&backgroundMusic, volume);
    }
}

void AudioManager::setSoundEffectVolume(float volume) {
    if (initialized) {
        // 设置引擎的主音量会影响所有音效
        volume = max(0.0f, min(1.0f, volume));
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