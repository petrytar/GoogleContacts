#include "AuthManager.h"

#include "Data/Auth/AuthSettings.h"
#include "Data/Auth/AuthServer.h"
#include "Data/Model/User.h"
#include "Data/debugAsserts.h"

namespace data
{

AuthManager::AuthManager(QObject* parent) :
    BaseClass(parent),
    m_networkAccessManager(new QNetworkAccessManager),
    m_authServer(new AuthServer())
{
    VERIFY(connect(m_authServer, SIGNAL(authCodeReceived(QString)), SLOT(requestAccessToken(QString))));
    VERIFY(connect(m_authServer, SIGNAL(authFailureReceived()), SLOT(onAuthFailureReceived())));
}

AuthManager::~AuthManager()
{
}

void AuthManager::start()
{
    m_authServer->start();
}

QUrl AuthManager::generateAuthorizationRequestUrl() const
{
    return AuthSettings::getAuthUri() +
            "?client_id=" + AuthSettings::getClientId() +
            "&redirect_uri=" + "http://localhost:" + QString::number(m_authServer->getPort()) +
            "&scope=https://www.googleapis.com/auth/contacts https://www.googleapis.com/auth/userinfo.email" +
            "&response_type=code";
}

void AuthManager::onAuthFailureReceived()
{
    emit error();
}

void AuthManager::requestAccessToken(const QString& authCode)
{
    QUrlQuery postData;
    postData.addQueryItem("code", authCode);
    postData.addQueryItem("client_id", AuthSettings::getClientId());
    postData.addQueryItem("client_secret", AuthSettings::getClientSecret());
    postData.addQueryItem("redirect_uri", QString("http://localhost:") + QString::number(m_authServer->getPort()));
    postData.addQueryItem("grant_type", "authorization_code");

    QNetworkRequest request(QUrl(AuthSettings::getTokenUri()));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply* reply = m_networkAccessManager->post(request, postData.toString(QUrl::FullyEncoded).toUtf8());
    VERIFY(connect(reply, SIGNAL(finished()), this, SLOT(onAccessTokenRequestFinished())));
}

void AuthManager::onAccessTokenRequestFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError)
    {
        emit error();
        return;
    }

    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    m_accessToken = json.object()["access_token"].toString();
    m_refreshToken = json.object()["refresh_token"].toString();

    requestEmail();
}

void AuthManager::requestEmail()
{
    QNetworkRequest request(QUrl(QString("https://www.googleapis.com/oauth2/v1/userinfo?alt=json&access_token=%1").arg(m_accessToken)));;
    QNetworkReply* reply = m_networkAccessManager->get(request);
    VERIFY(connect(reply, SIGNAL(finished()), this, SLOT(onEmailRequestFinished())));
}

void AuthManager::onEmailRequestFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError)
    {
        emit error();
        return;
    }

    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    QString email = json.object()["email"].toString();

    ptr<User> user(new User());
    user->setAccessToken(m_accessToken);
    user->setRefreshToken(m_refreshToken);
    user->setEmail(email);

    emit newUserInitialiazed(user);
}

} // namespace data
