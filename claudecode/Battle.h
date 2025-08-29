#ifndef BATTLE_H
#define BATTLE_H

#include "Player.h"
#include "Enemy.h"
#include "Skill.h"
#include "Colors.h"
#include "AudioManager.h"
#include "SoundEffects.h"
#include <iostream>
#include <vector>

#ifdef _WIN32
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

using namespace std;

class Battle {
private:
    Player* player;
    Enemy* enemy;
    bool battleEnded;
    bool playerWon;
    bool playerEscaped;  // 添加逃跑标志
    vector<DropItem> battleRewards;
    
public:
    Battle(Player* p, Enemy* e) : player(p), enemy(e), battleEnded(false), playerWon(false), playerEscaped(false) {}
    
    bool startBattle() {
        Colors::printSeparator(70);
        Colors::printTitle("战斗开始！");
        
        // 显示敌人信息
        Colors::printWarning("遭遇了敌人：");
        enemy->displayStats();
        
        Colors::printInfo("准备战斗...");
        waitForInput();
        
        // 战斗主循环
        while (!battleEnded) {
            // 清屏显示战斗状态
            clearScreen();
            displayBattleStatus();
            
            // 玩家回合
            if (!playerTurn()) {
                break; // 玩家选择逃跑或其他退出条件
            }
            
            // 检查敌人是否死亡
            if (!enemy->isAlive()) {
                playerWon = true;
                battleEnded = true;
                break;
            }
            
            // 敌人回合
            enemyTurn();
            
            // 检查玩家是否死亡
            if (!player->isAlive()) {
                playerWon = false;
                battleEnded = true;
                break;
            }
            
            waitForInput();
        }
        
        // 战斗结束处理
        if (playerWon) {
            handleVictory();
        } else if (!playerEscaped) {
            // 只有在真正死亡时才显示失败信息，逃跑时不显示
            handleDefeat();
        }
        
        return playerWon;
    }
    
private:
    void displayBattleStatus() const {
        Colors::printTitle("战斗中");
        
        cout << "【玩家】                    【敌人】" << endl;
        
        // 玩家名称和等级（左侧）
        cout << "  ";
        Colors::printCyan("勇者");
        cout << " (等级 " << player->getLevel() << ")";
        
        // 敌人名称（右侧）
        cout << "               ";
        Colors::printRed(enemy->getName());
        cout << " (等级 " << enemy->getLevel() << ")" << endl;
        
        // 玩家HP（左侧）
        cout << "  HP: ";
        double playerHpPercent = (double)player->getHP() / player->getMaxHP();
        if (playerHpPercent > 0.7) {
            Colors::printGreen(to_string(player->getHP()) + "/" + to_string(player->getMaxHP()));
        } else if (playerHpPercent > 0.3) {
            Colors::printYellow(to_string(player->getHP()) + "/" + to_string(player->getMaxHP()));
        } else {
            Colors::printRed(to_string(player->getHP()) + "/" + to_string(player->getMaxHP()));
        }
        
        // 敌人HP（右侧）
        cout << "               HP: ";
        double enemyHpPercent = (double)enemy->getHP() / enemy->getMaxHP();
        if (enemyHpPercent > 0.7) {
            Colors::printGreen(to_string(enemy->getHP()) + "/" + to_string(enemy->getMaxHP()));
        } else if (enemyHpPercent > 0.3) {
            Colors::printYellow(to_string(enemy->getHP()) + "/" + to_string(enemy->getMaxHP()));
        } else {
            Colors::printRed(to_string(enemy->getHP()) + "/" + to_string(enemy->getMaxHP()));
        }
        cout << endl;
        
        // 敌人攻击力和防御力（右侧）
        cout << "               攻击: ";
        Colors::printYellow(to_string(enemy->getAttack()));
        cout << " 防御: ";
        Colors::printCyan(to_string(enemy->getDefense()));
        cout << endl;
        
        Colors::printSubSeparator(70);
    }
    
