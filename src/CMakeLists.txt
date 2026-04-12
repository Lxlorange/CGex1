file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
set(GLAD_SOURCE "${CMAKE_SOURCE_DIR}/Thirdparty/glad/src/glad.c")

add_executable(CG_ex1 ${SRC_FILES} ${GLAD_SOURCE})

target_include_directories(CG_ex1 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Thirdparty/glad/include
)

target_link_libraries(CG_ex1 PRIVATE glfw opengl32)

# MinGW: avoid loading an older libstdc++-6.dll from PATH (e.g. Anaconda/Dev-Cpp),
# which breaks C++17 std::filesystem entry points at runtime.
if(MINGW)
    target_link_options(CG_ex1 PRIVATE -static-libgcc -static-libstdc++)
endif()

target_compile_definitions(CG_ex1 PRIVATE PROJECT_SOURCE_DIR_PATH="${CMAKE_SOURCE_DIR}")
