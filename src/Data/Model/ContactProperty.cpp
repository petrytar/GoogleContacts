#include "ContactProperty.h"

#include "Data/Model/ContactEntry.h"

#include <QxRegister/QxClass.h>
#include <QxDao/IxPersistable.h>
#include <QxDao/QxDao_Impl.h>
#include <QxConvert/QxConvert.h>
#include <QxConvert/QxConvert_Impl.h>

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

    t.relationManyToOne(&data::ContactProperty::m_contactEntry, "contactEntryId");
}}

namespace data
{

ContactProperty::ContactProperty() :
    m_name(),
    m_value(),
    m_type(),
    m_contactEntry()
{
}

ContactProperty::ContactProperty(ptr<ContactEntry> contactEntry, const QString& name, const QString& value, const QString& type) :
    m_name(name),
    m_value(value),
    m_type(type),
    m_contactEntry(contactEntry)
{
}

ContactProperty::~ContactProperty()
{
}

} // namespace data
