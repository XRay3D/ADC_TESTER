//#ifndef IRT_H
//#define IRT_H

//#include "common_interfaces.h"
//#include <elemer_ascii.h>

//#pragma pack(push, 1)
//typedef struct raw_t {
//    uint8_t dummy1;
//    uint8_t stat;
//    float v1;
//    float v2;
//    float v3;
//} raw_t;
//#pragma pack(pop)

//class Irt : public CommonInterface, public ElemerASCII {
//    Q_OBJECT

//public:
//    Irt();
//    ~Irt();

//    bool ping(const QString& portName) override;

//    int getAddress() const;
//    void setAddress(int value);

//    int getDev();
//    void getVal();
//    void getRaw();

//signals:
//    void Raw(const raw_t&);
//    void Val(double data);

//protected:
//    bool checkConnection() override;
//    void readyReadSlot() override;

//private:
//    QList<QByteArray> v;
//    QByteArray m_data;
//    QMutex m_mutex;
//    int dev = 0;
//    int address = 1;
//};

//#endif // IRT_H
