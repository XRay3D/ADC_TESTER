//#include "irt.h"

//#include <QMessageBox>

//int id1 = qRegisterMetaType<raw_t>("raw_t");
//int id2 = qRegisterMetaType<raw_t*>("raw_t*");

//Irt5920::Irt()
//{
//    port->setBaudRate(QSerialPort::Baud9600);
//    port->setParity(QSerialPort::NoParity);
//    port->setDataBits(QSerialPort::Data8);
//    port->setFlowControl(QSerialPort::NoFlowControl);
//}

//Irt5920::~Irt() { }

//bool Irt5920::ping(const QString& portName)
//{
//    QMutexLocker locker(&m_mutex);
//    m_connected = false;
//    m_semaphore.acquire(m_semaphore.available());
//    do {

//        if (port->isOpen()) {
//            emit close();
//            if (!m_semaphore.tryAcquire(1, 2000))
//                break;
//        }

//        if (!portName.isEmpty())
//            port->setPortName(portName);

//        emit open();
//        if (m_semaphore.tryAcquire(1, 1000)) {
//            port->setRequestToSend(true);
//            port->setDataTerminalReady(false);
//            port->waitForReadyRead(10);
//            m_connected = checkConnection();
//        } else {
//            emit close();
//            if (!m_semaphore.tryAcquire(1, 2000))
//                break;
//        }

//    } while (0);
//    // if (!ok)
//    //  QMessageBox::critical(nullptr, portName + " error", port->errorString());
//    return m_connected;
//}

//int Irt5920::getAddress() const { return address; }

//void Irt5920::setAddress(int value) { address = value; }

//int Irt5920::getDev()
//{
//    dev = 0;
//    emit write(createParcel(address, 0));
//    if (m_semaphore.tryAcquire(1, 1000))
//        dev = v[1].toInt();
//    return dev;
//}

//void Irt5920::getVal()
//{
//    QMutexLocker locker(&m_mutex);
//    if (isConnected()) {
//        QByteArray data;
//        m_semaphore.acquire(m_semaphore.available());
//        do {
//            emit write(createParcel(address, 5, 0));
//            if (!m_semaphore.tryAcquire(1, 1000))
//                break;
//            emit write(createParcel(address, 6));
//            if (!m_semaphore.tryAcquire(1, 1000))
//                break;
//            for (int i = 0; i < 5; ++i) {
//                if (i == 4)
//                    emit Val(*reinterpret_cast<float*>(data.data()));
//                else {
//                    emit write(createParcel(address, 6));
//                    if (!m_semaphore.tryAcquire(1, 1000))
//                        break;
//                    data.append(v[1].toInt());
//                }
//            }
//        } while (0);
//    }
//}

//void Irt5920::getRaw()
//{
//    QMutexLocker locker(&m_mutex);
//    if (isConnected()) {
//        m_semaphore.acquire(m_semaphore.available());
//        emit write(createParcel(address, 2, 255));
//        if (m_semaphore.tryAcquire(1, 1000)) {
//            QByteArray data(QByteArray::fromHex(v[1]));
//            raw_t* raw = reinterpret_cast<raw_t*>(data.data());
//            emit Raw(*raw /*{ raw->stat, raw->v1, raw->v2, raw->v3 }*/);
//        }
//    }
//}

//bool Irt5920::checkConnection()
//{
//    return getDev() == 9 || dev == 41;
//}

//void Irt5920::readyReadSlot()
//{
//    QMutexLocker locker(&m_readMutex);
//    m_data.append(port->readAll());
//    if (m_data.endsWith('\r')) {
//        if (checkParcel(m_data, v)) {
//            m_data.clear();
//            m_semaphore.release();
//        }
//    }
//}
