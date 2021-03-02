#include "testerth.h"
#include "adcdatamodel.h"
#include "hw/interface.h"

#include <QDebug>
#include <QTime>

TesterTh::TesterTh(AdcDataModel* model, QObject* parent)
    : QThread(parent)
    , model(model)
{
    connect(this, &TesterTh::getValues, mi::irtAdc(), &Irt5501::getAdcRawData);
    connect(this, &TesterTh::getValues, mi::irtI(), &Irt5920::getVal);
    connect(this, &TesterTh::getValues, mi::irtU(), &Irt5920::getVal);
}

void TesterTh::run()
{
    for (auto testNum : { Test1, Test2, Test3, Test4, Test5, Test6 }) {
        emit currentTest(testNum);

        QTime time;
        time.start();
        while (time.elapsed() < (testNum ? 3000 : 6000)) {
            msleep(100);
            mi::irtAdc()->waitAllReset();
            emit getValues();
            qDebug() << "waitAll 1" << mi::irtAdc()->waitAll(3, 1000);
            if (isInterruptionRequested()) {
                emit currentTest(0);
                return;
            }
        }

        ADC adc {};
        int avg {};
        do {
            mi::irtAdc()->waitAllReset();
            emit getValues();
            qDebug() << "waitAll 2" << mi::irtAdc()->waitAll(3, 1000);
            msleep(500);
            //            double u, i;
            Elemer::RawAdcData data { model->rawAdcData() };

            //            mi::irtAdc()->getAdcRawDataB(data);
            //            mi::irtI()->getValB(i);
            //            mi::irtU()->getValB(u);

            status = data.stat;
            if (status == 0xFF) {
                emit currentTest(0);
                return;
            }

            adc += std::pair { model->voltage(), model->current() };
            //            adc += std::pair { u, i };
            adc += data;

            if (isInterruptionRequested()) {
                emit currentTest(0);
                return;
            }
        } while (++avg < 1);

        adc /= avg;

        switch (testNum) {
        case Test1:
            test1(adc);
            continue;
        case Test2:
            test2(adc);
            continue;
        case Test3:
            test3(adc);
            continue;
        case Test4:
            test4(adc);
            continue;
        case Test5:
            test5(adc);
            continue;
        case Test6:
            test6(adc);
            continue;
        }
    }
    emit currentTest(0);
}

void TesterTh::test1(ADC adc)
{
    // Ток потребления и напряжение встроенного БП без нагрузки.
    bool fl = true;
    emit messageB("Test1");
    if (abs(adc.U1) > 0.0001) {
        emit messageR("ADC_U1 " + QString::number(abs(adc.U1) - 0.0001, 'f', 6));
        fl = false;
    }
    if (abs(adc.U2) > 0.0005) {
        emit messageR("ADC_U2 " + QString::number(abs(adc.U2) - 0.0005, 'f', 6));
        fl = false;
    }
    if (abs(adc.U3) > 0.0001) {
        emit messageR("ADC_U3 " + QString::number(abs(adc.U3) - 0.0001, 'f', 6));
        fl = false;
    }
    if (!rangeTest(23.55, 24.45, adc.U)) {
        emit messageR("23.55 > adc.U || adc.U > 24.45");
        fl = false;
    }
    if (adc.I > 15.00) {
        emit messageR("adc.I > 15.00");
        fl = false;
    }
    if (fl) {
        emit messageG("Ок");
    }
}

void TesterTh::test2(ADC adc)
{
    bool fl = true;
    emit messageB("Test2");
    if (status != 71) {
        emit messageR("status != 71");
        fl = false;
    }
    if (!(qFuzzyCompare(adc.U1, -32768.0) && qFuzzyCompare(adc.U2, -32768.0))) {
        emit messageR("!(qFuzzyCompare(adc.U1, -32768) && qFuzzyCompare(adc.U2, -32768))");
        fl = false;
    }
    if (abs(adc.U3) > 0.0001) {
        emit messageR("adc.U3 > 0.0001 " + QString::number(adc.U3));
        fl = false;
    }
    if (!rangeTest(23.55, 24.45, adc.U)) {
        emit messageR("23.55 > adc.U || adc.U > 24.45");
        fl = false;
    }
    if (adc.I > 15.00) {
        emit messageR("adc.I > 15.00");
        fl = false;
    }
    if (fl) {
        emit messageG("Ок");
    }
}

void TesterTh::test3(ADC adc)
{
    bool fl = true;
    emit messageB("Test3");
    if (abs(adc.U2 - adc.U3) > 0.005) {
        emit messageR("abs(adc.U2 - adc.U3) > 0.005");
        fl = false;
    }
    if (abs((adc.U2 + adc.U3) / 2 - adc.U1) < 0.001) {
        emit messageR("abs(abs(adc.U2 - adc.U3) - adc.U1) > 0.001 " + QString::number(abs((adc.U2 + adc.U3) / 2 - adc.U1)));
        fl = false;
    }
    if (fl) {
        emit messageG("Ок");
    }
}

void TesterTh::test4(ADC adc)
{
    bool fl = true;
    emit messageB("Test4");
    if (abs(adc.U1 - adc.U2) > 0.005) {
        emit messageR("abs(adc.U1 - adc.U2) > 0.005");
        fl = false;
    }
    if (abs(adc.U2 - adc.U3) > 0.005) {
        emit messageR("abs(adc.U2 - adc.U3) > 0.005");
        fl = false;
    }
    if (abs(adc.U3 - adc.U1) > 0.005) {
        emit messageR("abs(adc.U3 - adc.U1) > 0.005");
        fl = false;
    }
    if (fl) {
        emit messageG("Ок");
    }
}

void TesterTh::test5(ADC adc)
{
    bool fl = true;
    emit messageB("Test5");
    const double k = 0.01;
    if (abs(adc.U1 - adc.U2) > k) {
        emit messageR(QString("abs(U1 - U2) -> %1 > %2").arg(abs(adc.U1 - adc.U2)).arg(k));
        fl = false;
    }
    if (abs(adc.U2 - adc.U3) > k) {
        emit messageR(QString("abs(U2 - U3) -> %1 > %2").arg(abs(adc.U2 - adc.U3)).arg(k));
        fl = false;
    }
    if (abs(adc.U3 - adc.U1) > k) {
        emit messageR(QString("abs(U3 - U1) -> %1 > %2").arg(abs(adc.U3 - adc.U1)).arg(k));
        fl = false;
    }
    if (adc.U1 < 1.0) {
        emit messageR("adc.U1 < 1.0 ");
        fl = false;
    }
    if ((adc.U2 < 1.0) | 0) {
        emit messageR("adc.U2 < 1.0");
        fl = false;
    }
    if (adc.U3 < 1.0) {
        emit messageR("adc.U3 < 1.0");
        fl = false;
    }
    if (19.00 > adc.U /* || adc.U > 20.00*/) {
        emit messageR("19.00 > adc.U || adc.U > 20.00");
        fl = false;
    }
    if (!rangeTest(26.0, 50.0, adc.I)) {
        emit messageR("26.0 > adc.I || adc.I > 50.0");
        fl = false;
    }
    if (fl) {
        emit messageG("Ок");
    }
}

void TesterTh::test6(ADC adc)
{
    do {
        emit messageB("Test6");
        if (26.0 > adc.I || adc.I > 50.0) {
            emit messageR("adc.I > 50.0");
            break;
        }
    } while (0);
    emit messageG("Ок");
}
