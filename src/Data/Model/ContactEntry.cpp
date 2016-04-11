#include "ContactEntry.h"

#include "Data/debugAsserts.h"
#include <QxMemLeak.h>

QX_REGISTER_COMPLEX_CLASS_NAME_CPP_DATA(data::ContactEntry, data_ContactEntry)

namespace qx {
template <> void register_class(QxClass<data::ContactEntry>& t)
{
    t.setName("ContactEntry");
    t.id(&data::ContactEntry::m_id, "contactEntryId");
    t.data(&data::ContactEntry::m_googleContactId, "googleContactId");
    t.data(&data::ContactEntry::m_updatedTime, "updatedTime");
    t.data(&data::ContactEntry::m_title, "title");
    t.data(&data::ContactEntry::m_content, "content");
    t.data(&data::ContactEntry::m_nickname, "nickname");
    t.data(&data::ContactEntry::m_fileAs, "fileAs");
    t.data(&data::ContactEntry::m_orgName, "orgName");
    t.data(&data::ContactEntry::m_orgTitle, "orgTitle");

    t.relationOneToOne(&data::ContactEntry::m_givenName, "givenNameId");
    t.relationOneToOne(&data::ContactEntry::m_familyName, "familyNameId");

    t.relationOneToMany(&data::ContactEntry::m_emails, "emails", "contactEntryId");
    t.relationOneToMany(&data::ContactEntry::m_ims, "ims", "contactEntryId");
    t.relationOneToMany(&data::ContactEntry::m_phoneNumbers, "phoneNumbers", "contactEntryId");
    t.relationOneToMany(&data::ContactEntry::m_relations, "relations", "contactEntryId");
    t.relationOneToMany(&data::ContactEntry::m_userDefinedFields, "userDefinedFields", "contactEntryId");
    t.relationOneToMany(&data::ContactEntry::m_websites, "websites", "contactEntryId");
    t.relationOneToMany(&data::ContactEntry::m_structuredPostalAddresses, "structuredPostalAddresses", "contactEntryId");
    t.relationOneToMany(&data::ContactEntry::m_groupMembershipInfoList, "groupMembershipInfoList", "contactEntryId");
}}

namespace data
{

ContactEntry::ContactEntry(QObject* parent) :
    BaseClass(parent)
{
}

ContactEntry::~ContactEntry()
{
}

} // namespace data