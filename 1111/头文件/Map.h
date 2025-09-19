#ifndef MAP_H
#define MAP_H

#include "Room.h"
#include "Enemy.h"
#include "Colors.h"
#include "AsciiMap.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
using namespace std;

class Map {
private:
    static const int WIDTH = 8;
    static const int HEIGHT = 3;
    Room* rooms[HEIGHT][WIDTH];
    int playerX, playerY;
    int monstersDefeated;
    
public:
    Map() : playerX(0), playerY(0), monstersDefeated(0) {  // 玩家从左上角开始
        srand(time(nullptr));  // 初始化随机种子
        generateMap();
    }
    
    ~Map() {
        // 清理动态分配的房间
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                delete rooms[i][j];
            }
        }
    }
    
    void generateMap() {
        // 清空地图
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                rooms[i][j] = nullptr;
            }
        }
        
        // 设置起始房间（左上角）
        rooms[0][0] = new Room(RoomType::START);
        
        // 设置BOSS房间（右下角）
        rooms[HEIGHT-1][WIDTH-1] = new Room(RoomType::BOSS);
        
        // 记录特殊房间位置，避免重复
        bool specialRooms[HEIGHT][WIDTH] = {false};
        specialRooms[0][0] = true;  // 起始房间
        specialRooms[HEIGHT-1][WIDTH-1] = true;  // BOSS房间
        
        // 随机放置商店（3个）
        int shopsPlaced = 0;
        while (shopsPlaced < 3) {
            int x = rand() % WIDTH;
            int y = rand() % HEIGHT;
            if (!specialRooms[y][x] && rooms[y][x] == nullptr) {
                rooms[y][x] = new Room(RoomType::SHOP);
                specialRooms[y][x] = true;
                shopsPlaced++;
            }
        }
        
        // 随机放置1个空房间
        int emptyPlaced = 0;
        while (emptyPlaced < 1) {
            int x = rand() % WIDTH;
            int y = rand() % HEIGHT;
            if (!specialRooms[y][x] && rooms[y][x] == nullptr) {
                rooms[y][x] = new Room(RoomType::EMPTY);
                specialRooms[y][x] = true;
                emptyPlaced++;
            }
        }
        
        // 剩余位置全部设为怪物房间，并为每个怪物房间分配敌人
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                if (rooms[i][j] == nullptr) {
                    rooms[i][j] = new Room(RoomType::MONSTER);
                    // 为怪物房间分配随机敌人
                    rooms[i][j]->setEnemy(EnemyFactory::createRandomEnemy());
                }
            }
        }
        
        // 为BOSS房间分配BOSS
        rooms[HEIGHT-1][WIDTH-1]->setEnemy(EnemyFactory::createBoss());
        
        // 标记起始房间为已访问
        rooms[0][0]->enter();
        
        cout << "地图生成完成！怪物们正在等待挑战者..." << endl;
    }
    
    bool movePlayer(char direction) {
        int newX = playerX;
        int newY = playerY;
        
        switch (direction) {
            case 'w': case 'W':  // 北
                newY--;
                break;
            case 's': case 'S':  // 南
                newY++;
                break;
            case 'a': case 'A':  // 西
                newX--;
                break;
            case 'd': case 'D':  // 东
                newX++;
                break;
            default:
                cout << "无效的移动指令！使用 W/A/S/D 来移动。" << endl;
                return false;
        }
        
        // 检查边界
        if (newX < 0 || newX >= WIDTH || newY < 0 || newY >= HEIGHT) {
            cout << "不能走出地图边界！" << endl;
            return false;
        }
        
        // 移动成功
        playerX = newX;
        playerY = newY;
        cout << "移动到 (" << playerX << ", " << playerY << ")" << endl;
        
        // 进入新房间
        getCurrentRoom()->enter();
        return true;
    }
    
    void displayMap() const {
        // 使用新的ASCII艺术地图显示
        AsciiMapRenderer::displayAsciiMap(rooms, playerX, playerY, monstersDefeated);
        
        // 显示房间信息
        Room* currentRoom = getCurrentRoom();
        cout << "  ";
        Colors::printCyan("房间状态: ");
        switch(currentRoom->getType()) {
            case RoomType::START:
                Colors::printGreen("起始房间 - 安全");
                break;
            case RoomType::MONSTER:
                if (currentRoom->isCleared()) {
                    Colors::printGray("怪物房间 - 已清理");
                } else {
                    Colors::printRed("怪物房间 - 危险！");
                }
                break;
            case RoomType::SHOP:
                Colors::printYellow("商店 - 可购买物品");
                break;
            case RoomType::EMPTY:
                Colors::printBlue("空房间 - 可休息");
                break;
            case RoomType::BOSS:
                Colors::printMagenta("BOSS房间 - 最终挑战！");
                break;
        }
        cout << endl;
        
        // 导航提示
        cout << "\n  ";
        Colors::printCyan("可移动方向: ");
        vector<string> validMoves;
        if (canMove('w')) validMoves.push_back("北(W)");
        if (canMove('s')) validMoves.push_back("南(S)");
        if (canMove('a')) validMoves.push_back("西(A)");
        if (canMove('d')) validMoves.push_back("东(D)");
        
        if (validMoves.empty()) {
            Colors::printGray("无法移动");
        } else {
            for (size_t i = 0; i < validMoves.size(); i++) {
                Colors::printWhite(validMoves[i]);
                if (i < validMoves.size() - 1) cout << ", ";
            }
        }
        cout << endl;
        
        // 探索建议
        cout << "  ";
        if (monstersDefeated < 15) {
            Colors::printInfo("探索迷雾区域寻找更多怪物和宝藏");
        } else {
            Colors::printSuccess("你已经可以挑战BOSS了！寻找骷髅标记的房间");
        }
        cout << endl;
    }
    
    void displayControls() const {
        cout << "\n=== 操作说明 ===" << endl;
        cout << "W - 向北移动" << endl;
        cout << "A - 向西移动" << endl;
        cout << "S - 向南移动" << endl;
        cout << "D - 向东移动" << endl;
        cout << "M - 查看地图" << endl;
        cout << "P - 查看玩家状态" << endl;
        cout << "Q - 退出游戏" << endl;
        cout << "===============" << endl;
    }
    
    Room* getCurrentRoom() const {
        return rooms[playerY][playerX];
    }
    
    bool canMove(char direction) const {
        int newX = playerX;
        int newY = playerY;
        
        switch (direction) {
            case 'w': case 'W': newY--; break;
            case 's': case 'S': newY++; break;
            case 'a': case 'A': newX--; break;
            case 'd': case 'D': newX++; break;
            default: return false;
        }
        
        return (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT);
    }
    
    int getMonstersDefeated() const { return monstersDefeated; }
    void incrementMonstersDefeated() { monstersDefeated++; }
    
    // 获取玩家位置
    int getPlayerX() const { return playerX; }
    int getPlayerY() const { return playerY; }
    
    // 保存/加载支持方法
    bool isRoomVisited(int x, int y) const {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            return rooms[y][x]->isVisited();
        }
        return false;
    }
    
    bool isRoomCleared(int x, int y) const {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            return rooms[y][x]->isCleared();
        }
        return false;
    }
    
    void setPlayerPosition(int x, int y) {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            playerX = x;
            playerY = y;
        }
    }
    
    void setMonstersDefeated(int count) {
        monstersDefeated = count;
    }
    
    // 设置房间状态（用于加载游戏时恢复状态）
    void setRoomVisited(int x, int y, bool visited) {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            rooms[y][x]->setVisited(visited);
        }
    }
    
    void setRoomCleared(int x, int y, bool cleared) {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            rooms[y][x]->setCleared(cleared);
        }
    }
    
    // 检查游戏胜利条件
    bool isGameWon() const {
        // 在BOSS房间且已击败15只普通怪物
        Room* currentRoom = getCurrentRoom();
        return (currentRoom->getType() == RoomType::BOSS && monstersDefeated >= 15);
    }
    
    // 从GameData加载地图状态
    void loadFromData(int playerPosX, int playerPosY, int defeatedCount, 
                      bool roomVisited[3][7], bool roomCleared[3][7]) {
        // 设置玩家位置和击败计数
        setPlayerPosition(playerPosX, playerPosY);
        setMonstersDefeated(defeatedCount);
        
        // 恢复房间状态
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 7; j++) {
                setRoomVisited(j, i, roomVisited[i][j]);
                setRoomCleared(j, i, roomCleared[i][j]);
            }
        }
    }
};

#endif