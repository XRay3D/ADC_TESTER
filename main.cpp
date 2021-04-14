#include "devices/devices.h"
#include "mainwindow.h"

#include <QApplication>
#include <QSettings>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    QApplication::setOrganizationName("XrSoft");
    QApplication::setApplicationName("ADC");

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, {});

    [[maybe_unused]] Devices dev;

    MainWindow w;
    w.show();

    return a.exec();
}
