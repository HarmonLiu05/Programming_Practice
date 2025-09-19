#include "Colors.h"
#include <iostream>
using namespace std;

// 全局颜色实例
ConsoleColor console;

namespace Colors {
    void printRed(const string& text) {
        console.setColor(ConsoleColor::RED);
        cout << text;
        console.reset();
    }
    
    void printGreen(const string& text) {
        console.setColor(ConsoleColor::GREEN);
        cout << text;
        console.reset();
    }
    
    void printYellow(const string& text) {
        console.setColor(ConsoleColor::YELLOW);
        cout << text;
        console.reset();
    }
    
    void printBlue(const string& text) {
        console.setColor(ConsoleColor::BLUE);
        cout << text;
        console.reset();
    }
    
    void printMagenta(const string& text) {
        console.setColor(ConsoleColor::MAGENTA);
        cout << text;
        console.reset();
    }
    
    void printCyan(const string& text) {
        console.setColor(ConsoleColor::CYAN);
        cout << text;
        console.reset();
    }
    
    void printWhite(const string& text) {
        console.setColor(ConsoleColor::WHITE);
        cout << text;
        console.reset();
    }
    
    void printGray(const string& text) {
        console.setColor(ConsoleColor::DARK_GRAY);
        cout << text;
        console.reset();
    }
    
    void printTitle(const string& title) {
        cout << "\n";
        printCyan("╔");
        for (size_t i = 0; i < title.length() + 2; i++) {
            printCyan("═");
        }
        printCyan("╗\n");
        printCyan("║ ");
        printYellow(title);
        printCyan(" ║\n");
        printCyan("╚");
        for (size_t i = 0; i < title.length() + 2; i++) {
            printCyan("═");
        }
        printCyan("╝\n");
    }
    
    void printSeparator(int length) {
        cout << "\n";
        printCyan("");
        for (int i = 0; i < length; i++) {
            cout << "═";
        }
        cout << "";
        console.reset();
        cout << "\n";
    }
    
    void printSubSeparator(int length) {
        cout << "\n";
        printGray("");
        for (int i = 0; i < length; i++) {
            cout << "─";
        }
        cout << "";
        console.reset();
        cout << "\n";
    }
    
    void printSuccess(const string& text) {
        printGreen("✓ ");
        cout << text << endl;
    }
    
    void printWarning(const string& text) {
        printYellow("⚠ ");
        cout << text << endl;
    }
    
    void printError(const string& text) {
        printRed("✗ ");
        cout << text << endl;
    }
    
    void printInfo(const string& text) {
        printBlue("ℹ ");
        cout << text << endl;
    }
}