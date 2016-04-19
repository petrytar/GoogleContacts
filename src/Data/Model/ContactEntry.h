#ifndef DATA_CONTACTENTRY_H
#define DATA_CONTACTENTRY_H

#include "Data/Data_global.h"
#include <QxRegister/QxRegister.h>

namespace data
{

class ContactProperty;
class User;
typedef ptr<ContactProperty> ContactPropertyPtr;

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

    QString getGoogleContactId() const { return m_googleContactId; }
    void setGoogleContactId(const QString& googleContactId) { m_googleContactId = googleContactId; }

    QDateTime getUpdatedTime() const { return m_updatedTime; }
    void setUpdatedTime(const QDateTime& updatedTime) { m_updatedTime = updatedTime; }

    QString getTitle() const { return m_title; }
    void setTitle(const QString& title) { m_title = title; }

    QString getName() const { return m_name; }
    void setName(const QString& name) { m_name = name; }

    QString getContent() const { return m_content; }
    void setContent(const QString& content) { m_content = content; }

    /*ContactPropertyPtr getGivenName() const { return m_givenName; }
    void setGivenName(const QString& name,
                      const QString& value,
                      const QString& type) { m_givenName = ContactPropertyPtr(new ContactProperty(name, value, type)); }

    ContactPropertyPtr getFamilyName() const { return m_familyName; }
    void setFamilyName(const QString& name,
                       const QString& value,
                       const QString& type) { m_familyName = ContactPropertyPtr(new ContactProperty(name, value, type)); }*/

    QString getNickname() const { return m_nickname; }
    void setNickname(const QString& nickname) { m_nickname = nickname; }

    QString getFileAs() const { return m_fileAs; }
    void setFileAs(const QString& fileAs) { m_fileAs = fileAs; }

    QString getOrgName() const { return m_orgName; }
    void setOrgName(const QString& orgName) { m_orgName = orgName; }

    QString getOrgTitle() const { return m_orgTitle; }
    void setOrgTitle(const QString& orgTitle) { m_orgTitle = orgTitle; }

    QList<ContactPropertyPtr> getEmails() const { return m_emails; }
    void addEmail(ContactPropertyPtr email) { m_emails.push_back(email); }

    QList<ContactPropertyPtr> getPhoneNumbers() const { return m_phoneNumbers; }
    void addPhoneNumber(ContactPropertyPtr phoneNumber) { m_phoneNumbers.push_back(phoneNumber); }
    void setPhoneNumbers(QList<ContactPropertyPtr> phoneNumbers) { m_phoneNumbers = phoneNumbers; }

    /*QList<ContactPropertyPtr> getIms() const { return m_ims; }
    void addIm(const QString& name,
               const QString& value,
               const QString& type) { m_ims.push_back(ContactPropertyPtr(new ContactProperty(name, value, type))); }

    QList<ContactPropertyPtr> getStructuredPostalAddresses() const { return m_structuredPostalAddresses; }
    void addStructuredPostalAddress(const QString& name,
                                    const QString& value,
                                    const QString& type) { m_structuredPostalAddresses.push_back(ContactPropertyPtr(new ContactProperty(name, value, type))); }

    QList<ContactPropertyPtr> getRelations() const { return m_relations; }
    void addRelation(const QString& name,
                     const QString& value,
                     const QString& type) { m_relations.push_back(ContactPropertyPtr(new ContactProperty(name, value, type))); }

    QList<ContactPropertyPtr> getUserDefinedFields() const { return m_userDefinedFields; }
    void addUserDefinedField(const QString& name,
                             const QString& value,
                             const QString& type) { m_userDefinedFields.push_back(ContactPropertyPtr(new ContactProperty(name, value, type))); }

    QList<ContactPropertyPtr> getWebsites() const { return m_websites; }
    void addWebsite(const QString& name,
                    const QString& value,
                    const QString& type) { m_websites.push_back(ContactPropertyPtr(new ContactProperty(name, value, type))); }

    QList<ContactPropertyPtr> getGroupMembershipInfoList() const { return m_groupMembershipInfoList; }
    void addGroupMembershipInfo(const QString& name,
                                const QString& value,
                                const QString& type) { m_groupMembershipInfoList.push_back(ContactPropertyPtr(new ContactProperty(name, value, type))); }*/

    QString getVisibleName() const;
    QString getPrimaryEmail() const;
    QString getPrimaryPhoneNumber() const;

private:
    long m_id;
    ptr<User> m_user;
    QString m_googleContactId;
    QDateTime m_updatedTime;
    QString m_title;
    QString m_name;
    QString m_content;
    QString m_nickname;
    QString m_fileAs;
    QString m_orgName;
    QString m_orgTitle;

    /*ContactPropertyPtr m_givenName;
    ContactPropertyPtr m_familyName;*/

    QList<ContactPropertyPtr> m_emails;
    QList<ContactPropertyPtr> m_phoneNumbers;
    /*QList<ContactPropertyPtr> m_ims;
    QList<ContactPropertyPtr> m_structuredPostalAddresses;
    QList<ContactPropertyPtr> m_relations;
    QList<ContactPropertyPtr> m_userDefinedFields;
    QList<ContactPropertyPtr> m_websites;
    QList<ContactPropertyPtr> m_groupMembershipInfoList;*/
};

} // namespace data

typedef data::ptr<data::ContactEntry> ContactEntryPtr;
Q_DECLARE_METATYPE(data::ptr<data::ContactEntry>)

QX_REGISTER_COMPLEX_CLASS_NAME_HPP_DATA(data::ContactEntry, qx::trait::no_base_class_defined, 0, data_ContactEntry)

#endif // DATA_CONTACTENTRY_H
