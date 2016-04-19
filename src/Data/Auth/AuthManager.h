#ifndef DATA_AUTHCONFIGS_H
#define DATA_AUTHCONFIGS_H

#include "Data/Data_global.h"

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>

namespace data
{

class AuthServer;
class User;

/**
 * \brief The AuthConfigs class
 *
 * Setup Authentication(who you are?) and Authorization(what you are authorized to do?).
 */
class DATA_EXPORT AuthManager : public QObject
{
    Q_OBJECT
private:
    typedef QObject BaseClass;

public:
    explicit AuthManager(QNetworkAccessManager* networkAccessManager, QObject* parent);
    ~AuthManager();

    void start();
    QUrl generateAuthorizationRequestUrl() const;

    long getId() const { return m_id; }
    QString getAccessToken() const { return m_accessToken; }
    QString getRefreshToken() const { return m_refreshToken; }

signals:
    void error();
    void newUserInitialiazed(data::ptr<data::User> user);

private slots:
    void requestAccessToken(const QString& authCode);

    void onAuthFailureReceived();
    void onAccessTokenRequestFinished();
    void onEmailRequestFinished();

private:
    ushort getFreeOpenPort() const;

    void requestEmail();

    long m_id;
    QNetworkAccessManager* m_networkAccessManager;
    AuthServer* m_authServer;
    QString m_accessToken;
    QString m_refreshToken;
};

} // namespace data

#endif // DATA_AUTHCONFIGS_H
