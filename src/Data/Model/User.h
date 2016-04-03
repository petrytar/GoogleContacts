#ifndef DATA_USER_H
#define DATA_USER_H

#include "Data/stable.h"

#include <QString>

namespace data
{

class User
{
    QX_REGISTER_FRIEND_CLASS(data::User)
public:
    User();
    User(User* user);
    virtual ~User(){}

    long getId() const { return m_id; }
    void setId(long id) { m_id = id; }

    QString getEmail() const { return m_email; }
    void setEmail(QString email) { m_email = email; }

    QString getAccessToken() const { return m_accessToken; }
    void setAccessToken(const QString& accessToken) { m_accessToken = accessToken; }

    QString getRefreshToken() const { return m_refreshToken; }
    void setRefreshToken(const QString& refreshToken) { m_refreshToken = refreshToken; }

    void copyFrom(User* user);

private:
    long m_id;
    QString m_email;
    QString m_accessToken;
    QString m_refreshToken;
};

} // namespace data

typedef qx_shared_ptr<data::User> UserPtr;
typedef QVector<UserPtr> VectorOfUserPtr;

QX_REGISTER_COMPLEX_CLASS_NAME_HPP_DATA(data::User, qx::trait::no_base_class_defined, 0, data_User)

#endif // DATA_USER_H
