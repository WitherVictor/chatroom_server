#pragma once

#include <cstddef>
#include <memory>
#include <string>

#include <boost/asio.hpp>

class session : public std::enable_shared_from_this<session> {
public:
    //  数据包分隔符
    static constexpr char packet_separator = '\x1E';

    explicit session(boost::asio::ip::tcp::socket);
    ~session() = default;

    //  一个 session 指代一个 socket 链接，因此禁止复制。
    session(const session&) = delete;
    session& operator=(const session&) = delete;

    //  streambuf 不可移动，因此 session 也不可移动
    session(session&&) noexcept = delete;
    session& operator=(session&&) noexcept = delete;

    void read();
    void write(std::string);

    void data_sent(const boost::system::error_code&, std::size_t);
    void data_received(const boost::system::error_code&, std::size_t);

    std::string split_raw_data();
    void dispatch_request(std::string);
private:
    boost::asio::streambuf m_buffer;        //  流式缓冲区
    boost::asio::ip::tcp::socket m_socket;  //  socket 链接
};