    bool playerTurn() {
        Colors::printTitle("你的回合");
        Colors::printInfo("选择你的行动:");
        
        cout << "  ";
        Colors::printWhite("1");
        cout << " - 普通攻击" << endl;
        
        cout << "  ";
        Colors::printWhite("2");
        cout << " - 使用技能" << endl;
        
        cout << "  ";
        Colors::printWhite("3");
        cout << " - 使用道具" << endl;
        
        cout << "  ";
        Colors::printWhite("4");
        cout << " - 尝试逃跑" << endl;
        
        cout << "\n请选择 (1-4): ";
        char choice = getInput();
        cout << endl;
        
        switch(choice) {
            case '1':
                return handleAttack();
            case '2':
                return handleSkillUse();
            case '3':
                return handleItemUse();
            case '4':
                return handleEscape();
            default:
                Colors::printWarning("无效选择，自动进行普通攻击");
                return handleAttack();
        }
    }
    
    bool handleAttack() {
        int damage = max(1, player->getAttack() - enemy->getDefense());
        
        // 10%暴击几率
        bool isCritical = (rand() % 100) < 10;
        if (isCritical) {
            damage = (damage * 15) / 10; // 1.5倍伤害
            Colors::printYellow("暴击！");
            // 播放暴击音效
            AudioManager::getInstance().playSoundEffect(SoundEffects::CRITICAL_SOUND);
        } else {
            // 播放普通攻击音效
            AudioManager::getInstance().playSoundEffect(SoundEffects::ATTACK_SOUND);
        }
        
        enemy->takeDamage(damage);
        
        Colors::printSuccess("你攻击了 " + enemy->getName());
        if (isCritical) {
            Colors::printYellow("造成 " + to_string(damage) + " 点暴击伤害！");
        } else {
            Colors::printGreen("造成 " + to_string(damage) + " 点伤害！");
        }
        
        return true;
    }
    
    bool handleSkillUse() {
        const vector<Skill*>& skills = player->getSkills();
        
        if (skills.empty()) {
            Colors::printWarning("你还没有学会任何技能！");
            Colors::printInfo("自动进行普通攻击");
            return handleAttack();
        }
        
        Colors::printTitle("选择技能");
        for (size_t i = 0; i < skills.size(); i++) {
            cout << "  " << (i + 1) << " - ";
            skills[i]->displayShortInfo();
            cout << endl << "       ";
            Colors::printGray(skills[i]->getDescription());
            cout << endl;
        }
        cout << "  0 - 返回" << endl;
        
        cout << "\n选择技能 (0-" << skills.size() << "): ";
        char choice = getInput();
        cout << endl;
        
        if (choice == '0') {
            return playerTurn(); // 重新选择行动
        }
        
        int skillIndex = choice - '1';
        if (skillIndex >= 0 && skillIndex < (int)skills.size()) {
            if (player->useSkill(skillIndex, *enemy)) {
                // 播放技能音效
                AudioManager::getInstance().playSoundEffect(SoundEffects::SKILL_SOUND);
                return true;
            } else {
                // 技能使用失败，重新选择
                Colors::printInfo("请重新选择行动：");
                return playerTurn();
            }
        } else {
            Colors::printWarning("无效选择！自动进行普通攻击");
            return handleAttack();
        }
    }
    
    bool handleItemUse() {
        const vector<Item*>& inventory = player->getInventory();
        
        if (inventory.empty()) {
            Colors::printWarning("你的背包是空的！");
            Colors::printInfo("自动进行普通攻击");
            return handleAttack();
        }
        
        Colors::printTitle("选择道具");
        for (size_t i = 0; i < inventory.size(); i++) {
            cout << "  " << (i + 1) << " - ";
            inventory[i]->displayShortInfo();
            if (inventory[i]->canUseInBattle()) {
                Colors::printGreen(" [可用]");
            } else {
                Colors::printGray(" [战斗中不可用]");
            }
            cout << endl;
        }
        cout << "  0 - 返回" << endl;
        
        cout << "\n选择道具 (0-" << inventory.size() << "): ";
        char choice = getInput();
        cout << endl;
        
        if (choice == '0') {
            return playerTurn(); // 重新选择行动
        }
        
        int itemIndex = choice - '1';
        if (itemIndex >= 0 && itemIndex < (int)inventory.size()) {
            if (player->useItemInBattle(itemIndex, *enemy)) {
                // 播放道具使用音效
                AudioManager::getInstance().playSoundEffect(SoundEffects::ITEM_SOUND);
                return true;
            } else {
                // 道具使用失败，重新选择
                Colors::printInfo("请重新选择行动：");
                return playerTurn();
            }
        } else {
            Colors::printWarning("无效选择！自动进行普通攻击");
            return handleAttack();
        }
    }
    
