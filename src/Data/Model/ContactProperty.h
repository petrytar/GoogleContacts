#ifndef DATA_CONTACTPROPERTY_H
#define DATA_CONTACTPROPERTY_H

#include "Data/Data_global.h"
//#include "Data/Model/ContactEntry.h"

#include <QxOrm.h>
#include <QString>

namespace data
{

class ContactEntry;

/**
 * \brief The ContactProperty class.
 *
 * Add some description here.
 */
class ContactProperty
{
    QX_REGISTER_FRIEND_CLASS(data::ContactProperty)
public:
    typedef boost::shared_ptr<ContactEntry> ContactEntryPtr;

    explicit ContactProperty(const QString& name = "", const QString& value = "", const QString& type = "");
    virtual ~ContactProperty();

    QString getName() const { return m_name; }
    void setName(const QString& name) { m_name = name; }

    QString getValue() const { return m_value; }
    void setValue(const QString& value) { m_value = value; }

    QString getType() const { return m_type; }
    void setType(const QString& type) { m_type = type; }

    void setContactEntry(ContactEntryPtr contactEntryPtr) { m_contactEntryPtr = contactEntryPtr; }

private:
    long m_id;
    QString m_name;
    QString m_value;
    QString m_type;
    ContactEntryPtr m_contactEntryPtr;
};

} // namespace data


typedef boost::shared_ptr<data::ContactProperty> ContactPropertyPtr;
typedef QList<ContactPropertyPtr> ContactPropertyList;

QX_REGISTER_COMPLEX_CLASS_NAME_HPP_DATA(data::ContactProperty, qx::trait::no_base_class_defined, 0, data_ContactProperty)

#endif // DATA_CONTACTPROPERTY_H
