#include "Database.h"

#include "Data/Model/User.h"

#include <QxOrm.h>

namespace data
{

Database::Database()
{
}

void Database::open()
{
    const QString DATABASE_FILE_NAME("./contacts.db");
    bool createTables = false;
    if (!QFileInfo(DATABASE_FILE_NAME).exists())
    {
        createTables = true;
    }

    qx::QxSqlDatabase::getSingleton()->setDriverName("QSQLITE");
    qx::QxSqlDatabase::getSingleton()->setDatabaseName(DATABASE_FILE_NAME);
    qx::QxSqlDatabase::getSingleton()->setHostName("localhost");
    qx::QxSqlDatabase::getSingleton()->setUserName("root");
    qx::QxSqlDatabase::getSingleton()->setPassword("");

    if (createTables)
    {
        qx::dao::create_table<User>();
    }

    UserPtr user(new User());
    user->setEmail("a@b.com");
    qx::dao::insert(user);
}

} // namespace data
