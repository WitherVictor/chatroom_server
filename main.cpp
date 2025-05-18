//  C++ 标准库
#include <cstring>
#include <iostream>
#include <stacktrace>

//  spdlog
#include <spdlog/spdlog.h>

//  项目内头文件
#include "main_loop.h"
#include "utility.h"

void show_usage() {
    std::cout <<
    "可选参数：\n" <<
    "    --port 端口号    在指定的端口上开放服务器"
    << std::endl;
}

int main(int argc, char** argv) {
    //  设置日志等级为 trace，开发调试用
    spdlog::set_level(spdlog::level::trace);

    //  如果未提供任何参数，则使用默认参数
    if (argc == 1) {
        constexpr port default_port = 4242;
        main_loop::start(default_port);
    } else if (argc == 2) {
        //  若提供两个参数，则未提供端口号
        //  向用户提示用法
        show_usage();
        return -1;
    } else if (argc == 3) {
        //  若提供参数但未提供端口号
        if (std::strcmp(argv[1], "--port") != 0) {
            std::cout << "参数错误，请检查输入。" << std::endl;
            show_usage();
        }

        //  如果端口号的格式是错误的
        if (!utility::valid_port(argv[2])) {
            //  发出错误并提示正确用法
            std::cout << "错误：端口 [" << argv[2] << "] 不是合法端口！"
                    << std::endl;
            show_usage();
            return -1;
        }

        //  正确则使用提供的端口
        main_loop::start(utility::make_port(argv[2]));
    } else {
        std::cout << "参数错误，请检查输入。" << std::endl;
        show_usage();
        return -1;
    }
}