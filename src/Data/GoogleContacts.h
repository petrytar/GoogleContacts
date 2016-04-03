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

    void setAccessToken(const QString& accessToken) { m_accessToken = accessToken; }
    QString getAccessToken() const { return m_accessToken; }
    QList<ContactEntry*>& getContacts() { return m_contacts; }

    void loadContacts();

signals:
    void contactsLoad();

private slots:
    void onReplyFinished();

private:
    void readFromXmlDom(const QString& body);

    Database* m_database;
    QString m_accessToken;
    QNetworkAccessManager* m_networkAccessManager;
    QList<ContactEntry*> m_contacts;
};

} // namespace data

#endif // DATA_GOOGLECONTACTS_H
