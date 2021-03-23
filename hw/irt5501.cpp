#include "irt5501.h"

#include <QDebug>
#include <QElapsedTimer>

using namespace Elemer;

enum {
    Cmd2 = 2,
    Cmd255 = 255
};

Irt5501::Irt5501(QObject* parent)
    : Elemer::Device(parent)
{
}

bool Irt5501::getAdcRawData()
{
    QMutexLocker locker(&m_mutex);
    RawAdcData adcData1[2];
    bool success = isConnected() && readHex<Cmd2, Cmd255>(adcData1);
    if (success)
        emit RawAdcData(adcData1[0]);
    return success;
}
