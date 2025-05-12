#include <sqlite_orm/sqlite_orm.h>

struct account {
    std::size_t id;
    std::string name;
    std::string password;
};

class database {
public:
    //  单例对象禁止复制
    database(const database&) = delete;
    database& operator=(const database&) = delete;

    //  获取单例对象
    static auto& storage() {
        //  创建数据库
        static auto storage = [](){
            auto storage = sqlite_orm::make_storage("database.db",
                sqlite_orm::make_table("account",
                    sqlite_orm::make_column("id", &account::id, sqlite_orm::primary_key().autoincrement()),
                    sqlite_orm::make_column("name", &account::name),
                    sqlite_orm::make_column("password", &account::password)
                )
            );

            //  与存储文件同步
            storage.sync_schema();

            return storage;
        }();
        
        return storage;
    }
private:
    //  默认构造函数与默认析构函数私有
    database() = default;
    ~database() = default;
};