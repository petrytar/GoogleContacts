#ifndef DATA_DEBUGASSERTS_H
#define DATA_DEBUGASSERTS_H

// Macros that switch ASSERTs on-off was inspired by boost::assert logic
// ASSERTs are possible to completely disable by using the macro DISABLE_ASSERTS. (in debug and releas)

#include "Data_global.h"

#ifndef Q_CC_MSVC
    DATA_EXPORT void switchableAssertGCC(const char* fileName, int lineNumber, const char* assertString);
#endif // !Q_CC_MSVC

#ifndef ASSERT
    inline void emptyFunction() { }
    #ifdef DISABLE_ASSERTS
        #define ASSERT(f, s) ((void)(f, s))
    #else
        #ifdef Q_CC_MSVC
            #include "assert.h"
            // Attention! If NDEBUG is defined, assert(expr) will be rewritted by empty function, so in release mode ASSERTs will not work.
            #define ASSERT(f, s) assert((#f " && " #s))
        #else
            // For gcc we have to have defined our own ASSERT (classicall ASSERT will not work)
            #define ASSERT(f, s) (!(f) ? switchableAssertGCC(__FILE__, __LINE__, #f " && " #s) : emptyFunction())
        #endif // Q_CC_MSVC
    #endif //DISABLE_ASSERTS
#endif //ASSERT


#ifndef STATIC_ASSERT
    #ifndef NDEBUG
        #include "assert.h"
        #define STATIC_ASSERT(f, m) static_assert(f, m)
    #else
        #define STATIC_ASSERT(f, m)
    #endif
#endif

#ifndef VERIFY
    #ifndef NDEBUG
        #define VERIFY(f) ASSERT(f, "")
    #else   // _DEBUG
        #define VERIFY(f) ((void)(f))
    #endif
#endif

#endif // DATA_DEBUGASSERTS_H
