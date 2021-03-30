#include "testerth.h"
#include "adcdatamodel.h"
#include "devices/devices.h"

#include <QDebug>
#include <QTime>

TesterTh::TesterTh(AdcDataModel* model, QObject* parent)
    : QThread(parent)
    , model(model)
{
    connect(this, &TesterTh::getValues, Devices::irtAdc(), &Irt5501::getAdcRawData);
    connect(this, &TesterTh::getValues, Devices::irtI(), &Irt5920::getVal);
    connect(this, &TesterTh::getValues, Devices::irtU(), &Irt5920::getVal);
}

bool TesterTh::results() const { return m_resultsAll; }

bool* TesterTh::getResults() const { return const_cast<bool*>(m_results); }

void TesterTh::run()
{
    static std::function<void(TesterTh&)> tests[] = {
        &TesterTh::test1,
        &TesterTh::test2,
        &TesterTh::test3,
        &TesterTh::test4,
        &TesterTh::test5,
        &TesterTh::test6,
    };
    m_resultsAll = true;
    for (auto testNum : { Test1, Test2, Test3, Test4, Test5, Test6 }) {
        emit currentTest(testNum);

        QTime time;
        time.start();
        double avg_ = model->rawAdcData().v2;
        while (time.elapsed() < (testNum < Test3 ? 6000 : 2000) || (testNum == Test1 && avg_ > threshold)) {
            msleep(100);
            Devices::irtAdc()->waitAllReset();
            emit getValues();
            qDebug() << "waitAll 1" << Devices::irtAdc()->waitAll(3, 1000);
            avg_ += model->rawAdcData().v2;
            avg_ /= 2.0;
            if (isInterruptionRequested() || !model->rawAdcData().getReady())
                return;
        }

        adc.reset();
        int avg {};
        do {
            Devices::irtAdc()->waitAllReset();
            emit getValues();
            qDebug() << "waitAll 2" << Devices::irtAdc()->waitAll(3, 1000);
            msleep(500);

            RawAdcData data { model->rawAdcData() };

            if (!data.csOk() || !data.getReady())
                return;

            status = data.stat;
            if (status == 0xFF)
                return;

            adc += std::pair { model->voltage(), model->current() };
            adc += data;

            if (isInterruptionRequested()) {
                emit currentTest(0);
                return;
            }
        } while (++avg < 3);

        adc /= avg;
        tests[testNum](*this);
    }
}

void TesterTh::test1()
{
    //0.00009
    qDebug(__FUNCTION__);
    // Ток потребления и напряжение встроенного БП без нагрузки.
    m_results[Test1] = true;
    emit messageB("Тест №1");
    if (abs(adc.U1) > threshold) {
        emit messageR("ADC_U1 " + QString::number(abs(adc.U1) - threshold, 'f', 6));
        m_results[Test1] = false;
    }
    if (abs(adc.U2) > threshold) {
        emit messageR("ADC_U2 " + QString::number(abs(adc.U2) - threshold, 'f', 6));
        m_results[Test1] = false;
    }
    if (abs(adc.U3) > threshold) {
        emit messageR("ADC_U3 " + QString::number(abs(adc.U3) - threshold, 'f', 6));
        m_results[Test1] = false;
    }
    if (!rangeTest(23.55, 24.45, adc.U)) {
        emit messageR("23.55 > adc.U || adc.U > 24.45");
        m_results[Test1] = false;
    }
    if (adc.I > 15.00) {
        emit messageR("adc.I > 15.00");
        m_results[Test1] = false;
    }
    if (m_results[Test1]) {
        emit messageG("Ок");
    } else
        m_resultsAll = false;
}

void TesterTh::test2()
{
    qDebug(__FUNCTION__);
    m_results[Test2] = true;
    emit messageB("Тест №2");
    if (status != 71) {
        emit messageR("status != 71");
        m_results[Test2] = false;
    }
    if (!(qFuzzyCompare(adc.U1, -32768.0) && qFuzzyCompare(adc.U2, -32768.0))) {
        emit messageR("!(qFuzzyCompare(adc.U1, -32768) && qFuzzyCompare(adc.U2, -32768))");
        m_results[Test2] = false;
    }
    if (abs(adc.U3) > threshold) {
        emit messageR("adc.U3 > dk " + QString::number(adc.U3));
        m_results[Test2] = false;
    }
    if (!rangeTest(23.55, 24.45, adc.U)) {
        emit messageR("23.55 > adc.U || adc.U > 24.45");
        m_results[Test2] = false;
    }
    if (adc.I > 15.00) {
        emit messageR("adc.I > 15.00");
        m_results[Test2] = false;
    }
    if (m_results[Test2]) {
        emit messageG("Ок");
    } else
        m_resultsAll = false;
}

