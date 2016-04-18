#ifndef DATA_USER_H
#define DATA_USER_H

#include "Data/Data_global.h"
#include <QxRegister/QxRegister.h>

namespace data
{

class User
{
    QX_REGISTER_FRIEND_CLASS(data::User)
public:
    User();
    virtual ~User(){}

    long getId() const { return m_id; }
    void setId(long id) { m_id = id; }

    QString getEmail() const { return m_email; }
    void setEmail(QString email) { m_email = email; }

    QString getAccessToken() const { return m_accessToken; }
    void setAccessToken(const QString& accessToken) { m_accessToken = accessToken; }

    QString getRefreshToken() const { return m_refreshToken; }
    void setRefreshToken(const QString& refreshToken) { m_refreshToken = refreshToken; }

private:
    long m_id;
    QString m_email;
    QString m_accessToken;
    QString m_refreshToken;
};

} // namespace data

typedef data::ptr<data::User> UserPtr;

QX_REGISTER_COMPLEX_CLASS_NAME_HPP_DATA(data::User, qx::trait::no_base_class_defined, 0, data_User)

#endif // DATA_USER_H
