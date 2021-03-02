#include "tester.h"

#include <QDebug>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QTimer>

#define Dbg 0

class TesterPort : public QSerialPort, private MyProtokol {
    void readSlot();
    QByteArray m_data;
    qint64 counter = 0;

public:
    TesterPort(Tester* t);
    void closeSlot();
    void openSlot(int mode);
    void writeSlot(const QByteArray& data);
    Tester* m_t;
    using func = void (Tester::*)(Parcel*);
    QVector<func> m_f;
};

Tester::Tester(QObject* parent)
    : QObject(parent)
    , port(new TesterPort(this))
{
    port->moveToThread(&m_portThread);
    connect(&m_portThread, &QThread::finished, port, &QObject::deleteLater);
    m_portThread.start(QThread::InheritPriority);
    //    m_f[PING] = &Tester::RxPing;
    //    m_f[GET_STAGE] = &Tester::RxGetStage;
    //    m_f[SET_STAGE] = &Tester::RxSetStage;
    //    m_f[READY] = &Tester::RxReady;
    //    m_f[BUFFER_OVERFLOW] = &Tester::RxBufferOverflow;
    //    m_f[WRONG_COMMAND] = &Tester::RxWrongCommand;
    //    m_f[TEXTUAL_PARCEL] = &Tester::RxTextualParcel;
    //    m_f[CRC_ERROR] = &Tester::RxCrcError;

    //    port->setBaudRate(QSerialPort::Baud57600);
    //    port->setParity(QSerialPort::NoParity);
    //    port->setFlowControl(QSerialPort::NoFlowControl);
}

Tester::~Tester()
{
    m_portThread.quit();
    m_portThread.wait();
}

bool Tester::ping(const QString& PortName, int baud, int)
{
    QMutexLocker locker(&m_mutex);
    m_connected = false;
    do {
        if (port->isOpen()) {
            emit close();
            if (!m_semaphore.tryAcquire(1, 2000))
                break;
        }

        if (!PortName.isEmpty())
            port->setPortName(PortName);
        port->setBaudRate(baud);

        emit open(QIODevice::ReadWrite);
        if (!m_semaphore.tryAcquire(1, 1000))
            break;

        emit write(parcel(PING));
        if (!m_semaphore.tryAcquire(1, 1000)) {
            emit close();
            break;
        }

        m_connected = true;

    } while (0);
    if (!m_connected)
        QMessageBox::critical(nullptr, PortName + " error", port->errorString());
    return m_connected;
}

bool Tester::setStage(quint8 stage)
{
    QMutexLocker Locker(&m_mutex);
    if (isConnected()) {
        m_result = false;
        emit write(parcel(SET_STAGE, stage));
        if (m_semaphore.tryAcquire(1, 1000)) {
            m_result = true;
        }
    }
    return m_result;
}

bool Tester::getStage(int& stage)
{
    QMutexLocker Locker(&m_mutex);
    if (isConnected()) {
        m_result = false;
        emit write(parcel(GET_STAGE));
        if (m_semaphore.tryAcquire(1, 1000)) {
            stage = m_stage;
            m_result = true;
        }
    }
    return m_result;
}

void Tester::RxPing(Parcel* data)
{
    if (Dbg)
        qDebug() << "PING"; //<< data.command;
    Q_UNUSED(data)
    m_semaphore.release();
}

void Tester::RxGetStage(Parcel* data)
{
    if (Dbg)
        qDebug() << "GET_STAGE"; //<< data.command;
    Q_UNUSED(data)
    m_semaphore.release();
}

void Tester::RxSetStage(Parcel* data)
{
    if (Dbg)
        qDebug() << "SET_STAGE"; //<< data.command;
    Q_UNUSED(data)
    m_semaphore.release();
}

void Tester::RxReady(Parcel* data)
{
    if (Dbg)
        qDebug() << "READY"; //<< data.command;
    Q_UNUSED(data)
    //m_semaphore.release();
}

void Tester::RxBufferOverflow(Parcel* data)
{
    if (Dbg)
        qDebug() << "BUFFER_OVERFLOW"; //<< data.command;
    Q_UNUSED(data)
    //m_semaphore.release();
}

void Tester::RxWrongCommand(Parcel* data)
{
    if (Dbg)
        qDebug() << "WRONG_COMMAND"; //<< data.command;
    Q_UNUSED(data)
    //m_semaphore.release();
}

