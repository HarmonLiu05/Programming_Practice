#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <iostream>
#include <vector>
#include "Colors.h"

using namespace std;

// 前置声明
class Player;
class Character;

enum class ItemType {
    HEALING_POTION,    // 治疗药水
    ENERGY_GEL,        // 能量凝胶
    FIRST_AID_SPRAY,   // 急救喷雾
    ROAST_CHICKEN,     // 烤鸡
    ADRENALINE_SHOT,   // 兴奋剂
    DARK_PACT,         // 黑暗契约
    FAIRY_BOTTLE       // 瓶中妖精
};

class Item {
private:
    string name;
    string description;
    ItemType type;
    int value;      // 效果数值
    int price;      // 商店价格
    bool isAutoTrigger;  // 是否自动触发（如瓶中妖精）
    
public:
    Item(string n, string desc, ItemType t, int v, int p = 0, bool autoTrigger = false) 
        : name(n), description(desc), type(t), value(v), price(p), isAutoTrigger(autoTrigger) {}
    
    // 使用道具
    void use(Player& player) const;
    
    // 在战斗中使用道具
    void useInBattle(Player& player, Character& enemy) const;
    
    // 自动触发效果（用于瓶中妖精等）
    bool autoTrigger(Player& player) const;
    
    // 显示道具信息
    void displayInfo() const {
        Colors::printYellow(name);
        cout << " - " << description << endl;
        cout << "    ";
        
        if (type == ItemType::HEALING_POTION || type == ItemType::ENERGY_GEL || 
            type == ItemType::FIRST_AID_SPRAY || type == ItemType::ROAST_CHICKEN) {
            Colors::printGreen("治疗效果");
        } else if (type == ItemType::ADRENALINE_SHOT) {
            Colors::printRed("增益道具");
        } else if (type == ItemType::DARK_PACT) {
            Colors::printMagenta("危险道具");
        } else if (type == ItemType::FAIRY_BOTTLE) {
            Colors::printBlue("自动触发");
        }
        
        if (isAutoTrigger) {
            cout << " ";
            Colors::printCyan("[自动]");
        }
        cout << endl;
    }
    
    // 显示简短信息
    void displayShortInfo() const {
        Colors::printYellow(name);
        if (type == ItemType::HEALING_POTION || type == ItemType::ENERGY_GEL) {
            cout << " (+" << value << "HP)";
        } else if (type == ItemType::FIRST_AID_SPRAY) {
            cout << " (+70%HP)";
        } else if (type == ItemType::ROAST_CHICKEN) {
            cout << " (完全恢复)";
        } else if (type == ItemType::FAIRY_BOTTLE) {
            cout << " (复活)";
        }
    }
    
    // Getters
    string getName() const { return name; }
    string getDescription() const { return description; }
    ItemType getType() const { return type; }
    int getValue() const { return value; }
    int getPrice() const { return price; }
    bool isAutoTriggerItem() const { return isAutoTrigger; }
    
    // 检查是否可以在战斗中使用
    bool canUseInBattle() const {
        return type != ItemType::FAIRY_BOTTLE; // 瓶中妖精自动触发，不能手动使用
    }
};

// 道具工厂类
class ItemFactory {
public:
    // 治疗药水
    static Item* createHealingPotion() {
        return new Item("治疗药水", "一瓶装着红色液体的标准药水，能迅速治愈伤口", 
                       ItemType::HEALING_POTION, 50, 30);
    }
    
    // 能量凝胶
    static Item* createEnergyGel() {
        return new Item("能量凝胶", "一小包浓缩的高能量凝胶，味道像是奇怪的水果口味", 
                       ItemType::ENERGY_GEL, 30, 25); // 30%最大血量
    }
    
    // 急救喷雾
    static Item* createFirstAidSpray() {
        return new Item("急救喷雾", "高压喷雾罐，能快速消毒并覆盖伤口，在紧急情况下非常有用", 
                       ItemType::FIRST_AID_SPRAY, 70, 80); // 70%血量
    }
    
    // 烤鸡
    static Item* createRoastChicken() {
        return new Item("烤鸡", "一份藏在奇怪地方的美味烤鸡，散发着诱人的香味", 
                       ItemType::ROAST_CHICKEN, 100, 120); // 完全恢复
    }
    
    // 兴奋剂
    static Item* createAdrenalineShot() {
        return new Item("兴奋剂", "一支装有未知化学物质的注射器，能激发身体的潜能", 
                       ItemType::ADRENALINE_SHOT, 50, 100); // 50%血量+攻击力buff
    }
    
    // 黑暗契约
    static Item* createDarkPact() {
        return new Item("黑暗契约", "一张散发着不祥气息的卷轴，似乎需要付出代价才能使用", 
                       ItemType::DARK_PACT, 5, 150); // 完全恢复但-5%最大血量
    }
    
    // 瓶中妖精
    static Item* createFairyBottle() {
        return new Item("瓶中妖精", "一只被困在玻璃瓶中的小妖精，看起来很焦急", 
                       ItemType::FAIRY_BOTTLE, 50, 200, true); // 50%血量复活，自动触发
    }
    
    // 随机创建道具
    static Item* createRandomItem() {
        vector<Item*(*)()> itemCreators = {
            createHealingPotion, createEnergyGel, createFirstAidSpray,
            createRoastChicken, createAdrenalineShot, createDarkPact,
            createFairyBottle
        };
        
        // 根据稀有度分配权重
        vector<int> weights = {25, 20, 15, 10, 15, 5, 10}; // 对应上面道具的权重
        int totalWeight = 0;
        for (int w : weights) totalWeight += w;
        
        int random = rand() % totalWeight;
        int currentWeight = 0;
        
        for (size_t i = 0; i < weights.size(); i++) {
            currentWeight += weights[i];
            if (random < currentWeight) {
                return itemCreators[i]();
            }
        }
        
        return createHealingPotion(); // 默认返回治疗药水
    }
    
    // 根据名称创建道具（用于存档系统）
    static Item* createItemByName(const string& name) {
        if (name == "治疗药水") return createHealingPotion();
        else if (name == "能量凝胶") return createEnergyGel();
        else if (name == "急救喷雾") return createFirstAidSpray();
        else if (name == "烤鸡") return createRoastChicken();
        else if (name == "兴奋剂") return createAdrenalineShot();
        else if (name == "黑暗契约") return createDarkPact();
        else if (name == "瓶中妖精") return createFairyBottle();
        else return createHealingPotion(); // 默认返回治疗药水
    }
};

#endif