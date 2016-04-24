#ifndef DATA_GOOGLECONTACTS_H
#define DATA_GOOGLECONTACTS_H

#include "Data/Data_global.h"
#include "Data/Model/ContactEntry.h"

#include <QObject>
#include <QString>
#include <QNetworkReply>

class QNetworkAccessManager;
class QNetworkReply;

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
    explicit GoogleContacts(QNetworkAccessManager* networkAccessManager, Database* database, QObject* parent);
    virtual ~GoogleContacts();

    void setActiveUser(ptr<User> user);
    ptr<User> getActiveUser() const { return m_activeUser; }

    QList<ptr<ContactEntry>>& getContacts() { return m_contactEntries; }
    void addContact(ptr<ContactEntry> contactEntry);

    void syncContacts();

signals:
    void contactsSyncSuccessful();
    void authorizationError();
    void otherError(QNetworkReply::NetworkError error);

private:
    QString getAccessToken() const;

    void processGetContactsReply(QNetworkReply* reply);
    
    QList<ptr<ContactEntry>> parseContactEntries(const QString& xml);
    void syncContactEntries(QList<ptr<ContactEntry>> newContactEntries);

    void sendNextQueuedRequest();
    void sendCreateContactEntryRequest(ptr<ContactEntry> contactEntry);
    void processCreateContactEntryReply(ptr<ContactEntry> contactEntry, QNetworkReply* reply);
    void sendDeleteContactEntryRequest(ptr<ContactEntry> contactEntry);
    void processDeleteContactEntryReply(ptr<ContactEntry> contactEntry, QNetworkReply* reply);
    void sendUpdateContactEntryRequest(ptr<ContactEntry> contactEntry);
    void processUpdateContactEntryReply(ptr<ContactEntry> contactEntry, QNetworkReply* reply);

    bool finilizeAndCheckErrorsOnReply(const QString& description, QNetworkReply* reply);

    struct Request
    {
        enum EType
        {
            E_TYPE_CREATE,
            E_TYPE_DELETE,
            E_TYPE_UPDATE
        };

        Request(EType type, ptr<ContactEntry> contactEntry) :
            type(type),
            contactEntry(contactEntry)
        {}

        EType type;
        ptr<ContactEntry> contactEntry;
    };

    QNetworkAccessManager* m_networkAccessManager;
    QList<ptr<ContactEntry>> m_contactEntries;
    Database* m_database;
    ptr<User> m_activeUser;

    QList<Request> m_queuedRequests;
};

} // namespace data

#endif // DATA_GOOGLECONTACTS_H
