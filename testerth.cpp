#include "testerth.h"
#include "adcdatamodel.h"
#include "devices/devices.h"

#include <QDebug>
#include <QTime>

TesterTh::TesterTh(AdcDataModel* model, QObject* parent)
    : QThread(parent)
    , model(model) //
{
    connect(this, &TesterTh::getValues, Devices::irtAdc(), &Irt5501::getAdcRawData);
    connect(this, &TesterTh::getValues, Devices::irtI(), &Irt5920::getVal);
    connect(this, &TesterTh::getValues, Devices::irtU(), &Irt5920::getVal);
}

bool TesterTh::results() const { return m_resultsAll; }

bool* TesterTh::getResults() const { return const_cast<bool*>(m_results); }

void TesterTh::run() {
    static std::function<void(TesterTh&)> tests[] = {
        &TesterTh::test1,
        &TesterTh::test2,
        &TesterTh::test3,
        &TesterTh::test4,
        &TesterTh::test5,
        &TesterTh::test6,
    };
    m_resultsAll = true;
    for(auto testNum : {Test1, Test2, Test3, Test4, Test5, Test6}) {
        emit currentTest(testNum);

        QTime time;
        time.start();
        double avg_{};
        switch(testNum) {
        case Test1:
            avg_ = model->rawAdcData().v2;
            while(time.elapsed() < 6000 || avg_ > threshold * 0.9) {
                msleep(100);
                Devices::irtAdc()->waitAllReset();
                emit getValues();
                qDebug() << "waitAll 1" << Devices::irtAdc()->waitAll(3, 1000);
                avg_ += model->rawAdcData().v2;
                avg_ /= 2.0;
                if(isInterruptionRequested() || !model->rawAdcData().getReady())
                    return;
            }
            break;
        case Test2:
            avg_ = model->rawAdcData().v3;
            while(time.elapsed() < 6000 || avg_ > threshold * 0.9) {
                msleep(100);
                Devices::irtAdc()->waitAllReset();
                emit getValues();
                qDebug() << "waitAll 1" << Devices::irtAdc()->waitAll(3, 1000);
                avg_ += model->rawAdcData().v3;
                avg_ /= 2.0;
                if(isInterruptionRequested() || !model->rawAdcData().getReady())
                    return;
            }
            break;
        default:
            while(time.elapsed() < 2000) {
                msleep(100);
                Devices::irtAdc()->waitAllReset();
                emit getValues();
                qDebug() << "waitAll 1" << Devices::irtAdc()->waitAll(3, 1000);
                avg_ += model->rawAdcData().v2;
                avg_ /= 2.0;
                if(isInterruptionRequested() || !model->rawAdcData().getReady())
                    return;
            }
        }

        adc.reset();
        int avg{};
        do {
            Devices::irtAdc()->waitAllReset();
            emit getValues();
            qDebug() << "waitAll 2" << Devices::irtAdc()->waitAll(3, 1000);
            msleep(500);

            RawAdcData data{model->rawAdcData()};

            if(!data.csOk() || !data.getReady())
                return;

            status = data.stat;
            if(status == 0xFF)
                return;

            adc += std::pair{model->voltage(), model->current()};
            adc += data;

            if(isInterruptionRequested()) {
                emit currentTest(0);
                return;
            }
        } while(++avg < 3);

        adc /= avg;
        tests[testNum](*this);
        model->setCurrentTest(testNum + 1);
    }
}

void TesterTh::test1() {
    //0.00009
    qDebug(__FUNCTION__);
    // Ток потребления и напряжение встроенного БП без нагрузки.
    m_results[Test1] = true;
    emit messageB("Тест №1");
    if(abs(adc.uCh1) > threshold) {
        auto str = ErrorTexts[1][1].arg(abs(adc.uCh1)).arg(1).arg(threshold);
        emit messageR(str);
        m_results[Test1] = false;
    }
    if(abs(adc.uCh2) > threshold * 1.3) {
        auto str = ErrorTexts[1][2].arg(abs(adc.uCh2)).arg(2).arg(threshold * 1.3);
        emit messageR(str);
        m_results[Test1] = false;
    }
    if(abs(adc.uCh3) > threshold) {
        auto str = ErrorTexts[1][3].arg(abs(adc.uCh3)).arg(3).arg(threshold);
        emit messageR(str);
        m_results[Test1] = false;
    }
    if(!rangeTest(23.55, 24.45, adc.u)) {
        auto str = ErrorTexts[1][4].arg(adc.u).arg(23.55).arg(24.45);
        emit messageR(str);
        m_results[Test1] = false;
    }
    if(adc.i > 15.00) {
        auto str = ErrorTexts[1][5].arg(adc.i).arg(15.00);
        emit messageR(str);
        m_results[Test1] = false;
    }
    if(m_results[Test1]) {
        emit messageG("Ок");
    } else
        m_resultsAll = false;
}

