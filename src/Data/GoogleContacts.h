#ifndef DATA_GOOGLECONTACTS_H
#define DATA_GOOGLECONTACTS_H

#include "Data/Data_global.h"

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
};

} // namespace data

#endif // DATA_GOOGLECONTACTS_H
