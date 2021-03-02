#include "interface.h"

mi::mi()
{
    if (!semafore.available()) {
        m_irt1 = new Irt5501;
        m_irt2 = new Irt5920;
        m_irt3 = new Irt5920;
        m_tester = new Tester;

        m_irt1->moveToThread(&thread);
        m_irt2->moveToThread(&thread);
        m_irt3->moveToThread(&thread);
        m_tester->moveToThread(&thread);

        thread.connect(&thread, &QThread::finished, m_irt1, &QObject::deleteLater);
        thread.connect(&thread, &QThread::finished, m_irt2, &QObject::deleteLater);
        thread.connect(&thread, &QThread::finished, m_irt3, &QObject::deleteLater);
        thread.connect(&thread, &QThread::finished, m_tester, &QObject::deleteLater);

        thread.start(QThread::NormalPriority);
    }
    semafore.release();
}

mi::~mi()
{
    semafore.acquire();
    if (!semafore.available()) {
        thread.quit();
        thread.wait();
        m_irt1 = nullptr;
        m_irt2 = nullptr;
        m_irt3 = nullptr;
        m_tester = nullptr;
    }
}
