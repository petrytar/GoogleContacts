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
    t.data(&data::ContactProperty::m_label, "label");
    t.data(&data::ContactProperty::m_value, "value");
    t.data(&data::ContactProperty::m_type, "type");

    t.relationManyToOne(&data::ContactProperty::m_contactEntry, "contactEntryId");
}}

namespace data
{

QMap<ContactProperty::EType, QString> initTypes()
{
    QMap<ContactProperty::EType, QString> types;
    types.insert(ContactProperty::E_TYPE_EMAIL, "email");
    types.insert(ContactProperty::E_TYPE_PHONE_NUMBER, "phoneNumber");
    return types;
}

static const QMap<ContactProperty::EType, QString> TYPES = initTypes();

ContactProperty::ContactProperty() :
    m_label(),
    m_value(),
    m_type(),
    m_contactEntry()
{
}

ContactProperty::ContactProperty(ptr<ContactEntry> contactEntry, const QString& label, const QString& value, EType type) :
    m_label(label),
    m_value(value),
    m_type(TYPES.value(type)),
    m_contactEntry(contactEntry)
{
}

ContactProperty::~ContactProperty()
{
}

ContactProperty::EType ContactProperty::getType() const
{
    return TYPES.key(m_type, E_TYPE__END);
}

void ContactProperty::setType(EType type)
{
    m_type = TYPES.value(type);
}

} // namespace data
