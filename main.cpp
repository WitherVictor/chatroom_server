#include <iostream>

#include <spdlog/spdlog.h>

#include "utility.hpp"

void show_usage() {
    std::cout <<
    "可选参数：\n" <<
    "    --port 端口号    在指定的端口上开放服务器"
    << std::endl;
}

int main(int argc, char** argv) {
    if (argc - 1 < 2) {
        std::cout << "参数错误，请检查输入。" << std::endl;
        show_usage();
        return -1;
    }

    if (std::strcmp(argv[1], "--port") == 0) {
        if (!valid_port(argv[2])) {
            std::cout << "参数错误，请检查输入。\n"
                      << "    错误：端口 [" << argv[2] << "] 不是合法端口！"
                      << std::endl;
            return -1;
        }
        

    }
}