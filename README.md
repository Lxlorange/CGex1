# CG 实验一（二维/三维变换、相机与投影）

## 实验目标覆盖

项目基于现代 OpenGL（3.3 Core）实现：

1. 二维图形绘制
2. 二维变换（缩放、反射、切变、旋转、平移）
3. 二维组合变换
4. 三维图形绘制
5. 三维变换（缩放、平移、绕 x/y/z 轴旋转）
6. 三维组合变换
7. 键盘交互、相机移动、透视/正交投影切换

## 工程结构

```text
CGex1/
  CMakeLists.txt
  src/
    CMakeLists.txt
    app/       # App 框架与程序入口
    core/      # Shader 封装
    input/     # GLFW 回调封装
    math/      # 列主序 MyMath
    scene/     # Scene / Demo2D / Demo3D
  shaders/     # 2D/3D GLSL
  Thirdparty/glad/
  docs/
```

## 构建环境

- 操作系统：Windows
- CMake：>= 3.20
- 编译器：MSVC 或 MinGW（CMake 自动适配生成器）
- 依赖：
  - GLFW（通过 `FetchContent` 自动拉取）
  - GLAD（仓库内 `Thirdparty/glad`）
  - `opengl32`

## 构建与运行

在项目根目录执行：

```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

命令行帮助与自动验证：

```powershell
./CG_ex1.exe --help
./CG_ex1.exe --validate
```

`--validate` 会自动执行清单 1-7 的关键操作并把截图写入 `docs/screenshots/`。

## 键位说明

全局（App）：

- `1`：切换到 Demo2D
- `2`：切换到 Demo3D
- `F`：线框模式开关
- `Esc`：退出程序

Demo2D：

- `W/A/S/D`：平移
- `Q/E`：旋转
- `Z/X`：等比缩放
- `H/J`：X 方向切变增减
- `N/M`：Y 方向切变增减
- `Y`：X 轴反射开关
- `U`：Y 轴反射开关
- `T`：组合顺序切换（`T*R*...` 与 `R*T*...`）
- `R`：重置变换

Demo3D：

- `W/A/S/D`：相机前后左右移动
- `Q/E`：相机上下移动
- `↑/↓/←/→`：模型绕 X/Y 旋转
- `Z/X`：模型绕 Z 旋转
- `+/-`：模型缩放
- `I/J/K/L/O/U`：模型平移
- `P`：透视/正交投影切换
- `T`：组合顺序切换（`T*R*S` 与 `R*T*S`）
- `R`：重置模型与相机

## 常见问题

- 只能按 `1/2` 切场景：
  - 先点击渲染窗口，让窗口获得焦点。
  - 切到英文输入法再试（部分中文输入法会拦截字母键输入）。