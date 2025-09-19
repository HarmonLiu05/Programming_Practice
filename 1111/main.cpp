#include "Game.h"
#include <iostream>
using namespace std;

int main() {
    // 设置控制台编码为UTF-8（Windows）
    system("chcp 65001");
    
    try {
        cout << "正在启动游戏..." << endl;
        
        // 创建游戏实例
        Game game;
        
        // 运行游戏
        game.run();
        
        cout << "游戏结束。" << endl;
        
    } catch (const exception& e) {
        cerr << "游戏运行出错: " << e.what() << endl;
        return 1;
    }
    
    cout << "按任意键退出...";
    getch();
    return 0;
}