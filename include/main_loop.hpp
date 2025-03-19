#pragma once

//  第三方依赖
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>

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
        //  接收即将到来的 socket 链接
        tcp::socket socket{io_context};
        acceptor.accept(socket);

        
    }
}