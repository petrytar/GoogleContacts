#include "ContactEntry.h"

#include "Data/debugAsserts.h"

namespace data
{

ContactEntry::ContactEntry(QObject* parent) :
    BaseClass(parent),
    m_data()
{
}

ContactEntry::~ContactEntry()
{
}

QString ContactEntry::getData(const QString& tagName) const
{
    ASSERT(m_data.contains(tagName), "No such data");
    return m_data[tagName];
}

void ContactEntry::setData(const QString& tagName, const QString& tagValue)
{
    m_data[tagName] = tagValue;
}

} // namespace data
