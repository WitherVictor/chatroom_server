#pragma once

#include <atomic>
#include <vector>
#include <memory>
#include <mutex>

#include <boost/uuid/uuid.hpp>

#include "session.h"

class chatroom {
public:
    explicit chatroom();

    chatroom(const chatroom&) = delete;
    chatroom& operator=(const chatroom&) = delete;

    chatroom(chatroom&&) noexcept;
    chatroom& operator=(chatroom&&) noexcept;

    void reply_create_request(session&);

    static void create(session&);
    static std::vector<std::unique_ptr<chatroom>> rooms;
    static std::mutex m_rooms_mutex;
private:
    std::atomic<std::size_t> m_user_count;
    boost::uuids::uuid m_id;
};