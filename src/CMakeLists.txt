# --- 1. 新增：将下载的 ImGui 源文件编译为一个静态库 ---
file(GLOB IMGUI_SOURCES "${CMAKE_SOURCE_DIR}/Thirdparty/imgui/*.cpp")
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC "${CMAKE_SOURCE_DIR}/Thirdparty/imgui")

target_link_libraries(imgui PUBLIC glfw)

target_compile_definitions(imgui PRIVATE GLFW_INCLUDE_NONE)

target_link_libraries(imgui PUBLIC glfw)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
set(GLAD_SOURCE "${CMAKE_SOURCE_DIR}/Thirdparty/glad/src/glad.c")

# 定义你的主程序
add_executable(CG_ex1 ${SRC_FILES} ${GLAD_SOURCE})

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/BuildConfig.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/BuildConfig.h"
    @ONLY
)

# 配置头文件搜索路径
target_include_directories(CG_ex1 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/Thirdparty/glad/include
    ${CMAKE_SOURCE_DIR}/Thirdparty/imgui  
)

# 链接库
target_link_libraries(CG_ex1 PRIVATE glfw opengl32 imgui)

# 与 Demo2D/Demo3D 一致：禁止 glfw3.h 拉取系统 GL/gl.h（由 GLAD 提供 OpenGL 头）
target_compile_definitions(CG_ex1 PRIVATE GLFW_INCLUDE_NONE)
