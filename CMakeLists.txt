cmake_minimum_required(VERSION 3.18)
project(chatroom_server VERSION 0.1.0 LANGUAGES C CXX)

# 导出编译指令供 clangd 使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置 C++ 标准为 23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 导出编译指令
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置 Boost 头文件与库路径
set(BOOST_INCLUDE_PATH "D:/libraries/boost_1_87_0")
set(BOOST_LIBRARY_PATH ${BOOST_INCLUDE_PATH}/stage/lib)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${BOOST_INCLUDE_PATH}
)

target_link_directories(${PROJECT_NAME}
    PRIVATE ${BOOST_LIBRARY_PATH}
)