#include "tester.h"

#include <QDateTime>
#include <QDebug>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QTimer>
#include <span>

#define Dbg 0
using namespace XrProtokol;

QTime t;

Tester::Tester(QObject* parent)
    : Device(parent) //
{
    registerCallback<SetSwitch>(&Tester::RxSetSwitch);
    registerCallback<GetSwitch>(&Tester::RxGetSwitch);
    registerCallback<SetSwitchResistance>(&Tester::RxSetSwitchResistance);
    registerCallback<GetSwitchResistance>(&Tester::RxGetSwitchResistance);
    registerCallback<TestSwitch>(&Tester::RxTestSwitch);
    registerCallback<SysCmd::Text>(&Tester::RxTestSwitch2);
}

Tester::~Tester() { }

bool Tester::setSwitch(uint8_t test) {
    if(isConnected()) {
        emit write(parcel(SetSwitch, test));
        return m_semaphore.tryAcquire(1, 1000);
    }
    return {};
}

bool Tester::getSwitch(uint8_t& test) {
    if(isConnected()) {
        emit write(parcel(GetSwitch));
        return m_semaphore.tryAcquire(1, 1000);
    }
    return {};
}

bool Tester::setSwitchResistance(const K& value) {
    if(isConnected()) {
        emit write(parcel(SetSwitchResistance, value));
        return m_semaphore.tryAcquire(1, 1000);
    }
    return {};
}

bool Tester::getSwitchResistance(K& value) {
    if(isConnected()) {
        emit write(parcel(GetSwitchResistance));
        return m_semaphore.tryAcquire(1, 1000);
    }
    return {};
}

bool Tester::testSwitch() {
    QMutexLocker Locker(&m_mutex);
    t.start();
    if(isConnected()) {
        adcData.clear();
        adcData.reserve(1200);
        m_semaphore.acquire(m_semaphore.available());
        emit write(parcel(TestSwitch));
        if(m_semaphore.tryAcquire(1200, 25000)) {
            std::ranges::sort(adcData, {});
            int16_t delta{};
            uint8_t idx{};
            int32_t average[7]{};
            uint16_t ctr{};

            for(uint8_t val : adcData) {
                if(abs(delta - val) > 10) {
                    delta = val;
                    average[idx] /= ctr;
                    ctr = 0;
                    ++idx;
                }
                average[idx] += val;
                ++ctr;
            }
            average[idx] /= ctr;

            K k;
            idx = 0;
            for(int32_t val : std::span(average + 1, 6)) {
                k.data[idx++] = val;
            }
            setSwitchResistance(k);
            getSwitchResistance(k);
            qDebug() << __FUNCTION__ << t.elapsed();
        }
    }
    return {};
}

void Tester::RxSetSwitch(const QByteArray& data) {
    qDebug() << __FUNCTION__ << data.toHex().toUpper();
    //m_semaphore.release();
}

void Tester::RxGetSwitch(const QByteArray& data) {
    qDebug() << __FUNCTION__ << data.toHex().toUpper();
    //m_semaphore.release();
}

void Tester::RxSetSwitchResistance(const QByteArray& data) {
    qDebug() << __FUNCTION__ << data.toHex().toUpper();
    //m_semaphore.release();
}

void Tester::RxGetSwitchResistance(const QByteArray& data) {
    qDebug() << __FUNCTION__ << data.toHex().toUpper();
    //m_semaphore.release();
}

void Tester::RxTestSwitch(const QByteArray& data) {
    const Parcel* p = reinterpret_cast<const Parcel*>(data.data());
    qDebug() << __FUNCTION__ << *reinterpret_cast<const uint16_t*>(p->data)
             << static_cast<const int>(p->data[2])
             << static_cast<const int>(p->data[3])
             << static_cast<const int>(p->data[4]);
    adcData.emplace_back(p->data[2]);
    //m_semaphore.release();
}

void Tester::RxTestSwitch2(const QByteArray& data) {
    const Parcel* p = reinterpret_cast<const Parcel*>(data.data());
    qDebug() << __FUNCTION__
             << static_cast<const int>(p->data[0])
             << static_cast<const int>(p->data[1])
             << static_cast<const int>(p->data[2])
             << static_cast<const int>(p->data[3])
             << static_cast<const int>(p->data[4]);
}

Type Tester::type() const { return Type::AdcSwitch; }
