#include "RelValue.h"

#include <QMap>

namespace data
{

struct RelValueData
{
    RelValueData(const QString& url = QString(), const QString& label = QString()) :
        url(url),
        label(label)
    {}

    QString url;
    QString label;
};

static QMap<RelValue::ERelValue, RelValueData> initRelValues()
{
    QMap<RelValue::ERelValue, RelValueData> relValues;
    relValues.insert(RelValue::E_REL_VALUE_HOME, RelValueData("http://schemas.google.com/g/2005#home", "Home"));
    relValues.insert(RelValue::E_REL_VALUE_WORK, RelValueData("http://schemas.google.com/g/2005#work", "Work"));
    relValues.insert(RelValue::E_REL_VALUE_OTHER, RelValueData("http://schemas.google.com/g/2005#other", "Other"));
    relValues.insert(RelValue::E_REL_VALUE_FAX, RelValueData("http://schemas.google.com/g/2005#fax", "Fax"));
    relValues.insert(RelValue::E_REL_VALUE_HOME_FAX, RelValueData("http://schemas.google.com/g/2005#home_fax", "Home Fax"));
    relValues.insert(RelValue::E_REL_VALUE_WORK_FAX, RelValueData("http://schemas.google.com/g/2005#work_fax", "Work Fax"));
    relValues.insert(RelValue::E_REL_VALUE_MOBILE, RelValueData("http://schemas.google.com/g/2005#mobile", "Mobile"));
    relValues.insert(RelValue::E_REL_VALUE_MAIN, RelValueData("http://schemas.google.com/g/2005#main", "Main"));
    return relValues;
}

static QMap<RelValue::ERelValue, RelValueData> REL_VALUES = initRelValues();

RelValue::RelValue()
{
}

QString RelValue::getLabel(ERelValue relValue)
{
    return REL_VALUES.value(relValue).label;
}

QString RelValue::getUrl(ERelValue relValue)
{
    return REL_VALUES.value(relValue).url;
}

QList<RelValue::ERelValue> RelValue::getEmailRelValues()
{
    return QList<ERelValue>() << E_REL_VALUE_HOME
                              << E_REL_VALUE_WORK
                              << E_REL_VALUE_OTHER;
}

QList<RelValue::ERelValue> RelValue::getPhoneNumberRelValues()
{
    return QList<ERelValue>() << E_REL_VALUE_HOME
                              << E_REL_VALUE_WORK
                              << E_REL_VALUE_OTHER
                              << E_REL_VALUE_MOBILE
                              << E_REL_VALUE_MAIN
                              << E_REL_VALUE_FAX
                              << E_REL_VALUE_HOME_FAX
                              << E_REL_VALUE_WORK_FAX;
}

QString RelValue::getLabelFromUrl(const QString& url)
{
    return getLabel(getRelValueFromUrl(url));
}

QString RelValue::getUrlFromLabel(const QString& label)
{
    return getUrl(getRelValueFromLabel(label));
}

RelValue::ERelValue RelValue::getRelValueFromUrl(const QString& url)
{
    for (auto it = REL_VALUES.begin(); it != REL_VALUES.end(); ++it)
    {
        if (it.value().url == url)
        {
            return it.key();
        }
    }

    return E_REL_VALUE__END;
}

RelValue::ERelValue RelValue::getRelValueFromLabel(const QString& label)
{
    for (auto it = REL_VALUES.begin(); it != REL_VALUES.end(); ++it)
    {
        if (it.value().label == label)
        {
            return it.key();
        }
    }

    return E_REL_VALUE__END;
}

} // namespace data

