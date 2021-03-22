#pragma once

#include <QThread>
#include <device.h>
#include <qglobal.h>
#include <hw/irt5501.h>

class AdcDataModel;
class TesterTh : public QThread {
    Q_OBJECT

    enum {
        Test1,
        Test2,
        Test3,
        Test4,
        Test5,
        Test6,
        TestCount
    };

public:
    explicit TesterTh(AdcDataModel* model, QObject* parent = nullptr);

    bool results() const;

    bool* getResults() const;

signals:
    void currentTest(int);

    void messageR(const QString&);
    void messageG(const QString&);
    void messageB(const QString&);

    void getValues();

    //    void testResults(int, bool);

private:
    bool m_results[TestCount] {};
    bool m_resultsAll {};

    struct AdcMath {
        double U1 {};
        double U2 {};
        double U3 {};
        double U {};
        double I {};
        void reset()
        {
            U1 = {};
            U2 = {};
            U3 = {};
            U = {};
            I = {};
        }
        AdcMath operator=(const std::pair<double, double>& v)
        {
            U = v.first;
            I = v.second;
            return *this;
        }
        AdcMath operator=(const RawAdcData& data)
        {
            U1 = data.v1;
            U2 = data.v2;
            U3 = data.v3;
            return *this;
        }
        AdcMath operator+=(const std::pair<double, double>& v)
        {
            U += v.first;
            I += v.second;
            return *this;
        }
        AdcMath operator+=(const RawAdcData& data)
        {
            U1 += data.v1;
            U2 += data.v2;
            U3 += data.v3;
            return *this;
        }
        AdcMath operator/=(double v)
        {
            U1 /= v;
            U2 /= v;
            U3 /= v;
            U /= v;
            I /= v;
            return *this;
        }
    } adc;

    void test1();
    void test2();
    void test3();
    void test4();
    void test5();
    void test6();

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
