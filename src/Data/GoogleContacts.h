#ifndef DATA_GOOGLECONTACTS_H
#define DATA_GOOGLECONTACTS_H

#include "Data/Data_global.h"
#include "Data/ContactEntry.h"

#include <QObject>
#include <QString>

class QNetworkAccessManager;

namespace data
{
class Database;

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
    // Helping Enum of entry's tags
    enum ETagId
    {
        E_TAG__BEGIN = 0,
        E_TAG_UPDATED = E_TAG__BEGIN,
        E_TAG_TITLE,
        E_TAG_CONTENT,
        E_TAG_GIVEN_NAME,
        E_TAG_FAMILY_NAME,
        E_TAG_NICKNAME,
        E_TAG_FILE_AS,
        E_TAG_ORG_NAME,
        E_TAG_ORG_TITLE,
        E_TAG_EMAIL,                        // list
        E_TAG_IM,                           // list
        E_TAG_PHONE_NUMBER,                 // list
        E_TAG_STRUCTURED_POSTAL_ADDRESS,    // list
        E_TAG_RELATION,                     // list
        E_TAG_USER_DEFINED_FIELD,           // list
        E_TAG_WEBSITE,                      // list
        E_TAG_GROUP_MEMBERSHIP_INFO,        // list
        E_TAG__END
    };

    explicit GoogleContacts(QObject* parent);
    virtual ~GoogleContacts();

    // inline implementations
    void setAccessToken(const QString& accessToken) { m_accessToken = accessToken; }
    QString getAccessToken() const { return m_accessToken; }

    void loadContacts();
    QList<ContactEntry*>& getContacts();

signals:
    void contactsLoad();

private slots:
    void onReplyFinished();

private:
    void readFromXmlDom(const QString& body);
    void printAllContacts() const;
    QString columnIdToColumnString(int columnId) const;

    Database* m_database;
    QString m_accessToken;
    QNetworkAccessManager* m_networkAccessManager;
    QList<ContactEntry*> m_contactEntries;
};

} // namespace data

#endif // DATA_GOOGLECONTACTS_H
