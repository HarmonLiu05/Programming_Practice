#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Map.h"
#include "Colors.h"
#include "Skill.h"
#include "Enemy.h"
#include "Battle.h"
#include "SaveGame.h"
#include <iostream>
#include <string>

// 跨平台输入处理
#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    #define CLEAR_SCREEN "cls"
#else
    #include <termios.h>
    #include <unistd.h>
    #define CLEAR_SCREEN "clear"
    
    // Linux/Mac 下的 getch 实现
    char getch() {
        struct termios old, new_;
        tcgetattr(STDIN_FILENO, &old);
        new_ = old;
        new_.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_);
        char ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &old);
        return ch;
    }
#endif

using namespace std;

class Game {
private:
    Player* player;
    Map* gameMap;
    bool gameRunning;
    bool gameWon;
    
public:
    Game() {
        player = nullptr;
        gameMap = nullptr;
        gameRunning = true;
        gameWon = false;
    }
    
    ~Game() {
        delete player;
        delete gameMap;
    }
    
    void run() {
        clearScreen();
        Colors::printTitle("重返崂山!");
        Colors::printSuccess("欢迎来到回合制RPG冒险游戏！");
        
        // 显示开始菜单
        showStartMenu();
        
        if (player && gameMap) {
            // 显示固定的界面布局
            displayGameInterface();
            
            gameLoop();
        }
    }
    
    void gameLoop() {
        while (gameRunning) {
            char input = getPlayerInput();
            
            switch (input) {
                case 'w': case 'W':
                case 'a': case 'A':
                case 's': case 'S':
                case 'd': case 'D':
                    handleMovement(input);
                    break;
                    
                case 'm': case 'M':
                    displayGameInterface();
                    break;
                    
                case 'p': case 'P':
                    displayGameInterface();
                    break;
                    
                case 'i': case 'I':
                    player->displaySkillBag();
                    Colors::printInfo("按任意键返回...");
                    getPlayerInput();
                    displayGameInterface();
                    break;
                    
                case 'b': case 'B':
                    clearScreen();
                    player->displayInventory();
                    Colors::printInfo("按任意键返回...");
                    getPlayerInput();
                    displayGameInterface();
                    break;
                    
                case 'q': case 'Q':
                {
                    cout << "\n退出选项:" << endl;
                    cout << "  1 - 保存并退出" << endl;
                    cout << "  2 - 直接退出（不保存）" << endl;
                    cout << "  3 - 返回游戏" << endl;
                    cout << "请选择 (1-3): ";
                    char exitChoice = getPlayerInput();
                    cout << endl;
                    
                    switch(exitChoice) {
                        case '1':
                            if (SaveGame::saveGame(player, gameMap)) {
                                Colors::printSuccess("游戏已保存！感谢游戏！");
                            } else {
                                Colors::printError("保存失败，但仍将退出游戏。");
                            }
                            gameRunning = false;
                            break;
                        case '2':
                        {
                            cout << "确定要不保存直接退出吗？(y/n): ";
                            char confirm = getPlayerInput();
                            if (confirm == 'y' || confirm == 'Y') {
                                cout << "感谢游戏！再见！" << endl;
                                gameRunning = false;
                            }
                            break;
                        }
                        case '3':
                        default:
                            // 返回游戏，不做任何事
                            break;
                    }
                    break;
                }
                    
                case 'h': case 'H':
                    displayControls();
                    break;
                    
                case 'c': case 'C':
                    displayGameInterface();
                    break;
                    
                case 'x': case 'X':
                    // 手动保存游戏
                    Colors::printInfo("正在保存游戏...");
                    if (SaveGame::saveGame(player, gameMap)) {
                        Colors::printSuccess("游戏保存成功！");
                    } else {
                        Colors::printError("游戏保存失败！");
                    }
                    break;
                    
                default:
                    cout << "无效指令！按 H 查看帮助。" << endl;
                    break;
            }
            
            // 检查胜利条件
            checkWinCondition();
        }
    }
    
    void handleMovement(char direction) {
        if (gameMap->canMove(direction)) {
            cout << "\n正在移动..." << endl;
            
            if (gameMap->movePlayer(direction)) {
                // 移动成功后显示更新的界面
                displayGameInterface();
                
                // 处理房间事件
                handleRoomEntry();
                
            }
        } else {
            cout << "不能往那个方向移动！" << endl;
        }
    }
    
