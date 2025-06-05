#pragma once

#include <vector>
#include <memory>
#include <mutex>

#include <boost/uuid/uuid.hpp>
#include <nlohmann/json.hpp>

#include "session.h"

class chatroom {
public:
    explicit chatroom();

    chatroom(const chatroom&) = delete;
    chatroom& operator=(const chatroom&) = delete;

    chatroom(chatroom&&) noexcept = delete;
    chatroom& operator=(chatroom&&) noexcept = delete;

    void reply_create_request(session&);
    void broadcast_message(nlohmann::json);
    void update_usercount();

    static void create(session&);
    static void receive_message(session&, nlohmann::json);
    static void join_chatroom(session&, nlohmann::json);

    static std::vector<std::unique_ptr<chatroom>> rooms;
    static std::mutex m_rooms_mutex;
private:
    boost::uuids::uuid m_id;
    std::vector<session*> m_conns;
    mutable std::mutex m_conns_mutex;
};