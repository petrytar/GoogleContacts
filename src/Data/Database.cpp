#include "Database.h"

#include "Data/debugAsserts.h"
#include "Data/Model/User.h"
#include "Data/Model/ContactEntry.h"
#include "Data/Model/ContactProperty.h"

#include <QxDao/IxPersistable.h>
#include <QxDao/QxDao_Impl.h>
#include <QxConvert/QxConvert_Impl.h>

namespace data
{

Database::Database(QObject* parent) :
    BaseClass(parent)
{
}

Database::~Database() {}

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

    qx::QxSqlDatabase::getSingleton()->setForceParentIdToAllChildren(true); // see http://www.qxorm.com/forum/phpbb/viewtopic.php?f=2&t=454

    if (createTables)
    {
        qx::dao::create_table<User>();
        qx::dao::create_table<ContactEntry>();
        qx::dao::create_table<ContactProperty>();
    }

    QSqlError daoError = qx::dao::delete_all<ContactEntry>();
    daoError = qx::dao::delete_all<ContactProperty>();
    daoError = qx::dao::delete_all<User>();

    /*UserPtr user(new User());
    user->setEmail("a@b.com");
    qx::dao::insert(user);

    ContactEntryPtr contactEntryPtr(new ContactEntry());
    contactEntryPtr->setGoogleContactId("123");

    ContactPropertyPtr contactPropertyPtr_0(new ContactProperty(contactEntryPtr, "", "", ""));
    ContactPropertyPtr contactPropertyPtr_1(new ContactProperty(contactEntryPtr, "", "", ""));

    contactPropertyPtr_0->setName("Clint mob 0");
    contactPropertyPtr_0->setValue("777777770");
    contactPropertyPtr_0->setType("phoneNumber");

    contactPropertyPtr_1->setName("Clint mob 1");
    contactPropertyPtr_1->setValue("777777771");
    contactPropertyPtr_1->setType("phoneNumber");

    QSqlError daoError = qx::dao::insert(contactEntryPtr);
    daoError = qx::dao::insert(contactPropertyPtr_0);
    daoError = qx::dao::insert(contactPropertyPtr_1);*/
}

ptr<User> Database::getUser()
{
    QList<ptr<User>> users;
    qx::dao::fetch_all(users);
    ASSERT(users.size() <= 1, "users not empty");
    return users.empty() ? ptr<User>() : users.at(0);
}

void Database::insert(ptr<ContactEntry> contactEntry)
{
    qx::dao::insert(contactEntry);
    for (ptr<ContactProperty> property : contactEntry->getEmails())
    {
        qx::dao::insert(property);
    }
    for (ptr<ContactProperty> property : contactEntry->getPhoneNumbers())
    {
        qx::dao::insert(property);
    }
}

} // namespace data
