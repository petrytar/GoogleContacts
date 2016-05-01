#include "ContactEntry.h"

#include "Data/debugAsserts.h"
#include "Data/Model/ContactProperty.h"
#include "Data/Model/ContactGroup.h"
#include "Data/Model/User.h"
#include "Data/Model/RelValue.h"

#include <QxRegister/QxClass.h>
#include <QxDao/IxPersistable.h>
#include <QxDao/QxDao_Impl.h>
#include <QxConvert/QxConvert.h>
#include <QxConvert/QxConvert_Impl.h>
#include <QxMemLeak.h>

#include <QDomDocument>

QX_REGISTER_COMPLEX_CLASS_NAME_CPP_DATA(data::ContactEntry, data_ContactEntry)

namespace qx {
template <> void register_class(QxClass<data::ContactEntry>& t)
{
    t.setName("ContactEntry");
    t.id(&data::ContactEntry::m_id, "contactEntryId");
    t.relationManyToOne(&data::ContactEntry::m_user, "userId");
    t.data(&data::ContactEntry::m_googleId, "googleId");
    t.data(&data::ContactEntry::m_updatedTime, "updatedTime");
    t.data(&data::ContactEntry::m_name, "name");
    t.data(&data::ContactEntry::m_nickname, "nickname");
    t.data(&data::ContactEntry::m_fileAs, "fileAs");
    t.data(&data::ContactEntry::m_orgName, "orgName");
    t.data(&data::ContactEntry::m_orgTitle, "orgTitle");

    t.relationOneToMany(&data::ContactEntry::m_properties, "properties", "contactEntryId");

    t.relationManyToMany(&data::ContactEntry::m_contactGroups, "list_groups", "ContactGroup_ContactEntry", "entry_id", "group_id");

    t.data(&data::ContactEntry::m_deleted, "deleted");
}}

