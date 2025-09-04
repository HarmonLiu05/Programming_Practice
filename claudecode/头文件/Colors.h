#ifndef COLORS_H
#define COLORS_H

#include <iostream>
#include <string>

// 跨平台颜色支持
#ifdef _WIN32
    #include <windows.h>
    
    class ConsoleColor {
    private:
        HANDLE hConsole;
        
    public:
        ConsoleColor() {
            hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        }
        
        // Windows 颜色代码
        enum Color {
            BLACK = 0,
            DARK_BLUE = 1,
            DARK_GREEN = 2,
            DARK_CYAN = 3,
            DARK_RED = 4,
            DARK_MAGENTA = 5,
            DARK_YELLOW = 6,
            LIGHT_GRAY = 7,
            DARK_GRAY = 8,
            BLUE = 9,
            GREEN = 10,
            CYAN = 11,
            RED = 12,
            MAGENTA = 13,
            YELLOW = 14,
            WHITE = 15
        };
        
        void setColor(Color color) {
            SetConsoleTextAttribute(hConsole, color);
        }
        
        void reset() {
            SetConsoleTextAttribute(hConsole, LIGHT_GRAY);
        }
    };
    
#else
    // Linux/Mac ANSI 颜色代码
    class ConsoleColor {
    public:
        enum Color {
            BLACK = 30,
            RED = 31,
            GREEN = 32,
            YELLOW = 33,
            BLUE = 34,
            MAGENTA = 35,
            CYAN = 36,
            WHITE = 37,
            LIGHT_GRAY = 37,
            DARK_GRAY = 90,
            DARK_RED = 31,
            DARK_GREEN = 32,
            DARK_YELLOW = 33,
            DARK_BLUE = 34,
            DARK_MAGENTA = 35,
            DARK_CYAN = 36
        };
        
        void setColor(Color color) {
            std::cout << "\033[" << color << "m";
        }
        
        void reset() {
            std::cout << "\033[0m";
        }
    };
#endif

// 全局颜色实例
extern ConsoleColor console;

// 便捷的颜色输出函数
namespace Colors {
    void printRed(const std::string& text);
    void printGreen(const std::string& text);
    void printYellow(const std::string& text);
    void printBlue(const std::string& text);
    void printMagenta(const std::string& text);
    void printCyan(const std::string& text);
    void printWhite(const std::string& text);
    void printGray(const std::string& text);
    
    // 特殊输出函数
    void printTitle(const std::string& title);
    void printSeparator(int length = 60);
    void printSubSeparator(int length = 40);
    void printSuccess(const std::string& text);
    void printWarning(const std::string& text);
    void printError(const std::string& text);
    void printInfo(const std::string& text);
}

#endif