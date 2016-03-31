#ifndef DATA_AUTHCONFIGS_H
#define DATA_AUTHCONFIGS_H

#include "Data/Data_global.h"
#include "Data/Auth/AuthServer.h"
#include "Data/GoogleContacts.h"

#include <QObject>
#include <QUrl>

namespace data
{

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
    explicit AuthManager(QObject* parent);
    ~AuthManager();

    void start();
    QUrl generateAuthorizationRequestUrl() const;

    QString getAccessToken() const { return m_accessToken; }

signals:
    void accessTokenReceived();
    void error();

private slots:
    void onAuthCodeReceived(const QString& accessToken);
    void onAuthFailureReceived();
    void onReplyFinished();

private:
    ushort getFreeOpenPort() const;

    QNetworkAccessManager* m_networkAccessManager;
    AuthServer* m_authServer;
    QString m_accessToken;
};

} // namespace data

#endif // DATA_AUTHCONFIGS_H