namespace data
{

ContactEntry::ContactEntry() :
    m_id(0),
    m_deleted(false)
{
}

ContactEntry::~ContactEntry()
{
}

void ContactEntry::copyFrom(const ContactEntry& other)
{
    m_googleId = other.m_googleId;
    m_updatedTime = other.m_updatedTime;
    m_name = other.m_name;
    m_nickname = other.m_nickname;
    m_fileAs = other.m_fileAs;
    m_orgName = other.m_orgName;
    m_orgTitle = other.m_orgTitle;

    m_properties = other.m_properties;

    //m_deleted = other.m_deleted;  //deleted must state deleted!
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

void ContactEntry::fromXml(ptr<ContactEntry> contactEntry, QList<ptr<ContactGroup>> contactGroups, const QDomElement& contactEntryElement)
{
    auto createProperty = [](ptr<ContactEntry> parentEntry, QDomNode node, const QString& valueAttribute, ContactProperty::EType type) -> ContactPropertyPtr
    {
        QDomElement element = node.toElement();
        QString label = element.attribute("rel");
        if (label.isEmpty())
        {
            label = element.attribute("label");
        }
        ptr<ContactProperty> property(new ContactProperty(parentEntry, label, element.attribute(valueAttribute), type));
        return property;
    };

    QDomNodeList googleIdDomNodeList = contactEntryElement.elementsByTagName("id");
    if (!googleIdDomNodeList.isEmpty())
    {
        contactEntry->setGoogleId(googleIdDomNodeList.at(0).toElement().text());
    }

    QDomNodeList updatedTimeDomNodeList = contactEntryElement.elementsByTagName("updated");
    if (!updatedTimeDomNodeList.isEmpty())
    {
        QString timeText = updatedTimeDomNodeList.at(0).toElement().text();
        QDateTime dateTime = QDateTime::fromString(timeText, "yyyy-MM-ddThh:mm:ss.zzzZ");
        contactEntry->setUpdatedTime(dateTime);
    }

    QDomNodeList nicknameDomNodeList = contactEntryElement.elementsByTagName("nickname");
    if (!nicknameDomNodeList.isEmpty())
    {
        contactEntry->setNickname(nicknameDomNodeList.at(0).toElement().text());
    }

    QDomNodeList fileAsDomNodeList = contactEntryElement.elementsByTagName("fileAs");
    if (!fileAsDomNodeList.isEmpty())
    {
        contactEntry->setFileAs(fileAsDomNodeList.at(0).toElement().text());
    }

    QDomNodeList orgNameDomNodeList = contactEntryElement.elementsByTagName("orgName");
    if (!orgNameDomNodeList.isEmpty())
    {
        contactEntry->setOrgName(orgNameDomNodeList.at(0).toElement().text());
    }

    QDomNodeList orgTitleDomNodeList = contactEntryElement.elementsByTagName("orgTitle");
    if (!orgTitleDomNodeList.isEmpty())
    {
        contactEntry->setOrgTitle(orgTitleDomNodeList.at(0).toElement().text());
    }

    QDomNodeList nameDomNodeList = contactEntryElement.elementsByTagName("name");
    if (!nameDomNodeList.isEmpty())
    {
        QDomNodeList fullNameDomNodeList = contactEntryElement.elementsByTagName("fullName");
        if (!fullNameDomNodeList.isEmpty())
        {
            contactEntry->setName(fullNameDomNodeList.at(0).toElement().text());
        }
    }

    QDomNodeList emailDomNodeList = contactEntryElement.elementsByTagName("email");
    for (int i = 0; i < emailDomNodeList.size(); ++i)
    {
        contactEntry->addEmail(createProperty(contactEntry, emailDomNodeList.at(i), "address", ContactProperty::E_TYPE_EMAIL));
    }

    QDomNodeList phoneNumberDomNodeList = contactEntryElement.elementsByTagName("phoneNumber");
    for (int i = 0; i < phoneNumberDomNodeList.size(); ++i)
    {
        QDomNode phoneNumberNode = phoneNumberDomNodeList.at(i);
        QDomElement phoneNumberElement = phoneNumberNode.toElement();
        ContactPropertyPtr contactProperty = createProperty(contactEntry, phoneNumberNode, "uri", ContactProperty::E_TYPE_PHONE_NUMBER);
        contactEntry->addPhoneNumber(contactProperty);
        if (phoneNumberElement.attribute("uri").isEmpty())
        {
            contactProperty->setValue(phoneNumberElement.text());
        }
    }

    QDomNodeList groupDomNodeList = contactEntryElement.elementsByTagName("groupMembershipInfo");
    for (int i = 0; i < groupDomNodeList.size(); ++i)
    {
        QDomNode groupNode = groupDomNodeList.at(i);
        QDomElement groupElement = groupNode.toElement();
        QString groupId = groupElement.attribute("href");
        for (auto contactGroup : contactGroups)
        {
            if (contactGroup->getGoogleId() == groupId)
            {
                contactEntry->addContactGroup(contactGroup);
                break;
            }
        }
    }
}

QString ContactEntry::toXml()
{
    QDomDocument document;
    document.appendChild(document.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));

    QDomElement entryElement = document.createElement("entry");
    document.appendChild(entryElement);
    entryElement.setAttribute("xmlns", "http://www.w3.org/2005/Atom");
    entryElement.setAttribute("xmlns:gd", "http://schemas.google.com/g/2005");
    entryElement.setAttribute("xmlns:gContact", "http://schemas.google.com/contact/2008");

    QDomElement categoryElement = document.createElement("category");
    entryElement.appendChild(categoryElement);
    categoryElement.setAttribute("scheme", "http://schemas.google.com/g/2005#kind");
    categoryElement.setAttribute("term", "http://schemas.google.com/contact/2008#contact");

    QDomElement nameElement = document.createElement("gd:name");
    entryElement.appendChild(nameElement);
    QDomElement fullNameElement = document.createElement("gd:fullName");
    nameElement.appendChild(fullNameElement);
    fullNameElement.appendChild(document.createTextNode(m_name));

    QDomElement nicknameElement = document.createElement("gContact:nickname");
    entryElement.appendChild(nicknameElement);
    nicknameElement.appendChild(document.createTextNode(m_nickname));

    QDomElement fileAsElement = document.createElement("gContact:fileAs");
    entryElement.appendChild(fileAsElement);
    fileAsElement.appendChild(document.createTextNode(m_fileAs));

    QDomElement organizationElement = document.createElement("gd:organization");
    entryElement.appendChild(organizationElement);
    organizationElement.setAttribute("rel", "http://schemas.google.com/g/2005#other");
    QDomElement orgNameElement = document.createElement("gd:orgName");
    organizationElement.appendChild(orgNameElement);
    orgNameElement.appendChild(document.createTextNode(m_orgName));
    QDomElement orgTitleElement = document.createElement("gd:orgTitle");
    organizationElement.appendChild(orgTitleElement);
    orgTitleElement.appendChild(document.createTextNode(m_orgTitle));

    for (ContactPropertyPtr contactProperty : m_properties)
    {
        QString elementName;
        QString valueAttributeName;
        switch (contactProperty->getType())
        {
            case ContactProperty::E_TYPE_EMAIL:
            {
                elementName = "gd:email";
                valueAttributeName = "address";
                break;
            }
            case ContactProperty::E_TYPE_PHONE_NUMBER:
            {
                elementName = "gd:phoneNumber";
                break;
            }
            default:
            {
                continue;
            }
        }

        QDomElement propertyElement = document.createElement(elementName);
        entryElement.appendChild(propertyElement);
        if (!valueAttributeName.isEmpty())
        {
            propertyElement.setAttribute(valueAttributeName, contactProperty->getValue());
        }
        else
        {
            propertyElement.appendChild(document.createTextNode(contactProperty->getValue()));
        }

        RelValue::ERelValue relValue = RelValue::getRelValueFromUrl(contactProperty->getLabel());
        if (relValue != RelValue::E_REL_VALUE__END)
        {
            propertyElement.setAttribute("rel", contactProperty->getLabel());
        }
        else
        {
            propertyElement.setAttribute("label", contactProperty->getLabel());
        }
    }

    for (ContactGroupPtr contactGroup : m_contactGroups)
    {
        QDomElement groupElement = document.createElement("gContact:groupMembershipInfo");
        entryElement.appendChild(groupElement);
        groupElement.setAttribute("deleted", "false");
        groupElement.setAttribute("href", contactGroup->getGoogleId());
    }

    return document.toString(2);
}

QString ContactEntry::getGoogleShortId() const
{
    int indexOfSlash = m_googleId.lastIndexOf("/");
    if (indexOfSlash < 0)
    {
        return QString();
    }
    return m_googleId.mid(indexOfSlash + 1);
}

bool ContactEntry::removeContactGroup(ContactGroupPtr contactGroup)
{
    return m_contactGroups.removeOne(contactGroup);
}

} // namespace data
