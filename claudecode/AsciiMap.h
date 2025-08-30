#ifndef ASCIIMAP_H
#define ASCIIMAP_H

#include <string>
#include <vector>
#include <iostream>
#include "Colors.h"
#include "Room.h"

using namespace std;

class AsciiMapRenderer {
private:
    static const int WIDTH = 8;
    static const int HEIGHT = 3;
    
    // 每个房间的ASCII艺术表示（3行x5列）
    struct RoomAscii {
        vector<string> lines;
        RoomAscii() : lines(3) {}
    };
    
public:
    static void displayAsciiMap(Room* const rooms[HEIGHT][WIDTH], int playerX, int playerY, int monstersDefeated) {
        Colors::printTitle("崂山探险地图");
        
        // 创建ASCII艺术地图
        vector<vector<RoomAscii>> asciiRooms(HEIGHT, vector<RoomAscii>(WIDTH));
        
        // 为每个房间生成ASCII艺术
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                generateRoomAscii(asciiRooms[i][j], rooms[i][j], j == playerX && i == playerY);
            }
        }
        
        // 绘制地图边框
        cout << "╔";
        for (int j = 0; j < WIDTH * 6 - 1; j++) cout << "═";
        cout << "╗" << endl;
        
        // 逐行绘制地图
        for (int row = 0; row < HEIGHT; row++) {
            for (int line = 0; line < 3; line++) {
                cout << "║";
                for (int col = 0; col < WIDTH; col++) {
                    cout << asciiRooms[row][col].lines[line];
                    if (col < WIDTH - 1) cout << "│";
                }
                cout << "║" << endl;
            }
            if (row < HEIGHT - 1) {
                cout << "╟";
                for (int j = 0; j < WIDTH; j++) {
                    cout << "─────";
                    if (j < WIDTH - 1) cout << "┼";
                }
                cout << "╢" << endl;
            }
        }
        
        // 绘制底部边框
        cout << "╚";
        for (int j = 0; j < WIDTH * 6 - 1; j++) cout << "═";
        cout << "╝" << endl;
        
        // 显示图例
        displayLegend();
        
        // 显示状态信息
        displayStatus(playerX, playerY, monstersDefeated);
    }
    
private:
    static void generateRoomAscii(RoomAscii& ascii, Room* room, bool hasPlayer) {
        RoomType type = room->getType();
        bool visited = room->isVisited();
        bool cleared = room->isCleared();
        
        if (!visited) {
            // 未访问的房间显示为迷雾
            ascii.lines[0] = "░░░░░";
            ascii.lines[1] = "░░?░░";
            ascii.lines[2] = "░░░░░";
        } else {
            switch (type) {
                case RoomType::START:
                    generateStartRoom(ascii, hasPlayer);
                    break;
                case RoomType::MONSTER:
                    generateMonsterRoom(ascii, hasPlayer, cleared);
                    break;
                case RoomType::SHOP:
                    generateShopRoom(ascii, hasPlayer);
                    break;
                case RoomType::EMPTY:
                    generateEmptyRoom(ascii, hasPlayer);
                    break;
                case RoomType::BOSS:
                    generateBossRoom(ascii, hasPlayer);
                    break;
            }
        }
    }
    
    static void generateStartRoom(RoomAscii& ascii, bool hasPlayer) {
        ascii.lines[0] = " /▲\\ ";
        ascii.lines[1] = hasPlayer ? "/[@]/" : "/   /";
        ascii.lines[2] = "─────";
    }
    
    static void generateMonsterRoom(RoomAscii& ascii, bool hasPlayer, bool cleared) {
        if (cleared) {
            // 已清理的怪物房
            ascii.lines[0] = "     ";
            ascii.lines[1] = hasPlayer ? "  @  " : "     ";
            ascii.lines[2] = " ╳╳╳ ";
        } else {
            // 有怪物的房间
            ascii.lines[0] = " /♠\\ ";
            ascii.lines[1] = hasPlayer ? "/[@]/" : "/▼▼▼/";
            ascii.lines[2] = "\\___/";
        }
    }
    
    static void generateShopRoom(RoomAscii& ascii, bool hasPlayer) {
        ascii.lines[0] = "┌─$─┐";
        ascii.lines[1] = hasPlayer ? "│@$ │" : "│ $ │";
        ascii.lines[2] = "└───┘";
    }
    
    static void generateEmptyRoom(RoomAscii& ascii, bool hasPlayer) {
        ascii.lines[0] = "  ☾  ";
        ascii.lines[1] = hasPlayer ? " [@] " : "     ";
        ascii.lines[2] = " ∼∼∼ ";
    }
    
    static void generateBossRoom(RoomAscii& ascii, bool hasPlayer) {
        ascii.lines[0] = "▄▄▄▄▄";
        ascii.lines[1] = hasPlayer ? "█[@]█" : "█ ☠ █";
        ascii.lines[2] = "▀▀▀▀▀";
    }
    
    static void displayLegend() {
        Colors::printSubSeparator(60);
        Colors::printInfo("图例:");
        
        cout << "  ";
        Colors::printGreen("/▲\\"); cout << " = 起始点    ";
        Colors::printRed("/♠\\"); cout << " = 怪物房    ";
        Colors::printYellow("┌$┐"); cout << " = 商店房" << endl;
        
        cout << "  ";
        Colors::printBlue("☾"); cout << " = 休息房      ";
        Colors::printMagenta("☠"); cout << " = BOSS房     ";
        Colors::printCyan("@"); cout << " = 你的位置" << endl;
        
        cout << "  ";
        Colors::printGray("░?░"); cout << " = 未知区域   ";
        Colors::printGray("╳╳╳"); cout << " = 已清理" << endl;
    }
    
    static void displayStatus(int playerX, int playerY, int monstersDefeated) {
        Colors::printSubSeparator(60);
        
        cout << "  ";
        Colors::printCyan("已击败怪物: ");
        if (monstersDefeated >= 12) {
            Colors::printGreen(to_string(monstersDefeated) + "/12");
        } else {
            Colors::printYellow(to_string(monstersDefeated) + "/12");
        }
        cout << "      ";
        Colors::printCyan("当前位置: ");
        Colors::printWhite("(" + to_string(playerX) + ", " + to_string(playerY) + ")");
        cout << endl;
        
        Colors::printSeparator(60);
    }
};

#endif