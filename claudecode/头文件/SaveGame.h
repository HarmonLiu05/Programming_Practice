#ifndef SAVEGAME_H
#define SAVEGAME_H

#include <string>
#include <fstream>
#include <iostream>
#include "Player.h"
#include "Map.h"
#include "Colors.h"
using namespace std;

struct GameData {
    // 玩家数据
    string playerName;
    int level;
    int hp, maxHP;
    int attack, defense;
    int exp, gold;
    vector<string> skillNames;  // 移除了skillsUsed，因为技能使用后就删除了
    
    // 地图数据
    int playerX, playerY;
    int monstersDefeated;
    bool roomVisited[3][7];    // HEIGHT × WIDTH
    bool roomCleared[3][7];
    
    GameData() {
        // 初始化数组
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 7; j++) {
                roomVisited[i][j] = false;
                roomCleared[i][j] = false;
            }
        }
    }
};

class SaveGame {
private:
    static const string SAVE_FILE;
    
public:
    // 检查是否存在存档文件
    static bool hasSaveFile() {
        ifstream file(SAVE_FILE);
        return file.good();
    }
    
    // 保存游戏数据
    static bool saveGame(const Player* player, const Map* map) {
        try {
            ofstream file(SAVE_FILE);
            if (!file.is_open()) {
                Colors::printError("无法创建存档文件！");
                return false;
            }
            
            // 保存玩家数据
            file << "[PLAYER]" << endl;
            file << "name=" << player->getName() << endl;
            file << "level=" << player->getLevel() << endl;
            file << "hp=" << player->getHP() << endl;
            file << "maxHP=" << player->getMaxHP() << endl;
            file << "attack=" << player->getAttack() << endl;
            file << "defense=" << player->getDefense() << endl;
            file << "exp=" << player->getExp() << endl;
            file << "gold=" << player->getGold() << endl;
            
            // 保存技能数据（只保存技能名称，不保存使用状态）
            const vector<Skill*>& skills = player->getSkills();
            file << "skillCount=" << skills.size() << endl;
            for (size_t i = 0; i < skills.size(); i++) {
                file << "skill" << i << "=" << skills[i]->getName() << endl;
            }
            
            // 保存地图数据
            file << "[MAP]" << endl;
            file << "playerX=" << map->getPlayerX() << endl;
            file << "playerY=" << map->getPlayerY() << endl;
            file << "monstersDefeated=" << map->getMonstersDefeated() << endl;
            
            // 保存房间状态 (3x7网格)
            file << "[ROOMS]" << endl;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 7; j++) {
                    file << "room[" << i << "][" << j << "].visited=" << (map->isRoomVisited(j, i) ? 1 : 0) << endl;
                    file << "room[" << i << "][" << j << "].cleared=" << (map->isRoomCleared(j, i) ? 1 : 0) << endl;
                }
            }
            
            file.close();
            Colors::printSuccess("游戏已保存！");
            return true;
            
        } catch (const exception& e) {
            Colors::printError("保存游戏时出错: " + string(e.what()));
            return false;
        }
    }
    
    // 加载游戏数据
    static GameData* loadGame() {
        try {
            ifstream file(SAVE_FILE);
            if (!file.is_open()) {
                Colors::printError("无法打开存档文件！");
                return nullptr;
            }
            
            GameData* data = new GameData();
            string line, section;
            
            while (getline(file, line)) {
                // 跳过空行
                if (line.empty()) continue;
                
                // 检查是否是节段标记
                if (line[0] == '[' && line[line.length()-1] == ']') {
                    section = line;
                    continue;
                }
                
                // 解析键值对
                size_t equalPos = line.find('=');
                if (equalPos == string::npos) continue;
                
                string key = line.substr(0, equalPos);
                string value = line.substr(equalPos + 1);
                
                if (section == "[PLAYER]") {
                    parsePlayerData(data, key, value);
                } else if (section == "[MAP]") {
                    parseMapData(data, key, value);
                } else if (section == "[ROOMS]") {
                    parseRoomData(data, key, value);
                }
            }
            
            file.close();
            Colors::printSuccess("游戏存档已加载！");
            return data;
            
        } catch (const exception& e) {
            Colors::printError("加载游戏时出错: " + string(e.what()));
            return nullptr;
        }
    }
    
    // 删除存档文件
    static bool deleteSave() {
        if (remove(SAVE_FILE.c_str()) == 0) {
            Colors::printSuccess("存档已删除！");
            return true;
        } else {
            Colors::printWarning("删除存档失败或存档不存在。");
            return false;
        }
    }
    
private:
    // 解析玩家数据
    static void parsePlayerData(GameData* data, const string& key, const string& value) {
        if (key == "name") data->playerName = value;
        else if (key == "level") data->level = stoi(value);
        else if (key == "hp") data->hp = stoi(value);
        else if (key == "maxHP") data->maxHP = stoi(value);
        else if (key == "attack") data->attack = stoi(value);
        else if (key == "defense") data->defense = stoi(value);
        else if (key == "exp") data->exp = stoi(value);
        else if (key == "gold") data->gold = stoi(value);
        else if (key == "skillCount") {
            int count = stoi(value);
            data->skillNames.resize(count);
        }
        else if (key.substr(0, 5) == "skill" && key.find("Used") == string::npos) {
            // skill0, skill1, etc. 
            int index = stoi(key.substr(5));
            if (index < (int)data->skillNames.size()) {
                data->skillNames[index] = value;
            }
        }
    }
    
    // 解析地图数据
    static void parseMapData(GameData* data, const string& key, const string& value) {
        if (key == "playerX") data->playerX = stoi(value);
        else if (key == "playerY") data->playerY = stoi(value);
        else if (key == "monstersDefeated") data->monstersDefeated = stoi(value);
    }
    
    // 解析房间数据
    static void parseRoomData(GameData* data, const string& key, const string& value) {
        // 解析类似 "room[0][1].visited" 格式的键
        if (key.find("room[") == 0) {
            size_t start1 = key.find('[') + 1;
            size_t end1 = key.find(']');
            size_t start2 = key.find('[', end1) + 1;
            size_t end2 = key.find(']', end1 + 1);
            
            int row = stoi(key.substr(start1, end1 - start1));
            int col = stoi(key.substr(start2, end2 - start2));
            
            if (row >= 0 && row < 3 && col >= 0 && col < 7) {
                if (key.find(".visited") != string::npos) {
                    data->roomVisited[row][col] = (stoi(value) == 1);
                } else if (key.find(".cleared") != string::npos) {
                    data->roomCleared[row][col] = (stoi(value) == 1);
                }
            }
        }
    }
};

// 静态成员定义
const string SaveGame::SAVE_FILE = "game_save.txt";

#endif