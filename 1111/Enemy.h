#ifndef ENEMY_H
#define ENEMY_H

#include "Player.h"
#include "Skill.h"
#include "Colors.h"
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cmath>

using namespace std;

// 敌人类型枚举
enum class EnemyType {
    SLIME,     // 史莱姆
    GOBLIN,    // 哥布林  
    SKELETON,  // 骷髅兵
    ORC,       // 兽人
    DRAGON     // 龙王(BOSS)
    , SLIME_RED
    , GOBLIN_SCOUT
    , TAOIST
    , GARGOYLE
    , MIST_WRAITH
    , FALLEN_SOUL
};

// 掉落物类型
enum class DropType {
    SKILL,     // 技能
    GOLD,      // 金币
    POTION,    // 药水
    NONE       // 无掉落
};

// 掉落物结构
struct DropItem {
    DropType type;
    int value;          // 金币数量或药水效果值
    Skill* skill;       // 掉落的技能（如果是技能掉落）
    string name;        // 掉落物名称
    
    DropItem(DropType t, int v = 0, Skill* s = nullptr, string n = "")
        : type(t), value(v), skill(s), name(n) {}
};

class Enemy : public Character {
private:
    EnemyType enemyType;
    int expReward;
    int goldReward;
    vector<DropItem> dropTable;    // 掉落表
    vector<Skill*> skills;         // 敌人的技能
    string flavorText;             // 风味描述文字
    
public:
    Enemy(EnemyType type) : Character("", 0, 0, 0), enemyType(type) {
        initializeEnemy(type);
    }
    
    ~Enemy() {
        // 清理技能
        for (auto skill : skills) {
            delete skill;
        }
        // 清理掉落表中的技能
        for (auto& drop : dropTable) {
            if (drop.type == DropType::SKILL && drop.skill) {
                delete drop.skill;
            }
        }
    }
    
    void initializeEnemy(EnemyType type) {
        switch(type) {
            case EnemyType::SLIME:
                name = "绿史莱姆";
                hp = maxHP = 45;
                attack = 8;
                defense = 4;
                level = 1;
                expReward = 25;
                goldReward = 15;
                flavorText = "一只软软的绿色史莱姆，看起来很温和";
                setupSlimeDrops();
                break;
                
            case EnemyType::GOBLIN:
                name = "邪恶哥布林";
                hp = maxHP = 65;
                attack = 12;
                defense = 5;
                level = 2;
                expReward = 35;
                goldReward = 25;
                flavorText = "手持木棒的绿皮小恶魔，眼中闪烁着狡诈的光芒";
                setupGoblinDrops();
                break;
                
            case EnemyType::SKELETON:
                name = "骷髅战士";
                hp = maxHP = 85;
                attack = 15;
                defense = 7;
                level = 3;
                expReward = 45;
                goldReward = 30;
                flavorText = "骨架咯吱作响的不死战士，手持生锈的长剑";
                setupSkeletonDrops();
                break;
                
            case EnemyType::ORC:
                name = "狂暴兽人";
                hp = maxHP = 120;
                attack = 20;
                defense = 12;
                level = 4;
                expReward = 70;
                goldReward = 50;
                flavorText = "肌肉发达的绿皮战士，手持巨大的战斧";
                setupOrcDrops();
                // 给兽人添加技能
                skills.push_back(SkillFactory::createRevenge());
                break;
                
            // 新增怪物类型
            case EnemyType::SLIME_RED:
                name = "史莱姆·赤";
                hp = maxHP = 45;
                attack = 8;
                defense = 4;
                level = 1;
                expReward = 25;
                goldReward = 15;
                flavorText = "赤色史莱姆，行动迟缓";
                setupSlimeDrops();
                break;

            case EnemyType::GOBLIN_SCOUT:
                name = "哥布林斥候";
                hp = maxHP = 65;
                attack = 12;
                defense = 5;
                level = 2;
                expReward = 35;
                goldReward = 25;
                flavorText = "身手敏捷的侦查兵";
                setupGoblinDrops();
                break;

            case EnemyType::TAOIST:
                name = "幽影道士";
                hp = maxHP = 80;
                attack = 18;
                defense = 6;
                level = 4;
                expReward = 65;
                goldReward = 60;
                flavorText = "操纵阴影的术士";
                setupSkeletonDrops();
                break;

            case EnemyType::GARGOYLE:
                name = "石像鬼";
                hp = maxHP = 130;
                attack = 16;
                defense = 14;
                level = 5;
                expReward = 75;
                goldReward = 40;
                flavorText = "坚硬的石像守卫";
                setupSkeletonDrops();
                break;

            case EnemyType::MIST_WRAITH:
                name = "雾妖·崂山";
                hp = maxHP = 70;
                attack = 10;
                defense = 3;
                level = 3;
                expReward = 40;
                goldReward = 10;
                flavorText = "缥缈如雾，难以捉摸";
                setupSlimeDrops();
                break;

            case EnemyType::FALLEN_SOUL:
                name = "堕仙残魂";
                hp = maxHP = 95;
                attack = 22;
                defense = 8;
                level = 5;
                expReward = 80;
                goldReward = 100;
                flavorText = "失落的仙灵残影";
                setupOrcDrops();
                break;

            case EnemyType::DRAGON:
                name = "崂山龙王";
                hp = maxHP = 350;
                attack = 38;
                defense = 16;
                level = 10;
                expReward = 200;
                goldReward = 200;
                flavorText = "传说中的崂山守护者，巨大的身躯散发着威严的气息";
                setupDragonDrops();
                // 给龙王添加强力技能
                skills.push_back(SkillFactory::createReversal());
                skills.push_back(SkillFactory::createExecute());
                break;
        }
    }
    
