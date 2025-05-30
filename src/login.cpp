#include "login.h"

#include "database.h"

#include <sqlite_orm/sqlite_orm.h>
#include <spdlog/spdlog.h>

using namespace boost::asio::ip;

void login::process_request(session& connection, nlohmann::json json_data) {
    using namespace sqlite_orm;
    using json = nlohmann::json;

    spdlog::debug("开始解析登录请求数据.");

    auto username = json_data.at("username").get<std::string>();
    auto password = json_data.at("password").get<std::string>();

    //  获取数据库对象并准备发回的 json
    auto storage = database::storage();
    
    json reply_json{};
    reply_json["request_type"] = "login";
    
    auto result_account_vec = storage.get_all<account>(
        where(c(&account::name) == username),
        limit(1)
    );

    if (!result_account_vec.empty()) {
        const auto& result_account = result_account_vec.front();
        if (result_account.password == password) {
            reply_json["result"] = "passed";
        } else {
            reply_json["result"] = "denied";
            reply_json["reason"] = "wrong password";
        }
    } else {
        spdlog::error("无法在数据库中查找到用户名: {},", username);
        reply_json["result"] = "denied";
        reply_json["reason"] = "用户名不存在, 请重新输入.";
    }

    //  将 json 对象转为字节流
    auto raw_data = reply_json.dump();

    spdlog::debug("即将发送数据: {}", raw_data);

    connection.write(raw_data);
}