#include "Skill.h"
#include "Player.h"
#include "Enemy.h"
#include <algorithm>
#include <cmath>

// Buff tracking
static bool nextAttackEnhanced = false;
static int  nextAttackDamageMultiplier = 100;
static bool enemyMarkedForDeath = false;
static int  markedTurnsRemaining = 0;
static bool enemyWeaknessMarked = false;
static bool sharedFateActive = false; // kept but unreachable (skill removed)
static std::string lastEnemySkill = "";

bool Skill::canUse(Player& user) const {
    switch (type) {
        case SkillType::REVENGE:
            return ((double)user.getHP() / user.getMaxHP()) < 0.4;
        default:
            return true;
    }
}

bool Skill::useSkill(Player& user, Character& target) {
    if (!canUse(user)) {
        Colors::printWarning("当前状态下无法使用此技能！");
        return false;
    }

    int damage = 0;
    int healing = 0;

    switch (type) {
        case SkillType::EXECUTE: {
            double enemyHpPercent = (double)target.getHP() / target.getMaxHP() * 100.0;
            double damagePercent = 80.0 + (100.0 - enemyHpPercent) * 2.0;
            damage = (int)((user.getAttack() * damagePercent) / 100.0);
            target.takeDamage(damage);
            Colors::printMagenta("使用技能: " + name);
            Colors::printRed("致死打击造成 " + std::to_string(damage) + " 点伤害！");
            break;
        }
        case SkillType::LIFE_SIPHON: {
            damage = (user.getAttack() * 110) / 100;
            healing = damage / 2;
            target.takeDamage(damage);
            user.restoreHP(healing);
            Colors::printMagenta("使用技能: " + name);
            Colors::printRed("造成 " + std::to_string(damage) + " 点伤害！");
            Colors::printGreen("吸取 " + std::to_string(healing) + " 点生命值！");
            break;
        }
        case SkillType::REVENGE: {
            damage = (user.getAttack() * 250) / 100;
            target.takeDamage(damage);
            Colors::printMagenta("使用技能: " + name);
            Colors::printRed("复仇之火！造成 " + std::to_string(damage) + " 点伤害！");
            break;
        }
        case SkillType::MARKED_FOR_DEATH: {
            enemyMarkedForDeath = true;
            markedTurnsRemaining = 5;
            Colors::printMagenta("使用技能: " + name);
            Colors::printYellow("敌人被死亡标记！接下来5回合受到150%伤害！");
            break;
        }
        case SkillType::CRITICAL_HIT: {
            nextAttackEnhanced = true;
            if (rand() % 100 < 10) {
                nextAttackDamageMultiplier = 300;
                Colors::printYellow("超级暴击！");
            } else {
                nextAttackDamageMultiplier = 150 + rand() % 51; // 150-200%
            }
            Colors::printMagenta("使用技能: " + name);
            Colors::printYellow("下次攻击将造成 " + std::to_string(nextAttackDamageMultiplier) + "% 伤害！");
            break;
        }
        case SkillType::WEAKNESS_STRIKE: {
            damage = user.getAttack();
            target.takeDamage(damage);
            enemyWeaknessMarked = true;
            Colors::printMagenta("使用技能: " + name);
            Colors::printRed("造成 " + std::to_string(damage) + " 点伤害！");
            Colors::printYellow("敌人被虚弱标记！下次受到的伤害+50%！");
            break;
        }
        case SkillType::WEALTH_STRIKE: {
            damage = user.getGold() * 1; // simplified
            target.takeDamage(damage);
            Colors::printMagenta("使用技能: " + name);
            Colors::printYellow("财富的力量！");
            Colors::printRed("造成 " + std::to_string(damage) + " 点伤害！");
            break;
        }
        case SkillType::LUCKY_DICE: {
            int diceRoll = 1 + rand() % 6;
            damage = (user.getAttack() * diceRoll * 50) / 100;
            target.takeDamage(damage);
            Colors::printMagenta("使用技能: " + name);
            Colors::printYellow("骰子点数: " + std::to_string(diceRoll));
            Colors::printRed("造成 " + std::to_string(damage) + " 点伤害！");
            break;
        }
        case SkillType::REVERSAL: {
            double hpPercent = (double)user.getHP() / user.getMaxHP() * 100.0;
            int damagePercent;
            if (hpPercent <= 5) damagePercent = 400;
            else if (hpPercent <= 10) damagePercent = 300;
            else if (hpPercent <= 25) damagePercent = 200;
            else if (hpPercent <= 50) damagePercent = 150;
            else damagePercent = 100;
            damage = (user.getAttack() * damagePercent) / 100;
            target.takeDamage(damage);
            Colors::printMagenta("使用技能: " + name);
            Colors::printRed("绝地反击！造成 " + std::to_string(damage) + " 点伤害！");
            break;
        }
    }

    return true;
}

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
            Colors::printYellow("同生共死效果：攻击者也受到 " + std::to_string(sharedAmount) + " 点伤害！");
        }
    }

    void setLastEnemySkill(const std::string& skillName) { lastEnemySkill = skillName; }

    void resetAllEffects() {
        nextAttackEnhanced = false;
        nextAttackDamageMultiplier = 100;
        enemyMarkedForDeath = false;
        markedTurnsRemaining = 0;
        enemyWeaknessMarked = false;
        sharedFateActive = false;
        lastEnemySkill.clear();
    }
}
