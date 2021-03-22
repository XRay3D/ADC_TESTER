#include "irt5920.h"

#include <QDebug>
#include <QElapsedTimer>

using namespace Elemer;

enum {
    Success,
    Cmd0 = 0,
    Cmd5 = 5,
    Cmd6 = 6,
};

Irt5920::Irt5920(QObject* parent)
    : Elemer::AsciiDevice(parent, DTR::Off, DTS::On)
{
}

bool Irt5920::getVal()
{
    QMutexLocker locker(&m_mutex);
    do {
        if (isConnected() && writeStr<Cmd5, Cmd0>() != Success)
            break;
        if (writeStr<Cmd6>() != Success)
            break;
        QByteArray data(4, '\0');
        for (char& c : data) {
            if (!readStr<Cmd6>(c))
                return {};
        }
        emit Val(fromHex<float>(data));
        waitAllSemaphore.release();
        return true;
    } while (0);
    return {};
}

bool Irt5920::getValB(double& data)
{
    QMutexLocker locker(&m_mutex);
    do {
        if (isConnected() && writeStr<Cmd5, Cmd0>() != Success)
            break;
        if (writeStr<Cmd6>() != Success)
            break;
        QByteArray array(4, '\0');
        for (char& c : array) {
            if (!readStr<Cmd6>(c))
                return {};
        }
        emit Val(data = fromHex<float>(array));
        waitAllSemaphore.release();
        return true;
    } while (0);
    return {};
}