    void setupSlimeDrops() {
        // 史莱姆掉落表：提升技能掉落稀有度
        dropTable.push_back(DropItem(DropType::GOLD, 10 + rand() % 11)); // 10-20金币
        dropTable.push_back(DropItem(DropType::SKILL, 0, SkillFactory::createRandomSkillByRarity(SkillRarity::UNCOMMON)));
        dropTable.push_back(DropItem(DropType::NONE));
    }
    
    void setupGoblinDrops() {
        // 哥布林掉落表：提升技能稀有度
        dropTable.push_back(DropItem(DropType::GOLD, 15 + rand() % 16)); // 15-30金币
        dropTable.push_back(DropItem(DropType::SKILL, 0, SkillFactory::createRandomSkillByRarity(SkillRarity::RARE)));
        dropTable.push_back(DropItem(DropType::POTION, 30, nullptr, "小血瓶"));
    }
    
    void setupSkeletonDrops() {
        // 骷髅兵掉落表：50%史诗技能，40%金币，10%药水
        dropTable.push_back(DropItem(DropType::SKILL, 0, SkillFactory::createRandomSkillByRarity(SkillRarity::EPIC)));
        dropTable.push_back(DropItem(DropType::GOLD, 20 + rand() % 21)); // 20-40金币
        dropTable.push_back(DropItem(DropType::POTION, 50, nullptr, "大血瓶"));
    }
    
    void setupOrcDrops() {
        // 兽人掉落表：60%传说技能，30%金币，10%药水
        dropTable.push_back(DropItem(DropType::SKILL, 0, SkillFactory::createRandomSkillByRarity(SkillRarity::LEGENDARY)));
        dropTable.push_back(DropItem(DropType::GOLD, 40 + rand() % 31)); // 40-70金币
        dropTable.push_back(DropItem(DropType::POTION, 100, nullptr, "特级血瓶"));
    }
    
    void setupDragonDrops() {
        // 龙王掉落表：100%传说技能 + 大量金币
        dropTable.push_back(DropItem(DropType::SKILL, 0, SkillFactory::createRandomSkillByRarity(SkillRarity::LEGENDARY)));
        dropTable.push_back(DropItem(DropType::GOLD, 150 + rand() % 101)); // 150-250金币
    }
    
    // 生成掉落物
    vector<DropItem> generateDrops() {
        vector<DropItem> actualDrops;
        
        for (auto& drop : dropTable) {
            int dropChance = rand() % 100;
            int threshold = 90; // 90%掉落率
            
            // 根据敌人类型调整掉落率
            switch(enemyType) {
                case EnemyType::SLIME: threshold = 95; break; // 提高史莱姆掉落率用于测试
                case EnemyType::GOBLIN: threshold = 95; break; // 提高哥布林掉落率用于测试
                case EnemyType::SKELETON: threshold = 95; break;
                case EnemyType::ORC: threshold = 95; break;
                case EnemyType::DRAGON: threshold = 100; break; // BOSS必掉
            }
            
            if (dropChance < threshold && drop.type != DropType::NONE) {
                // 创建掉落物的副本
                if (drop.type == DropType::SKILL && drop.skill) {
                    // 为技能创建新的实例
                    Skill* newSkill = SkillFactory::createRandomSkillByRarity(drop.skill->getRarity());
                    actualDrops.push_back(DropItem(drop.type, drop.value, newSkill, drop.name));
                } else {
                    actualDrops.push_back(DropItem(drop.type, drop.value, nullptr, drop.name));
                }
            }
        }
        
        return actualDrops;
    }
    
