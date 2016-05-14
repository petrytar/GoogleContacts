#ifndef MAINAPPCONSOLE_CONSOLEOPERATOR_H
#define MAINAPPCONSOLE_CONSOLEOPERATOR_H

#include <QObject>
#include <QNetworkReply>

namespace data
{
class Database;
class GoogleContacts;
class AuthManager;
}

class QNetworkAccessManager;

class ConsoleOperator : public QObject
{
    Q_OBJECT
private:
    typedef QObject BaseClass;

public:
    explicit ConsoleOperator(QObject* parent = 0);

    bool init();

    void findContacts(const QString& pattern);
    void createContact(const QString& name);
    void deleteContacts(const QString& pattern);

public slots:
    void syncContacts();

private slots:
    void onGroupsSyncSuccessful();
    void onContactsSyncSuccessful();
    void onContactsAuthorizationError();
    void onContactsOtherError(QNetworkReply::NetworkError error);

    void onAuthFailed();
    void onAccessTokenReceived(const QString& accessToken);
    void onInvalidRefreshToken();

private:
    QNetworkAccessManager* m_networkAccessManager;
    data::AuthManager* m_authManager;
    data::Database* m_database;
    data::GoogleContacts* m_googleContacts;

    bool m_firstAuthorizationAttempt;
};

#endif // MAINAPPCONSOLE_CONSOLEOPERATOR_H