void TesterTh::test2() {
    qDebug(__FUNCTION__);
    m_results[Test2] = true;
    emit messageB("Тест №2");
    if(status != 71) {
        auto str = ErrorTexts[2][1].arg(status);
        emit messageR(str);
        m_results[Test2] = false;
    }
    if(!qFuzzyCompare(adc.uCh1, -32768.0)) {
        auto str = ErrorTexts[2][2].arg(adc.uCh1).arg(1).arg(-32768.0);
        emit messageR(str);
        m_results[Test2] = false;
    }
    if(!qFuzzyCompare(adc.uCh2, -32768.0)) {
        auto str = ErrorTexts[2][3].arg(adc.uCh2).arg(2).arg(-32768.0);
        emit messageR(str);
        m_results[Test2] = false;
    }
    if(abs(adc.uCh3) > threshold) {
        auto str = ErrorTexts[2][4].arg(abs(adc.uCh3)).arg(3).arg(threshold);
        emit messageR(str);
        m_results[Test2] = false;
    }
    if(!rangeTest(23.55, 24.45, adc.u)) {
        auto str = ErrorTexts[2][5].arg(adc.u).arg(23.55).arg(24.45);
        emit messageR(str);
        m_results[Test2] = false;
    }
    if(adc.i > 15.00) {
        auto str = ErrorTexts[2][6].arg(adc.i).arg(15.00);
        emit messageR(str);
        m_results[Test2] = false;
    }
    if(m_results[Test2]) {
        emit messageG("Ок");
    } else
        m_resultsAll = false;
}

void TesterTh::test3() {
    qDebug(__FUNCTION__);
    m_results[Test3] = true;
    emit messageB("Тест №3");
    if(auto val = abs(adc.uCh2 - adc.uCh3); val > 0.005) {
        auto str = ErrorTexts[3][1].arg(val).arg(adc.uCh2).arg(adc.uCh3);
        emit messageR(str);
        m_results[Test3] = false;
    }
    if(auto val = abs((adc.uCh2 + adc.uCh3) / 2 - adc.uCh1); val < 0.001) {
        auto str = ErrorTexts[3][2].arg(val).arg(adc.uCh2 + adc.uCh3).arg(adc.uCh2).arg(adc.uCh3).arg(adc.uCh1);
        emit messageR(str);
        m_results[Test3] = false;
    }
    if(m_results[Test3]) {
        emit messageG("Ок");
    } else
        m_resultsAll = false;
}

void TesterTh::test4() {
    qDebug(__FUNCTION__);
    m_results[Test4] = true;
    emit messageB("Тест №4");
    constexpr double k = 0.005;
    if(auto val = abs(adc.uCh1 - adc.uCh2); val > k) {
        auto str = ErrorTexts[4][1].arg(val).arg(adc.uCh1).arg(adc.uCh2);
        emit messageR(str);
        m_results[Test4] = false;
    }
    if(auto val = abs(adc.uCh2 - adc.uCh3); val > k) {
        auto str = ErrorTexts[4][2].arg(val).arg(adc.uCh2).arg(adc.uCh3);
        emit messageR(str);
        m_results[Test4] = false;
    }
    if(auto val = abs(adc.uCh3 - adc.uCh1); val > k) {
        auto str = ErrorTexts[4][3].arg(val).arg(adc.uCh1).arg(adc.uCh3);
        emit messageR(str);
        m_results[Test4] = false;
    }
    if(m_results[Test4]) {
        emit messageG("Ок");
    } else
        m_resultsAll = false;
}

void TesterTh::test5() {
    qDebug(__FUNCTION__);
    m_results[Test5] = true;
    emit messageB("Тест №5");
    {
        constexpr double k = 0.01;
        if(auto val = abs(adc.uCh1 - adc.uCh2); val > k) {
            auto str = ErrorTexts[5][1].arg(val).arg(adc.uCh1).arg(adc.uCh2);
            emit messageR(str);
            m_results[Test4] = false;
        }
        if(auto val = abs(adc.uCh2 - adc.uCh3); val > k) {
            auto str = ErrorTexts[5][2].arg(val).arg(adc.uCh2).arg(adc.uCh3);
            emit messageR(str);
            m_results[Test4] = false;
        }
        if(auto val = abs(adc.uCh3 - adc.uCh1); val > k) {
            auto str = ErrorTexts[5][3].arg(val).arg(adc.uCh1).arg(adc.uCh3);
            emit messageR(str);
            m_results[Test4] = false;
        }
    }
    {
        constexpr double k = 1.0;
        if(adc.uCh1 < k) {
            auto str = ErrorTexts[5][4].arg(adc.uCh1).arg(1).arg(k);
            emit messageR(str);
            m_results[Test5] = false;
        }
        if((adc.uCh2 < k)) {
            auto str = ErrorTexts[5][5].arg(adc.uCh2).arg(2).arg(k);
            emit messageR(str);
            m_results[Test5] = false;
        }
        if(adc.uCh3 < k) {
            auto str = ErrorTexts[5][6].arg(adc.uCh3).arg(3).arg(k);
            emit messageR(str);
            m_results[Test5] = false;
        }
    }
    if(19.00 > adc.u) {
        auto str = ErrorTexts[5][7].arg(adc.u).arg(19.00);
        emit messageR(str);
        m_results[Test5] = false;
    }
    if(!rangeTest(26.0, 50.0, adc.i)) {
        auto str = ErrorTexts[5][8].arg(adc.i).arg(26.0).arg(50.0);
        emit messageR(str);
        m_results[Test5] = false;
    }
    if(m_results[Test5]) {
        emit messageG("Ок");
    } else
        m_resultsAll = false;
}

void TesterTh::test6() {
    qDebug(__FUNCTION__);
    m_results[Test6] = true;
    emit messageB("Тест №6");
    if(!rangeTest(26.0, 50.0, adc.i)) {
        auto str = ErrorTexts[6][1].arg(adc.i).arg(26.0).arg(50.0);
        emit messageR(str);
        m_results[Test6] = false;
    }
    if(m_results[Test6]) {
        emit messageG("Ок");
    } else
        m_resultsAll = false;
}
