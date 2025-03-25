#pragma once

//  第三方依赖
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>

//  项目内头文件
#include "boost/system/detail/error_code.hpp"
#include "login.hpp"

using port = unsigned short;

inline void start_accept(tcp::acceptor& acceptor) {
    auto socket_ptr = std::make_shared<tcp::socket>(acceptor.get_executor());

    acceptor.async_accept(*socket_ptr, [socket_ptr, &acceptor](boost::system::error_code ec) {
        if (!ec) {
            login::instance().acquire_socket_ptr(socket_ptr);
        } else {
            spdlog::error("acceptor 建立链接失败！错误码：{}，错误信息：{}", ec.value(), ec.message());
        }

        //  形成递归调用
        start_accept(acceptor);
    });
}

inline void main_loop(port server_port) {
    using namespace boost::asio::ip;

    spdlog::info("服务器监听目标端口：{}。", server_port);

    //  io 上下文
    boost::asio::io_context io_context{};

    //  监听 IPv4 的指定端口
    tcp::endpoint endpoint{tcp::v4(), server_port};
    tcp::acceptor acceptor{io_context, endpoint};

    start_accept(acceptor);

    io_context.run();
}