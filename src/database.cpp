#include "database.h"

#include <sqlite_orm/sqlite_orm.h>

database::storage_type database::storage() {
    static auto new_storage = []() {
        auto storage = sqlite_orm::make_storage(database::file_path,
                            sqlite_orm::make_table("account",
                                sqlite_orm::make_column("id", &account::id,
                                    sqlite_orm::primary_key().autoincrement()),
                                sqlite_orm::make_column("name", &account::name),
                                sqlite_orm::make_column("password", &account::password)
                            )
                        );

        storage.sync_schema();
        return storage;
    }();
    
    return new_storage;
}