#include "mainwindow.h"
#include "adcdatamodel.h"
#include "testerth.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QSerialPortInfo>
#include <QSettings>

#include <hw/interface.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tester(new TesterTh)
    , no(QStringLiteral("C:/Windows/Media/Windows Critical Stop.wav"))
    , yes(QStringLiteral("C:/Windows/Media/Windows Notify.wav"))
{
    ui->setupUi(this);
    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts()) {
        if (info.manufacturer().contains("FTDI"))
            ui->cbxPortRelay->addItem(info.portName());
        ui->cbxPortAdc->addItem(info.portName());
        ui->cbxPortI->addItem(info.portName());
        ui->cbxPortU->addItem(info.portName());
    }
    auto model = new AdcDataModel(ui->tvRawAdcData);
    ui->tvRawAdcData->setModel(model);
    ui->tvRawAdcData->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tvRawAdcData->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    readSettings();

    connect(mi::irtAdc(), &Irt5501::Raw, this, &MainWindow::Raw);
    connect(mi::irtI(), &Irt5920::Val, ui->dsbxI, &QDoubleSpinBox::setValue);
    connect(mi::irtU(), &Irt5920::Val, ui->dsbxU, &QDoubleSpinBox::setValue);

    connect(mi::irtAdc(), &Irt5501::Raw, model, &AdcDataModel::setRawAdcData);
    connect(mi::irtI(), &Irt5920::Val, model, &AdcDataModel::setCurrent);
    connect(mi::irtU(), &Irt5920::Val, model, &AdcDataModel::setVoltage);

    connect(&timer, &QTimer::timeout, mi::irtAdc(), &Irt5501::getAdcRawData);
    connect(&timer, &QTimer::timeout, mi::irtI(), &Irt5920::getVal);
    connect(&timer, &QTimer::timeout, mi::irtU(), &Irt5920::getVal);

    connect(tester, &TesterTh::messageR, this, &MainWindow::messageR);
    connect(tester, &TesterTh::messageG, this, &MainWindow::messageG);
    connect(tester, &TesterTh::messageB, this, &MainWindow::messageB);

    connect(tester, &TesterTh::currentTest, ui->comboBox, qOverload<int>(&QComboBox::setCurrentIndex));

    on_pbPing_clicked();
}

MainWindow::~MainWindow()
{
    timer.stop();

    writeSettings();

    //    emit mi::irt1()->close();
    //    emit mi::irt2()->close();
    //    emit mi::irt3()->close();

    delete ui;
}

void MainWindow::on_pbPing_clicked()
{
    timer.stop();
    QString str;
    if (!mi::tester()->ping(ui->cbxPortRelay->currentText(), 57600))
        str.append("cbxPortRelay!\n");
    if (!mi::irtAdc()->ping(ui->cbxPortAdc->currentText(), 9600, 1))
        str.append("cbxPortAdc!\n");
    if (!mi::irtI()->ping(ui->cbxPortI->currentText(), 9600, 2))
        str.append("cbxPortI!\n");
    if (!mi::irtU()->ping(ui->cbxPortU->currentText(), 9600, 3))
        str.append("cbxPortU!\n");
    if (str.isEmpty()) {
        //        QMessageBox::information(this, "", "Ok");
        timer.start(1000);
    } else {
        QMessageBox::warning(this, "", str);
        timer.stop();
    }
    ui->groupBox_3->setEnabled(str.isEmpty());
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("State", saveState());
    settings.setValue("cbxPort1", ui->cbxPortRelay->currentText());
    settings.setValue("cbxPort2", ui->cbxPortAdc->currentText());
    settings.setValue("cbxPort3", ui->cbxPortI->currentText());
    settings.setValue("cbxPort4", ui->cbxPortU->currentText());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("Geometry").toByteArray());
    restoreState(settings.value("State").toByteArray());
    ui->cbxPortRelay->setCurrentText(settings.value("cbxPort1").toString());
    ui->cbxPortAdc->setCurrentText(settings.value("cbxPort2").toString());
    ui->cbxPortI->setCurrentText(settings.value("cbxPort3").toString());
    ui->cbxPortU->setCurrentText(settings.value("cbxPort4").toString());
    settings.endGroup();
}

void MainWindow::finished()
{
    ui->pbTest->setChecked(false);
    ui->comboBox->setCurrentIndex(0);
    timer.start();
    yes.play();
    //    QMessageBox::information(this, "", "Тесты закончились");
}

void MainWindow::messageR(const QString& text)
{
    ui->textEdit->setTextColor({ 100, 0, 0 });
    ui->textEdit->append(text);
}

void MainWindow::messageG(const QString& text)
{
    ui->textEdit->setTextColor({ 0, 100, 0 });
    ui->textEdit->append(text);
}

void MainWindow::messageB(const QString& text)
{
    ui->textEdit->setTextColor({ 0, 0, 0 });
    ui->textEdit->append(text);
}

void MainWindow::Raw(const Elemer::RawAdcData& data)
{
    auto status = data.stat;

    QString str = QString::number(status, 2);
    str = QString("00000000").left(8 - str.length()) + str;

    ui->leStatus->setText(str);
    ui->dsbxU1->setValue(data.v1);
    ui->dsbxU2->setValue(data.v2);
    ui->dsbxU3->setValue(data.v3);
}

void MainWindow::on_pbTest_clicked(bool checked)
{
    if (checked) {
        ui->widget_1->setLedColor(Qt::yellow);
        ui->widget_2->setLedColor(Qt::yellow);
        ui->widget_3->setLedColor(Qt::yellow);
        ui->widget_4->setLedColor(Qt::yellow);
        ui->widget_5->setLedColor(Qt::yellow);
        ui->widget_6->setLedColor(Qt::yellow);

        ui->textEdit->clear();

        timer.stop();

        connect(tester, &QThread::finished, this, &MainWindow::finished);

        tester->start();
    } else {
        disconnect(tester, &QThread::finished, this, &MainWindow::finished);
        if (tester->isRunning()) {
            tester->requestInterruption();
            tester->wait();
        }
        timer.start();
    }
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    qDebug() << "comboBox" << index;
    mi::tester()->setStage(index);
}
