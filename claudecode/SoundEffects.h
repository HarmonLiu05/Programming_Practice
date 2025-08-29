#ifndef SOUNDEFFECTS_H
#define SOUNDEFFECTS_H

#include <string>

// 音效文件路径常量
namespace SoundEffects {
    // 战斗音效 - 使用Windows系统声音作为临时音效
    const std::string ATTACK_SOUND = "C:\\Windows\\Media\\Windows Ding.wav";
    const std::string CRITICAL_SOUND = "C:\\Windows\\Media\\Windows Exclamation.wav";
    const std::string SKILL_SOUND = "C:\\Windows\\Media\\Windows Notify.wav";
    const std::string ITEM_SOUND = "C:\\Windows\\Media\\Windows Navigation Start.wav";
    const std::string VICTORY_SOUND = "C:\\Windows\\Media\\Windows Notify System Generic.wav";
    const std::string DEFEAT_SOUND = "C:\\Windows\\Media\\Windows Critical Stop.wav";
    const std::string ESCAPE_SOUND = "C:\\Windows\\Media\\Windows Pop-up Blocked.wav";
    
    // 界面音效
    const std::string MENU_SELECT = "C:\\Windows\\Media\\Windows Hardware Insert.wav";
    const std::string LEVEL_UP = "C:\\Windows\\Media\\Windows Notify Messaging.wav";
}

#endif