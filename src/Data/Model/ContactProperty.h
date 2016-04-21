#ifndef DATA_CONTACTPROPERTY_H
#define DATA_CONTACTPROPERTY_H

#include "Data/Data_global.h"
#include <QxRegister/QxRegister.h>

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
    enum EType
    {
        E_TYPE__BEGIN,
            E_TYPE_EMAIL,
            E_TYPE_PHONE_NUMBER,
        E_TYPE__END
    };

    ContactProperty();
    explicit ContactProperty(ptr<ContactEntry> contactEntry, const QString& label, const QString& value, EType type);
    virtual ~ContactProperty();

    QString getLabel() const { return m_label; }
    void setLabel(const QString& label) { m_label = label; }

    QString getValue() const { return m_value; }
    void setValue(const QString& value) { m_value = value; }

    EType getType() const;
    void setType(EType type);

private:
    long m_id;
    QString m_label;
    QString m_value;
    QString m_type;
    ptr<ContactEntry> m_contactEntry;
};

} // namespace data


typedef data::ptr<data::ContactProperty> ContactPropertyPtr;
Q_DECLARE_METATYPE(data::ptr<data::ContactProperty>)

QX_REGISTER_COMPLEX_CLASS_NAME_HPP_DATA(data::ContactProperty, qx::trait::no_base_class_defined, 0, data_ContactProperty)

#endif // DATA_CONTACTPROPERTY_H