void Tester::RxTextualParcel(Parcel* data)
{
    if (Dbg)
        qDebug() << "TEXTUAL_PARCEL"; //<< data.command << data.text();
    Q_UNUSED(data)
    //m_semaphore.release();
}

void Tester::RxCrcError(Parcel* data)
{
    if (Dbg)
        qDebug() << "CRC_ERROR"; //<< data.command;
    Q_UNUSED(data)
    //m_semaphore.release();
}

void Tester::RxNullFunction(Parcel* data)
{
    if (Dbg)
        qDebug() << "RxNullFunction"; //<< data.command;
    Q_UNUSED(data)
    //m_semaphore.release();
}

//bool Tester::checkConnection()
//{
//    return false;
//}

//void Tester::readyReadSlot()
//{
//    QMutexLocker locker(&m_readMutex);
//    m_data.append(port->readAll());
//    for (int i = 0; i < m_data.size() - 3; ++i) {
//        const Parcel* const parcel = reinterpret_cast<const Parcel*>(m_data.constData() + i);
//        if (parcel->start == RX) {
//            if ((parcel->len + i) <= m_data.size()) {
//                if (checkParcel(m_data.mid(i, parcel->len)))
//                    (this->*m_f[parcel->cmd])(*parcel);
//                else {
//                    (this->*m_f[CRC_ERROR])(*parcel);
//                    m_data.clear();
//                }
//                m_data.remove(0, i + parcel->len);
//                i = -1;
//            }
//        }
//    }
//}

/////////////////////////////////////////

TesterPort::TesterPort(Tester* t)
    : m_t(t)
    , m_f(0x100, &Tester::RxNullFunction)
{
    m_f[PING] = &Tester::RxPing;
    m_f[GET_STAGE] = &Tester::RxGetStage;
    m_f[SET_STAGE] = &Tester::RxSetStage;
    m_f[READY] = &Tester::RxReady;
    m_f[BUFFER_OVERFLOW] = &Tester::RxBufferOverflow;
    m_f[WRONG_COMMAND] = &Tester::RxWrongCommand;
    m_f[TEXTUAL_PARCEL] = &Tester::RxTextualParcel;
    m_f[CRC_ERROR] = &Tester::RxCrcError;

    setBaudRate(Baud57600);
    setDataBits(Data8);
    setFlowControl(NoFlowControl);
    setParity(NoParity);

    connect(t, &Tester::open, this, &TesterPort::openSlot);
    connect(t, &Tester::close, this, &TesterPort::closeSlot);
    connect(t, &Tester::write, this, &TesterPort::writeSlot);
    //    //get the virtual table pointer of object obj
    //    int* vptr = *(int**)amkTester;
    //    // we shall call the function fn, but first the following assembly code
    //    //  is required to make obj as 'this' pointer as we shall call
    //    //  function fn() directly from the virtual table
    //    //__asm mov ecx, amkTester;
    //    amkTester;
    //    //function fn is the first entry of the virtual table, so it's vptr[0]
    //    ((void (*)(const QByteArray&))vptr[0])(QByteArray("0123456789"));

    //    typedef void (Tester::*func)(const QByteArray&);
    //    CallBack* ptr = t;
    //    func* vptr = *(func**)(ptr);
    //    (t->*vptr[0])(QByteArray("0123456789"));
    connect(this, &QSerialPort::readyRead, this, &TesterPort::readSlot, Qt::DirectConnection);
}

void TesterPort::closeSlot()
{
    close();
    m_t->m_semaphore.release();
}

void TesterPort::openSlot(int mode)
{
    if (open(static_cast<OpenMode>(mode)))
        m_t->m_semaphore.release();
    else {
        qDebug() << errorString();
    }
}

void TesterPort::writeSlot(const QByteArray& data)
{
    qDebug() << write(data) << data.toHex('|').toUpper();
}

void TesterPort::readSlot()
{
    m_data.append(readAll());
    for (int i = 0; i < m_data.size() - 3; ++i) {
        Parcel* d = reinterpret_cast<Parcel*>(m_data.data() + i);
        if (d->start == RX && d->len <= m_data.size()) {
            counter += d->len;
            if (checkParcel(m_data.constData() + i))
                (m_t->*m_f[d->cmd])(d);
            else
                (m_t->*m_f[CRC_ERROR])(d);

            m_t->m_semaphore.release();
            m_data.remove(0, i + d->len);
            i = 0;
        }
    }
}
