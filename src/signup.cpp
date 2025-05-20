#include "signup.h"

#include <spdlog/spdlog.h>

#include "database.h"
#include "session.h"

void signup::process_request(session& conn, nlohmann::json json_data) {
    spdlog::info("开始解析注册请求数据.");

    const auto username = json_data["username"];
    const auto password = json_data["password"];

    auto storage = database::storage();
    storage.insert(account{0, username, password});

    nlohmann::json reply_json;
    reply_json["request_type"] = "register";
    reply_json["result"] = "passed";

    auto raw_data = reply_json.dump() + session::packet_separator;
    conn.write(raw_data);
}