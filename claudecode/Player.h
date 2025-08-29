#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <iostream>
#include "Colors.h"
#include "Skill.h"
#include "Item.h"
#include "AudioManager.h"
#include "SoundEffects.h"
using namespace std;

// 基础角色类
class Character {
protected:
    int hp, maxHP, attack, defense, level;
    string name;

public:
    Character(string n, int h, int a, int d, int l = 1) 
        : hp(h), maxHP(h), attack(a), defense(d), level(l), name(n) {}
    
    virtual ~Character() {}
    
    virtual void takeDamage(int damage) {
        hp -= damage;
        if (hp < 0) hp = 0;
    }
    
    virtual int dealDamage() const {
        return attack;
    }
    
    virtual bool isAlive() const {
        return hp > 0;
    }
    
    int getHP() const { return hp; }
    int getMaxHP() const { return maxHP; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getLevel() const { return level; }
    string getName() const { return name; }
    
    virtual void displayStats() const = 0;
};

// 玩家类
class Player : public Character {
private:
    int exp, gold;
    vector<Skill*> skills;      // 技能列表
    vector<Item*> inventory;    // 道具背包
    static const int MAX_SKILLS = 10;   // 最大技能数量
    static const int MAX_ITEMS = 8;     // 最大道具数量
    
public:
    Player(string playerName = "勇者") : Character(playerName, 100, 15, 5, 1) {
        exp = 0;
        gold = 100;
        
        // 初始给玩家3个随机技能
        for (int i = 0; i < 3; i++) {
            Skill* skill = SkillFactory::createRandomSkill();
            skills.push_back(skill);
        }
    }
    
    ~Player() {
        // 清理技能
        for (auto skill : skills) {
            delete skill;
        }
        // 清理道具
        for (auto item : inventory) {
            delete item;
        }
    }
    
    void gainExp(int amount) {
        exp += amount;
        Colors::printGreen("+ " + to_string(amount) + " EXP");
        cout << endl;
        
        // 检查是否升级
        int requiredExp = level * 100;
        if (exp >= requiredExp) {
            levelUp();
        }
    }
    
    void levelUp() {
        exp -= level * 100;  // 消耗经验值
        level++;
        
        // 播放升级音效
        AudioManager::getInstance().playSoundEffect(SoundEffects::LEVEL_UP);
        
        // 属性提升
        int oldMaxHP = maxHP;
        int oldAttack = attack;
        int oldDefense = defense;
        
        maxHP += 20;
        hp += 20;  // 升级时恢复血量
        attack += 3;
        defense += 2;
        
        Colors::printSeparator(50);
        Colors::printTitle("等级提升!");
        Colors::printSuccess("新等级: " + to_string(level));
        cout << "属性提升:" << endl;
        Colors::printGreen("  生命值: " + to_string(oldMaxHP) + " → " + to_string(maxHP));
        Colors::printGreen("  攻击力: " + to_string(oldAttack) + " → " + to_string(attack));  
        Colors::printGreen("  防御力: " + to_string(oldDefense) + " → " + to_string(defense));
        cout << endl;
        Colors::printSeparator(50);
    }
    
    void addGold(int amount) {
        gold += amount;
        Colors::printYellow("+ " + to_string(amount) + " 金币");
        cout << endl;
    }
    
    bool spendGold(int amount) {
        if (gold >= amount) {
            gold -= amount;
            return true;
        }
        return false;
    }
    
    void displayStats() const override {
        Colors::printTitle(name + " 状态");
        
        // 基本信息
        cout << "  ";
        Colors::printCyan("等级: ");
        Colors::printWhite(to_string(level));
        cout << endl;
        
        // 生命值显示（用颜色表示健康状况）
        cout << "  ";
        Colors::printCyan("生命值: ");
        double hpPercent = (double)hp / maxHP;
        if (hpPercent > 0.7) {
            Colors::printGreen(to_string(hp) + "/" + to_string(maxHP));
        } else if (hpPercent > 0.3) {
            Colors::printYellow(to_string(hp) + "/" + to_string(maxHP));
        } else {
            Colors::printRed(to_string(hp) + "/" + to_string(maxHP));
        }
        
        // 生命值条
        cout << " [";
        int barLength = 20;
        int filledLength = (int)(hpPercent * barLength);
        for (int i = 0; i < barLength; i++) {
            if (i < filledLength) {
                if (hpPercent > 0.7) Colors::printGreen("█");
                else if (hpPercent > 0.3) Colors::printYellow("█");
                else Colors::printRed("█");
            } else {
                Colors::printGray("░");
            }
        }
        cout << "]" << endl;
        
        // 其他属性
        cout << "  ";
        Colors::printCyan("攻击力: ");
        Colors::printRed(to_string(attack));
        cout << "    ";
        Colors::printCyan("防御力: ");
        Colors::printBlue(to_string(defense));
        cout << endl;
        
        cout << "  ";
        Colors::printCyan("经验值: ");
        Colors::printMagenta(to_string(exp) + "/" + to_string(level * 100));
        cout << "    ";
        Colors::printCyan("金币: ");
        Colors::printYellow(to_string(gold));
        cout << endl;
        
        cout << "  ";
        Colors::printCyan("技能数量: ");
        Colors::printWhite(to_string(skills.size()) + "/" + to_string(MAX_SKILLS));
        cout << endl;
        
        Colors::printSubSeparator(40);
    }
    
    bool addSkill(Skill* skill) {
        if (skills.size() >= MAX_SKILLS) {
            Colors::printWarning("技能栏已满！无法学习新技能。");
            delete skill; // 清理未使用的技能
            return false;
        }
        skills.push_back(skill);
        Colors::printSuccess("学会了新技能: " + skill->getName());
        return true;
    }
    
