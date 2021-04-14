#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "adcdatamodel.h"
#include "devices/devices.h"
#include "testerth.h"

#include <QMessageBox>
#include <QSerialPortInfo>
#include <QSettings>
#include <algorithm>
#include <ranges>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model(new AdcDataModel(this))
    , tester(new TesterTh(model))
    , no(QStringLiteral("C:/Windows/Media/Windows Critical Stop.wav"))
    , yes(QStringLiteral("C:/Windows/Media/Windows Notify.wav"))
    , msgBox("", "Тест завершён", QMessageBox::Information, QMessageBox::Ok, {}, {}, this) {
    ui->setupUi(this);

    leds = {
        ui->widget_1,
        ui->widget_2,
        ui->widget_3,
        ui->widget_4,
        ui->widget_5,
        ui->widget_6,
    };
    auto availablePorts{QSerialPortInfo::availablePorts().toVector()};
    std::ranges::sort(availablePorts, {}, [](const QSerialPortInfo& info) { return info.portName().midRef(3).toInt(); });

    for(auto& portInfo : availablePorts) {
        if(portInfo.manufacturer().contains("FTDI"))
            ui->cbxPortRelay->addItem(portInfo.portName());
        ui->cbxPortAdc->addItem(portInfo.portName());
        ui->cbxPortI->addItem(portInfo.portName());
        ui->cbxPortU->addItem(portInfo.portName());
    }
    ui->tvRawAdcData->setModel(model);
    ui->tvRawAdcData->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tvRawAdcData->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    readSettings();

    connect(Devices::irtAdc(), &Irt5501::rawAdcData, model, &AdcDataModel::setRawAdcData);
    connect(Devices::irtAdc(), &Irt5501::rawAdcData, this, &MainWindow::autoRunTest);
    connect(Devices::irtI(), &Irt5920::value, model, &AdcDataModel::setCurrent);
    connect(Devices::irtU(), &Irt5920::value, model, &AdcDataModel::setVoltage);

    connect(&getValuesTimer, &QTimer::timeout, Devices::irtAdc(), &Irt5501::getAdcRawData);
    connect(&getValuesTimer, &QTimer::timeout, Devices::irtI(), &Irt5920::getVal);
    connect(&getValuesTimer, &QTimer::timeout, Devices::irtU(), &Irt5920::getVal);

    connect(tester, &TesterTh::messageR, this, &MainWindow::messageR);
    connect(tester, &TesterTh::messageG, this, &MainWindow::messageG);
    connect(tester, &TesterTh::messageB, this, &MainWindow::messageB);

    connect(tester, &TesterTh::currentTest, Devices::tester(), &Tester::setSwitch);
    connect(tester, &TesterTh::currentTest, ui->comboBox, &QComboBox::setCurrentIndex);
    connect(ui->comboBox, qOverload<int>(&QComboBox::currentIndexChanged), Devices::tester(), &Tester::setSwitch);

    on_pbPing_clicked();
}

MainWindow::~MainWindow() {
    Devices::tester()->setSwitch(0);

    getValuesTimer.stop();

    writeSettings();

    delete ui;
}

void MainWindow::on_pbPing_clicked() {
    getValuesTimer.stop();
    QString str;
    if(!Devices::tester()->ping(ui->cbxPortRelay->currentText()))
        str.append("PortRelay!" + Devices::tester()->port()->errorString() + "\n");
    if(!Devices::irtAdc()->ping(ui->cbxPortAdc->currentText(), 9600, 1))
        str.append("PortAdc!" + Devices::irtAdc()->port()->errorString() + "\n");
    if(!Devices::irtI()->ping(ui->cbxPortI->currentText(), 9600, 2))
        str.append("PortI!" + Devices::irtI()->port()->errorString() + "\n");
    if(!Devices::irtU()->ping(ui->cbxPortU->currentText(), 9600, 3))
        str.append("PortU!" + Devices::irtU()->port()->errorString() + "\n");
    if(str.isEmpty()) {
        //        QMessageBox::information(this, "", "Ok");
        getValuesTimer.start(1000);
    } else {
        QMessageBox::warning(this, "", str);
        getValuesTimer.stop();
    }
    ui->groupBox_3->setEnabled(str.isEmpty());
}

void MainWindow::writeSettings() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("State", saveState());
    settings.setValue("PortR", ui->cbxPortRelay->currentText());
    settings.setValue("PortA", ui->cbxPortAdc->currentText());
    settings.setValue("PortI", ui->cbxPortI->currentText());
    settings.setValue("PortU", ui->cbxPortU->currentText());
    settings.endGroup();
}

void MainWindow::readSettings() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("Geometry").toByteArray());
    restoreState(settings.value("State").toByteArray());
    ui->cbxPortRelay->setCurrentText(settings.value("PortR").toString());
    ui->cbxPortAdc->setCurrentText(settings.value("PortA").toString());
    ui->cbxPortI->setCurrentText(settings.value("PortI").toString());
    ui->cbxPortU->setCurrentText(settings.value("PortU").toString());
    settings.endGroup();
}

void MainWindow::finished() {
    ui->pbTest->setChecked(false);
    Devices::tester()->setSwitch(0);
    ui->comboBox->setCurrentIndex(0);
    getValuesTimer.start();
    tester->results() ? yes.play()
                      : no.play();

    for(int i = 0; i < 6; ++i)
        leds[i]->setLedColor(tester->getResults()[i] ? Qt::green : Qt::red);

    disconnect(tester, &QThread::finished, this, &MainWindow::finished);
    msgBox.exec();
    //    QMessageBox::information(this, "", "Тесты закончились");
}

void MainWindow::messageR(const QString& text) {
    ui->textEdit->setTextColor({100, 0, 0});
    ui->textEdit->append(text);
}

void MainWindow::messageG(const QString& text) {
    ui->textEdit->setTextColor({0, 100, 0});
    ui->textEdit->append(text);
}

void MainWindow::messageB(const QString& text) {
    ui->textEdit->setTextColor({0, 0, 0});
    ui->textEdit->append(text);
}

void MainWindow::autoRunTest(const RawAdcData& rawAdcData) {
    if(tester->isRunning())
        return;
    if(rawAdcData.stat == 64 && !autoRunTestFl) {
        on_pbTest_clicked(true);
        autoRunTestFl = true;
    } else if(rawAdcData.stat == 0xFF && autoRunTestFl) {
        autoRunTestFl = false;
    }
}

void MainWindow::on_pbTest_clicked(bool checked) {
    if(1) {
        if(checked) {
            ui->widget_1->setLedColor(Qt::yellow);
            ui->widget_2->setLedColor(Qt::yellow);
            ui->widget_3->setLedColor(Qt::yellow);
            ui->widget_4->setLedColor(Qt::yellow);
            ui->widget_5->setLedColor(Qt::yellow);
            ui->widget_6->setLedColor(Qt::yellow);

            ui->textEdit->clear();

            getValuesTimer.stop();
            connect(tester, &QThread::finished, this, &MainWindow::finished);

            tester->start();
            msgBox.accept();
        } else {
            disconnect(tester, &QThread::finished, this, &MainWindow::finished);
            if(tester->isRunning()) {
                tester->requestInterruption();
                tester->wait();
                msgBox.accept();
            }
            getValuesTimer.start();
        }
        ui->pbTest->setChecked(checked);
    } else {
        Devices::tester()->testSwitch();
    }
}
