#pragma once

#include "devices/irt5501.h"
#include <QThread>
#include <ed_device.h>
#include <qglobal.h>

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
    bool m_results[TestCount]{};
    bool m_resultsAll{};

    struct AdcMath {
        double uCh1{};
        double uCh2{};
        double uCh3{};
        double u{};
        double i{};
        void reset() {
            uCh1 = {};
            uCh2 = {};
            uCh3 = {};
            u = {};
            i = {};
        }
        AdcMath operator=(const std::pair<double, double>& v) {
            u = v.first;
            i = v.second;
            return *this;
        }
        AdcMath operator=(const RawAdcData& data) {
            uCh1 = data.v1;
            uCh2 = data.v2;
            uCh3 = data.v3;
            return *this;
        }
        AdcMath operator+=(const std::pair<double, double>& v) {
            u += v.first;
            i += v.second;
            return *this;
        }
        AdcMath operator+=(const RawAdcData& data) {
            uCh1 += data.v1;
            uCh2 += data.v2;
            uCh3 += data.v3;
            return *this;
        }
        AdcMath operator/=(double v) {
            uCh1 /= v;
            uCh2 /= v;
            uCh3 /= v;
            u /= v;
            i /= v;
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
    inline bool rangeTest(double min, double max, double val, bool eq = false) {
        return eq ? (min <= val && val <= max)
                  : (min < val && val < max);
    }
    int status{};

    AdcDataModel* const model;

    static inline std::map<int, std::map<int, const QString>> ErrorTexts{
        {1, {
                {1, QString("1.1 Шумы %1 канала %2 АЦП выше допуска %3")},
                {2, QString("1.2 Шумы %1 канала %2 АЦП выше допуска %3")},
                {3, QString("1.3 Шумы %1 канала %2 АЦП выше допуска %3")},
                {4, QString("1.4 Выходное напряжение встроенного ИП %1 вне допуска %2 - %3 В")},
                {5, QString("1.5 Ток потребления %1 выше допуска %2 мА")},
            }},
        {2, {
                {1, QString("2.1 Значение статуса %1 не равно 71")},
                {2, QString("2.2 Значение %1 канала %2 АЦП не равно %3")},
                {3, QString("2.3 Значение %1 канала %2 АЦП не равно %3")},
                {4, QString("2.4 Шумы %1 канала %2 АЦП выше допуска %3")},
                {5, QString("2.5 Выходное напряжение встроенного ИП %1 вне допуска %2 - %3 В")},
                {6, QString("2.6 Ток потребления %1 выше допуска %2 мА")},
            }},
        {3, {
                {1, QString("3.1 Большая  разница %1 между значениями каналов 2(%2) и 3(%3) АЦП")},
                {2, QString("3.2 Большая  разница %1 между суммой %2 каналов 2 (%3), 3 (%4) и каналом 1 (%5) АЦП")},
            }},
        {4, {
                {1, QString("4.1 Большая  разница %1 между значениями каналов 1 (%2) и 2 (%3) АЦП")},
                {2, QString("4.2 Большая  разница %1 между значениями каналов 2 (%2) и 3 (%3) АЦП")},
                {3, QString("4.3 Большая  разница %1 между значениями каналов 1 (%2) и 3 (%3) АЦП")},
            }},
        {5, {
                {1, QString("5.1 Большая  разница %1 между значениями каналов 1 (%2) и 2 (%3) АЦП")},
                {2, QString("5.2 Большая  разница %1 между значениями каналов 2 (%2) и 3 (%3) АЦП")},
                {3, QString("5.3 Большая  разница %1 между значениями каналов 1 (%2) и 3 (%3) АЦП")},
                {4, QString("5.4 Значение %1 канала %2 АЦП ниже %3")},
                {5, QString("5.5 Значение %1 канала %2 АЦП ниже %3")},
                {6, QString("5.6 Значение %1 канала %2 АЦП ниже %3")},
                {7, QString("5.7 Выходное напряжение встроенного ИП %1 ниже допуска %2 В")},
                {8, QString("5.8 Ток потребления %1 вне допуска %2 - %3 мА")},
            }},
        {6, {
                {1, QString("6.1 Ток встроенного ИП %1 вне допуска %2 - %3 мА")},
            }}};

protected:
    // QThread interface
    void run() override;
};
