#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSound>
#include <QTimer>
#include <common_types.h>

class QSound;

class AdcDataModel;
namespace Ui {
class MainWindow;
}

class TesterTh;

class MainWindow : public QMainWindow {
    Q_OBJECT

    enum {
        ADC_U1,
        ADC_U2,
        ADC_U3,
        U,
        I,
    };

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private slots:
    void on_pbPing_clicked();
    void on_pbTest_clicked(bool checked);
    void on_comboBox_currentIndexChanged(int index);

signals:
    void Start();

private:
    Ui::MainWindow* ui;

    void writeSettings();
    void readSettings();

    void finished();

    void messageR(const QString& text);
    void messageG(const QString& text);
    void messageB(const QString& text);
    void autoRunTest(const Elemer::RawAdcData& rawAdcData);
    //    int status = 0;
    //    int avg = -1;
    //    int stage = 0;
    //    int counter = 0;
    QTimer getValuesTimer;
    AdcDataModel* const model;
    TesterTh* tester;
    QSound no;
    QSound yes;
    bool autoRunTestFl {};
    QMessageBox msgBox;
};

#endif // MAINWINDOW_H
