#include "session.h"

#include <string>
#include <utility>

//  第三方库
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

//  项目内
#include "chatroom.h"
#include "login.h"
#include "signup.h"

using namespace boost;
using namespace boost::asio::ip;

session::session(tcp::socket socket)
    : m_socket{std::move(socket)} {}

void session::read() {
    //  打印链接日志
    auto remote_endpoint = m_socket.remote_endpoint();
    spdlog::debug("等待 {}:{} 传输数据。",
                    remote_endpoint.address().to_string(),
                    remote_endpoint.port()
    );

    //  异步等待数据输入
    asio::async_read_until(m_socket, m_buffer, packet_separator,
                                [session_shared_ptr = shared_from_this()](const boost::system::error_code& error_code, std::size_t bytes) {
                                            session_shared_ptr->data_received(error_code, bytes);
                                        }
    );
}

void session::write(std::string data) {
    data.push_back(session::packet_separator);

    asio::async_write(m_socket, asio::buffer(data),
        [session_shared_ptr = shared_from_this()] (const boost::system::error_code& error_code, std::size_t bytes_sent) {
            session_shared_ptr->data_sent(error_code, bytes_sent);
    });
}

void session::data_sent(const boost::system::error_code& error_code, std::size_t bytes_transferred) {
    if (error_code == boost::asio::error::eof) {
        auto remote_endpoint = m_socket.remote_endpoint();
        spdlog::error("客户端 {}:{} 已结束与服务器的链接，通信终止。", remote_endpoint.address().to_string(), remote_endpoint.port());
    } else if (error_code) {
        spdlog::error("消息发送出现错误！错误码：{}，错误消息：{}", error_code.value(), error_code.message());
        return;
    }

    spdlog::debug("消息已发送，传输字节数：{}", bytes_transferred);

    //  链式调用
    read();
}

void session::data_received(const boost::system::error_code& error_code, std::size_t bytes_transferred) {
    spdlog::debug("接收到客户端消息，数据长度：{}", bytes_transferred);

    //  通讯结束
    if (error_code == boost::asio::error::eof) {
        auto remote_endpoint = m_socket.remote_endpoint();
        spdlog::warn("客户端 {}:{} 已结束与服务器的链接，通信终止。", remote_endpoint.address().to_string(), remote_endpoint.port());
        return;
    }

    //  其他错误
    if (error_code) {
        spdlog::error("消息发送出现错误！错误码：{}，错误消息：{}", error_code.value(), error_code.message());
        return;
    }

    dispatch_request(split_raw_data());

    spdlog::debug("请求处理完成.");
}

std::string session::split_raw_data() {
    //  将读入的数据存储到 std::string 中等待处理
    std::istream stream(&m_buffer);

    //  存储分割后的数据
    std::string raw_json_data{};

    std::getline(stream, raw_json_data, packet_separator);
    m_buffer.consume(raw_json_data.size());

    return raw_json_data;
}

void session::dispatch_request(std::string raw_data) {
    spdlog::debug("开始分发请求: {}", raw_data);
    auto json_data = nlohmann::json::parse(raw_data);
    const auto& request_type = json_data.at("request_type").get<std::string>();

    if (request_type == "login") {
        login::process_request(*this, std::move(json_data));
    } else if (request_type == "register") {
        signup::process_request(*this, std::move(json_data));
    } else if (request_type == "create_chatroom" ) {
        chatroom::create(*this);
    } else if (request_type == "message") {
        chatroom::receive_message(*this, std::move(json_data));
    } else {
        spdlog::error("未知请求类型: {}", request_type);
    }
}