    void handleRoomEntry() {
        Room* currentRoom = gameMap->getCurrentRoom();
        RoomType roomType = currentRoom->getType();
        
        switch (roomType) {
            case RoomType::START:
                Colors::printSuccess("回到了起始点，这里很安全。");
                break;
                
            case RoomType::MONSTER:
                if (currentRoom->hasEnemy()) {
                    Enemy* enemy = currentRoom->getEnemy();
                    Colors::printWarning("发现敌人：" + enemy->getName() + "！");
                    waitForInput();
                    
                    // 开始战斗
                    Battle battle(player, enemy);
                    bool playerWon = battle.startBattle();
                    
                    if (playerWon) {
                        // 处理战斗奖励
                        auto rewards = battle.getBattleRewards();
                        for (const auto& reward : rewards) {
                            if (reward.type == DropType::SKILL) {
                                player->addSkill(new Skill(*reward.skill)); // 创建技能副本
                            }
                        }
                        
                        // 清理房间
                        currentRoom->clear();
                        gameMap->incrementMonstersDefeated();
                        
                        // 战斗后显示更新的地图
                        clearScreen();
                        Colors::printTitle("战斗胜利，地图已更新");
                        displayGameInterface();
                        
                    } else {
                        // 玩家死亡或逃跑
                        if (!player->isAlive()) {
                            Colors::printTitle("游戏结束");
                            Colors::printError("你在冒险中失败了...");
                            gameRunning = false;
                            return;
                        } else if (battle.didPlayerEscape()) {
                            // 逃跑成功
                            waitForInput();
                            displayGameInterface();
                        }
                    }
                    
                } else {
                    Colors::printSuccess("这个房间已经被清理过了，很安全。");
                }
                break;
                
            case RoomType::SHOP:
                Colors::printTitle("欢迎光临神秘商店!");
                Colors::printYellow("店主: '勇敢的冒险者，欢迎光临！'");
                handleShop();
                break;
                
            case RoomType::EMPTY:
            {
                Colors::printTitle("宁静的房间");
                Colors::printInfo("这是一个安静的房间，你感到很平静。");
                
                if (currentRoom->canRest()) {
                    Colors::printSuccess("在这里休息让你完全恢复了！");
                    player->restoreToFullHP();
                    currentRoom->setRested();
                } else {
                    Colors::printGray("你已经在这里休息过了，不能再次恢复。");
                }
                
                break;
            }
                
            case RoomType::BOSS:
                if (gameMap->getMonstersDefeated() >= 15) {
                    Colors::printTitle("最终挑战");
                    Colors::printRed("崂山龙王出现了！");
                    
                    Enemy* boss = currentRoom->getEnemy();
                    Colors::printMagenta("这是你最后的考验...");
                    waitForInput();
                    
                    // BOSS战斗
                    Battle bossBattle(player, boss);
                    bool playerWon = bossBattle.startBattle();
                    
                    if (playerWon) {
                        // 处理BOSS奖励
                        auto rewards = bossBattle.getBattleRewards();
                        for (const auto& reward : rewards) {
                            if (reward.type == DropType::SKILL) {
                                player->addSkill(new Skill(*reward.skill));
                            }
                        }
                        
                        currentRoom->clear();
                        Colors::printTitle("恭喜！你成功击败了崂山龙王！");
                        Colors::printSuccess("游戏胜利！");
                        gameWon = true;
                        gameRunning = false;
                        return;
                    } else {
                        if (!player->isAlive()) {
                            Colors::printTitle("游戏结束");
                            Colors::printError("你被强大的龙王击败了...");
                            gameRunning = false;
                            return;
                        }
                    }
                    
                } else {
                    Colors::printTitle("龙王的威压");
                    Colors::printRed("一股强大的威压阻止了你前进...");
                    Colors::printWarning("你还没有准备好面对BOSS。");
                    cout << "  需要先击败 ";
                    Colors::printYellow(to_string(15 - gameMap->getMonstersDefeated()));
                    cout << " 只普通怪物才能挑战BOSS。" << endl;
                    Colors::printInfo("先去其他房间寻找更多的怪物吧！");
                }
                break;
        }
        
        // 只在非战斗情况下等待输入
        if (roomType != RoomType::MONSTER && roomType != RoomType::BOSS) {
            if (roomType == RoomType::EMPTY) {
                waitForInput();
            }
        } else if (currentRoom->isCleared()) {
            // 如果是已清理的战斗房间，也不需要等待
            // 直接返回让玩家输入下一个指令
        }
    }
    
    char getPlayerInput() {
        return getch();  // Windows版本的字符输入
    }
    
    void clearScreen() {
        system(CLEAR_SCREEN);
    }
    
    void waitForInput() {
        cout << "\n按任意键继续...";
        getch();
        cout << endl;
    }
    
    void checkWinCondition() {
        if (gameMap->isGameWon()) {
            cout << "\n=== 恭喜！ ===" << endl;
            cout << "你成功完成了崂山的冒险！" << endl;
            cout << "击败了所有的敌人并到达了BOSS房间！" << endl;
            cout << "=============" << endl;
            gameWon = true;
            gameRunning = false;
        }
    }
    
