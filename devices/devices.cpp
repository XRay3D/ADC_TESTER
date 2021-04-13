#include "devices.h"

Devices::Devices() {
    if(!semafore.available()) {
        QObject* objects[]{
            m_irt1 = new Irt5501,
            m_irt2 = new Irt5920,
            m_irt3 = new Irt5920,
            m_tester = new Tester,
        };
        for(int i = 0; i < 4; ++i) {
            objects[i]->moveToThread(&threads[i]);
            threads[i].connect(&threads[i], &QThread::finished, objects[i], &QObject::deleteLater);
            threads[i].start(QThread::NormalPriority);
        }
    }
    semafore.release();
}

Devices::~Devices() {
    semafore.acquire();
    if(!semafore.available()) {
        for(auto& thr : threads) {
            thr.quit();
            thr.wait();
        }
        m_irt1 = nullptr;
        m_irt2 = nullptr;
        m_irt3 = nullptr;
        m_tester = nullptr;
    }
}

Irt5501* Devices::irtAdc() { return m_irt1; }
Irt5920* Devices::irtI() { return m_irt2; }
Irt5920* Devices::irtU() { return m_irt3; }
Tester* Devices::tester() { return m_tester; }
