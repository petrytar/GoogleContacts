#include "ContactEntry.h"

#include "Data/debugAsserts.h"
#include "Data/Model/ContactProperty.h"
#include "Data/Model/User.h"

#include <QxRegister/QxClass.h>
#include <QxDao/IxPersistable.h>
#include <QxDao/QxDao_Impl.h>
#include <QxConvert/QxConvert.h>
#include <QxConvert/QxConvert_Impl.h>
#include <QxMemLeak.h>

QX_REGISTER_COMPLEX_CLASS_NAME_CPP_DATA(data::ContactEntry, data_ContactEntry)

namespace qx {
template <> void register_class(QxClass<data::ContactEntry>& t)
{
    t.setName("ContactEntry");
    t.id(&data::ContactEntry::m_id, "contactEntryId");
    t.relationManyToOne(&data::ContactEntry::m_user, "userId");
    t.data(&data::ContactEntry::m_googleContactId, "googleContactId");
    t.data(&data::ContactEntry::m_updatedTime, "updatedTime");
    t.data(&data::ContactEntry::m_title, "title");
    t.data(&data::ContactEntry::m_name, "name");
    t.data(&data::ContactEntry::m_content, "content");
    t.data(&data::ContactEntry::m_nickname, "nickname");
    t.data(&data::ContactEntry::m_fileAs, "fileAs");
    t.data(&data::ContactEntry::m_orgName, "orgName");
    t.data(&data::ContactEntry::m_orgTitle, "orgTitle");

    t.relationOneToMany(&data::ContactEntry::m_properties, "properties", "contactEntryId");
}}

namespace data
{

ContactEntry::ContactEntry()
{
}

ContactEntry::~ContactEntry()
{
}

void ContactEntry::copyFrom(const ContactEntry& other)
{
    m_googleContactId = other.m_googleContactId;
    m_updatedTime = other.m_updatedTime;
    m_title = other.m_title;
    m_name = other.m_name;
    m_content = other.m_content;
    m_nickname = other.m_nickname;
    m_fileAs = other.m_fileAs;
    m_orgName = other.m_orgName;
    m_orgTitle = other.m_orgTitle;

    m_properties = other.m_properties;
}

QString ContactEntry::getVisibleName() const
{
    if (!m_fileAs.isEmpty())
    {
        return m_fileAs;
    }
    return m_name;
}

QList<ContactPropertyPtr> ContactEntry::getEmails() const
{
    QList<ContactPropertyPtr> emails;
    for (ptr<ContactProperty> property : m_properties)
    {
        if (property->getType() == ContactProperty::E_TYPE_EMAIL)
        {
            emails << property;
        }
    }
    return emails;
}

QList<ContactPropertyPtr> ContactEntry::getPhoneNumbers() const
{
    QList<ContactPropertyPtr> phoneNumbers;
    for (ptr<ContactProperty> property : m_properties)
    {
        if (property->getType() == ContactProperty::E_TYPE_PHONE_NUMBER)
        {
            phoneNumbers << property;
        }
    }
    return phoneNumbers;
}

QString ContactEntry::getPrimaryEmail() const
{
    auto emails = getEmails();
    if (!emails.isEmpty())
    {
        return emails[0]->getValue();
    }
    return QString();
}

QString ContactEntry::getPrimaryPhoneNumber() const
{
    auto phoneNumbers = getPhoneNumbers();
    if (!phoneNumbers.isEmpty())
    {
        return phoneNumbers[0]->getValue();
    }
    return QString();
}

} // namespace data
