# Makefile for 重返崂山 RPG Game

# 编译器设置
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -static-libgcc -static-libstdc++

# 目标文件
TARGET = game
SOURCES = main.cpp Colors.cpp Item.cpp Skill.cpp

# 头文件
HEADERS = Player.h Room.h Map.h Game.h Colors.h Skill.h Enemy.h Battle.h Item.h SaveGame.h AsciiMap.h

# 默认目标
all: $(TARGET)

# 编译主程序
$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

# 清理编译文件
clean:
	del /Q $(TARGET).exe 2>nul || true

# 运行程序
run: $(TARGET)
	./$(TARGET)

# 重新编译
rebuild: clean all

# 显示帮助
help:
	@echo "可用的命令:"
	@echo "  make       - 编译程序"
	@echo "  make run   - 编译并运行程序" 
	@echo "  make clean - 清理编译文件"
	@echo "  make rebuild - 重新编译"
	@echo "  make help  - 显示此帮助信息"

.PHONY: all clean run rebuild help