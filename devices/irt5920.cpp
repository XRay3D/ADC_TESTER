#include "irt5920.h"

#include <QDebug>
#include <QElapsedTimer>

using namespace Elemer;

Irt5920::Irt5920(QObject* parent)
    : Elemer::Device(parent, DTR::Off, DTS::On)
{
}

bool Irt5920::getVal()
{
    QMutexLocker locker(&m_mutex);

    enum {
        Success,
        Cmd0 = 0,
        Cmd5 = 5,
        Cmd6 = 6,
    };

    if (isConnected() && write<Cmd5, Cmd0>() != Success)
        return {};

    if (write<Cmd6>() != Success)
        return {};

    uint8_t data[4];
    for (auto& c : data) {
        if (!read<Cmd6>(c))
            return {};
    }

    emit value(*reinterpret_cast<float*>(data));
    waitAllSemaphore.release();
    return true;
}
