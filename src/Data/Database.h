#ifndef DATA_DATABASE_H
#define DATA_DATABASE_H

#include "Data/Data_global.h"

#include <QObject>

namespace data
{

class User;
class ContactEntry;

class Database : public QObject
{
    Q_OBJECT
private:
    typedef QObject BaseClass;

public:
    explicit Database(QObject* parent = 0);
    virtual ~Database();

    void open();

    QList<ptr<User>> getUsers();
    void save(ptr<User> user);
    void saveOrGetByEmail(ptr<User> user);
    void remove(ptr<User> user);

    QList<ptr<ContactEntry>> getContactEntries(ptr<User> user);
    void save(ptr<ContactEntry> contactEntry);
    void update(ptr<ContactEntry> existingContactEntry, ptr<ContactEntry> updatedContactEntry);
    void update(ptr<ContactEntry> contactEntry);
    void remove(ptr<ContactEntry> contactEntry);
};

} // namespace data

#endif // DATA_DATABASE_H
