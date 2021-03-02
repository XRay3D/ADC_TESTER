#include "irt5501.h"

#include <QDebug>
#include <QElapsedTimer>

using namespace Elemer;

Irt5501::Irt5501(QObject* parent)
    : Elemer::AsciiDevice(parent)
{
}

bool Irt5501::getAdcRawDataB(RawAdcData& data)
{
    QMutexLocker locker(&m_mutex);
#ifdef EMU
    QRandomGenerator generator(QTime::currentTime().msecsSinceStartOfDay());
    constexpr double k = 0.01;
    val = val * k + generator.bounded(static_cast<int>((set - 0.5) * 100), static_cast<int>((set + 0.5) * 100)) * 0.01 * (1.0 - k);
    emit measuredValue(val);
    return /*true*/;
#endif
#ifndef ALWAYS_OPEN
    Elemer::PortOpener po(this);
#endif
    if (m_connected) {
        emit write(createParcel(address, 2, 255));
        if (wait()) {
            emit Raw(data = fromHex<std::pair<RawAdcData, RawAdcData>>(1).first);
            return true;
        }
    }
    return {};
}

void Irt5501::getAdcRawData()
{
    QMutexLocker locker(&m_mutex);
#ifdef EMU
    QRandomGenerator generator(QTime::currentTime().msecsSinceStartOfDay());
    constexpr double k = 0.01;
    val = val * k + generator.bounded(static_cast<int>((set - 0.5) * 100), static_cast<int>((set + 0.5) * 100)) * 0.01 * (1.0 - k);
    emit measuredValue(val);
    return /*true*/;
#endif
#ifndef ALWAYS_OPEN
    Elemer::PortOpener po(this);
#endif
    if (m_connected) {
        emit write(createParcel(address, 2, 255));
        if (wait())
            emit Raw(fromHex<std::pair<RawAdcData, RawAdcData>>(1).first);
    }
}
