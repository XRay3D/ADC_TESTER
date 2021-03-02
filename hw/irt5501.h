#pragma once

#include <ascii_device.h>

class ElemerPort;

class Irt5501 final : public Elemer::AsciiDevice {
    Q_OBJECT

public:
    Irt5501(QObject* parent = nullptr);

    Elemer::DeviceType type() const override { return Elemer::IRT5501; }

    void getAdcRawData();
    bool getAdcRawDataB(Elemer::RawAdcData& data);

signals:
    void Raw(const Elemer::RawAdcData&);
};
