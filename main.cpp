#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
#include <hw/interface.h>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QApplication::setOrganizationName("XrSoft");
    QApplication::setApplicationName("ADC");

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());

    mi mi_;
    Q_UNUSED(mi_)

    MainWindow w;
    w.show();

    return a.exec();
}
