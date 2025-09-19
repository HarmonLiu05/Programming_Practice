#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Map.h"
#include "Colors.h"
#include "Skill.h"
#include "Enemy.h"
#include "Battle.h"
#include "SaveGame.h"
#include "AudioManager.h"
#include <iostream>
#include <string>

// ?
#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    #define CLEAR_SCREEN "cls"
#else
    #include <termios.h>
    #include <unistd.h>
    #define CLEAR_SCREEN "clear"
    
    // Linux/Mac  getch 
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
        Colors::printTitle("!");
        Colors::printSuccess("RPG?"\);
        
        // ?        AudioManager& audioManager = AudioManager::getInstance();
        if (audioManager.initialize()) {
            // 
            audioManager.playBackgroundMusic("music/DDRKirby(ISQ) - Dawn.mp3");
        }
        
        // ?        showStartMenu();
        
        if (player && gameMap) {
            // 
            displayGameInterface();
            
            gameLoop();
        }
        
        // 
        audioManager.cleanup();
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
                    Colors::printInfo("...");
                    getPlayerInput();
                    displayGameInterface();
                    break;
                    
                case 'b': case 'B':
                    clearScreen();
                    player->displayInventory();
                    Colors::printInfo("...");
                    getPlayerInput();
                    displayGameInterface();
                    break;
                    
                case 'q': case 'Q':
                {
                    cout << "\n:" << endl;
                    cout << "  1 - ? << endl;
                    cout << "  2 - " << endl;
                    cout << "  3 - " << endl;
                    cout << " (1-3): ";
                    char exitChoice = getPlayerInput();
                    cout << endl;
                    
                    switch(exitChoice) {
                        case '1':
                            if (SaveGame::saveGame(player, gameMap)) {
                                Colors::printSuccess("?"\);
                            } else {
                                Colors::printError("?"\);
                            }
                            gameRunning = false;
                            break;
                        case '2':
                        {
                            cout << "?y/n): ";
                            char confirm = getPlayerInput();
                            if (confirm == 'y' || confirm == 'Y') {
                                cout << "" << endl;
                                gameRunning = false;
                            }
                            break;
                        }
                        case '3':
                        default:
                            // 
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
                    // 
                    Colors::printInfo("...");
                    if (SaveGame::saveGame(player, gameMap)) {
                        Colors::printSuccess("?"\);
                    } else {
                        Colors::printError("?"\);
                    }
                    break;
                    
                case 't': case 'T':
                    // ?                    {
                        AudioManager& audioManager = AudioManager::getInstance();
                        if (audioManager.isBackgroundMusicPlaying()) {
                            audioManager.stopBackgroundMusic();
                            Colors::printInfo("?"\);
                        } else {
                            audioManager.playBackgroundMusic("music/DDRKirby(ISQ) - Dawn.mp3");
                            Colors::printInfo("?"\);
                        }
                    }
                    break;
                    
                default:
                    cout << " H ? << endl;
                    break;
            }
            
            // ?            checkWinCondition();
        }
    }
    
    void handleMovement(char direction) {
        if (gameMap->canMove(direction)) {
            cout << "\n..." << endl;
            
            if (gameMap->movePlayer(direction)) {
                // 
                displayGameInterface();
                
                // 
                handleRoomEntry();
                
            }
        } else {
            cout << "? << endl;
        }
    }
    
    void handleRoomEntry() {
        Room* currentRoom = gameMap->getCurrentRoom();
        RoomType roomType = currentRoom->getType();
        
        switch (roomType) {
            case RoomType::START:
                Colors::printSuccess("?"\);
                break;
                
            case RoomType::MONSTER:
                if (currentRoom->hasEnemy()) {
                    Enemy* enemy = currentRoom->getEnemy();
                    Colors::printWarning("? + enemy->getName() + "?);
                    waitForInput();
                    
                    // ?                    Battle battle(player, enemy);
                    bool playerWon = battle.startBattle();
                    
                    if (playerWon) {
                        // 
                        auto rewards = battle.getBattleRewards();
                        for (const auto& reward : rewards) {
                            if (reward.type == DropType::SKILL) {
                                if (reward.skill) { if (!player->addSkill(reward.skill)) { delete reward.skill; } }
                                //  - 
                                if (reward.name == "? || reward.name == "") {"
                                    { Item* it = ItemFactory::createHealingPotion(); if (!player->addItem(it)) delete it; }
                                } else if (reward.name == "") {
                                    { Item* it = ItemFactory::createEnergyGel(); if (!player->addItem(it)) delete it; }
                                } else if (reward.name == "") {
                                    { Item* it = ItemFactory::createFirstAidSpray(); if (!player->addItem(it)) delete it; }
                                } else if (reward.name == "?) {"
                                    { Item* it = ItemFactory::createFirstAidSpray(); if (!player->addItem(it)) delete it; } // 
                                } else if (reward.name == "?) {"
                                    { Item* it = ItemFactory::createRoastChicken(); if (!player->addItem(it)) delete it; } // 
                                } else {
                                    // ?                                    { Item* it = ItemFactory::createHealingPotion(); if (!player->addItem(it)) delete it; }
                                }
                            }
                        }
                        
                        // 
                        currentRoom->clear();
                        gameMap->incrementMonstersDefeated();
                        
                        // 
                        clearScreen();
                        Colors::printTitle("");
                        displayGameInterface();
                        
                    } else {
                        // 
                        if (!player->isAlive()) {
                            Colors::printTitle("");
                            Colors::printError("...");
                            gameRunning = false;
                            return;
                        } else if (battle.didPlayerEscape()) {
                            // 
                            waitForInput();
                            displayGameInterface();
                        }
                    }
                    
                } else {
                    Colors::printSuccess("?"\);
                }
                break;
                
            case RoomType::SHOP:
                Colors::printTitle("!");
                Colors::printYellow(": '?");
                handleShop();
                break;
                
            case RoomType::EMPTY:
            {
                Colors::printTitle("?"\);
                Colors::printInfo("?"\);
                
                if (currentRoom->canRest()) {
                    Colors::printSuccess("?"\);
                    player->restoreToFullHP();
                    currentRoom->setRested();
                } else {
                    Colors::printGray("?"\);
                }
                
                break;
            }
                
            case RoomType::BOSS:
                if (gameMap->getMonstersDefeated() >= 12) {
                    Colors::printTitle("?"\);
                    Colors::printRed("");
                    
                    Enemy* boss = currentRoom->getEnemy();
                    Colors::printMagenta("...");
                    waitForInput();
                    
                    // BOSS
                    Battle bossBattle(player, boss);
                    bool playerWon = bossBattle.startBattle();
                    
                    if (playerWon) {
                        // BOSS
                        auto rewards = bossBattle.getBattleRewards();
                        for (const auto& reward : rewards) {
                            if (reward.type == DropType::SKILL) {
                                if (reward.skill) { if (!player->addSkill(reward.skill)) { delete reward.skill; } }
                            } else if (reward.type == DropType::POTION) {
                                //  - 
                                if (reward.name == "? || reward.name == "") {"
                                    { Item* it = ItemFactory::createHealingPotion(); if (!player->addItem(it)) delete it; }
                                } else if (reward.name == "") {
                                    { Item* it = ItemFactory::createEnergyGel(); if (!player->addItem(it)) delete it; }
                                } else if (reward.name == "") {
                                    { Item* it = ItemFactory::createFirstAidSpray(); if (!player->addItem(it)) delete it; }
                                } else if (reward.name == "?) {"
                                    { Item* it = ItemFactory::createFirstAidSpray(); if (!player->addItem(it)) delete it; } // 
                                } else if (reward.name == "?) {"
                                    { Item* it = ItemFactory::createRoastChicken(); if (!player->addItem(it)) delete it; } // 
                                } else {
                                    // ?                                    { Item* it = ItemFactory::createHealingPotion(); if (!player->addItem(it)) delete it; }
                                }
                            }
                        }
                        
                        currentRoom->clear();
                        Colors::printTitle("");
                        Colors::printSuccess("?"\);
                        gameWon = true;
                        gameRunning = false;
                        return;
                    } else {
                        if (!player->isAlive()) {
                            Colors::printTitle("");
                            Colors::printError("...");
                            gameRunning = false;
                            return;
                        }
                    }
                    
                } else {
                    Colors::printTitle("?"\);
                    Colors::printRed("...");
                    Colors::printWarning("BOSS?"\);
                    cout << "   ";
                    Colors::printYellow(to_string(12 - gameMap->getMonstersDefeated()));
                    cout << " BOSS? << endl;
                    Colors::printInfo("");
                }
                break;
        }
        
        // 
        if (roomType != RoomType::MONSTER && roomType != RoomType::BOSS) {
            if (roomType == RoomType::EMPTY) {
                waitForInput();
            }
        } else if (currentRoom->isCleared()) {
            // ?            // ?        }
    }
    
    char getPlayerInput() {
        return getch();  // Windows?    }
    
    void clearScreen() {
        system(CLEAR_SCREEN);
    }
    
    void waitForInput() {
        cout << "\n...";
        getch();
        cout << endl;
    }
    
    void checkWinCondition() {
        if (gameMap->isGameWon()) {
            cout << "\n=== ?===" << endl;
            cout << "" << endl;
            cout << "BOSS? << endl;
            cout << "=============" << endl;
            gameWon = true;
            gameRunning = false;
        }
    }
    
    void handleShop() {
        Colors::printSubSeparator(50);
        Colors::printInfo("");
        
        cout << "  ";
        Colors::printWhite("1");
        cout << " - ?(";
        Colors::printYellow("100");
        cout << " )" << endl;
        
        cout << "  ";
        Colors::printWhite("2");
        cout << " -  (";
        Colors::printYellow("30");
        cout << " ) - 50HP" << endl;
        
        cout << "  ";
        Colors::printWhite("3");
        cout << " -  (";
        Colors::printYellow("25");
        cout << " ) - 30%HP" << endl;
        
        cout << "  ";
        Colors::printWhite("4");
        cout << " -  (";
        Colors::printYellow("80");
        cout << " ) - 70%HP" << endl;
        
        cout << "  ";
        Colors::printWhite("5");
        cout << " -  (";
        Colors::printYellow("120");
        cout << " ) - " << endl;
        
        cout << "  ";
        Colors::printWhite("6");
        cout << " - " << endl;
        
        cout << "\n ";
        Colors::printYellow(to_string(player->getGold()));
        cout << "  (1-6): ";
        
        char choice = getPlayerInput();
        cout << endl;
        
        switch(choice) {
            case '1':
                if (player->getGold() >= 100) {
                    if (player->getSkillCount() < player->getMaxSkills()) {
                        player->spendGold(100);
                        // 70%?0%
                        Skill* newSkill;
                        if (rand() % 100 < 70) {
                            newSkill = SkillFactory::createRandomSkillByRarity(SkillRarity::LEGENDARY);
                        } else {
                            newSkill = SkillFactory::createRandomSkillByRarity(SkillRarity::EPIC);
                        }
                        player->addSkill(newSkill);
                        Colors::printSuccess("?"\);
                    } else {
                        Colors::printWarning("?"\);
                    }
                } else {
                    Colors::printWarning("?"\);
                }
                break;
                
            case '2':
                if (player->getGold() >= 30) {
                    if (player->getItemCount() < player->getMaxItems()) {
                        player->spendGold(30);
                        { Item* it = ItemFactory::createHealingPotion(); if (!player->addItem(it)) delete it; }
                        Colors::printSuccess("?"\);
                    } else {
                        Colors::printWarning("?"\);
                    }
                } else {
                    Colors::printWarning("?"\);
                }
                break;
                
            case '3':
                if (player->getGold() >= 25) {
                    if (player->getItemCount() < player->getMaxItems()) {
                        player->spendGold(25);
                        { Item* it = ItemFactory::createEnergyGel(); if (!player->addItem(it)) delete it; }
                        Colors::printSuccess("?"\);
                    } else {
                        Colors::printWarning("?"\);
                    }
                } else {
                    Colors::printWarning("?"\);
                }
                break;
                
            case '4':
                if (player->getGold() >= 80) {
                    if (player->getItemCount() < player->getMaxItems()) {
                        player->spendGold(80);
                        { Item* it = ItemFactory::createFirstAidSpray(); if (!player->addItem(it)) delete it; }
                        Colors::printSuccess("?"\);
                    } else {
                        Colors::printWarning("?"\);
                    }
                } else {
                    Colors::printWarning("?"\);
                }
                break;
                
            case '5':
                if (player->getGold() >= 120) {
                    if (player->getItemCount() < player->getMaxItems()) {
                        player->spendGold(120);
                        { Item* it = ItemFactory::createRoastChicken(); if (!player->addItem(it)) delete it; }
                        Colors::printSuccess("?"\);
                    } else {
                        Colors::printWarning("?"\);
                    }
                } else {
                    Colors::printWarning("?"\);
                }
                break;
                
            case '6':
                Colors::printInfo(": '?");
                break;
                
            default:
                Colors::printWarning("?"\);
                break;
        }
        Colors::printSubSeparator(50);
    }
    
    void displayControls() const {
        Colors::printTitle("");
        cout << "  ";
        Colors::printCyan(":");
        cout << " ";
        Colors::printWhite("W");
        cout << "-?";
        Colors::printWhite("A");
        cout << "-?";
        Colors::printWhite("S");
        cout << "-?";
        Colors::printWhite("D");
        cout << "-? << endl;
        
        cout << "  ";
        Colors::printCyan(":");
        cout << " ";
        Colors::printWhite("M");
        cout << "- ";
        Colors::printWhite("P");
        cout << "-?";
        Colors::printWhite("I");
        cout << "-?";
        Colors::printWhite("B");
        cout << "- ";
        Colors::printWhite("H");
        cout << "- ";
        Colors::printWhite("C");
        cout << "-" << endl;
        
        cout << "  ";
        Colors::printCyan(":");
        cout << " ";
        Colors::printWhite("Q");
        cout << "-?";
        Colors::printWhite("X");
        cout << "- ";
        Colors::printWhite("T");
        cout << "-? << endl;
        
        Colors::printSubSeparator(50);
    }
    
    // 
    void displayGameInterface() {
        clearScreen();
        
        // ?        player->displayStats();
        
        // 
        gameMap->displayMap();
        
        // 
        displayGameControls();
    }
    
    // ?    void displayGameControls() {
        Colors::printSubSeparator(60);
        Colors::printInfo(": W/A/S/D- | M- P-?I-?B- | Q-?X- T- H-");
        cout << "\n? ";
    }
    
private:
    // ?    void showStartMenu() {
        while (true) {
            clearScreen();
            Colors::printTitle(" - ?"\);
            
            cout << "  ";
            Colors::printWhite("1");
            cout << " - " << endl;
            
            if (SaveGame::hasSaveFile()) {
                cout << "  ";
                Colors::printWhite("2");
                cout << " - " << endl;
            } else {
                cout << "  ";
                Colors::printGray("2");
                Colors::printGray(" -  (?");
                cout << endl;
            }
            
            cout << "  ";
            Colors::printWhite("3");
            cout << " - ? << endl;
            
            if (SaveGame::hasSaveFile()) {
                cout << "  ";
                Colors::printWhite("4");
                cout << " - " << endl;
            }
            
            cout << "\n: ";
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
                        Colors::printWarning("?"\);
                        waitForInput();
                    }
                    break;
                case '3':
                    gameRunning = false;
                    return;
                case '4':
                    if (SaveGame::hasSaveFile()) {
                        cout << "?y/n): ";
                        char confirm = getPlayerInput();
                        if (confirm == 'y' || confirm == 'Y') {
                            SaveGame::deleteSave();
                        }
                        waitForInput();
                    }
                    break;
                default:
                    Colors::printWarning("?"\);
                    waitForInput();
                    break;
            }
        }
    }
    
    // 
    bool startNewGame() {
        clearScreen();
        Colors::printTitle("?"\);
        
        cout << "? ";
        string playerName;
        
        // ?        #ifdef _WIN32
            SetConsoleCP(65001);
            SetConsoleOutputCP(65001);
        #endif
        
        // ?        getline(cin, playerName);
        
        // ?        if (playerName.empty()) {
            playerName = "?;"
        }
        
        // ?        if (player) delete player;
        if (gameMap) delete gameMap;
        
        player = new Player(playerName);
        gameMap = new Map();
        
        clearScreen();
        Colors::printSuccess("? " + playerName + "?"\);
        Colors::printInfo("?..");
        waitForInput();
        
        return true;
    }
    
    // 
    bool loadGame() {
        Colors::printInfo("...");
        
        GameData* data = SaveGame::loadGame();
        if (!data) {
            Colors::printError("?"\);
            waitForInput();
            return false;
        }
        
        // ?        if (player) delete player;
        if (gameMap) delete gameMap;
        
        player = new Player(data->playerName);
        gameMap = new Map();
        
        // 
        player->loadFromData(data->level, data->hp, data->maxHP,
                            data->attack, data->defense, data->exp, data->gold);
        player->loadSkills(data->skillNames);
        
        // 
        gameMap->loadFromData(data->playerX, data->playerY, data->monstersDefeated,
                             data->roomVisited, data->roomCleared);
        
        delete data;
        
        clearScreen();
        Colors::printSuccess("" + player->getName() + "?"\);
        waitForInput();
        
        return true;
    }
};

#endif



