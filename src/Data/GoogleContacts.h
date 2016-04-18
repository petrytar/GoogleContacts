#ifndef DATA_GOOGLECONTACTS_H
#define DATA_GOOGLECONTACTS_H

#include "Data/Data_global.h"
#include "Data/Model/ContactEntry.h"

#include <QObject>
#include <QString>

class QNetworkAccessManager;

namespace data
{

class Database;
class User;

/**
 * \brief The GoogleContacts class.
 *
 * Add some description here.
 */
class DATA_EXPORT GoogleContacts : public QObject
{
    Q_OBJECT
private:
    typedef QObject BaseClass;

public:
    explicit GoogleContacts(Database* database, QObject* parent);
    virtual ~GoogleContacts();

    void setActiveUser(ptr<User> user) { m_activeUser = user; }

    QList<ptr<ContactEntry>>& getContacts() { return m_contactEntries; }

    void loadContacts();

signals:
    void contactsLoad();
    void userDataChanged(User* user);

private slots:
    void onReplyFinished();

private:
    QString getAccessToken() const;
    void saveUserEmail(const QString& userEmail);
    
    QList<ptr<ContactEntry>> parseContactEntries(const QString& xml);
    void syncContactEntries(QList<ptr<ContactEntry>> newContactEntries);

    QNetworkAccessManager* m_networkAccessManager;
    QList<ptr<ContactEntry>> m_contactEntries;
    Database* m_database;
    ptr<User> m_activeUser;
};

} // namespace data

#endif // DATA_GOOGLECONTACTS_H
