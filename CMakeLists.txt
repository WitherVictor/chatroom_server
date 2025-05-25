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
set(BOOST_INCLUDE_PATH "D:/Development/boost_1_88_0")
set(BOOST_LIBRARY_PATH ${BOOST_INCLUDE_PATH}/stage/lib)

# 设置 URL 下载为新标准
cmake_policy(SET CMP0135 NEW)

# 下载依赖 sqlite3 sqlite_orm spdlog
include(FetchContent)
FetchContent_Declare(
    sqlite3
    URL https://www.sqlite.org/2025/sqlite-amalgamation-3490100.zip
    URL_HASH SHA3_256=e7eb4cfb2d95626e782cfa748f534c74482f2c3c93f13ee828b9187ce05b2da7
)
FetchContent_Declare(
    sqlite_orm
    GIT_REPOSITORY https://github.com/fnc12/sqlite_orm.git
    GIT_TAG v1.9.1
    GIT_SHALLOW TRUE
)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(sqlite3 spdlog nlohmann_json)

# 将 sqlite3 编译成库
add_library(sqlite3 STATIC ${sqlite3_SOURCE_DIR}/sqlite3.c)
target_include_directories(sqlite3 PUBLIC ${sqlite3_SOURCE_DIR})

# 因为使用下载的 sqlite3，需要覆盖 SQLite3 的查找结果
# 随后安装依赖 sqlite_orm 才可以通过
set(SQLite3_INCLUDE_DIR ${sqlite3_SOURCE_DIR})
set(SQLite3_LIBRARY sqlite3)
set(SQLite3_FOUND TRUE)
FetchContent_MakeAvailable(sqlite_orm)

# 将 main.cpp 添加到目标 chatroom_server
add_executable(${PROJECT_NAME} main.cpp)

# chatroom_server 需要的头文件
target_include_directories(${PROJECT_NAME}
    PRIVATE
        include
        ${BOOST_INCLUDE_PATH}
        ${nlohmann_json_SOURCE_DIR}/single_include
        ${sqlite_orm_SOURCE_DIR}/include
)

# chatroom_server 需要的源文件
target_sources(${PROJECT_NAME}
    PRIVATE
        src/chatroom.cpp
        src/database.cpp
        src/login.cpp
        src/main_loop.cpp
        src/session.cpp
        src/signup.cpp
        src/utility.cpp
)

# chatroom_server 所需的库
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        spdlog
        sqlite3
        nlohmann_json
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # 如果平台是 Windows，则 boost.asio 需要的 impl 库 ws2_32
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            ws2_32
            mswsock
    )
    if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
        target_link_libraries(${PROJECT_NAME} PRIVATE stdc++exp)
    endif ()
endif ()

# 因为 Boost 需要本地环境存在，因此单独链接目录下所有库
target_link_directories(${PROJECT_NAME}
    PRIVATE ${BOOST_LIBRARY_PATH}
)

# 单元测试目标
set(UNIT_TEST ${PROJECT_NAME}_unittest)
set(GTEST_VERSION "v1.16.0")

# 下载并安装 googletest
FetchContent_Declare(
    gtest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG ${GTEST_VERSION}
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(gtest)

# 查找单元测试下的所有文件
file(GLOB_RECURSE UNIT_TEST_SOURCES tests/*.cpp)

# 将单元测试添加到 unit_test 目标
add_executable(${UNIT_TEST} ${UNIT_TEST_SOURCES})

# 添加单元测试需要测试的函数源码
target_sources(${UNIT_TEST}
    PRIVATE
        src/utility.cpp
)

# unit_test 需要的头文件
target_include_directories(${UNIT_TEST}
    PRIVATE
        include
        ${nlohmann_json_SOURCE_DIR}/single_include
)

# 链接日志库 spdlog 和单元测试库 googletest
target_link_libraries(${UNIT_TEST}
    PRIVATE
        spdlog
        nlohmann_json
        GTest::gtest
        GTest::gtest_main
)