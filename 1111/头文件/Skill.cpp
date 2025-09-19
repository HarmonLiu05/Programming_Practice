#include "Skill.h"
#include "Player.h"
#include "Enemy.h"
#include <algorithm>
#include <cmath>

// 全局变量用于跟踪buff状态
static bool nextAttackEnhanced = false;
static int nextAttackDamageMultiplier = 100;
static bool enemyMarkedForDeath = false;
static int markedTurnsRemaining = 0;
static bool enemyWeaknessMarked = false;
static bool sharedFateActive = false;
static string lastEnemySkill = "";

bool Skill::canUse(Player& user) const {
    switch(type) {
        case SkillType::REVENGE:
            // 只有当生命值低于40%时才能使用
            return ((double)user.getHP() / user.getMaxHP()) < 0.4;
        default:
            return true;
    }
}

bool Skill::useSkill(Player& user, Character& target) {
    // 检查是否可以使用
    if (!canUse(user)) {
        Colors::printWarning("当前状态下无法使用此技能！");
        return false;
    }
    
    int damage = 0;
    int healing = 0;
    
    switch(type) {
        case SkillType::EXECUTE:
        {
            // 致死打击：基础80%伤害，敌人每少1%血量增加2%伤害
            double enemyHpPercent = (double)target.getHP() / target.getMaxHP() * 100;
            double damagePercent = 80 + (100 - enemyHpPercent) * 2;
            damage = (user.getAttack() * damagePercent) / 100;
            target.takeDamage(damage);
            Colors::printMagenta("使用技能: " + name);
            Colors::printRed("致死打击造成 " + to_string(damage) + " 点毁灭性伤害！");
            break;
        }
        
        case SkillType::LIFE_SIPHON:
        {
            // 生命吸取：110%伤害，50%转为治疗
            damage = (user.getAttack() * 110) / 100;
            healing = damage / 2;
            target.takeDamage(damage);
            
            int oldHp = user.getHP();
            int newHp = min(user.getMaxHP(), oldHp + healing);
            // 这里需要直接访问Player的私有成员，我们需要添加一个友元函数或public方法
            user.restoreHP(healing);
            
            Colors::printMagenta("使用技能: " + name);
            Colors::printRed("造成 " + to_string(damage) + " 点伤害！");
            Colors::printGreen("吸取 " + to_string(healing) + " 点生命值！");
            break;
        }
        
        case SkillType::REVENGE:
        {
            // 复仇：250%伤害（已在canUse中检查血量条件）
            damage = (user.getAttack() * 250) / 100;
            target.takeDamage(damage);
            Colors::printMagenta("使用技能: " + name);
            Colors::printRed("复仇之火！造成 " + to_string(damage) + " 点恐怖伤害！");
            break;
        }
        
        case SkillType::MARKED_FOR_DEATH:
        {
            // 死亡标记：标记敌人5回合
            enemyMarkedForDeath = true;
            markedTurnsRemaining = 5;
            Colors::printMagenta("使用技能: " + name);
            Colors::printYellow("敌人被死亡标记！接下来5回合受到150%伤害！");
            break;
        }
        
        case SkillType::CRITICAL_HIT:
        {
            // 致命一击：下次攻击150%-200%伤害，10%概率300%
            nextAttackEnhanced = true;
            if (rand() % 100 < 10) {
                nextAttackDamageMultiplier = 300;
                Colors::printYellow("超级暴击！");
            } else {
                nextAttackDamageMultiplier = 150 + rand() % 51; // 150-200%
            }
            Colors::printMagenta("使用技能: " + name);
            Colors::printYellow("下次攻击将造成 " + to_string(nextAttackDamageMultiplier) + "% 伤害！");
            break;
        }
        
        case SkillType::WEAKNESS_STRIKE:
        {
            // 虚弱打击：100%伤害，敌人下次受伤+50%
            damage = user.getAttack();
            target.takeDamage(damage);
            enemyWeaknessMarked = true;
            Colors::printMagenta("使用技能: " + name);
            Colors::printRed("造成 " + to_string(damage) + " 点伤害！");
            Colors::printYellow("敌人被虚弱标记！下次受到的伤害+50%！");
            break;
        }
        
        case SkillType::WEALTH_STRIKE:
        {
            // 财富一击：金币 × 0.1%
            damage = user.getGold() * 1; // 0.1% = 1/1000，但为了平衡性改为0.1倍
            target.takeDamage(damage);
            Colors::printMagenta("使用技能: " + name);
            Colors::printYellow("财富的力量！");
            Colors::printRed("造成 " + to_string(damage) + " 点金钱伤害！");
            break;
        }
        
        case SkillType::LUCKY_DICE:
        {
            // 运气骰子：1-6点 × 50%
            int diceRoll = 1 + rand() % 6;
            damage = (user.getAttack() * diceRoll * 50) / 100;
            target.takeDamage(damage);
            Colors::printMagenta("使用技能: " + name);
            Colors::printYellow("骰子点数: " + to_string(diceRoll));
            Colors::printRed("造成 " + to_string(damage) + " 点伤害！");
            break;
        }
        
        case SkillType::SHARED_FATE:
        {
            // 同生共死：建立连接
            sharedFateActive = true;
            Colors::printMagenta("使用技能: " + name);
            Colors::printYellow("与敌人建立生死连接！双方伤害将共享40%！");
            break;
        }
        
        case SkillType::REVERSAL:
        {
            // 起死回生：血量越少伤害越高
            double hpPercent = (double)user.getHP() / user.getMaxHP() * 100;
            int damagePercent;
            if (hpPercent <= 5) damagePercent = 400;
            else if (hpPercent <= 10) damagePercent = 300;
            else if (hpPercent <= 25) damagePercent = 200;
            else if (hpPercent <= 50) damagePercent = 150;
            else damagePercent = 100;
            
            damage = (user.getAttack() * damagePercent) / 100;
            target.takeDamage(damage);
            Colors::printMagenta("使用技能: " + name);
            Colors::printRed("绝地反击！造成 " + to_string(damage) + " 点伤害！");
            break;
        }
        
        case SkillType::FOUL_PLAY:
        {
            // 欺诈：使用敌人的攻击力
            damage = max(1, target.getAttack() - user.getDefense());
            target.takeDamage(damage);
            Colors::printMagenta("使用技能: " + name);
            Colors::printBlue("以彼之道，还施彼身！");
            Colors::printRed("造成 " + to_string(damage) + " 点伤害！");
            break;
        }
    }
    
    return true;
}

