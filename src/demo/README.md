# src/demo 目录

将“演示层逻辑”和“可调参数”放这里，不污染 core/math/scene。

适合放置：

1. DemoPreset.h / DemoPreset.cpp
- 预设变换轨迹（课堂演示一键播放）
- 不同展示方案（基础版、答辩版、炫酷版）

2. KeymapCard.h
- 按键映射说明文本（可用于窗口标题/HUD）

3. TuningProfile.h
- 统一管理移动速度、旋转速度、缩放速率
- 便于调参，不必改散落在多个 scene 文件里的常量

4. ScreenshotScript.h
- 定义自动截图清单与命名规范
- 给实验报告直接喂图

当前用 README 占位。
