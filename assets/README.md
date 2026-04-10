# assets 目录

这个目录放“可替换的视觉资源”。

推荐结构：

- textures/
  - noise_blue_512.png（蓝噪声）
  - grid_hex_1024.png（六边形网格）
  - ui_hud_mask.png（HUD 遮罩）
- palettes/
  - cyber_teal_orange.json（主配色）
  - graphite_neon.json（备选配色）
- references/
  - style_board.png（风格参考图）
  - demo_capture_before_after.png（优化前后对比）

方向建议：

1. 主色：深蓝灰背景 + 青蓝霓虹高光 + 少量洋红点缀。
2. 纹理：轻微扫描线、细网格、低强度噪声，不要喧宾夺主。
3. 动效：慢速脉冲和呼吸光，避免频闪。

当前项目状态：

- 2D/3D shader 已支持时间驱动的轻量动态效果。
- 后续若加纹理采样，可先放在 textures/ 下并在 shader 增加 sampler2D。
