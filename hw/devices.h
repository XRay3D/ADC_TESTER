#pragma once

#include "irt5501.h"
#include "irt5920.h"
#include "tester.h"

class mi {
    static inline QSemaphore semafore;
    static inline QThread thread[4];

    static inline Irt5501* m_irt1 = nullptr;
    static inline Irt5920* m_irt2 = nullptr;
    static inline Irt5920* m_irt3 = nullptr;

    static inline Tester* m_tester = nullptr;

public:
    mi();
    ~mi();

    static auto irtAdc() { return m_irt1; }
    static auto irtI() { return m_irt2; }
    static auto irtU() { return m_irt3; }

    static auto tester() { return m_tester; }
};
