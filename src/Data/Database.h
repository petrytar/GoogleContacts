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

    void save(ptr<ContactEntry> contactEntry);
    void update(ptr<ContactEntry> existingContactEntry, ptr<ContactEntry> updatedContactEntry);

    ptr<User> getUser();
};

} // namespace data

#endif // DATA_DATABASE_H
