#include "irt5501.h"

#include <QDebug>
#include <QElapsedTimer>

using namespace Elemer;

enum {
    Cmd2 = 2,
    Cmd255 = 255
};

Irt5501::Irt5501(QObject* parent)
    : Elemer::AsciiDevice(parent)
{
}

bool Irt5501::getAdcRawDataB(RawAdcData& data)
{
    QMutexLocker locker(&m_mutex);
    RawAdcData adcData2;
    bool success = isConnected() && readHex<Cmd2, Cmd255>(data, adcData2);
    if (success) {
        emit Raw(data);
        waitAllSemaphore.release();
    }
    return success;
}

bool Irt5501::getAdcRawData()
{
    QMutexLocker locker(&m_mutex);
    RawAdcData adcData1[2];
    bool success = isConnected() && readHex<Cmd2, Cmd255>(adcData1);
    if (success)
        emit Raw(adcData1[0]);
    return success;
}
