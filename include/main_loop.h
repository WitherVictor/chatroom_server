#pragma once

//  第三方依赖
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <spdlog/spdlog.h>

using port = unsigned short;

namespace main_loop {
    using namespace boost::asio::ip;

    void accept(tcp::acceptor&);
    void start(port);
}