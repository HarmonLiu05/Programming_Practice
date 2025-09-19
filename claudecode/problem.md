解决以下问题：

 - 存档网格尺寸不一致：Map 是 3×8，但 SaveGame 用的是 3×7（数组与读写循环），会丢失最右一列的访问/清理
    状态并越界风险。

BOSS 条件不一致：BOSS 进入阈值用 12（Game 中），胜利用 15（Map::isGameWon()），AsciiMap 的状态展示
  又用 12/12。建议统一一处配置，如都用 12

技能/道具“效果系统”未接入伤害计算：SkillEffects::applyAttackBuff/applySharedDamage 与
  ItemEffects::applyAdrenalineBuff/applyAdrenalineDebuff 未在 Battle 的普攻/受击路径中调用，导致描述与
  实际未对齐。

源码重复：根目录与 头文件/ 下有重复同名文件，容易导致编译/引用混乱，建议移除或统一单份来源。