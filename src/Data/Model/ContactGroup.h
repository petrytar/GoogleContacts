#ifndef DATA_CONTACTGROUP_H
#define DATA_CONTACTGROUP_H

#include "Data/Data_global.h"
#include <QxRegister/QxRegister.h>

class QDomElement;

namespace data
{
class User;

class ContactGroup
{
    QX_REGISTER_FRIEND_CLASS(data::ContactGroup)

public:
    explicit ContactGroup();
    virtual ~ContactGroup();

    void copyFrom(const ContactGroup& other);

    void setUser(ptr<User> user) { m_user = user; }

    QString getGoogleId() const { return m_googleId; }
    QString getGoogleShortId() const;
    void setGoogleId(const QString& googleId) { m_googleId = googleId; }

    QDateTime getUpdatedTime() const { return m_updatedTime; }
    void setUpdatedTime(const QDateTime& updatedTime) { m_updatedTime = updatedTime; }

    QString getTitle() const { return m_title; }
    void setTitle(const QString& title) { m_title = title; }

    bool isSystemGroup() const { return m_isSystemGroup; }
    void setSystemGroup(bool isSystemGroup) { m_isSystemGroup = isSystemGroup; }

    bool isDeleted() const { return m_deleted; }
    void setDeleted(bool deleted) { m_deleted = deleted; }

    static void fromXml(ptr<ContactGroup> contactGroup, const QDomElement& contactGroupElement);

private:
    long m_id;
    ptr<User> m_user;
    QString m_googleId;
    QDateTime m_updatedTime;
    QString m_title;
    bool m_isSystemGroup;

    bool m_deleted;
};

} // namespace data

typedef data::ptr<data::ContactGroup> ContactGroupPtr;
Q_DECLARE_METATYPE(data::ptr<data::ContactGroup>)

QX_REGISTER_COMPLEX_CLASS_NAME_HPP_DATA(data::ContactGroup, qx::trait::no_base_class_defined, 0, data_ContactGroup)

#endif // DATA_CONTACTGROUP_H
