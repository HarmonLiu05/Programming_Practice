#ifndef SKILL_H
#define SKILL_H

#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Colors.h"

using namespace std;

// 前置声明
class Character;
class Player;
class Enemy;

// 技能效果类型枚举
enum class SkillType {
    EXECUTE,         // 致死打击
    LIFE_SIPHON,     // 生命吸取
    REVENGE,         // 复仇
    MARKED_FOR_DEATH,// 死亡标记
    CRITICAL_HIT,    // 致命一击
    WEAKNESS_STRIKE, // 虚弱打击
    WEALTH_STRIKE,   // 财富一击
    LUCKY_DICE,      // 运气骰子
    REVERSAL         // 起死回生
};

// 技能稀有度
enum class SkillRarity {
    COMMON,      // 普通（白色）
    UNCOMMON,    // 不常见（绿色）
    RARE,        // 稀有（蓝色）
    EPIC,        // 史诗（紫色）
    LEGENDARY    // 传说（橙色）
};

class Skill {
private:
    string name;
    string description;
    SkillType type;
    SkillRarity rarity;
    bool isUsed;         // 是否已使用（一次性技能）
    
public:
    // 构造函数
    Skill(string n, string desc, SkillType t, SkillRarity rar) 
        : name(n), description(desc), type(t), rarity(rar), isUsed(false) {}
    
    // 使用技能 - 返回是否成功使用
    bool useSkill(Player& user, Character& target);
    
    // 检查是否可以使用
    bool canUse(Player& user) const;
    
    // 使用技能
    void use() {
        isUsed = true;
    }
    
    // 重置技能使用状态
    void reset() {
        isUsed = false;
    }
    
    // 设置技能使用状态（用于加载存档）
    void setUsed(bool used) {
        isUsed = used;
    }
    
    // 显示技能信息
    void displayInfo() const {
        // 根据稀有度显示不同颜色
        switch(rarity) {
            case SkillRarity::COMMON:
                Colors::printWhite(name);
                break;
            case SkillRarity::UNCOMMON:
                Colors::printGreen(name);
                break;
            case SkillRarity::RARE:
                Colors::printBlue(name);
                break;
            case SkillRarity::EPIC:
                Colors::printMagenta(name);
                break;
            case SkillRarity::LEGENDARY:
                Colors::printYellow(name);
                break;
        }
        
        cout << " - " << description << endl;
        cout << "    ";
        Colors::printGray("[一次性]");
        cout << endl;
    }
    
    // 显示简短信息（用于战斗菜单）
    void displayShortInfo() const {
        switch(rarity) {
            case SkillRarity::COMMON: Colors::printWhite(name); break;
            case SkillRarity::UNCOMMON: Colors::printGreen(name); break;
            case SkillRarity::RARE: Colors::printBlue(name); break;
            case SkillRarity::EPIC: Colors::printMagenta(name); break;
            case SkillRarity::LEGENDARY: Colors::printYellow(name); break;
        }
        cout << " ";
        Colors::printGray("[一次性]");
    }
    
    // Getters
    string getName() const { return name; }
    string getDescription() const { return description; }
    SkillType getType() const { return type; }
    SkillRarity getRarity() const { return rarity; }
    bool getIsUsed() const { return isUsed; }
};

// 技能工厂类 - 用于创建预定义的技能
class SkillFactory {
public:
    // 致死打击
    static Skill* createExecute() {
        return new Skill("致死打击", "对生命值较低的敌人造成毁灭性打击（伤害随敌人血量降低而增加）", 
                        SkillType::EXECUTE, SkillRarity::EPIC);
    }
    
    // 生命吸取
    static Skill* createLifeSiphon() {
        return new Skill("生命吸取", "攻击敌人并吸取生命值（110%伤害，50%转为治疗）", 
                        SkillType::LIFE_SIPHON, SkillRarity::UNCOMMON);
    }
    
    // 复仇
    static Skill* createRevenge() {
        return new Skill("复仇", "低血量时的强力反击（生命值低于40%时可用，250%伤害）", 
                        SkillType::REVENGE, SkillRarity::RARE);
    }
    
    // 死亡标记
    static Skill* createMarkedForDeath() {
        return new Skill("死亡标记", "标记敌人，使其在5回合内受到150%伤害", 
                        SkillType::MARKED_FOR_DEATH, SkillRarity::RARE);
    }
    
    // 致命一击
    static Skill* createCriticalHit() {
        return new Skill("致命一击", "下次攻击150%-200%伤害，10%概率300%", 
                        SkillType::CRITICAL_HIT, SkillRarity::UNCOMMON);
    }
    
    // 虚弱打击
    static Skill* createWeaknessStrike() {
        return new Skill("虚弱打击", "攻击并使敌人下次受伤+50%", 
                        SkillType::WEAKNESS_STRIKE, SkillRarity::COMMON);
    }
    
    // 财富一击
    static Skill* createWealthStrike() {
        return new Skill("财富一击", "伤害基于金币数量（金币×0.1%）", 
                        SkillType::WEALTH_STRIKE, SkillRarity::LEGENDARY);
    }
    
    // 运气骰子
    static Skill* createLuckyDice() {
        return new Skill("运气骰子", "投掷骰子决定伤害倍率（1-6点×50%）", 
                        SkillType::LUCKY_DICE, SkillRarity::UNCOMMON);
    }
    
    // 起死回生
    static Skill* createReversal() {
        return new Skill("起死回生", "血量越少伤害越高（低血量时400%伤害）", 
                        SkillType::REVERSAL, SkillRarity::LEGENDARY);
    }
    
    // 随机创建技能
    static Skill* createRandomSkill() {
        vector<Skill*(*)()> skillCreators = {
            createExecute, createLifeSiphon, createRevenge, createMarkedForDeath,
            createCriticalHit, createWeaknessStrike, createWealthStrike,
            createLuckyDice, createReversal
        };
        
        int randomIndex = rand() % skillCreators.size();
        return skillCreators[randomIndex]();
    }
    
    // 根据名称创建技能（用于加载存档）
    static Skill* createSkillByName(const string& name) {
        if (name == "致死打击") return createExecute();
        else if (name == "生命吸取") return createLifeSiphon();
        else if (name == "复仇") return createRevenge();
        else if (name == "死亡标记") return createMarkedForDeath();
        else if (name == "致命一击") return createCriticalHit();
        else if (name == "虚弱打击") return createWeaknessStrike();
        else if (name == "财富一击") return createWealthStrike();
        else if (name == "运气骰子") return createLuckyDice();
        else if (name == "起死回生") return createReversal();
        else return createCriticalHit(); // 默认返回致命一击
    }
    
    // 根据稀有度创建随机技能
    static Skill* createRandomSkillByRarity(SkillRarity rarity) {
        switch(rarity) {
            case SkillRarity::COMMON:
                return createWeaknessStrike();
            case SkillRarity::UNCOMMON:
                return rand() % 3 == 0 ? createLifeSiphon() : 
                       rand() % 2 == 0 ? createCriticalHit() : createLuckyDice();
            case SkillRarity::RARE:
                return rand() % 2 == 0 ? createRevenge() : createMarkedForDeath();
            case SkillRarity::EPIC:
                return createExecute();
            case SkillRarity::LEGENDARY:
                return rand() % 2 == 0 ? createWealthStrike() : createReversal();
            default:
                return createWeaknessStrike();
        }
    }
};

#endif
