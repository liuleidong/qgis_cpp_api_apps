#ifndef LD_LIB_SINGLETON_H
#define LD_LIB_SINGLETON_H

#include <QMutex>
#include <QScopedPointer>

#define DECLARE_SINGLETON(Class) \
Q_DISABLE_COPY(Class) \
public: \
    static Class* Instance() \
    { \
        static QMutex mutex; \
        static QScopedPointer<Class> inst; \
        if (Q_UNLIKELY(!inst)) { \
            mutex.lock(); \
            if (!inst) inst.reset(new Class); \
            mutex.unlock(); \
        } \
        return inst.data(); \
    }


#endif // LD_LIB_SINGLETON_H
