#include "ContactGroup.h"

#include "Data/Model/User.h"

#include <QxRegister/QxClass.h>
#include <QxDao/IxPersistable.h>
#include <QxDao/QxDao_Impl.h>
#include <QxConvert/QxConvert.h>
#include <QxConvert/QxConvert_Impl.h>
#include <QxMemLeak.h>

#include <QDomElement>

QX_REGISTER_COMPLEX_CLASS_NAME_CPP_DATA(data::ContactGroup, data_ContactGroup)

namespace qx {
template <> void register_class(QxClass<data::ContactGroup>& t)
{
    t.setName("ContactGroup");
    t.id(&data::ContactGroup::m_id, "contactGroupId");
    t.relationManyToOne(&data::ContactGroup::m_user, "userId");
    t.data(&data::ContactGroup::m_googleId, "googleId");
    t.data(&data::ContactGroup::m_updatedTime, "updatedTime");
    t.data(&data::ContactGroup::m_title, "title");
    t.data(&data::ContactGroup::m_isSystemGroup, "isSystemGroup");

    t.data(&data::ContactGroup::m_deleted, "deleted");
}}

namespace data
{

ContactGroup::ContactGroup() :
    m_id(0),
    m_deleted(false)
{
}

ContactGroup::~ContactGroup()
{
}

void ContactGroup::copyFrom(const ContactGroup& other)
{
    m_googleId = other.m_googleId;
    m_updatedTime = other.m_updatedTime;
    m_title = other.m_title;
    m_isSystemGroup = other.m_isSystemGroup;

    //m_deleted = other.m_deleted;  //deleted must state deleted!
}

QString ContactGroup::getGoogleShortId() const
{
    int indexOfSlash = m_googleId.lastIndexOf("/");
    if (indexOfSlash < 0)
    {
        return QString();
    }
    return m_googleId.mid(indexOfSlash + 1);
}

void ContactGroup::fromXml(ptr<ContactGroup> contactGroup, const QDomElement& contactGroupElement)
{
    QDomNodeList googleIdDomNodeList = contactGroupElement.elementsByTagName("id");
    if (!googleIdDomNodeList.isEmpty())
    {
        contactGroup->setGoogleId(googleIdDomNodeList.at(0).toElement().text());
    }

    QDomNodeList updatedTimeDomNodeList = contactGroupElement.elementsByTagName("updated");
    if (!updatedTimeDomNodeList.isEmpty())
    {
        QString timeText = updatedTimeDomNodeList.at(0).toElement().text();
        QDateTime dateTime = QDateTime::fromString(timeText, "yyyy-MM-ddThh:mm:ss.zzzZ");
        contactGroup->setUpdatedTime(dateTime);
    }

    QDomNodeList titleDomNodeList = contactGroupElement.elementsByTagName("title");
    if (!titleDomNodeList.isEmpty())
    {
        contactGroup->setTitle(titleDomNodeList.at(0).toElement().text());
    }

    QDomNodeList systemGroupNodeList = contactGroupElement.elementsByTagName("systemGroup");
    contactGroup->setSystemGroup(!systemGroupNodeList.isEmpty());
}

} // namespace data
