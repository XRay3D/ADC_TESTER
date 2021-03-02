#pragma once

#include <ascii_device.h>

class Irt5920 final : public Elemer::AsciiDevice {
    Q_OBJECT

public:
    Irt5920(QObject* parent = nullptr);

    Elemer::DeviceType type() const override { return Elemer::IRT5920; }

    void getVal();
    bool getValB(double& data);

signals:
    void Val(double data);
};