// 全局函数用于处理buff效果
namespace SkillEffects {
    void applyAttackBuff(int& damage) {
        if (nextAttackEnhanced) {
            damage = (damage * nextAttackDamageMultiplier) / 100;
            nextAttackEnhanced = false;
            nextAttackDamageMultiplier = 100;
        }
        
        if (enemyMarkedForDeath && markedTurnsRemaining > 0) {
            damage = (damage * 150) / 100;
            markedTurnsRemaining--;
            if (markedTurnsRemaining == 0) {
                enemyMarkedForDeath = false;
                Colors::printGray("死亡标记效果消失");
            }
        }
        
        if (enemyWeaknessMarked) {
            damage = (damage * 150) / 100;
            enemyWeaknessMarked = false;
            Colors::printGray("虚弱标记触发");
        }
    }
    
    void applySharedDamage(Character& attacker, Character& defender, int damage) {
        if (sharedFateActive) {
            int sharedAmount = damage * 40 / 100;
            attacker.takeDamage(sharedAmount);
            Colors::printYellow("同生共死效果：攻击者也受到 " + to_string(sharedAmount) + " 点伤害");
        }
    }
    
    void setLastEnemySkill(const string& skillName) {
        lastEnemySkill = skillName;
    }
    
    void resetAllEffects() {
        nextAttackEnhanced = false;
        nextAttackDamageMultiplier = 100;
        enemyMarkedForDeath = false;
        markedTurnsRemaining = 0;
        enemyWeaknessMarked = false;
        sharedFateActive = false;
        lastEnemySkill = "";
    }
}