#ifndef DATA_CONTACTENTRY_H
#define DATA_CONTACTENTRY_H

#include "Data/Data_global.h"
#include <QxRegister/QxRegister.h>

class QDomElement;
class QDomNode;

namespace data
{

class ContactProperty;
class User;
class ContactGroup;
typedef ptr<ContactProperty> ContactPropertyPtr;
typedef ptr<ContactGroup> ContactGroupPtr;

/**
 * \brief The ContactEntry class.
 *
 * Class that holds all GoogleContact's entry data
 */
class ContactEntry
{
    QX_REGISTER_FRIEND_CLASS(data::ContactEntry)

public:
    explicit ContactEntry();
    virtual ~ContactEntry();

    void copyFrom(const ContactEntry& other);

    long getId() const { return m_id; }

    void setUser(ptr<User> user) { m_user = user; }

    QString getGoogleId() const { return m_googleId; }
    QString getGoogleShortId() const;
    void setGoogleId(const QString& googleContactId) { m_googleId = googleContactId; }

    QDateTime getUpdatedTime() const { return m_updatedTime; }
    void setUpdatedTime(const QDateTime& updatedTime) { m_updatedTime = updatedTime; }

    QString getName() const { return m_name; }
    void setName(const QString& name) { m_name = name; }

    QString getNickname() const { return m_nickname; }
    void setNickname(const QString& nickname) { m_nickname = nickname; }

    QString getFileAs() const { return m_fileAs; }
    void setFileAs(const QString& fileAs) { m_fileAs = fileAs; }

    QString getOrgName() const { return m_orgName; }
    void setOrgName(const QString& orgName) { m_orgName = orgName; }

    QString getOrgTitle() const { return m_orgTitle; }
    void setOrgTitle(const QString& orgTitle) { m_orgTitle = orgTitle; }

    void setProperties(QList<ContactPropertyPtr> properties) { m_properties = properties; }

    QList<ContactPropertyPtr> getEmails() const;
    void addEmail(ContactPropertyPtr email) { m_properties.push_back(email); }

    QList<ContactPropertyPtr> getPhoneNumbers() const;
    void addPhoneNumber(ContactPropertyPtr phoneNumber) { m_properties.push_back(phoneNumber); }

    const QList<ContactGroupPtr>& getContactGroups() const { return m_contactGroups; }
    QList<ContactGroupPtr>& getContactGroups() { return m_contactGroups; }
    void addContactGroup(ContactGroupPtr contactGroup) { m_contactGroups.push_back(contactGroup); }
    bool removeContactGroup(ContactGroupPtr contactGroup);
    void setContactGroups(QList<ContactGroupPtr> groups) { m_contactGroups = groups; }

    QString getVisibleName() const;
    QString getPrimaryEmail() const;
    QString getPrimaryPhoneNumber() const;

    bool isDeleted() const { return m_deleted; }
    void setDeleted(bool deleted) { m_deleted = deleted; }

    static void fromXml(ptr<ContactEntry> contactEntry, QList<ContactGroupPtr> contactGroups, const QDomElement& contactEntryElement);
    QString toXml();
    void toXmlDom(QDomNode& parentNode);

private:
    long m_id;
    ptr<User> m_user;
    QString m_googleId;
    QDateTime m_updatedTime;
    QString m_name;
    QString m_nickname;
    QString m_fileAs;
    QString m_orgName;
    QString m_orgTitle;

    QList<ContactPropertyPtr> m_properties;

    QList<ContactGroupPtr> m_contactGroups;

    bool m_deleted;
};

} // namespace data

typedef data::ptr<data::ContactEntry> ContactEntryPtr;
Q_DECLARE_METATYPE(data::ptr<data::ContactEntry>)

QX_REGISTER_COMPLEX_CLASS_NAME_HPP_DATA(data::ContactEntry, qx::trait::no_base_class_defined, 0, data_ContactEntry)

#endif // DATA_CONTACTENTRY_H
