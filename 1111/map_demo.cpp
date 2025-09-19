#include <iostream>
#include "Colors.h"
using namespace std;

int main() {
    system("chcp 65001");
    
    Colors::printTitle("崂山探险地图");
    
    cout << "╔═════════════════════════════════════════════════╗" << endl;
    cout << "║ /▲\\  │░░░░░│ /♠\\  │░░░░░│┌─$─┐│░░░░░│▄▄▄▄▄║" << endl;
    cout << "║/[@]/│░░?░░│/▼▼▼/│░░?░░│ │ $ ││░░?░░│█ ☠ █║" << endl;
    cout << "║─────│░░░░░│\\___/│░░░░░│ └───┘│░░░░░│▀▀▀▀▀║" << endl;
    cout << "╟─────┼─────┼─────┼─────┼─────┼─────┼─────╢" << endl;
    cout << "║░░░░░│  ☾  │░░░░░│ /♠\\  │░░░░░│  ☾  │░░░░░║" << endl;
    cout << "║░░?░░│     │░░?░░│/▼▼▼/│░░?░░│     │░░?░░║" << endl;  
    cout << "║░░░░░│ ∼∼∼ │░░░░░│\\___/│░░░░░│ ∼∼∼ │░░░░░║" << endl;
    cout << "╟─────┼─────┼─────┼─────┼─────┼─────┼─────╢" << endl;
    cout << "║░░░░░│ /♠\\  │░░░░░│┌─$─┐│░░░░░│░░░░░│░░░░░║" << endl;
    cout << "║░░?░░│/▼▼▼/│░░?░░│ │ $ ││░░?░░│░░?░░│░░?░░║" << endl;
    cout << "║░░░░░│\\___/│░░░░░│ └───┘│░░░░░│░░░░░│░░░░░║" << endl;
    cout << "╚═════════════════════════════════════════════════╝" << endl;
    
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
    
    Colors::printSubSeparator(60);
    cout << "  ";
    Colors::printCyan("当前位置: "); Colors::printWhite("(0, 0)");
    cout << "      ";
    Colors::printCyan("已击败怪物: "); Colors::printYellow("0/15");
    cout << endl;
    
    Colors::printInfo("探索迷雾区域发现更多房间和怪物！");
    Colors::printSeparator(60);
    
    cout << "\n按任意键退出...";
    cin.get();
    
    return 0;
}