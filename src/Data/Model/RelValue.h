#ifndef DATA_RELVALUE_H
#define DATA_RELVALUE_H

#include "Data/Data_global.h"

#include <QString>

namespace data
{

class DATA_EXPORT RelValue
{
public:
    enum ERelValue
    {
        E_REL_VALUE__BEGIN = 0,
            E_REL_VALUE_HOME = E_REL_VALUE__BEGIN,
            E_REL_VALUE_WORK,
            E_REL_VALUE_OTHER,
            E_REL_VALUE_MOBILE,
            E_REL_VALUE_MAIN,
            E_REL_VALUE_FAX,
            E_REL_VALUE_HOME_FAX,
            E_REL_VALUE_WORK_FAX,
        E_REL_VALUE__END
    };

    static QList<ERelValue> getEmailRelValues();
    static QList<ERelValue> getPhoneNumberRelValues();

    static QString getLabel(ERelValue relValue);
    static QString getUrl(ERelValue relValue);

    static QString getLabelFromUrl(const QString& url);
    static QString getUrlFromLabel(const QString& label);

    static ERelValue getRelValueFromUrl(const QString& url);
    static ERelValue getRelValueFromLabel(const QString& label);

private:
    RelValue();
};

} // namespace data

#endif // DATA_RELVALUE_H
