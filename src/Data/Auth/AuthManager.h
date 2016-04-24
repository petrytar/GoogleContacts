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

    void refreshAccessToken(const QString& refreshToken);

signals:
    void error();
    void newUserInitialiazed(data::ptr<data::User> user);
    void accessTokenReceived(const QString& accessToken);
    void invalidRefreshToken();

private slots:
    void requestAccessToken(const QString& authCode);
    void onAuthFailureReceived();

private:
    ushort getFreeOpenPort() const;

    void requestEmail();
    bool finalizeAndCheckErrorsOnReply(const QString& description, QNetworkReply* reply);

    void processAccessTokenRequestResult(QNetworkReply* reply);
    void processEmailRequestResult(const QString& accessToken, const QString& refreshToken, QNetworkReply* reply);
    void processRefreshTokenRequestResult(QNetworkReply* reply);

    QNetworkAccessManager* m_networkAccessManager;
    AuthServer* m_authServer;
};

} // namespace data

#endif // DATA_AUTHCONFIGS_H
