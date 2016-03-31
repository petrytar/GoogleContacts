#include "ContactProperty.h"

#include "Data/Model/ContactEntry.h"

#include <QxMemLeak.h>

QX_REGISTER_COMPLEX_CLASS_NAME_CPP_DATA(data::ContactProperty, data_ContactProperty)

namespace qx {
template <> void register_class(QxClass<data::ContactProperty>& t)
{
    t.setName("ContactProperty");
    t.id(&data::ContactProperty::m_id, "contactPropertyId");
    t.data(&data::ContactProperty::m_name, "name");
    t.data(&data::ContactProperty::m_value, "value");
    t.data(&data::ContactProperty::m_type, "type");

    t.relationManyToOne(&data::ContactProperty::m_contactEntryPtr, "contactEntryId");
}}

namespace data
{

ContactProperty::ContactProperty(const QString& name, const QString& value, const QString& type) :
    m_name(name),
    m_value(value),
    m_type(type)
{
}

ContactProperty::~ContactProperty()
{
}

} // namespace data
