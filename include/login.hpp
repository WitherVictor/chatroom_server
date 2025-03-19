#pragma once

#include <boost/asio.hpp>

using namespace boost::asio::ip;

class login {
public:
    login(const login&) = delete;
    login& operator=(const login&) = delete;
private:
    tcp::socket m_socket;
};