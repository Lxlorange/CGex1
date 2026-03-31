# CG实验一

**@Environment**: Windows / CLion / CMake / MinGW / Legacy OpenGL

## 项目简介

本项目是《计算机图形学》第一次实验的代码实现，使用现代 OpenGL (Core Profile) 编写。

`MyMath.h` 简单实现了矩阵乘法操作

## 编译与运行

本项目尝试使用 CMake 的 `FetchContent` 来便捷配置。 Windows用户可直接按照下述步骤执行，其它OS的用户请自行下载配置项目需要的第三方依赖。

1.  克隆或下载本代码仓库。
2.  使用 IDE （推荐Clion）打开项目根目录。
3.  等待 CMake 自动从 GitHub 拉取 `GLFW` 源码（首次配置需保持网络畅通）。
4.  CMake 刷新完成后，直接运行 Target（CG_ex1）。
5.  *(注：本项目已配置链接 Windows 原生 `opengl32` 图形接口库，无需额外下载配置 GLAD)*。

## 第三方依赖

本项目需要依赖以下第三方图形库。

### 1. GLFW (窗口与输入管理)
* **作用**：负责与操作系统交互，创建 OpenGL 上下文窗口，并处理键盘/鼠标输入。
* **下载地址**：[GLFW 官网下载页](https://www.glfw.org/download.html)
* **版本选择**：**Pre-compiled binaries** (预编译版本)。
  * *注意：本项目的 CMake 默认配置为 `lib-mingw-w64`。如果你使用的是 Visual Studio，请修改 `CMakeLists.txt` 中的库路径为对应的 `lib-vc202X`。*

### 2. GLAD (OpenGL 函数指针加载器)
* **作用**：在运行时获取显卡驱动中 OpenGL 具体实现的函数地址。
* **下载地址**：前往 [GLAD 在线生成器](https://glad.dav1d.de/)，通过下列配置生成压缩包并下载：
  * **Language**: C/C++
  * **Specification**: OpenGL
  * **API gl**: Version 3.3
  * **Profile**: Core
