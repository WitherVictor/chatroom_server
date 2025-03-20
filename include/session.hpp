#pragma once

#include <cstddef>
#include <functional>
#include <memory>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using namespace boost;
using namespace boost::asio::ip;
using namespace boost::placeholders;

class session : public std::enable_shared_from_this<session> {
public:
    explicit session(tcp::socket socket, std::function<void(std::shared_ptr<session>, const std::string&)> callback)
        : m_socket{std::move(socket)}, m_callback{std::move(callback)} {}

    session(const session&) = delete;
    session& operator=(const session&) = delete;

    void read() {
        asio::async_read_until(m_socket,
                               m_buffer,
                               packet_separator,
                               boost::bind(&session::data_received,
                                            this,
                                            asio::placeholders::error,
                                            asio::placeholders::bytes_transferred));
    }

    void write(std::string data) {
        m_socket.async_send(asio::buffer(data),
                            boost::bind(&session::data_sent, 
                                        this,
                                        asio::placeholders::error,
                                        asio::placeholders::bytes_transferred));
    }

    void data_sent(const boost::system::error_code& ec, std::size_t bytes_transferred) {
        if (ec) {
            spdlog::error("消息发送出现错误！错误码：{}，错误消息：{}", ec.value(), ec.message());
            return;
        }

        spdlog::info("消息已发送，传输字节数：{}", bytes_transferred);
    }

    void data_received(const boost::system::error_code& ec, std::size_t bytes_transferred) {
        //  处理错误
        if (ec) {
            spdlog::error("消息传输出现错误！错误码：{}，错误消息：{}", ec.value(), ec.message());
            return;
        }

        //  将读入的数据存储到 std::string 中等待处理
        std::istream stream(&m_buffer);
        std::string raw_json_data{};
        std::getline(stream, raw_json_data, packet_separator);
        m_buffer.consume(bytes_transferred);

        //  调用回调函数处理数据
        m_callback(shared_from_this(), raw_json_data);

        read();
    }

    inline static constexpr char packet_separator = '\x1E';
private:
    boost::asio::streambuf m_buffer;
    tcp::socket m_socket;

    //  接收到数据后调用回调函数处理数据
    std::function<void(std::shared_ptr<session>, const std::string&)> m_callback;
};