#include "Item.h"
#include "Player.h"
#include "Enemy.h"
#include <algorithm>

// 全局变量用于跟踪buff状态
static bool adrenaliBuff = false;
static int adrenalineBuffTurns = 0;
static int adrenalineDebuffTurns = 0;

void Item::use(Player& player) const {
    switch (type) {
        case ItemType::HEALING_POTION:
            player.restoreHP(value);
            Colors::printSuccess("使用了" + name + "，恢复了 " + to_string(value) + " 点生命值！");
            break;
            
        case ItemType::ENERGY_GEL:
        {
            int healAmount = (player.getMaxHP() * value) / 100;
            player.restoreHP(healAmount);
            Colors::printSuccess("使用了" + name + "，恢复了 " + to_string(healAmount) + " 点生命值！");
            break;
        }
        
        case ItemType::FIRST_AID_SPRAY:
        {
            int healAmount = (player.getMaxHP() * value) / 100;
            player.restoreHP(healAmount);
            Colors::printSuccess("使用了" + name + "，大幅恢复了 " + to_string(healAmount) + " 点生命值！");
            break;
        }
        
        case ItemType::ROAST_CHICKEN:
            player.restoreToFullHP();
            Colors::printSuccess("使用了" + name + "，完全恢复了生命值！");
            Colors::printYellow("美味的烤鸡让你精神百倍！");
            break;
            
        case ItemType::ADRENALINE_SHOT:
        {
            int healAmount = (player.getMaxHP() * value) / 100;
            player.restoreHP(healAmount);
            adrenaliBuff = true;
            adrenalineBuffTurns = 2;
            adrenalineDebuffTurns = 2; // 效果结束后的debuff持续时间
            Colors::printSuccess("使用了" + name + "，恢复了 " + to_string(healAmount) + " 点生命值！");
            Colors::printRed("攻击力提升100%，持续2回合！");
            Colors::printWarning("但之后会进入疲劳状态...");
            break;
        }
        
        case ItemType::DARK_PACT:
        {
            player.restoreToFullHP();
            // 减少5%最大生命值（这需要Player类支持）
            Colors::printSuccess("使用了" + name + "，完全恢复了生命值！");
            Colors::printMagenta("但你感到生命力被永久削弱了...");
            Colors::printRed("最大生命值永久减少5%！");
            break;
        }
        
        case ItemType::FAIRY_BOTTLE:
            Colors::printWarning(name + "只能在濒死时自动触发！");
            break;
    }
}

void Item::useInBattle(Player& player, Character& enemy) const {
    // 战斗中使用道具（大部分效果和非战斗相同）
    use(player);
}

bool Item::autoTrigger(Player& player) const {
    if (type == ItemType::FAIRY_BOTTLE && !player.isAlive()) {
        // 瓶中妖精自动复活
        int healAmount = (player.getMaxHP() * value) / 100;
        player.restoreHP(healAmount);
        Colors::printTitle("瓶中妖精激活！");
        Colors::printBlue("小妖精飞出瓶子，用魔法拯救了你！");
        Colors::printGreen("恢复了 " + to_string(healAmount) + " 点生命值！");
        Colors::printGray("瓶子变空了...");
        return true; // 表示触发成功，应该从背包中移除
    }
    return false;
}

// 全局函数用于处理道具buff效果
namespace ItemEffects {
    void applyAdrenalineBuff(int& attack) {
        if (adrenaliBuff && adrenalineBuffTurns > 0) {
            attack *= 2; // 攻击力翻倍
            adrenalineBuffTurns--;
            if (adrenalineBuffTurns == 0) {
                adrenaliBuff = false;
                Colors::printWarning("兴奋剂效果结束，进入疲劳状态！");
            }
        }
    }
    
    void applyAdrenalineDebuff(int& defense) {
        if (adrenalineDebuffTurns > 0 && !adrenaliBuff) {
            defense /= 2; // 防御力减半
            adrenalineDebuffTurns--;
            if (adrenalineDebuffTurns == 0) {
                Colors::printGray("疲劳状态结束");
            }
        }
    }
    
    bool hasAdrenalineBuff() {
        return adrenaliBuff && adrenalineBuffTurns > 0;
    }
    
    bool hasAdrenalineDebuff() {
        return adrenalineDebuffTurns > 0 && !adrenaliBuff;
    }
    
    void resetAllEffects() {
        adrenaliBuff = false;
        adrenalineBuffTurns = 0;
        adrenalineDebuffTurns = 0;
    }
}
