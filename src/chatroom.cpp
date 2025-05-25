#include "chatroom.h"

#include <memory>
#include <mutex>
#include <vector>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>

#include "session.h"

std::vector<std::unique_ptr<chatroom>> chatroom::rooms = {};
std::mutex chatroom::m_rooms_mutex = {};

chatroom::chatroom()
    : m_id(boost::uuids::random_generator{}()) {}

chatroom::chatroom(chatroom&& other) noexcept
    : m_user_count{other.m_user_count.load()}, m_id{other.m_id} {}

void chatroom::create(session& connection) {
    auto new_chatroom_ptr = std::make_unique<chatroom>();
    new_chatroom_ptr->m_user_count++;
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