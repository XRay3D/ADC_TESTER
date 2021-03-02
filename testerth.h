#pragma once

#include <QThread>
#include <ascii_device.h>
#include <qglobal.h>

class TesterTh : public QThread {
    Q_OBJECT
public:
    explicit TesterTh(QObject* parent = nullptr);

signals:
    void currentTest(int);

    void messageR(const QString&);
    void messageG(const QString&);
    void messageB(const QString&);

private:
    enum {
        Test1,
        Test2,
        Test3,
        Test4,
        Test5,
        Test6
    };

    void test1();
    void test2();
    void test3();
    void test4();
    void test5();
    void test6();

    void reset();
    typedef struct ADC {
        ADC() { }

        //int status = 0;
        double U1 = 0.0;
        double U2 = 0.0;
        double U3 = 0.0;
        double U = 0.0;
        double I = 0.0;

        void reset()
        {
            //status = 0;
            U1 = 0.0;
            U2 = 0.0;
            U3 = 0.0;
            U = 0.0;
            I = 0.0;
        }

        ADC operator+=(const QVector<double>& v)
        {
            const double* p = v.data();
            U += *p++;
            I += *p++;
            return *this;
        }
        ADC operator+=(const Elemer::RawAdcData& data)
        {
            U1 += data.v1;
            U2 += data.v2;
            U3 += data.v3;
            return *this;
        }
        ADC operator/=(double v)
        {
            U1 /= v;
            U2 /= v;
            U3 /= v;
            U /= v;
            I /= v;
            return *this;
        }
    } ADC;

    ADC adc;
    //template<typename T>
    inline bool rangeTest(double min, double max, double val, bool eq = false)
    {
        if (eq)
            return min <= val || val <= max;
        return min < val || val < max;
    }
    int status {};

protected:
    // QThread interface
    void run() override;
};
