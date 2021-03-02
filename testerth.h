#pragma once

#include <QThread>
#include <ascii_device.h>
#include <qglobal.h>

class AdcDataModel;
class TesterTh : public QThread {
    Q_OBJECT
public:
    explicit TesterTh(AdcDataModel* model, QObject* parent = nullptr);

signals:
    void currentTest(int);

    void messageR(const QString&);
    void messageG(const QString&);
    void messageB(const QString&);

    void getValues();

private:
    enum {
        Test1,
        Test2,
        Test3,
        Test4,
        Test5,
        Test6
    };

    struct ADC {
        double U1 {};
        double U2 {};
        double U3 {};
        double U {};
        double I {};

        ADC operator=(const std::pair<double, double>& v)
        {
            U = v.first;
            I = v.second;
            return *this;
        }
        ADC operator=(const Elemer::RawAdcData& data)
        {
            U1 = data.v1;
            U2 = data.v2;
            U3 = data.v3;
            return *this;
        }
        ADC operator+=(const std::pair<double, double>& v)
        {
            U += v.first;
            I += v.second;
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
    };

    void test1(ADC adc);
    void test2(ADC adc);
    void test3(ADC adc);
    void test4(ADC adc);
    void test5(ADC adc);
    void test6(ADC adc);

    void reset();

    //template<typename T>
    inline bool rangeTest(double min, double max, double val, bool eq = false)
    {
        if (eq)
            return min <= val || val <= max;
        return min < val || val < max;
    }
    int status {};

    AdcDataModel* const model;

protected:
    // QThread interface
    void run() override;
};
