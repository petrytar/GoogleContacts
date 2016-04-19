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

    /*QSqlError daoError = qx::dao::delete_all<ContactEntry>();
    daoError = qx::dao::delete_all<ContactProperty>();
    daoError = qx::dao::delete_all<User>();*/

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

    contactEntryPtr->addPhoneNumber(contactPropertyPtr_0);
    contactEntryPtr->addPhoneNumber(contactPropertyPtr_1);

    qx::dao::save_with_relation_recursive(contactEntryPtr);

    QList<ContactPropertyPtr> newProperties;
    newProperties.append(contactPropertyPtr_1);
    contactEntryPtr->setPhoneNumbers(newProperties);

    qx::dao::save_with_relation_recursive(contactEntryPtr);*/
}

QList<ptr<User>> Database::getUsers()
{
    QList<ptr<User>> users;
    qx::dao::fetch_all(users);
    return users;
}

void Database::saveOrGetByEmail(ptr<User> user)
{
    qx::QxSqlQuery query;
    query.where("User.email").isEqualTo(user->getEmail());
    QList<ptr<User>> users;
    qx::dao::fetch_by_query(query, users);
    if (!users.empty())
    {
        user->setId(users.at(0)->getId());
    }

    qx::dao::save(user);
}

QList<ptr<ContactEntry>> Database::getContactEntries(ptr<User> user)
{
    QList<ptr<ContactEntry>> contactEntries;
    qx::QxSqlQuery query;
    query.where("ContactEntry.userId").isEqualTo(static_cast<int>(user->getId()));
    qx::dao::fetch_by_query_with_all_relation(query, contactEntries);
    return contactEntries;
}

void Database::save(ptr<ContactEntry> contactEntry)
{
    qx::dao::save_with_relation_recursive(contactEntry);
}

void Database::update(ptr<ContactEntry> existingContactEntry, ptr<ContactEntry> updatedContactEntry)
{
    existingContactEntry->copyFrom(*updatedContactEntry.get());
    qx::QxSqlQuery query;
    query.where("ContactProperty.contactEntryId").isEqualTo(static_cast<int>(existingContactEntry->getId()));
    qx::dao::delete_by_query<ContactProperty>(query);
    save(existingContactEntry);
}

} // namespace data
