解决以下问题：

 - 存档网格尺寸不一致：Map 是 3×8，但 SaveGame 用的是 3×7（数组与读写循环），会丢失最右一列的访问/清理
    状态并越界风险。

BOSS 条件不一致：BOSS 进入阈值用 12（Game 中），胜利用 15（Map::isGameWon()），AsciiMap 的状态展示
  又用 12/12。建议统一一处配置，如都用 12

技能/道具“效果系统”未接入伤害计算：SkillEffects::applyAttackBuff/applySharedDamage 与
  ItemEffects::applyAdrenalineBuff/applyAdrenalineDebuff 未在 Battle 的普攻/受击路径中调用，导致描述与
  实际未对齐。

源码重复：根目录与 头文件/ 下有重复同名文件，容易导致编译/引用混乱，建议移除或统一单份来源。



1. 架构与主程序：规划项目结构与主循环（Game）、输入处理、固定布局UI输出、Windows UTF-8 显示/输入适配。
2. 战斗系统：回合流程、普通攻击与百分比减伤公式、暴击判定、逃跑机制、胜负结算与战利品生成。
3. 技能系统：一次性技能框架、工厂与随机生成/按名加载映射，完成功能技能（致死打击、生命吸取、复仇、死亡标记、致命一击、虚
     弱打击、财富一击、运气骰子）
4.  敌人与BOSS：敌人类与工厂、随机刷怪接入；新增6类敌人落地；BOSS三阶段AI（喷火/龙息风暴/觉醒回血与强攻）。
5. 地图与探索：3×8 ASCII地图渲染、房间类型（起点/怪物/商店/休息/BOSS）、移动与进入事件处理。
6.  道具与商店：道具工厂与背包容量校验、战斗内外使用规则、价格与购买流程。
7.  存档系统：保存/加载/删除存档，断点继续。
8. 音频与体验：miniaudio集成，背景音乐与基础音效。



中文显示/输入乱码
      - 现象：Windows 控制台中文标题/文案出现问号或乱码，输入姓名不稳定。
            - 解决：启动时执行 chcp 65001，使用 SetConsoleCP/SetConsoleOutputCP 切换 UTF-8；源码文件统一 UTF-8 无 BOM；控制台
    选择等宽中文字体。

 随机刷怪种类受限
      - 现象：怪物房间重复度高、种类单一，体验单调。
            - 解决：扩充 EnemyType（史莱姆·赤、哥布林斥候、幽影道士、石像鬼、雾妖·崂山、堕仙残魂）并加入工厂随机池；维持 BOSS
    独立创建。
    -
      - 数值平衡（普攻过强）
            -       - 现象：玩家仅用普攻即可碾压，技能存在感弱。
              - 解决：重写伤害为“百分比减伤”公式（上限 50%），上调普通/精英怪 HP/DEF 曲线，保留 BOSS 数值与三阶段；实测回合数更
      匹配，技能价值提升。

cin 与 getline 混用
      - 现象：读取字符串被跳过（立即返回空行）。
            - 解决方式：在使用 getline 前先清空输入缓冲（cin.ignore(numeric_limits::max(), '\n')），或全程统一用 getline + 手
    动解析。

无法播放北京音乐
      - 现象：希望添加一个背景音乐，但无法实现
            - 解决方式：使用github开源的头文件MINIAUDIO，仓库代码：[mackron/miniaudio: Audio playback and capture library written in C, in a single source file.](https://github.com/mackron/miniaudio)，阅读文档实现自定义背景音乐的播放









