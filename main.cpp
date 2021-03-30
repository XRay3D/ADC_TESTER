#include "devices/devices.h"
#include "mainwindow.h"

#include <QApplication>
#include <QSettings>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QApplication::setOrganizationName("XrSoft");
    QApplication::setApplicationName("ADC");

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());

    [[maybe_unused]] Devices dev;
    [[maybe_unused]] Devices dev2;

    MainWindow w;
    w.show();

    return a.exec();
}
