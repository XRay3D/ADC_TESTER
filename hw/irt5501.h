#pragma once

#include <device.h>

class ElemerPort;

[[maybe_unused]] constexpr double threshold = 0.00009;

#pragma pack(push, 1)

struct RawAdcData {
    uint8_t ready;
    uint8_t stat;
    float v1;
    float v2;
    float v3;
    uint8_t controlSum;

    bool getReady() const noexcept { return !(ready & 0x01); }
    bool csOk() const noexcept
    {
        uint8_t cs = 0;
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(this);
        for (size_t K = 1; K < sizeof(RawAdcData) - 1; ++K)
            cs = cs + ptr[K];
        cs = ~cs;
        return (cs == controlSum);
    }
};

#pragma pack(pop)

int inline id1 = qRegisterMetaType<RawAdcData>("RawAdcData");
int inline id2 = qRegisterMetaType<RawAdcData*>("RawAdcData*");

class Irt5501 final : public Elemer::AsciiDevice {
    Q_OBJECT

public:
    Irt5501(QObject* parent = nullptr);

    Elemer::DeviceType type() const override { return Elemer::IRT5501; }

    bool getAdcRawData();
    bool getAdcRawDataB(RawAdcData& data);

signals:
    void Raw(const RawAdcData&);
};
