#include "chatroom.h"

#include <algorithm>
#include <chrono>
#include <memory>
#include <mutex>
#include <vector>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>

#include "boost/uuid/uuid.hpp"
#include "session.h"

std::vector<std::unique_ptr<chatroom>> chatroom::rooms = {};
std::mutex chatroom::m_rooms_mutex = {};

chatroom::chatroom()
    : m_id(boost::uuids::random_generator{}()) {}

void chatroom::create(session& connection) {
    auto new_chatroom_ptr = std::make_unique<chatroom>();
    new_chatroom_ptr->m_conns.push_back(&connection);
    new_chatroom_ptr->reply_create_request(connection);

    std::unique_lock lock{chatroom::m_rooms_mutex};
    rooms.push_back(std::move(new_chatroom_ptr));
}

void chatroom::reply_create_request(session& connection) {
    using json = nlohmann::json;

    //  构造回应请求的 json
    json reply_json;
    reply_json["request_type"] = "create_chatroom";
    reply_json["uuid"] = boost::uuids::to_string(m_id);

    auto raw_data = reply_json.dump();

    spdlog::debug("准备回应请求, 内容: {}.", raw_data);
    
    connection.write(raw_data);
}

void chatroom::receive_message(session& conn, nlohmann::json json_data) {
    spdlog::debug("开始处理接收的聊天消息.");

    //  检查请求类型是否正确
    const auto& request_type = json_data.at("request_type").get<std::string>();
    if (request_type != "message") {
        spdlog::error("请求类型错误! 实际类型: {}.", request_type);
        return;
    }

    const auto& target_id = json_data.at("room_id").get<std::string>();
    auto find_result = std::ranges::find_if(rooms, [&target_id](const auto& room_ptr) {
        return room_ptr->m_id == boost::uuids::string_generator{}(target_id);
    });

    if (find_result != rooms.end()) {
        (*find_result)->broadcast_message(std::move(json_data));
    } else {
        spdlog::error("聊天请求的 room_id 未找到! 实际 ID: {}.", target_id);
    }
}

void chatroom::broadcast_message(nlohmann::json json_data) {
    const auto& username = json_data.at("username").get<std::string>();
    const auto& message = json_data.at("message").get<std::string>();

    nlohmann::json reply_json;
    reply_json["request_type"] = "message";
    reply_json["username"] = username;
    reply_json["message"] = message;
    reply_json["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();

    const auto& raw_data = reply_json.dump();
    spdlog::debug("准备广播聊天室消息请求: {}", raw_data);

    for (const auto& conn_ptr : m_conns) {
        conn_ptr->write(raw_data);
    }
}

void chatroom::join_chatroom(session& conn, nlohmann::json json_data) {
    const auto& target_id_string = json_data.at("uuid").get<std::string>();
    const auto target_uuid = boost::uuids::string_generator{}(target_id_string);

    auto find_result = std::ranges::find_if(rooms, [&target_uuid](const auto& room_ptr) {
        return room_ptr->m_id == target_uuid;
    });

    //  回应请求的 json
    nlohmann::json reply_json;
    reply_json["request_type"] = "join_chatroom";

    if (find_result != rooms.end()) {
        auto& chatroom = *(*find_result);

        std::unique_lock lock{chatroom.m_conns_mutex};
        chatroom.m_conns.push_back(&conn);
        lock.unlock();

        reply_json["result"] = "success";
        reply_json["uuid"] = boost::uuids::to_string(chatroom.m_id);

        chatroom.update_usercount();
    } else {
        reply_json["result"] = "failed";
        reply_json["reason"] = "未找到指定 ID 的聊天室!";
    }

    auto raw_data = reply_json.dump();
    spdlog::debug("回应加入聊天室请求, 回应内容: {}.", raw_data);
    conn.write(raw_data);
}

void chatroom::update_usercount() {
    using nlohmann::json;

    spdlog::debug("准备广播用户人数更新请求.");

    auto user_count = m_conns.size();

    json reply_json;
    reply_json["request_type"] = "update_usercount";
    reply_json["user_count"] = user_count;

    auto raw_data = reply_json.dump();

    spdlog::debug("请求内容: {}.", raw_data);

    for (const auto& session_ptr : m_conns) {
        session_ptr->write(raw_data);
    }
}