#pragma once

#include "irt5501.h"
#include "irt5920.h"
#include "tester.h"

class Devices {
    enum {
        Irt1,
        Irt2,
        Irt3,
        Tester_,
        Count
    };

    static inline QSemaphore semafore;
    static inline QThread threads[Count];

    static inline Irt5501* m_irt1 = nullptr;
    static inline Irt5920* m_irt2 = nullptr;
    static inline Irt5920* m_irt3 = nullptr;
    static inline Tester* m_tester = nullptr;

public:
    Devices();
    ~Devices();

    static Irt5501* irtAdc();
    static Irt5920* irtI();
    static Irt5920* irtU();
    static Tester* tester();
};
