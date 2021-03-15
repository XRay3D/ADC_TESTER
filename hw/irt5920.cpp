#include "irt5920.h"

#include <QDebug>
#include <QElapsedTimer>

using namespace Elemer;

Irt5920::Irt5920(QObject* parent)
    : Elemer::AsciiDevice(parent, {}, DTS::On)
{
}

void Irt5920::getVal()
{
    QMutexLocker locker(&m_mutex);
#ifdef EMU
    QRandomGenerator generator(QTime::currentTime().msecsSinceStartOfDay());
    constexpr double k = 0.01;
    val = val * k + generator.bounded(static_cast<int>((set - 0.5) * 100), static_cast<int>((set + 0.5) * 100)) * 0.01 * (1.0 - k);
    emit measuredValue(val);
    return /*true*/;
#endif
#ifndef EL_ALWAYS_OPEN
    PortOpener po(this);
#endif
    if (m_connected) {
        do {
            emit write(createParcel(address, 5, 0));
            if (!wait())
                break;
            emit write(createParcel(address, 6));
            if (!wait())
                break;
            QByteArray data;
            for (int i = 0; i < 5; ++i) {
                if (i == 4) {
                    emit Val(fromHex<float>(data));
                    waitAllSemaphore.release();
                } else {
                    emit write(createParcel(address, 6));
                    if (!wait())
                        break;
                    data.append(m_data[1].toInt());
                }
            }
        } while (0);
    }
    return /*{}*/;
}

bool Irt5920::getValB(double& data)
{
    QMutexLocker locker(&m_mutex);
#ifdef EMU
    QRandomGenerator generator(QTime::currentTime().msecsSinceStartOfDay());
    constexpr double k = 0.01;
    val = val * k + generator.bounded(static_cast<int>((set - 0.5) * 100), static_cast<int>((set + 0.5) * 100)) * 0.01 * (1.0 - k);
    emit measuredValue(val);
    return /*true*/;
#endif
#ifndef EL_ALWAYS_OPEN
    PortOpener po(this);
#endif
    if (m_connected) {
        do {
            emit write(createParcel(address, 5, 0));
            if (!wait())
                break;
            emit write(createParcel(address, 6));
            if (!wait())
                break;
            QByteArray dataArr;
            for (int i = 0; i < 5; ++i) {
                if (i == 4) {
                    emit Val(data = fromHex<float>(dataArr));
                    waitAllSemaphore.release();
                } else {
                    emit write(createParcel(address, 6));
                    if (!wait())
                        return {};
                    dataArr.append(m_data[1].toInt());
                }
            }
            return true;
        } while (0);
    }
    return {};
}
