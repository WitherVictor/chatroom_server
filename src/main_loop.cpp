#include "main_loop.h"

//  项目内依赖
#include "session.h"

void main_loop::accept(tcp::acceptor& acceptor) {
    auto socket_ptr = std::make_shared<tcp::socket>(acceptor.get_executor());

    acceptor.async_accept(*socket_ptr, [socket_ptr, &acceptor](boost::system::error_code error_code) {
        if (!error_code) {
            auto new_session_ptr = std::make_shared<session>(std::move(*socket_ptr));
            new_session_ptr->read();
        } else {
            spdlog::error("acceptor 建立链接失败！错误码：{}，错误信息：{}", error_code.value(), error_code.message());
        }

        //  形成递归调用
        main_loop::accept(acceptor);
    });
}

void main_loop::start(port server_port) {
    using namespace boost::asio::ip;

    spdlog::debug("服务器监听目标端口：{}。", server_port);

    //  io 上下文
    boost::asio::io_context io_context{};

    //  监听 IPv4 的指定端口
    tcp::endpoint endpoint{tcp::v4(), server_port};
    tcp::acceptor acceptor{io_context, endpoint};

    main_loop::accept(acceptor);

    io_context.run();
}