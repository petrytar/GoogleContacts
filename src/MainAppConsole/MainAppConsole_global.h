#ifndef MAINAPPCONSOLE_GLOBAL
#define MAINAPPCONSOLE_GLOBAL

#include "Data/Data_global.h"
#include "Data/debugAsserts.h"

#include <QTextStream>

inline QTextStream& qStdOut()
{
    static QTextStream ts(stdout);
    return ts;
}

inline QTextStream& qStdIn()
{
    static QTextStream ts(stdin);
    return ts;
}

#endif // MAINAPPCONSOLE_GLOBAL

