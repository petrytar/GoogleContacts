#include "ContactEntry.h"

#include "Data/debugAsserts.h"

namespace data
{

ContactEntry::ContactEntry(QObject* parent) :
    BaseClass(parent),
    m_columnIdToValueMap()
{
}

ContactEntry::~ContactEntry()
{
}

QList<Value*> ContactEntry::getData(int columnId) const
{
    if (m_columnIdToValueMap.contains(columnId))
    {
        return m_columnIdToValueMap[columnId];
    }
    return QList<Value*>() << new Value(QString(), QList<Attribute*>());
}

void ContactEntry::setData(int columnId, QList<Value*> values)
{
    m_columnIdToValueMap[columnId] = values;
}

} // namespace data
