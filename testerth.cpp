#include "testerth.h"
#include "hw/interface.h"
TesterTh::TesterTh(QObject* parent)
    : QThread(parent)
{
}

void TesterTh::run()
{
    //    status = data.stat;

    //    QString str = QString::number(status, 2);
    //    str = QString("00000000").left(8 - str.length()) + str;

    //    ui->leStatus->setText(str);
    //    ui->dsbxU1->setValue(data.v1);
    //    ui->dsbxU2->setValue(data.v2);
    //    ui->dsbxU3->setValue(data.v3);

    //    ui->dsbxU1->setStyleSheet("QDoubleSpinBox{background-color:rgb(255, 128, 128)}");

    //    if (!ui->pbTest->isChecked())
    //        return;
    for (int testNum : { 0, 1, 2, 3, 4, 5 }) {
        emit currentTest(testNum);

        for (int i = 0; i < 30; ++i) { // delay 2 sec
            msleep(100);
            if (isInterruptionRequested()) {
                emit currentTest(0);
                return;
            }
        }

        Elemer::RawAdcData data;
        double u, i;

        int avg {};
        for (; avg < 3; ++avg) {
            mi::irtAdc()->getAdcRawDataB(data);
            mi::irtI()->getValB(i);
            mi::irtU()->getValB(u);

            status = data.stat;

            adc += QVector { u, i };
            adc += data;

            msleep(500);
            if (isInterruptionRequested()) {
                emit currentTest(0);
                return;
            }
        }

        adc /= avg;

        switch (testNum) {
        case Test1:
            test1();
            break;
        case Test2:
            test2();
            break;
        case Test3:
            test3();
            break;
        case Test4:
            test4();
            break;
        case Test5:
            test5();
            break;
        case Test6:
            test6();
            break;
        }
        //        ui->comboBox->setCurrentIndex(stage);
        //        ui->textEdit->setTextColor(Qt::black);
        adc.reset();
    }
    emit currentTest(0);
}

void TesterTh::test1()
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

void TesterTh::test2()
{
    bool fl = true;
    emit messageB("Test2");
    if (status != 71) {
        emit messageR("status != 71");
        fl = false;
    }
    if (!qFuzzyCompare(adc.U1, -32768) || !qFuzzyCompare(adc.U2, -32768)) {
        emit messageR("!qFuzzyCompare(adc.U1, -32768) || !qFuzzyCompare(adc.U2, -32768)");
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

void TesterTh::test3()
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

void TesterTh::test4()
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

void TesterTh::test5()
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

void TesterTh::test6()
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
