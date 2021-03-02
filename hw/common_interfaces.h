//#ifndef COMMON_INTERFACES_H
//#define COMMON_INTERFACES_H

//#include <QDebug>
//#include <QElapsedTimer>
//#include <QMutexLocker>
//#include <QSemaphore>
//#include <QSerialPort>
//#include <QString>
//#include <QThread>

//class Port;

//class CommonInterface : public QObject {
//    Q_OBJECT
//    friend class Port;

//public:
//    CommonInterface();
//    ~CommonInterface();
//    virtual bool ping(const QString& portName = QString()) = 0;
//    /*virtual*/ bool isConnected() const;

//signals:
//    void open();
//    void close();
//    void write(const QByteArray& data);

//protected:
//    virtual bool checkConnection() = 0;
//    virtual void readyReadSlot() = 0;

//    Port* port;
//    QSemaphore m_semaphore;
//    bool m_connected = false;
//    QMutex m_readMutex;

//private:
//    QThread m_portThread;
//};

//class Port : public QSerialPort {
//    Q_OBJECT

//public:
//    Port(CommonInterface* device);
//    ~Port();
//    void openSlot();
//    void closeSlot();
//    void writeSlot(const QByteArray& data);
//    CommonInterface* m_device;
//};

//#endif // COMMON_INTERFACES_H
