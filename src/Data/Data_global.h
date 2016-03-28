#ifndef DATA_DATA_GLOBAL_H
#define DATA_DATA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DATA_LIBRARY)
#  define DATA_EXPORT Q_DECL_EXPORT
#else
#  define DATA_EXPORT Q_DECL_IMPORT
#endif

#endif // DATA_DATA_GLOBAL_H
