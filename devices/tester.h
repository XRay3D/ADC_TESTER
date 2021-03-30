#pragma once

#include <QMutex>
#include <QSemaphore>
#include <QSerialPort>
#include <QThread>
#include <QVector>
#include <commoninterfaces.h>
#include <myprotokol.h>

enum CommandEnum {
    PING,
    GET_STAGE,
    SET_STAGE,
    READY,
    BUFFER_OVERFLOW,
    WRONG_COMMAND,
    TEXTUAL_PARCEL,
    CRC_ERROR
};

#pragma pack(push, 1)
#pragma pack(pop)

class Tester;

class CallBack {
public:
    virtual ~CallBack() { }
    virtual void RxPing(Parcel* data) = 0;
    virtual void RxGetStage(Parcel* data) = 0;
    virtual void RxSetStage(Parcel* data) = 0;
    virtual void RxReady(Parcel* data) = 0;
    virtual void RxBufferOverflow(Parcel* data) = 0;
    virtual void RxWrongCommand(Parcel* data) = 0;
    virtual void RxTextualParcel(Parcel* data) = 0;
    virtual void RxCrcError(Parcel* data) = 0;
    virtual void RxNullFunction(Parcel* data) = 0;
};

class Tester final : public QObject, public CommonInterfaces, private MyProtokol, public CallBack {
    Q_OBJECT
    friend class TesterPort;

public:
    Tester(QObject* parent = nullptr);
    ~Tester();

    bool ping(const QString& PortName = {}, int baud = 9600, int = {}) override;
    bool setStage(quint8 stage);
    bool getStage(int& stage);

signals:
    void open(int mode) override;
    void close() override;
    void write(const QByteArray& data);

private:
    QByteArray m_data;
    QMutex m_mutex;
    QSemaphore m_semaphore;

    QThread m_portThread;
    TesterPort* port;

    int m_stage;
    mutable bool m_result;

    void RxPing(Parcel* data) override;
    void RxGetStage(Parcel* data) override;
    void RxSetStage(Parcel* data) override;
    void RxReady(Parcel* data) override;
    void RxBufferOverflow(Parcel* data) override;
    void RxWrongCommand(Parcel* data) override;
    void RxTextualParcel(Parcel* data) override;
    void RxCrcError(Parcel* data) override;
    void RxNullFunction(Parcel* data) override;
};
