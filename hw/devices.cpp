#include "interface.h"

mi::mi()
{
    if (!semafore.available()) {
        m_irt1 = new Irt5501;
        m_irt2 = new Irt5920;
        m_irt3 = new Irt5920;
        m_tester = new Tester;

        m_irt1->moveToThread(&thread[1]);
        m_irt2->moveToThread(&thread[2]);
        m_irt3->moveToThread(&thread[3]);
        m_tester->moveToThread(&thread[0]);

        thread[1].connect(&thread[1], &QThread::finished, m_irt1, &QObject::deleteLater);
        thread[2].connect(&thread[2], &QThread::finished, m_irt2, &QObject::deleteLater);
        thread[3].connect(&thread[3], &QThread::finished, m_irt3, &QObject::deleteLater);
        thread[0].connect(&thread[0], &QThread::finished, m_tester, &QObject::deleteLater);

        for (auto& thr : thread)
            thr.start(QThread::NormalPriority);
    }
    semafore.release();
}

mi::~mi()
{
    semafore.acquire();
    if (!semafore.available()) {
        for (auto& thr : thread) {
            thr.quit();
            thr.wait();
        }
        m_irt1 = nullptr;
        m_irt2 = nullptr;
        m_irt3 = nullptr;
        m_tester = nullptr;
    }
}
