#pragma once

#include <list>

#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "session.hpp"

using json = nlohmann::json;
using namespace boost::asio::ip;

class login {
public:
    login(const login&) = delete;
    login& operator=(const login&) = delete;

    static login& instance() {
        static login instance;
        return instance;
    }

    void process_login_request(std::shared_ptr<session> connection, const std::string& raw_json_data) {
        try {
            auto request_json = json::parse(raw_json_data);

            auto request_type = request_json.at("request_type").get<std::string>();
            if (request_type == "login") {
                auto username = request_json.at("username").get<std::string>();
                auto password = request_json.at("password").get<std::string>();

                if (username == "123" && password == "456") {
                    spdlog::info("用户验证通过！");
                    json reply_json{};
                    reply_json["request_type"] = "login";
                    reply_json["result"] = "passed";

                    auto raw_data = reply_json.dump() + session::packet_separator;
                    connection->write(raw_data);
                }
            } else {
                spdlog::error("登录模块接收的请求类型不匹配，实际请求类型：{}", request_type);
            }
        } catch (std::exception& ex) {
            spdlog::error("处理登录请求错误！原因：{}", ex.what());
        }
    }

    void acquire_socket(tcp::socket socket) {
        auto data_received_callback = [this](std::shared_ptr<session> connection, const std::string& data) {
            this->process_login_request(connection, data);
        };
        auto new_session = std::make_shared<session>(std::move(socket), data_received_callback);
        m_socket_list.emplace_back(new_session);
        m_socket_list.back()->read();
    }
private:
    login() = default;

    //  等待连接建立之后 socket 被转移到这里
    mutable std::mutex m_list_mutex;
    std::list<std::shared_ptr<session>> m_socket_list;
};