    // 敌人AI行动
    void performAction(Character& target) {
        // 龙王BOSS三阶段AI（来自 partC(2).docx）
        if (enemyType == EnemyType::DRAGON) {
            auto applyDefense = [&](int baseDmg, int def){
                double dr = std::min(0.5, def * 0.04);
                return std::max(1, (int)std::floor(baseDmg * (1.0 - dr)));
            };
            double hpPct = (double)hp / (double)maxHP;
            if (hpPct > 0.60) {
                // 阶段一：普通攻击 or 固定喷火15
                if (rand() % 100 < 50) {
                    int damage = applyDefense(attack, target.getDefense());
                    target.takeDamage(damage);
                    Colors::printRed(name + " 普通攻击，造成 " + to_string(damage) + " 点伤害！");
                } else {
                    int damage = 15; // 固定伤害
                    target.takeDamage(damage);
                    Colors::printRed(name + " 喷出火焰，造成 " + to_string(damage) + " 点灼烧伤害！");
                }
            } else if (hpPct > 0.30) {
                // 阶段二：龙息风暴（固定20）
                int damage = 20;
                target.takeDamage(damage);
                Colors::printMagenta(name + " 施放了‘龙息风暴’，造成 " + to_string(damage) + " 点伤害！");
            } else {
                // 阶段三：攻击+30%，回合末自愈15
                int boosted = (attack * 130) / 100;
                int damage = applyDefense(boosted, target.getDefense());
                target.takeDamage(damage);
                Colors::printRed(name + " 龙渊觉醒，强力一击造成 " + to_string(damage) + " 点伤害！");
                int before = hp;
                hp = std::min(maxHP, hp + 15);
                int healed = hp - before;
                if (healed > 0) Colors::printGreen(name + " 回复了 " + to_string(healed) + " 点生命。");
            }
            return;
        }
        if (skills.empty() || rand() % 100 < 70) {
            // 70%几率普通攻击
            int damage = std::max(1, (int)std::floor(attack * (1.0 - std::min(0.5, target.getDefense() * 0.04))));
            target.takeDamage(damage);
            Colors::printRed(name + " 攻击了你，造成 " + to_string(damage) + " 点伤害！");
        } else {
            // 使用技能攻击
            Skill* chosenSkill = skills[rand() % skills.size()];
            if (!chosenSkill->getIsUsed()) {
                int damage = (attack * 130) / 100; // 简化：1.3倍伤害
                if (damage > 0) {
                    target.takeDamage(damage);
                    Colors::printMagenta(name + " 使用了 ");
                    chosenSkill->displayShortInfo();
                    Colors::printRed("，造成 " + to_string(damage) + " 点伤害！");
                }
                chosenSkill->use();
            } else {
                // 技能已用完，普通攻击
                int damage = std::max(1, (int)std::floor(attack * (1.0 - std::min(0.5, target.getDefense() * 0.04))));
                target.takeDamage(damage);
                Colors::printRed(name + " 攻击了你，造成 " + to_string(damage) + " 点伤害！");
            }
        }
    }
    
    void displayStats() const override {
        // 根据敌人类型显示不同颜色
        switch(enemyType) {
            case EnemyType::SLIME:
                Colors::printGreen(name);
                break;
            case EnemyType::GOBLIN:
                Colors::printYellow(name);
                break;
            case EnemyType::SKELETON:
                Colors::printGray(name);
                break;
            case EnemyType::ORC:
                Colors::printRed(name);
                break;
            case EnemyType::DRAGON:
                Colors::printMagenta(name);
                break;
        }
        
        cout << " (等级 " << level << ")" << endl;
        Colors::printGray(flavorText);
        cout << endl;
        
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
        cout << endl;
        
        cout << "  ";
        Colors::printCyan("攻击力: ");
        Colors::printRed(to_string(attack));
        cout << "    ";
        Colors::printCyan("防御力: ");
        Colors::printBlue(to_string(defense));
        cout << endl;
    }
    
    // Getters
    EnemyType getEnemyType() const { return enemyType; }
    int getExpReward() const { return expReward; }
    int getGoldReward() const { return goldReward; }
    string getFlavorText() const { return flavorText; }
    const vector<Skill*>& getSkills() const { return skills; }
};

// 敌人工厂类
class EnemyFactory {
public:
    static Enemy* createRandomEnemy() {
        vector<EnemyType> commonEnemies = {
            EnemyType::SLIME,
            EnemyType::SLIME_RED,
            EnemyType::GOBLIN,
            EnemyType::GOBLIN_SCOUT,
            EnemyType::SKELETON,
            EnemyType::TAOIST,
            EnemyType::GARGOYLE,
            EnemyType::MIST_WRAITH,
            EnemyType::ORC,
            EnemyType::FALLEN_SOUL
        };
        
        EnemyType randomType = commonEnemies[rand() % commonEnemies.size()];
        return new Enemy(randomType);
    }
    
    static Enemy* createEnemyByType(EnemyType type) {
        return new Enemy(type);
    }
    
    static Enemy* createBoss() {
        return new Enemy(EnemyType::DRAGON);
    }
};

#endif
