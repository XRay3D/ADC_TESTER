#pragma once

#include <ed_device.h>

class Irt5920 final : public Elemer::Device {
    Q_OBJECT

public:
    Irt5920(QObject* parent = nullptr);

    Elemer::DeviceType type() const override { return Elemer::IRT_5920; }

    bool getVal();

signals:
    void value(double data);
};
