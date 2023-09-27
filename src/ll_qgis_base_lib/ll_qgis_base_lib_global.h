#ifndef LD_LIB_APP_GLOBAL_H
#define LD_LIB_APP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LD_LIB_APP_LIBRARY)
#  define LD_LIB_APP_EXPORT Q_DECL_EXPORT
#else
#  define LD_LIB_APP_EXPORT Q_DECL_IMPORT
#endif

#endif // LD_LIB_APP_GLOBAL_H
