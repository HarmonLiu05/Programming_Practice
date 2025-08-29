#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <windows.h>
#include <iomanip>
#include <conio.h>
#include <ctime>

// --- 角色类 (完整定义) ---
class Character {
public:
    std::string name;
    int hp;
    int attackPower;

    Character(std::string name, int hp, int attackPower)
        : name(name), hp(hp), attackPower(attackPower) {}

    void takeDamage(int damage) {
        hp -= damage;
        if (hp < 0) {
            hp = 0;
        }
    }

    bool isAlive() {
        return hp > 0;
    }

    virtual std::string getStatusString() {
        return name + " - HP: " + std::to_string(hp);
    }

    virtual ~Character() {}
};

class Player : public Character {
public:
    Player(std::string name, int hp, int attackPower)
        : Character(name, hp, attackPower) {}

    std::string getStatusString() override {
        return "[玩家] " + name + " - HP: " + std::to_string(hp);
    }
};

class Enemy : public Character {
public:
    Enemy(std::string name, int hp, int attackPower)
        : Character(name, hp, attackPower) {}

    std::string getStatusString() override {
        return "[敌人] " + name + " - HP: " + std::to_string(hp);
    }
};

// --- 游戏核心类 ---

enum class RoomType {
    EMPTY,
    MONSTER,
    BOSS
};

class Room {
public:
    RoomType type;
    bool cleared = false;

    Room(RoomType type) : type(type) {}

    char getDisplayChar() const {
        if (cleared) return ' ';
        switch (type) {
            case RoomType::EMPTY:   return ' ';
            case RoomType::MONSTER: return 'M';
            case RoomType::BOSS:    return 'B';
            default:                return '?';
        }
    }
};

class Map {
public:
    std::vector<Room> rooms;
    int playerPosition = 0;

    Map() {
        rooms.emplace_back(RoomType::EMPTY);
        rooms.emplace_back(RoomType::MONSTER);
        rooms.emplace_back(RoomType::EMPTY);
        rooms.emplace_back(RoomType::MONSTER);
        rooms.emplace_back(RoomType::BOSS);
    }

    void movePlayer(int direction) {
        int newPosition = playerPosition + direction;
        if (newPosition >= 0 && newPosition < rooms.size()) {
            playerPosition = newPosition;
        }
    }

    Room& getCurrentRoom() {
        return rooms[playerPosition];
    }
};

enum class GameState {
    MAP,
    BATTLE,
    END
};

class Game {
private:
    Player player;
    Map gameMap;
    GameState currentState;
    Enemy* currentEnemy = nullptr;

public:
    Game() : player("英雄", 100, 20), currentState(GameState::MAP) {}

    ~Game() {
        delete currentEnemy;
    }

    void start() {
        while (currentState != GameState::END) {
            switch (currentState) {
                case GameState::MAP:
                    runMapLoop();
                    break;
                case GameState::BATTLE:
                    runBattleLoop();
                    break;
                case GameState::END:
                    // Loop will terminate
                    break;
            }
        }
        showGameOver();
    }

private:
    void runMapLoop() {
        displayMapView();
        handleMapInput();
        checkRoom();
    }

    void runBattleLoop() {
        displayBattleStatus();
        handleBattleInput();

        if (currentEnemy && !currentEnemy->isAlive()) {
            std::cout << "\n你击败了 " << currentEnemy->name << "！" << std::endl;
            if (currentEnemy->name == "巨龙") { // Check if it was the boss
                currentState = GameState::END; // End the game after boss is defeated
                return;
            }
            delete currentEnemy;
            currentEnemy = nullptr;
            gameMap.getCurrentRoom().cleared = true;
            currentState = GameState::MAP;
            std::cout << "按任意键返回地图..." << std::endl;
            _getch();
        } else if (!player.isAlive()) {
            currentState = GameState::END;
        }
    }

    void displayMapView() {
        system("cls"); // 清屏
        std::cout << "--- 地图 ---" << std::endl;
        std::cout << "你的位置: " << gameMap.playerPosition + 1 << " / " << gameMap.rooms.size() << std::endl;
        for (int i = 0; i < gameMap.rooms.size(); ++i) {
            if (i == gameMap.playerPosition) {
                std::cout << "[P]";
            } else {
                std::cout << "[" << gameMap.rooms[i].getDisplayChar() << "]";
            }
        }
        std::cout << "\n\n'a' 向左移动, 'd' 向右移动" << std::endl;
    }

    void handleMapInput() {
        char choice = _getch();
        if (choice == 'a' || choice == 'A') {
            gameMap.movePlayer(-1);
        } else if (choice == 'd' || choice == 'D') {
            gameMap.movePlayer(1);
        }
    }

    void checkRoom() {
        Room& room = gameMap.getCurrentRoom();
        if (!room.cleared) {
            if (room.type == RoomType::MONSTER) {
                system("cls");
                std::cout << "\n遭遇了怪物！" << std::endl;
                currentEnemy = new Enemy("哥布林", 50, 10);
                currentState = GameState::BATTLE;
                _getch();
            } else if (room.type == RoomType::BOSS) {
                system("cls");
                std::cout << "\n遭遇了最终BOSS！" << std::endl;
                currentEnemy = new Enemy("巨龙", 150, 25);
                currentState = GameState::BATTLE;
                _getch();
            }
        }
    }

    void displayBattleStatus() {
        system("cls");
        std::cout << "--- 战斗 ---" << std::endl;
        std::string playerInfo = player.getStatusString();
        std::string enemyInfo = currentEnemy ? currentEnemy->getStatusString() : "";
        std::cout << std::left << std::setw(35) << playerInfo << std::setw(35) << enemyInfo << std::endl;
        std::cout << "------------------------------------------------------------------" << std::endl;
    }

    void handleBattleInput() {
        std::cout << "你的回合，选择你的行动: 1. 普通攻击" << std::endl;
        char choice = 0;
        while(choice != '1') { choice = _getch(); }

        std::cout << "\n你选择了 [普通攻击]！" << std::endl;
        if(currentEnemy) currentEnemy->takeDamage(player.attackPower);
        
        if(currentEnemy && currentEnemy->isAlive()){
            std::cout << "敌人的回合:" << std::endl;
            player.takeDamage(currentEnemy->attackPower);
            std::cout << "\n======== 回合结束 ======== (按任意键继续)" << std::endl;
            _getch();
        }
    }

    void showGameOver() {
        system("cls");
        std::cout << "\n--- 游戏结束 ---" << std::endl;
        if (player.isAlive()) {
            std::cout << "恭喜你，通关了游戏！" << std::endl;
        } else {
            std::cout << "你失败了..." << std::endl;
        }
    }
};

// --- C++程序入口 ---
int main() {
    SetConsoleOutputCP(65001);
    srand(time(0));

    Game game;
    game.start();

    return 0;
}