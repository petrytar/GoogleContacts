#include "Database.h"

#include "Data/Model/User.h"
#include "Data/Model/ContactEntry.h"
#include "Data/Model/ContactProperty.h"

namespace data
{

Database::Database(QObject* parent) :
    BaseClass(parent)
{
}

Database::~Database() {}

void Database::open(User* user)
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
        qx::dao::create_table<ContactEntry>();
        qx::dao::create_table<ContactProperty>();
    }
    else
    {
        VectorOfUserPtr vectorOfUserPtr;
        qx::dao::fetch_all(vectorOfUserPtr);
        if (vectorOfUserPtr.size())
        {
            user->copyFrom(vectorOfUserPtr.at(0).get());
        }
    }

//    UserPtr user(new User());
//    user->setEmail("a@b.com");
//    qx::dao::insert(user);

//    ContactEntryPtr contactEntryPtr;
//    contactEntryPtr.reset(new ContactEntry());

//    ContactPropertyPtr contactPropertyPtr_0;
//    contactPropertyPtr_0.reset(new ContactProperty());

//    ContactPropertyPtr contactPropertyPtr_1;
//    contactPropertyPtr_1.reset(new ContactProperty());

//    contactPropertyPtr_0->setContactEntry(contactEntryPtr);
//    contactPropertyPtr_0->setName("Clint mob 0");
//    contactPropertyPtr_0->setValue("777777770");
//    contactPropertyPtr_0->setType("phoneNumber");

//    contactPropertyPtr_1->setContactEntry(contactEntryPtr);
//    contactPropertyPtr_1->setName("Clint mob 1");
//    contactPropertyPtr_1->setValue("777777771");
//    contactPropertyPtr_1->setType("phoneNumber");

//    QSqlError daoError = qx::dao::delete_all<ContactEntry>();
//    daoError = qx::dao::delete_all<ContactProperty>();
//    daoError = qx::dao::insert(contactEntryPtr);
//    daoError = qx::dao::insert(contactPropertyPtr_0);
//    daoError = qx::dao::insert(contactPropertyPtr_1);
}

void Database::onUserDataChanged(User* user)
{
    UserPtr userPtr(new User(user));
    qx::dao::insert(userPtr);
}

} // namespace data
