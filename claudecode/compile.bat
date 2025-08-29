@echo off
echo 正在编译重返崂山RPG游戏（新技能和道具系统）...
echo.

REM 设置编译器选项
set COMPILER=g++
set FLAGS=-std=c++17 -Wall -Wextra -O2 -static-libgcc -static-libstdc++
set OUTPUT=game_latest.exe
set SOURCES=main.cpp Colors.cpp Item.cpp Skill.cpp

REM 删除旧的exe文件
if exist %OUTPUT% del %OUTPUT%

REM 编译游戏
%COMPILER% %FLAGS% -o %OUTPUT% %SOURCES%

REM 检查编译结果
if exist %OUTPUT% (
    echo.
    echo ✅ 编译成功！生成了 %OUTPUT%
    echo 📁 位置: %cd%\%OUTPUT%
    echo.
    echo 🎮 新功能:
    echo   - 12种全新技能：致死打击、生命吸取、复仇、财富一击等
    echo   - 7种创新道具：治疗药水、能量凝胶、烤鸡、瓶中妖精等
    echo   - 复杂的技能机制：血量条件、伤害共享、状态标记
    echo   - 智能道具效果：百分比治疗、自动触发、buff/debuff
    echo.
    echo 运行游戏请双击 %OUTPUT% 或在命令行输入: %OUTPUT%
) else (
    echo.
    echo ❌ 编译失败！请检查错误信息。
    echo 确保安装了MinGW-w64或其他C++编译器。
)

echo.
pause