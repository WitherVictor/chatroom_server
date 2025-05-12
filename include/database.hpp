#include <sqlite_orm/sqlite_orm.h>

#include <string>
#include <mutex>

//  account 表结构
struct account {
    std::size_t id;
    std::string name;
    std::string password;
};

class database {
public:
    static constexpr auto file_path = "account.db";

    //  单例对象禁止复制
    database(const database&) = delete;
    database& operator=(const database&) = delete;

    //  亦不考虑移动
    database(database&&) = delete;
    database& operator=(database&&) = delete;

    //  Meyers' Singleton
    static database& instance() {
        static database instance;
        return instance;
    }

private:
    //  默认构造函数与默认析构函数私有
    database() = default;
    ~database() = default;

    static auto& storage() {
        static auto storage = []() {
            auto storage_instance = sqlite_orm::make_storage(database::file_path,
                                        sqlite_orm::make_table("account",
                                            sqlite_orm::make_column("id", &account::id, sqlite_orm::primary_key().autoincrement()),
                                            sqlite_orm::make_column("name", &account::name),
                                            sqlite_orm::make_column("password", &account::password)
                                        )
                                    );

            //  与存储文件同步
            storage_instance.sync_schema();
            return storage_instance;
        }();

        return storage;
    }

    static std::mutex m_storage_mutex;
};