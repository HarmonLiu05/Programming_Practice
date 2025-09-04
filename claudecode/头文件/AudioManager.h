#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include "miniaudio.h"
#include <string>
#include <iostream>
#include <thread>
#include <atomic>

using namespace std;

class AudioManager {
private:
    static AudioManager* instance;
    ma_engine engine;
    ma_sound backgroundMusic;
    bool initialized;
    atomic<bool> musicLoaded;
    atomic<bool> musicPlaying;
    
public:
    AudioManager();
    ~AudioManager();
    
    static AudioManager& getInstance();
    
    // 初始化音频系统
    bool initialize();
    
    // 播放背景音乐（循环）
    bool playBackgroundMusic(const string& filePath);
    
    // 停止背景音乐
    void stopBackgroundMusic();
    
    // 播放音效（一次性）
    bool playSoundEffect(const string& filePath);
    
    // 设置背景音乐音量 (0.0 - 1.0)
    void setBackgroundMusicVolume(float volume);
    
    // 设置音效音量 (0.0 - 1.0)
    void setSoundEffectVolume(float volume);
    
    // 检查是否已初始化
    bool isInitialized() const { return initialized; }
    
    // 检查背景音乐是否在播放
    bool isBackgroundMusicPlaying() const { return musicPlaying.load(); }
    
    // 清理资源
    void cleanup();
};

#endif