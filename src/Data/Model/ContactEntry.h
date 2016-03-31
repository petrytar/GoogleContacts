#ifndef DATA_CONTACTENTRY_H
#define DATA_CONTACTENTRY_H

#include "Data/Data_global.h"
#include "Data/Model/ContactProperty.h"

#include <QxOrm.h>
#include <QObject>
#include <QMap>

namespace data
{

/**
 * \brief The ContactEntry class.
 *
 * Class that holds all GoogleContact's entry data
 */
class ContactEntry : public QObject
{
    Q_OBJECT
    QX_REGISTER_FRIEND_CLASS(data::ContactEntry)
private:
    typedef QObject BaseClass;

public:
    explicit ContactEntry(QObject* parent = nullptr);
    virtual ~ContactEntry();

    QString getGoogleContactId() const { return m_googleContactId; }
    void setGoogleContactId(const QString& googleContactId) { m_googleContactId = googleContactId; }

    QString getUpdatedTime() const { return m_updatedTime; }
    void setUpdatedTime(const QString& updatedTime) { m_updatedTime = updatedTime; }

    QString getTitle() const { return m_title; }
    void setTitle(const QString& title) { m_title = title; }

    QString getContent() const { return m_content; }
    void setContent(const QString& content) { m_content = content; }

    ContactPropertyPtr getGivenName() const { return m_givenName; }
    void setGivenName(const QString& name,
                      const QString& value,
                      const QString& type) { m_givenName = ContactPropertyPtr(new ContactProperty(name, value, type)); }

    ContactPropertyPtr getFamilyName() const { return m_familyName; }
    void setFamilyName(const QString& name,
                       const QString& value,
                       const QString& type) { m_familyName = ContactPropertyPtr(new ContactProperty(name, value, type)); }

    QString getNickname() const { return m_nickname; }
    void setNickname(const QString& nickname) { m_nickname = nickname; }

    QString getFileAs() const { return m_fileAs; }
    void setFileAs(const QString& fileAs) { m_fileAs = fileAs; }

    QString getOrgName() const { return m_orgName; }
    void setOrgName(const QString& orgName) { m_orgName = orgName; }

    QString getOrgTitle() const { return m_orgTitle; }
    void setOrgTitle(const QString& orgTitle) { m_orgTitle = orgTitle; }

    ContactPropertyList getEmails() const { return m_emails; }
    void addEmail(const QString& name,
                  const QString& value,
                  const QString& type) { m_emails.push_back(ContactPropertyPtr(new ContactProperty(name, value, type))); }

    ContactPropertyList getIms() const { return m_ims; }
    void addIm(const QString& name,
               const QString& value,
               const QString& type) { m_ims.push_back(ContactPropertyPtr(new ContactProperty(name, value, type))); }

    ContactPropertyList getPhoneNumbers() const { return m_phoneNumbers; }
    void addPhoneNumber(const QString& name,
                        const QString& value,
                        const QString& type) { m_phoneNumbers.push_back(ContactPropertyPtr(new ContactProperty(name, value, type))); }

    ContactPropertyList getStructuredPostalAddresses() const { return m_structuredPostalAddresses; }
    void addStructuredPostalAddress(const QString& name,
                                    const QString& value,
                                    const QString& type) { m_structuredPostalAddresses.push_back(ContactPropertyPtr(new ContactProperty(name, value, type))); }

    ContactPropertyList getRelations() const { return m_relations; }
    void addRelation(const QString& name,
                     const QString& value,
                     const QString& type) { m_relations.push_back(ContactPropertyPtr(new ContactProperty(name, value, type))); }

    ContactPropertyList getUserDefinedFields() const { return m_userDefinedFields; }
    void addUserDefinedField(const QString& name,
                             const QString& value,
                             const QString& type) { m_userDefinedFields.push_back(ContactPropertyPtr(new ContactProperty(name, value, type))); }

    ContactPropertyList getWebsites() const { return m_websites; }
    void addWebsite(const QString& name,
                    const QString& value,
                    const QString& type) { m_websites.push_back(ContactPropertyPtr(new ContactProperty(name, value, type))); }

    ContactPropertyList getGroupMembershipInfoList() const { return m_groupMembershipInfoList; }
    void addGroupMembershipInfo(const QString& name,
                                const QString& value,
                                const QString& type) { m_groupMembershipInfoList.push_back(ContactPropertyPtr(new ContactProperty(name, value, type))); }

private:
    long m_id;
    QString m_googleContactId;
    QString m_updatedTime;
    QString m_title;
    QString m_content;
    QString m_nickname;
    QString m_fileAs;
    QString m_orgName;
    QString m_orgTitle;

    ContactPropertyPtr m_givenName;
    ContactPropertyPtr m_familyName;

    ContactPropertyList m_emails;
    ContactPropertyList m_ims;
    ContactPropertyList m_phoneNumbers;
    ContactPropertyList m_structuredPostalAddresses;
    ContactPropertyList m_relations;
    ContactPropertyList m_userDefinedFields;
    ContactPropertyList m_websites;
    ContactPropertyList m_groupMembershipInfoList;
};

} // namespace data

typedef boost::shared_ptr<data::ContactEntry> ContactEntryPtr;

QX_REGISTER_COMPLEX_CLASS_NAME_HPP_DATA(data::ContactEntry, qx::trait::no_base_class_defined, 0, data_ContactEntry)

#endif // DATA_CONTACTENTRY_H
