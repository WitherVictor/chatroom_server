#pragma once

//  第三方依赖
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>

//  项目内头文件
#include "login.hpp"

using port = unsigned short;

inline void main_loop(port server_port) {
    using namespace boost::asio::ip;

    spdlog::info("服务器监听目标端口：{}。", server_port);

    //  io 上下文
    boost::asio::io_context io_context{};

    //  监听 IPv4 的指定端口
    tcp::endpoint endpoint{tcp::v4(), server_port};
    tcp::acceptor acceptor{io_context, endpoint};

    while (true) {
        //  接收即将到来的 socket 连接
        tcp::socket socket{io_context};
        acceptor.accept(socket);

        //  连接建立后，将 socket 移动到 login 模块处理登录
        auto& login_handler = login::instance();
        login_handler.acquire_socket(std::move(socket));
    }
}