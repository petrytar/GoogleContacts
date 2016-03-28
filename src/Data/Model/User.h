#ifndef DATA_USER_H
#define DATA_USER_H

#include "Data/Data_global.h"

#include <QxOrm.h>
#include <QString>

namespace data
{

class User
{
    QX_REGISTER_FRIEND_CLASS(data::User)

public:
    User();

    QString getEmail() const { return m_email; }
    void setEmail(QString email) { m_email = email; }

private:
    long m_id;
    QString m_email;
};

} // namespace data

typedef qx_shared_ptr<data::User> UserPtr;

QX_REGISTER_COMPLEX_CLASS_NAME_HPP_DATA(data::User, qx::trait::no_base_class_defined, 0, data_User)

#endif // DATA_USER_H
