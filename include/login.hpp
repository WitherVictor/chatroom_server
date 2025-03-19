#pragma once

#include <list>

#include <boost/asio.hpp>

#include "session.hpp"

using namespace boost::asio::ip;

class login {
public:
    login(const login&) = delete;
    login& operator=(const login&) = delete;

    static login& instance() {
        static login instance;
        return instance;
    }

    void acquire_socket(tcp::socket socket) {
        m_socket_list.emplace_back(session{std::move(socket)});
        m_socket_list.back().read();
    }
private:
    login() = default;

    //  等待连接建立之后 socket 被转移到这里
    mutable std::mutex m_list_mutex;
    std::list<session> m_socket_list;
};