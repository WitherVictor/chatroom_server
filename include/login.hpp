#pragma once

#include <list>

#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <sqlite_orm/sqlite_orm.h>

#include "session.hpp"
#include "database.hpp"

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
        using namespace sqlite_orm;

        spdlog::info("开始解析登录请求数据。");
        try {
            auto request_json = json::parse(raw_json_data);

            auto request_type = request_json.at("request_type").get<std::string>();
            if (request_type == "login") {
                auto username = request_json.at("username").get<std::string>();
                auto password = request_json.at("password").get<std::string>();

                json reply_json{};
                reply_json["request_type"] = "login";

                auto storage = database::storage();

                auto account_ptr = storage.get_pointer<account>(where(c(&account::name) == username));
                if (account_ptr) {
                    if (account_ptr->password == password) {
                        reply_json["result"] = "passed";
                    } else {
                        reply_json["result"] = "denied";
                        reply_json["reason"] = "wrong password";
                    }
                } else {
                    spdlog::error("无法在数据库中查找到用户名!");
                    reply_json["result"] = "denied";
                    reply_json["reason"] = "No username";
                }

                auto raw_data = reply_json.dump() + session::packet_separator;
                spdlog::info("即将发送数据：{}", raw_data);
                connection->write(raw_data);
            } else {
                spdlog::error("登录模块接收的请求类型不匹配，实际请求类型：{}", request_type);
            }
        } catch (std::exception& ex) {
            spdlog::error("处理登录请求错误！原因：{}", ex.what());
        }
    }

    void acquire_socket_ptr(std::shared_ptr<tcp::socket> socket_ptr) {
        spdlog::info("将 socket 连接移交至 login 模块。");
        auto data_received_callback = [this](std::shared_ptr<session> connection, const std::string& data) {
            this->process_login_request(connection, data);
        };
        auto new_session = std::make_shared<session>(std::move(*socket_ptr), data_received_callback);
        m_socket_list.emplace_back(new_session);
        m_socket_list.back()->read();
    }
private:
    login() = default;

    //  等待连接建立之后 socket 被转移到这里
    mutable std::mutex m_list_mutex;
    std::list<std::shared_ptr<session>> m_socket_list;
};