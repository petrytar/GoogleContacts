#include "AuthSettings.h"

#include "Data/debugAsserts.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QJsonArray>

namespace data
{

QStringList fromQJsonArray(const QJsonArray& redirectUris)
{
    QStringList uris;
    for (const QJsonValue& redirectUri : redirectUris)
    {
        uris << redirectUri.toString();
    }
    return uris;
}

AuthSettings::AuthSettings()
{
    QString jsonData;

    QFile jsonFile(":/OAuth_2.0_client_ID.json");
    ASSERT(jsonFile.exists(), "OAuth_2.0_client_ID.json file does not exists.");
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    jsonData = jsonFile.readAll();
    jsonFile.close();

    QJsonDocument qJsonDocument = QJsonDocument::fromJson(jsonData.toUtf8());
    QJsonObject qJsonObject = qJsonDocument.object()["installed"].toObject();
    m_jsonFile.clientId = qJsonObject["client_id"].toString();
    m_jsonFile.projectId = qJsonObject["project_id"].toString();
    m_jsonFile.authUri = qJsonObject["auth_uri"].toString();
    m_jsonFile.tokenUri = qJsonObject["token_uri"].toString();
    m_jsonFile.authProviderX509CertUrl = qJsonObject["auth_provider_x509_cert_url"].toString();
    m_jsonFile.clientSecret = qJsonObject["client_secret"].toString();
    m_jsonFile.redirectUris = fromQJsonArray(qJsonObject["redirect_uris"].toArray());
}

const AuthSettings& AuthSettings::getInstance()
{
    static AuthSettings instance;
    return instance;
}

QString AuthSettings::getAuthUri()
{
    return getInstance().m_jsonFile.authUri;
}

QString AuthSettings::getTokenUri()
{
    return getInstance().m_jsonFile.tokenUri;
}

QString AuthSettings::getClientId()
{
    return getInstance().m_jsonFile.clientId;
}

QString AuthSettings::getClientSecret()
{
    return getInstance().m_jsonFile.clientSecret;
}

} // namespace data