    bool handleEscape() {
        int escapeChance = 70; // 70%逃跑成功率
        
        if (rand() % 100 < escapeChance) {
            Colors::printSuccess("你成功逃离了战斗！");
            // 播放逃跑音效
            AudioManager::getInstance().playSoundEffect(SoundEffects::ESCAPE_SOUND);
            battleEnded = true;
            playerWon = false;
            playerEscaped = true;  // 设置逃跑标志
            return false;
        } else {
            Colors::printWarning("逃跑失败！敌人阻止了你的去路！");
            return true; // 继续战斗
        }
    }
    
    void enemyTurn() {
        Colors::printTitle("敌人的回合");
        enemy->performAction(*player);
    }
    
    void handleVictory() {
        Colors::printSeparator(70);
        Colors::printTitle("战斗胜利！");
        Colors::printSuccess("你击败了 " + enemy->getName() + "！");
        
        // 播放胜利音效
        AudioManager::getInstance().playSoundEffect(SoundEffects::VICTORY_SOUND);
        
        // 获得经验值和金币
        int exp = enemy->getExpReward();
        int gold = enemy->getGoldReward();
        
        player->gainExp(exp);
        player->addGold(gold);
        
        // 生成掉落物
        battleRewards = enemy->generateDrops();
        
        if (!battleRewards.empty()) {
            Colors::printInfo("获得战利品:");
            for (const auto& drop : battleRewards) {
                switch(drop.type) {
                    case DropType::SKILL:
                        cout << "  - 技能: ";
                        drop.skill->displayShortInfo();
                        cout << endl;
                        // 这里应该将技能添加到玩家技能列表中
                        break;
                    case DropType::GOLD:
                        cout << "  - 额外金币: ";
                        Colors::printYellow(to_string(drop.value));
                        cout << endl;
                        player->addGold(drop.value);
                        break;
                    case DropType::POTION:
                        cout << "  - 道具: ";
                        Colors::printBlue(drop.name);
                        cout << " (+" << drop.value << " HP)" << endl;
                        // 这里应该将道具添加到玩家背包中
                        break;
                    case DropType::NONE:
                        break;
                }
            }
        }
        
        Colors::printSeparator(70);
        // 恢复waitForInput()，让玩家看到战利品信息
        waitForInput();
    }
    
    void handleDefeat() {
        Colors::printSeparator(70);
        Colors::printTitle("战斗失败...");
        Colors::printError("你被 " + enemy->getName() + " 击败了！");
        
        // 播放失败音效
        AudioManager::getInstance().playSoundEffect(SoundEffects::DEFEAT_SOUND);
        
        Colors::printWarning("游戏结束！");
        Colors::printSeparator(70);
        waitForInput();
    }
    
    char getInput() {
        #ifdef _WIN32
            return getch();
        #else
            struct termios old, new_;
            tcgetattr(STDIN_FILENO, &old);
            new_ = old;
            new_.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &new_);
            char ch = getchar();
            tcsetattr(STDIN_FILENO, TCSANOW, &old);
            return ch;
        #endif
    }
    
    void waitForInput() {
        cout << "\n";
        Colors::printGray("按任意键继续...");
        getInput();
        cout << endl;
    }
    
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
    
public:
    // Getters for rewards
    const vector<DropItem>& getBattleRewards() const { return battleRewards; }
    bool didPlayerWin() const { return playerWon; }
    bool didPlayerEscape() const { return playerEscaped; }
};

#endif