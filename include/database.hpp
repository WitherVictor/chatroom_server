#include <sqlite_orm/sqlite_orm.h>

#include <string>

//  account 表结构
struct account {
    std::size_t id;
    std::string name;
    std::string password;
};

namespace database {
    static constexpr auto file_path = "account.db";

    inline auto storage() {
        auto new_storage = sqlite_orm::make_storage(database::file_path,
                                sqlite_orm::make_table("account",
                                    sqlite_orm::make_column("id", &account::id, sqlite_orm::primary_key().autoincrement()),
                                    sqlite_orm::make_column("name", &account::name),
                                    sqlite_orm::make_column("password", &account::password)
                                )
                            );

        new_storage.sync_schema();
        return new_storage;
    }
};  //  end of namespace database