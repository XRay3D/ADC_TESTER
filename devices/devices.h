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

    //    struct TD {
    //        //        TD(QObject* const object)
    //        //            : object { object }
    //        TD& operator=(QObject* object)
    //        {
    //            qDebug(__FUNCTION__);
    //            object->moveToThread(&thread);
    //            thread.connect(&thread, &QThread::finished, object, &QObject::deleteLater);
    //            thread.start(QThread::NormalPriority);
    //            return (*this);
    //        }
    //        //        ~TD()
    //        //        {
    //        //            qDebug(__FUNCTION__);
    //        //            thread.quit();
    //        //            thread.wait();
    //        //        }

    //        template <class T>
    //        operator T*() { return reinterpret_cast<T*>(object); }

    //        QObject* object;
    //        QThread thread;
    //    };

    static inline QSemaphore semafore;
    //    static inline TD devices[Count]; // = { new Irt5501, new Irt5920, new Irt5920, new Tester };
    static inline QThread threads[Count]; // = { new Irt5501, new Irt5920, new Irt5920, new Tester };

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
