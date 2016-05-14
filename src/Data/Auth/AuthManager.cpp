#include "AuthManager.h"

#include "Data/Auth/AuthSettings.h"
#include "Data/Auth/AuthServer.h"
#include "Data/Model/User.h"
#include "Data/debugAsserts.h"

namespace data
{

AuthManager::AuthManager(QNetworkAccessManager* networkAccessManager, QObject* parent) :
    BaseClass(parent),
    m_networkAccessManager(networkAccessManager),
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
    qDebug() << "Auth declined by server";
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
    auto onReplyFinished = [this, reply]()
    {
        if (finalizeAndCheckErrorsOnReply("requestAccessToken", reply))
        {
            processAccessTokenRequestResult(reply);
        }
    };
    VERIFY(connect(reply, &QNetworkReply::finished, onReplyFinished));
}

void AuthManager::processAccessTokenRequestResult(QNetworkReply* reply)
{
    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    QString accessToken = json.object()["access_token"].toString();
    QString refreshToken = json.object()["refresh_token"].toString();

    QNetworkRequest request(QUrl(QString("https://www.googleapis.com/oauth2/v1/userinfo?alt=json&access_token=%1").arg(accessToken)));;
    QNetworkReply* emailReply = m_networkAccessManager->get(request);
    auto onReplyFinished = [this, emailReply, accessToken, refreshToken]()
    {
        if (finalizeAndCheckErrorsOnReply("requestEmail", emailReply))
        {
            processEmailRequestResult(accessToken, refreshToken, emailReply);
        }
    };
    VERIFY(connect(emailReply, &QNetworkReply::finished, onReplyFinished));
}

void AuthManager::processEmailRequestResult(const QString& accessToken, const QString& refreshToken, QNetworkReply* reply)
{
    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    QString email = json.object()["email"].toString();

    ptr<User> user(new User());
    user->setAccessToken(accessToken);
    user->setRefreshToken(refreshToken);
    user->setEmail(email);

    emit newUserInitialiazed(user);
}

bool AuthManager::finalizeAndCheckErrorsOnReply(const QString& description, QNetworkReply* reply)
{
    qDebug() << description << "finished with code:" << reply->error();
    reply->deleteLater();
    if (reply->error() == QNetworkReply::NoError)
    {
        return true;
    }
    qDebug() << "Error reply contents:";
    QByteArray replyData = reply->readAll();
    //QString replyText(replyData);
    //std::cout << replyText.toStdString() << std::endl;

    QJsonParseError parseError;
    QJsonDocument json = QJsonDocument::fromJson(replyData, &parseError);
    if (parseError.error == QJsonParseError::NoError && json.object().contains("error"))
    {
        QString error = json.object()["error"].toString();
        if (error == "invalid_grant")
        {
            emit invalidRefreshToken();
            return false;
        }
    }

    emit error();
    return false;
}

void AuthManager::refreshAccessToken(const QString& refreshToken)
{
    QUrlQuery postData;
    postData.addQueryItem("client_id", AuthSettings::getClientId());
    postData.addQueryItem("client_secret", AuthSettings::getClientSecret());
    postData.addQueryItem("refresh_token", refreshToken);
    postData.addQueryItem("grant_type", "refresh_token");

    QNetworkRequest request(QUrl(AuthSettings::getTokenUri()));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply* reply = m_networkAccessManager->post(request, postData.toString(QUrl::FullyEncoded).toUtf8());
    auto onReplyFinished = [this, reply]()
    {
        if (finalizeAndCheckErrorsOnReply("refreshAccessToken", reply))
        {
            processRefreshTokenRequestResult(reply);
        }
    };
    VERIFY(connect(reply, &QNetworkReply::finished, onReplyFinished));
}

void AuthManager::processRefreshTokenRequestResult(QNetworkReply* reply)
{
    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    QString accessToken = json.object()["access_token"].toString();
    emit accessTokenReceived(accessToken);
}

} // namespace data
