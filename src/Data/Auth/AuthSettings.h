#ifndef DATA_AUTHSETTINGS_H
#define DATA_AUTHSETTINGS_H

#include <QString>
#include <QStringList>

namespace data
{

/**
 * @brief A helper class holding static information related to authentication
 */
class AuthSettings
{
public:
    static QString getAuthUri();
    static QString getTokenUri();
    static QString getClientId();
    static QString getClientSecret();

private:
    AuthSettings();

    static const AuthSettings& getInstance();

    // helpping structure to hold data from json file
    struct JsonFile
    {
        QString clientId;
        QString projectId;
        QString authUri;
        QString tokenUri;
        QString authProviderX509CertUrl;
        QString clientSecret;
        QStringList redirectUris;
    };

    JsonFile m_jsonFile;
};

} // namespace data

#endif // DATA_AUTHSETTINGS_H
