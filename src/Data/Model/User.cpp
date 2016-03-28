#include "User.h"

#include <QxMemLeak.h>

QX_REGISTER_COMPLEX_CLASS_NAME_CPP_DATA(data::User, data_User)

namespace qx {
template <> void register_class(QxClass<data::User>& t)
{
    t.setName("user");
    t.id(&data::User::m_id, "id");
    t.data(&data::User::m_email, "email");
}}

namespace data
{

User::User()
{
}

} // namespace data

