#include "irt5501.h"

using namespace Elemer;

Irt5501::Irt5501(QObject* parent)
    : Elemer::Device(parent)
{
}

bool Irt5501::getAdcRawData()
{
    QMutexLocker locker(&m_mutex);
    enum {
        Cmd2 = 2,
        Cmd255 = 255
    };
    RawAdcData adcData[2];
    bool success = isConnected() && readHex<Cmd2, Cmd255>(adcData);
    if (success) {
        emit rawAdcData(adcData[0]);
        waitAllSemaphore.release();
    }
    return success;
}
