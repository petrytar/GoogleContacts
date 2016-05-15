#ifndef DATA_GOOGLECONTACTS_H
#define DATA_GOOGLECONTACTS_H

#include "Data/Data_global.h"

#include <QObject>
#include <QString>
#include <QNetworkReply>

class QNetworkAccessManager;
class QNetworkReply;

namespace data
{

class Database;
class User;
class ContactEntry;
class ContactGroup;

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

    QList<ptr<ContactGroup>>& getGroups() { return m_contactGroups; }

    QList<ptr<ContactEntry>>& getContacts() { return m_contactEntries; }
    void addContact(ptr<ContactEntry> contactEntry);

    void syncGroupsAndContacts();

    QList<ptr<ContactEntry>> findContacts(const QString& str);

    bool exportContacts(const QString& fileName);
    bool importContacts(const QString& fileName);

signals:
    void groupsSyncSuccessful();
    void contactsSyncSuccessful();
    void authorizationError();
    void otherError(QNetworkReply::NetworkError error);

private:
    QString getAccessToken() const;

    void remediateGroupsAndContacts();

    void syncContacts();
    void syncGroups();

    void processGetGroupsReply(QNetworkReply* reply);
    void processGetContactsReply(QNetworkReply* reply);
    
    QList<ptr<ContactGroup>> parseContactGroups(const QString& xml);
    void syncContactGroups(QList<ptr<ContactGroup>> newContactGroups);

    QList<ptr<ContactEntry>> parseContactEntries(const QString& xml);
    void syncContactEntries(QList<ptr<ContactEntry>> newContactEntries);

    void sendNextQueuedRequest();
    void sendCreateContactEntryRequest(ptr<ContactEntry> contactEntry);
    void processCreateContactEntryReply(ptr<ContactEntry> contactEntry, QNetworkReply* reply);
    void sendDeleteContactEntryRequest(ptr<ContactEntry> contactEntry);
    void processDeleteContactEntryReply(ptr<ContactEntry> contactEntry, QNetworkReply* reply);
    void sendUpdateContactEntryRequest(ptr<ContactEntry> contactEntry);
    void processUpdateContactEntryReply(ptr<ContactEntry> contactEntry, QNetworkReply* reply);

    bool finalizeAndCheckErrorsOnReply(const QString& description, QNetworkReply* reply);

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
    QList<ptr<ContactGroup>> m_contactGroups;
    QList<ptr<ContactEntry>> m_contactEntries;
    Database* m_database;
    ptr<User> m_activeUser;

    QList<Request> m_queuedRequests;
};

} // namespace data

#endif // DATA_GOOGLECONTACTS_H
