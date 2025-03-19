#pragma once

#include <cstddef>
#include <cstring>
#include <cctype>

#include <charconv>

#include <spdlog/spdlog.h>

using cstring = const char*;
using port = unsigned short;

inline bool valid_port(cstring port_str) {
    spdlog::debug("开始验证端口: {}", port_str);

    for (std::size_t index{}; index < std::strlen(port_str); index++) {
        if (!std::isdigit(port_str[index])) {
            spdlog::debug("端口 [{}] 不是有效端口！", port_str);
            return false;
        }
    }

    spdlog::debug("端口 [{}] 是合法端口。", port_str);
    return true;
}

inline port make_port(cstring port_str) {
    port result_port{};
    std::from_chars(port_str, port_str + std::strlen(port_str), result_port);
    return result_port;
}