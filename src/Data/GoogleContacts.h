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
    explicit GoogleContacts(QObject* parent);
    virtual ~GoogleContacts();

    // inline implementations
    void setAccessToken(const QString& accessToken) { m_accessToken = accessToken; }
    QString getAccessToken() const { return m_accessToken; }

    void loadContacts();

private slots:
    void onReplyFinished();

private:
    void readFromXmlDom(const QString& body);
    void printAllContacts() const;

    Database* m_database;
    QString m_accessToken;
    QNetworkAccessManager* m_networkAccessManager;
    QList<ContactEntry*> m_contactEntries;
};

} // namespace data

#endif // DATA_GOOGLECONTACTS_H
