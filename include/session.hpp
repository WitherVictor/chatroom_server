#pragma once

#include "boost/asio/placeholders.hpp"
#include "boost/system/detail/error_code.hpp"
#include <cstddef>
#include <array>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

using namespace boost;
using namespace boost::asio::ip;

class session {
public:
    explicit session(tcp::socket socket)
        : m_socket{std::move(socket)} {}

    void read() {
        m_socket.async_receive(asio::buffer(m_buffer),
                               boost::bind(&session::data_received,
                                                    this,
                                                    asio::placeholders::error,
                                                    asio::placeholders::bytes_transferred));
    }

    void data_received(const boost::system::error_code& ec, std::size_t bytes_trasferred) {


        read();
    }

    inline static constexpr std::size_t buffer_size = 4096;
private:
    std::array<std::byte, session::buffer_size> m_buffer;
    tcp::socket m_socket;
};