    // 添加道具到背包
    bool addItem(Item* item) {
        if (inventory.size() >= MAX_ITEMS) {
            Colors::printWarning("道具背包已满！无法获得新道具。");
            delete item;
            return false;
        }
        inventory.push_back(item);
        Colors::printSuccess("获得道具: " + item->getName());
        return true;
    }
    
    // 使用道具
    bool useItem(int index) {
        if (index < 0 || index >= (int)inventory.size()) {
            Colors::printError("无效的道具索引");
            return false;
        }
        
        Item* item = inventory[index];
        item->use(*this);
        
        // 使用后删除道具
        delete item;
        inventory.erase(inventory.begin() + index);
        
        return true;
    }
    
    // 在战斗中使用道具
    bool useItemInBattle(int index, Character& enemy) {
        if (index < 0 || index >= (int)inventory.size()) {
            Colors::printError("无效的道具索引");
            return false;
        }
        
        Item* item = inventory[index];
        if (!item->canUseInBattle()) {
            Colors::printWarning("这个道具不能在战斗中使用！");
            return false;
        }
        
        item->useInBattle(*this, enemy);
        
        // 使用后删除道具
        delete item;
        inventory.erase(inventory.begin() + index);
        
        return true;
    }
    
    // 显示道具背包
    void displayInventory() const {
        if (inventory.empty()) {
            Colors::printGray("道具背包为空");
            return;
        }
        
        Colors::printTitle("道具背包");
        Colors::printInfo("拥有道具: " + to_string(inventory.size()) + "/" + to_string(MAX_ITEMS));
        
        for (size_t i = 0; i < inventory.size(); i++) {
            cout << "  " << (i + 1) << ". ";
            inventory[i]->displayInfo();
            cout << endl;
        }
        Colors::printSubSeparator(40);
    }
    
    bool useSkill(int index, Character& target) {
        if (index < 0 || index >= (int)skills.size()) {
            Colors::printError("无效的技能索引");
            return false;
        }
        
        Skill* skill = skills[index];
        
        // 使用新的技能系统
        bool success = skill->useSkill(*this, target);
        
        if (success) {
            // 使用后删除技能
            Colors::printGray("技能 " + skill->getName() + " 已消耗");
            delete skill;
            skills.erase(skills.begin() + index);
        }
        
        return success;
    }
    
    void displaySkills() const {
        if (skills.empty()) {
            Colors::printGray("技能背包为空");
            return;
        }
        
        Colors::printTitle("技能背包");
        Colors::printInfo("可用技能: " + to_string(skills.size()) + "/" + to_string(MAX_SKILLS));
        
        for (size_t i = 0; i < skills.size(); i++) {
            cout << "  " << (i + 1) << ". ";
            skills[i]->displayInfo();
        }
        Colors::printSubSeparator(40);
    }
    
    // 显示技能背包（用于游戏中查看）
    void displaySkillBag() const {
        clearScreen();
        if (skills.empty()) {
            Colors::printTitle("技能背包");
            Colors::printGray("你的技能背包是空的");
            Colors::printInfo("击败敌人或在商店购买可以获得技能！");
        } else {
            Colors::printTitle("技能背包");
            Colors::printInfo("拥有技能: " + to_string(skills.size()) + "/" + to_string(MAX_SKILLS));
            
            for (size_t i = 0; i < skills.size(); i++) {
                cout << "  " << (i + 1) << ". ";
                skills[i]->displayInfo();
            }
            
            Colors::printInfo("技能使用说明: 战斗中按'2'使用技能，一次性消耗");
        }
        Colors::printSubSeparator(50);
    }
    
    // 恢复生命值
    void restoreHP(int amount) {
        int oldHp = hp;
        hp = min(maxHP, hp + amount);
        int actualRestore = hp - oldHp;
        if (actualRestore > 0) {
            Colors::printGreen("恢复了 " + to_string(actualRestore) + " 点生命值！");
        }
    }
    
    // 完全恢复生命值
    void restoreToFullHP() {
        int oldHp = hp;
        hp = maxHP;
        int actualRestore = hp - oldHp;
        if (actualRestore > 0) {
            Colors::printGreen("完全恢复！生命值: " + to_string(hp) + "/" + to_string(maxHP));
        } else {
            Colors::printInfo("你的生命值已经满了。");
        }
    }
    
    // 获取器方法
    int getExp() const { return exp; }
    int getGold() const { return gold; }
    const vector<Skill*>& getSkills() const { return skills; }
    int getSkillCount() const { return skills.size(); }
    int getMaxSkills() const { return MAX_SKILLS; }
    const vector<Item*>& getInventory() const { return inventory; }
    int getItemCount() const { return inventory.size(); }
    int getMaxItems() const { return MAX_ITEMS; }
    
    // 设置玩家名字
    void setName(const string& playerName) {
        name = playerName;
    }
    
    // 用于加载存档的方法
    void loadFromData(int newLevel, int newHp, int newMaxHP, int newAttack, int newDefense, int newExp, int newGold) {
        level = newLevel;
        hp = newHp;
        maxHP = newMaxHP;
        attack = newAttack;
        defense = newDefense;
        exp = newExp;
        gold = newGold;
    }
    
    // 清空并重新加载技能列表（不再需要skillsUsed参数，因为使用后会删除）
    void loadSkills(const vector<string>& skillNames) {
        // 清理现有技能
        for (auto skill : skills) {
            delete skill;
        }
        skills.clear();
        
        // 根据名称重新创建技能
        for (const string& skillName : skillNames) {
            Skill* skill = SkillFactory::createSkillByName(skillName);
            if (skill) {
                skills.push_back(skill);
            }
        }
    }
    
    // 添加 clearScreen 声明（如果还没有的话）
private:
    void clearScreen() const {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
};

#endif