void TesterTh::test3()
{
    qDebug(__FUNCTION__);
    m_results[Test3] = true;
    emit messageB("Тест №3");
    if (abs(adc.U2 - adc.U3) > 0.005) {
        emit messageR("abs(adc.U2 - adc.U3) > 0.005");
        m_results[Test3] = false;
    }
    if (abs((adc.U2 + adc.U3) / 2 - adc.U1) < 0.001) {
        emit messageR("abs(abs(adc.U2 - adc.U3) - adc.U1) > 0.001 " + QString::number(abs((adc.U2 + adc.U3) / 2 - adc.U1)));
        m_results[Test3] = false;
    }
    if (m_results[Test3]) {
        emit messageG("Ок");
    } else
        m_resultsAll = false;
}

void TesterTh::test4()
{
    qDebug(__FUNCTION__);
    m_results[Test4] = true;
    emit messageB("Тест №4");
    if (abs(adc.U1 - adc.U2) > 0.005) {
        emit messageR("abs(adc.U1 - adc.U2) > 0.005");
        m_results[Test4] = false;
    }
    if (abs(adc.U2 - adc.U3) > 0.005) {
        emit messageR("abs(adc.U2 - adc.U3) > 0.005");
        m_results[Test4] = false;
    }
    if (abs(adc.U3 - adc.U1) > 0.005) {
        emit messageR("abs(adc.U3 - adc.U1) > 0.005");
        m_results[Test4] = false;
    }
    if (m_results[Test4]) {
        emit messageG("Ок");
    } else
        m_resultsAll = false;
}

void TesterTh::test5()
{
    qDebug(__FUNCTION__);
    m_results[Test5] = true;
    emit messageB("Тест №5");
    const double k = 0.01;
    if (abs(adc.U1 - adc.U2) > k) {
        emit messageR(QString("abs(U1 - U2) -> %1 > %2").arg(abs(adc.U1 - adc.U2)).arg(k));
        m_results[Test5] = false;
    }
    if (abs(adc.U2 - adc.U3) > k) {
        emit messageR(QString("abs(U2 - U3) -> %1 > %2").arg(abs(adc.U2 - adc.U3)).arg(k));
        m_results[Test5] = false;
    }
    if (abs(adc.U3 - adc.U1) > k) {
        emit messageR(QString("abs(U3 - U1) -> %1 > %2").arg(abs(adc.U3 - adc.U1)).arg(k));
        m_results[Test5] = false;
    }
    if (adc.U1 < 1.0) {
        emit messageR("adc.U1 < 1.0 ");
        m_results[Test5] = false;
    }
    if ((adc.U2 < 1.0) | 0) {
        emit messageR("adc.U2 < 1.0");
        m_results[Test5] = false;
    }
    if (adc.U3 < 1.0) {
        emit messageR("adc.U3 < 1.0");
        m_results[Test5] = false;
    }
    if (19.00 > adc.U /* || adc.U > 20.00*/) {
        emit messageR("19.00 > adc.U || adc.U > 20.00");
        m_results[Test5] = false;
    }
    if (!rangeTest(26.0, 50.0, adc.I)) {
        emit messageR("26.0 > adc.I || adc.I > 50.0");
        m_results[Test5] = false;
    }
    if (m_results[Test5]) {
        emit messageG("Ок");
    } else
        m_resultsAll = false;
}

void TesterTh::test6()
{
    qDebug(__FUNCTION__);
    m_results[Test6] = true;
    emit messageB("Тест №6");
    if (26.0 > adc.I || adc.I > 50.0) {
        emit messageR("adc.I > 50.0");
        m_results[Test6] = false;
    }
    if (m_results[Test6]) {
        emit messageG("Ок");
    } else
        m_resultsAll = false;
}
