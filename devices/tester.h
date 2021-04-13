#pragma once

#include <QMutex>
#include <QSemaphore>
#include <QSerialPort>
#include <QThread>
#include <QVector>
#include <commoninterfaces.h>
#include <xrdevice.h>

enum CommandEnum {
    SetSwitch = 1,
    GetSwitch,
    SetSwitchResistance,
    GetSwitchResistance,
    TestSwitch,
};

#pragma pack(push, 1)
struct K {
    uint8_t data[6]{};
};
#pragma pack(pop)

struct Callback {
    virtual ~Callback() { }
    virtual void RxSetSwitch(const QByteArray& data) = 0;
    virtual void RxGetSwitch(const QByteArray& data) = 0;
    virtual void RxSetSwitchResistance(const QByteArray& data) = 0;
    virtual void RxGetSwitchResistance(const QByteArray& data) = 0;
    virtual void RxTestSwitch(const QByteArray& data) = 0;
};

class Tester final : public XrProtokol::Device, Callback {
    Q_OBJECT
    friend class TesterPort;

public:
    Tester(QObject* parent = nullptr);
    ~Tester();

    bool setSwitch(uint8_t test);
    bool getSwitch(uint8_t& test);
    bool setSwitchResistance(const K& value);
    bool getSwitchResistance(K& value);
    bool testSwitch();

    // Device interface
    XrProtokol::Type type() const override;

    void RxSetSwitch(const QByteArray& data) override;
    void RxGetSwitch(const QByteArray& data) override;
    void RxSetSwitchResistance(const QByteArray& data) override;
    void RxGetSwitchResistance(const QByteArray& data) override;
    void RxTestSwitch(const QByteArray& data) override;
    void RxTestSwitch2(const QByteArray& data);

private:
    QMutex m_mutex;
    std::vector<uint8_t> adcData;

    //    bool ping(const QString& PortName = {}, int baud = 115200, int = {}) override;
    //        bool setSwitch(quint8 stage);
    //    bool getStage(int& stage);

    //    void RxPing(XrProtokol::Parcel* data) override;
    //    void RxGetStage(XrProtokol::Parcel* data) override;
    //    void RxSetStage(XrProtokol::Parcel* data) override;
    //    void RxReady(XrProtokol::Parcel* data) override;
    //    void RxBufferOverflow(XrProtokol::Parcel* data) override;
    //    void RxWrongCommand(XrProtokol::Parcel* data) override;
    //    void RxText(XrProtokol::Parcel* data) override;
    //    void RxCrcError(XrProtokol::Parcel* data) override;
    //    void RxNullFunction(XrProtokol::Parcel* data) override;
};