    void handleShop() {
        Colors::printSubSeparator(50);
        Colors::printInfo("店主热情地向你介绍商品：");
        
        cout << "  ";
        Colors::printWhite("1");
        cout << " - 购买随机技能 (";
        Colors::printYellow("100");
        cout << " 金币)" << endl;
        
        cout << "  ";
        Colors::printWhite("2");
        cout << " - 治疗药水 (";
        Colors::printYellow("30");
        cout << " 金币) - 恢复50HP" << endl;
        
        cout << "  ";
        Colors::printWhite("3");
        cout << " - 能量凝胶 (";
        Colors::printYellow("25");
        cout << " 金币) - 恢复30%最大HP" << endl;
        
        cout << "  ";
        Colors::printWhite("4");
        cout << " - 急救喷雾 (";
        Colors::printYellow("80");
        cout << " 金币) - 恢复70%最大HP" << endl;
        
        cout << "  ";
        Colors::printWhite("5");
        cout << " - 烤鸡 (";
        Colors::printYellow("120");
        cout << " 金币) - 完全恢复" << endl;
        
        cout << "  ";
        Colors::printWhite("6");
        cout << " - 离开商店" << endl;
        
        cout << "\n你当前有 ";
        Colors::printYellow(to_string(player->getGold()));
        cout << " 金币。选择购买 (1-6): ";
        
        char choice = getPlayerInput();
        cout << endl;
        
        switch(choice) {
            case '1':
                if (player->getGold() >= 100) {
                    if (player->getSkillCount() < player->getMaxSkills()) {
                        player->spendGold(100);
                        Skill* newSkill = SkillFactory::createRandomSkill();
                        player->addSkill(newSkill);
                        Colors::printSuccess("购买成功！");
                    } else {
                        Colors::printWarning("技能栏已满！");
                    }
                } else {
                    Colors::printWarning("金币不足！");
                }
                break;
                
            case '2':
                if (player->getGold() >= 30) {
                    if (player->getItemCount() < player->getMaxItems()) {
                        player->spendGold(30);
                        player->addItem(ItemFactory::createHealingPotion());
                        Colors::printSuccess("购买成功！");
                    } else {
                        Colors::printWarning("道具背包已满！");
                    }
                } else {
                    Colors::printWarning("金币不足！");
                }
                break;
                
            case '3':
                if (player->getGold() >= 25) {
                    if (player->getItemCount() < player->getMaxItems()) {
                        player->spendGold(25);
                        player->addItem(ItemFactory::createEnergyGel());
                        Colors::printSuccess("购买成功！");
                    } else {
                        Colors::printWarning("道具背包已满！");
                    }
                } else {
                    Colors::printWarning("金币不足！");
                }
                break;
                
            case '4':
                if (player->getGold() >= 80) {
                    if (player->getItemCount() < player->getMaxItems()) {
                        player->spendGold(80);
                        player->addItem(ItemFactory::createFirstAidSpray());
                        Colors::printSuccess("购买成功！");
                    } else {
                        Colors::printWarning("道具背包已满！");
                    }
                } else {
                    Colors::printWarning("金币不足！");
                }
                break;
                
            case '5':
                if (player->getGold() >= 120) {
                    if (player->getItemCount() < player->getMaxItems()) {
                        player->spendGold(120);
                        player->addItem(ItemFactory::createRoastChicken());
                        Colors::printSuccess("购买成功！");
                    } else {
                        Colors::printWarning("道具背包已满！");
                    }
                } else {
                    Colors::printWarning("金币不足！");
                }
                break;
                
            case '6':
                Colors::printInfo("店主: '欢迎下次光临！'");
                break;
                
            default:
                Colors::printWarning("无效选择！");
                break;
        }
        Colors::printSubSeparator(50);
    }
    
    void displayControls() const {
        Colors::printTitle("操作说明");
        cout << "  ";
        Colors::printCyan("移动控制:");
        cout << " ";
        Colors::printWhite("W");
        cout << "-北 ";
        Colors::printWhite("A");
        cout << "-西 ";
        Colors::printWhite("S");
        cout << "-南 ";
        Colors::printWhite("D");
        cout << "-东" << endl;
        
        cout << "  ";
        Colors::printCyan("信息查看:");
        cout << " ";
        Colors::printWhite("M");
        cout << "-地图 ";
        Colors::printWhite("P");
        cout << "-状态 ";
        Colors::printWhite("I");
        cout << "-技能背包 ";
        Colors::printWhite("B");
        cout << "-道具背包 ";
        Colors::printWhite("H");
        cout << "-帮助 ";
        Colors::printWhite("C");
        cout << "-清屏" << endl;
        
        cout << "  ";
        Colors::printCyan("系统控制:");
        cout << " ";
        Colors::printWhite("Q");
        cout << "-退出游戏 ";
        Colors::printWhite("X");
        cout << "-保存游戏" << endl;
        
        Colors::printSubSeparator(50);
    }
    
