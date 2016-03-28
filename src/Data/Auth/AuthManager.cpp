#include "AuthManager.h"

#include "Data/Auth/AuthSettings.h"
#include "Data/debugAsserts.h"

namespace data
{

AuthManager::AuthManager(QObject* parent) :
    BaseClass(parent),
    m_networkAccessManager(new QNetworkAccessManager),
    m_authServer(new AuthServer())
{
    VERIFY(connect(m_authServer, SIGNAL(authCodeReceived(QString)), SLOT(onAuthCodeReceived(QString))));
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
            "&scope=https://www.googleapis.com/auth/contacts" +
            "&response_type=code";
}

void AuthManager::onAuthFailureReceived()
{
    emit error();
}

void AuthManager::onAuthCodeReceived(const QString& authCode)
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
    VERIFY(connect(reply, SIGNAL(finished()), this, SLOT(onReplyFinished())));
}

void AuthManager::onReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() != QNetworkReply::NoError)
    {
        emit error();
        return;
    }

    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    m_accessToken = json.object()["access_token"].toString();
    emit accessTokenReceived();
}

} // namespace data
