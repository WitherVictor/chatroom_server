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

include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(spdlog)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${BOOST_INCLUDE_PATH}
    PRIVATE include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE spdlog
)

target_link_directories(${PROJECT_NAME}
    PRIVATE ${BOOST_LIBRARY_PATH}
)

set(UNIT_TEST ${PROJECT_NAME}_unittest)
set(GTEST_VERSION "v1.16.0")

FetchContent_Declare(
    gtest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG ${GTEST_VERSION}
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(gtest)

file(GLOB_RECURSE UNIT_TEST_SOURCES tests/*.cpp)

add_executable(${UNIT_TEST} ${UNIT_TEST_SOURCES})

target_include_directories(${UNIT_TEST}
    PRIVATE include
)

target_link_libraries(${UNIT_TEST}
    PRIVATE spdlog
    PRIVATE GTest::gtest
    PRIVATE GTest::gtest_main
)