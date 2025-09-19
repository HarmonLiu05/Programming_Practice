#ifndef ROOM_H
#define ROOM_H

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Colors.h"
#include "Enemy.h"
using namespace std;

enum class RoomType { MONSTER, SHOP, EMPTY, BOSS, START };

class Room {
private:
    RoomType type;
    bool visited;
    bool cleared;
    bool hasRested;  // 新增：是否已经在此房间休息过
    char symbol;
    string description;
    Enemy* enemy;  // 房间内的敌人
    
public:
    Room(RoomType t) : type(t), visited(false), cleared(false), hasRested(false), enemy(nullptr) {
        switch(type) {
            case RoomType::START:
                symbol = 'S';
                description = "起始房间 - 安全的出发点";
                cleared = true;
                break;
            case RoomType::MONSTER:
                symbol = 'M';
                description = "怪物房间 - 里面有敌人在等待";
                break;
            case RoomType::SHOP:
                symbol = '$';
                description = "商店 - 可以购买物品和技能";
                cleared = true;
                break;
            case RoomType::EMPTY:
                symbol = 'E';
                description = "空房间 - 可以在这里休息";
                cleared = true;
                break;
            case RoomType::BOSS:
                symbol = 'B';
                description = "BOSS房间 - 最终挑战！";
                break;
        }
    }
    
    ~Room() {
        if (enemy) {
            delete enemy;
        }
    }
    
    void enter() {
        if (!visited) {
            visited = true;
            Colors::printSubSeparator(50);
            
            switch(type) {
                case RoomType::START:
                    Colors::printTitle("起始房间");
                    Colors::printInfo("这里是你的冒险起点，准备好了吗？");
                    break;
                case RoomType::MONSTER:
                    if (!cleared) {
                        Colors::printTitle("怪物房间");
                        Colors::printWarning("你感受到了危险的气息...");
                    } else {
                        Colors::printTitle("空旷房间");
                        Colors::printSuccess("这个房间已经被清理过了。");
                    }
                    break;
                case RoomType::SHOP:
                    Colors::printTitle("神秘商店");
                    Colors::printSuccess("店主热情地向你挥手！");
                    break;
                case RoomType::EMPTY:
                    Colors::printTitle("宁静房间");
                    Colors::printInfo("这是个安静的房间，你可以在这里休息。");
                    break;
                case RoomType::BOSS:
                    Colors::printTitle("龙王殿");
                    Colors::printRed("空气中弥漫着强大的魔力...这里就是最终之战的地点！");
                    break;
            }
            Colors::printSubSeparator(50);
        } else {
            Colors::printGray("你又回到了这个熟悉的房间。");
        }
    }
    
    // 获取房间信息
    RoomType getType() const { return type; }
    bool isVisited() const { return visited; }
    bool isCleared() const { return cleared; }
    char getSymbol() const { 
        if (visited) {
            return symbol;
        }
        return '?';
    }
    
    string getDescription() const { return description; }
    Enemy* getEnemy() const { return enemy; }
    
    // 设置敌人（用于怪物房间和BOSS房间）
    void setEnemy(Enemy* e) { 
        if (enemy) {
            delete enemy; // 清理旧敌人
        }
        enemy = e; 
    }
    
    // 清理房间（击败怪物后）
    void clear() {
        cleared = true;
        if (enemy) {
            delete enemy;
            enemy = nullptr;
        }
    }
    
    // 检查房间是否有敌人
    bool hasEnemy() const {
        return enemy != nullptr && !cleared;
    }
    
    // 用于加载游戏时直接设置状态
    void setVisited(bool v) {
        visited = v;
    }
    
    void setCleared(bool c) {
        cleared = c;
        if (c && enemy) {
            delete enemy;
            enemy = nullptr;
        }
    }
    
    // 休息房相关方法
    bool canRest() const {
        return type == RoomType::EMPTY && !hasRested;
    }
    
    void setRested() {
        hasRested = true;
    }
    
    // 显示房间状态
    void displayRoomInfo() const {
        cout << "房间类型: ";
        switch(type) {
            case RoomType::START: cout << "起始房间"; break;
            case RoomType::MONSTER: cout << "怪物房间"; break;
            case RoomType::SHOP: cout << "商店"; break;
            case RoomType::EMPTY: cout << "空房间"; break;
            case RoomType::BOSS: cout << "BOSS房间"; break;
        }
        cout << " | 状态: " << (visited ? "已访问" : "未访问");
        if (type == RoomType::MONSTER || type == RoomType::BOSS) {
            cout << " | " << (cleared ? "已清理" : "未清理");
        }
        cout << endl;
    }
};

#endif