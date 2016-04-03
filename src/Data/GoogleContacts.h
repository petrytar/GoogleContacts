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
    explicit GoogleContacts(QObject* parent);
    virtual ~GoogleContacts();

    void setAccessToken(const QString& accessToken);
    QString getAccessToken() const;
    QList<ContactEntry*>& getContacts() { return m_contacts; }

    void loadContacts();

signals:
    void contactsLoad();
    void userDataChanged(User* user);

private slots:
    void onReplyFinished();

private:
    void readFromXmlDom(const QString& body);

    QNetworkAccessManager* m_networkAccessManager;
    QList<ContactEntry*> m_contacts;
    Database* m_database;
    User* m_user;
};

} // namespace data

#endif // DATA_GOOGLECONTACTS_H