    // 显示固定的游戏界面布局
    void displayGameInterface() {
        clearScreen();
        
        // 顶部显示玩家状态
        player->displayStats();
        
        // 中间显示地图
        gameMap->displayMap();
        
        // 底部显示操作控制
        displayGameControls();
    }
    
    // 显示游戏控制说明（简化版）
    void displayGameControls() {
        Colors::printSubSeparator(60);
        Colors::printInfo("控制: W/A/S/D-移动 | M-地图 P-状态 I-技能 B-道具 | Q-退出 X-保存 H-帮助");
        cout << "\n请输入指令: ";
    }
    
private:
    // 显示开始菜单
    void showStartMenu() {
        while (true) {
            clearScreen();
            Colors::printTitle("重返崂山 - 主菜单");
            
            cout << "  ";
            Colors::printWhite("1");
            cout << " - 开始新游戏" << endl;
            
            if (SaveGame::hasSaveFile()) {
                cout << "  ";
                Colors::printWhite("2");
                cout << " - 继续游戏" << endl;
            } else {
                cout << "  ";
                Colors::printGray("2");
                Colors::printGray(" - 继续游戏 (无存档)");
                cout << endl;
            }
            
            cout << "  ";
            Colors::printWhite("3");
            cout << " - 退出游戏" << endl;
            
            if (SaveGame::hasSaveFile()) {
                cout << "  ";
                Colors::printWhite("4");
                cout << " - 删除存档" << endl;
            }
            
            cout << "\n请选择: ";
            char choice = getPlayerInput();
            cout << endl;
            
            switch(choice) {
                case '1':
                    if (startNewGame()) {
                        return;
                    }
                    break;
                case '2':
                    if (SaveGame::hasSaveFile()) {
                        if (loadGame()) {
                            return;
                        }
                    } else {
                        Colors::printWarning("没有找到存档文件！");
                        waitForInput();
                    }
                    break;
                case '3':
                    gameRunning = false;
                    return;
                case '4':
                    if (SaveGame::hasSaveFile()) {
                        cout << "确定要删除存档吗？(y/n): ";
                        char confirm = getPlayerInput();
                        if (confirm == 'y' || confirm == 'Y') {
                            SaveGame::deleteSave();
                        }
                        waitForInput();
                    }
                    break;
                default:
                    Colors::printWarning("无效选择！");
                    waitForInput();
                    break;
            }
        }
    }
    
    // 开始新游戏
    bool startNewGame() {
        clearScreen();
        Colors::printTitle("创建新角色");
        
        cout << "请输入你的角色名字: ";
        string playerName;
        
        // 设置控制台输入编码支持中文
        #ifdef _WIN32
            SetConsoleCP(65001);
            SetConsoleOutputCP(65001);
        #endif
        
        // 读取完整的中文输入
        getline(cin, playerName);
        
        // 验证名字不为空
        if (playerName.empty()) {
            playerName = "无名勇者";
        }
        
        // 创建新的玩家和地图
        if (player) delete player;
        if (gameMap) delete gameMap;
        
        player = new Player(playerName);
        gameMap = new Map();
        
        clearScreen();
        Colors::printSuccess("欢迎你, " + playerName + "！");
        Colors::printInfo("你的冒险即将开始...");
        waitForInput();
        
        return true;
    }
    
    // 加载游戏
    bool loadGame() {
        Colors::printInfo("正在加载游戏...");
        
        GameData* data = SaveGame::loadGame();
        if (!data) {
            Colors::printError("加载游戏失败！");
            waitForInput();
            return false;
        }
        
        // 创建新的玩家和地图
        if (player) delete player;
        if (gameMap) delete gameMap;
        
        player = new Player(data->playerName);
        gameMap = new Map();
        
        // 加载玩家数据
        player->loadFromData(data->level, data->hp, data->maxHP,
                            data->attack, data->defense, data->exp, data->gold);
        player->loadSkills(data->skillNames);
        
        // 加载地图数据
        gameMap->loadFromData(data->playerX, data->playerY, data->monstersDefeated,
                             data->roomVisited, data->roomCleared);
        
        delete data;
        
        clearScreen();
        Colors::printSuccess("游戏加载成功！欢迎回来，" + player->getName() + "！");
        waitForInput();
        
        return true;
    }
};

#endif