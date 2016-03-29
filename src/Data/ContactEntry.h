#ifndef DATA_CONTACTENTRY_H
#define DATA_CONTACTENTRY_H

#include <QObject>
#include <QMap>

namespace data
{

class Value;
class Attribute;

/**
 * \brief The ContactEntry class.
 *
 * Class that holds all GoogleContact's entry data
 */
class ContactEntry : public QObject
{
    Q_OBJECT
private:
    typedef QObject BaseClass;
    typedef QMap<int, QList<Value*>> IntToListOfValuesMap;

public:
    explicit ContactEntry(QObject* parent);
    virtual ~ContactEntry();

    QList<Value*> getData(int columnId) const;
    void setData(int columnId, QList<Value*> values);

    // inline implementation
    const IntToListOfValuesMap& getData() const { return m_columnIdToValueMap; }

private:
    IntToListOfValuesMap m_columnIdToValueMap;
};

// class for storing values
class Value
{
public:
    explicit Value(const QString& text, QList<Attribute*> attributes) :
        m_text(text),
        m_attrinutes(attributes)
    {
    }
    virtual ~Value() { }

    QString getText() const { return m_text; }
    QList<Attribute*>& getAttributes() { return m_attrinutes; }

private:
    QString m_text;
    QList<Attribute*> m_attrinutes;
};

// class for storing attributes
class Attribute
{
public:
    explicit Attribute(const QString& attrName, const QString& attrValue) :
        m_attrName(attrName),
        m_attrValue(attrValue)
    {
    }
    virtual ~Attribute() { }

    QString getAttrName() const { return m_attrName; }
    QString getAttrValue() const { return m_attrValue; }

private:
    QString m_attrName;
    QString m_attrValue;
};

} // namespace data

#endif // DATA_CONTACTENTRY_H
