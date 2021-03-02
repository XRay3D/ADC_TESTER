//#include "common_interfaces.h"
//#include <QMessageBox>
//////////////////////////////////////
///// \brief CommonInterface::CommonInterface
/////
//CommonInterface::CommonInterface()
//{
//    port = new Port(this);
//    port->moveToThread(&m_portThread);
//    connect(&m_portThread, &QThread::finished, port, &QObject::deleteLater);

//    connect(this, &CommonInterface::open, port, &Port::openSlot);
//    connect(this, &CommonInterface::close, port, &Port::closeSlot);
//    connect(this, &CommonInterface::write, port, &Port::writeSlot);
//    connect(port, &QSerialPort::readyRead, this, &CommonInterface::readyReadSlot, Qt::DirectConnection);

//    m_portThread.start(QThread::NormalPriority);
//}

//CommonInterface::~CommonInterface()
//{
//    m_portThread.quit();
//    m_portThread.wait();
//}

//bool CommonInterface::isConnected() const
//{
//    return m_connected;
//}

////////////////////////////////////////
/////// \brief Port::Port
/////// \param manInterface
///////
//Port::Port(CommonInterface* device)
//    : m_device(device)
//{
//}

//Port::~Port() {}

//void Port::openSlot()
//{
//    if (open(ReadWrite))
//        m_device->m_semaphore.release();
//}

//void Port::closeSlot()
//{
//    close();
//    m_device->m_connected = false;
//    m_device->m_semaphore.release();
//}

//void Port::writeSlot(const QByteArray& data)
//{
//    if (isOpen())
//        write(data);
//}
