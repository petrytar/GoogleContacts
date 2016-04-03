#include "User.h"

#include <QxMemLeak.h>

QX_REGISTER_COMPLEX_CLASS_NAME_CPP_DATA(data::User, data_User)

namespace qx {
template <> void register_class(QxClass<data::User>& t)
{
    t.setName("User");
    t.id(&data::User::m_id, "id");
    t.data(&data::User::m_email, "email");
    t.data(&data::User::m_accessToken, "accessToken");
    t.data(&data::User::m_refreshToken, "refreshToken");
}}

namespace data
{

User::User()
{
}

User::User(User* user)
{
    m_id = user->getId();
    m_email = user->getEmail();
    m_accessToken = user->getAccessToken();
    m_refreshToken = user->getRefreshToken();
}

void User::copyFrom(User* user)
{
    m_id = user->getId();
    m_email = user->getEmail();
    m_accessToken = user->getAccessToken();
    m_refreshToken = user->getRefreshToken();
}

} // namespace data

