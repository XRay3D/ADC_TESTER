#pragma once

#include <ed_device.h>

[[maybe_unused]] constexpr double threshold = 0.00009; //0.0001

#pragma pack(push, 1)
struct RawAdcData {
    uint8_t ready;
    uint8_t stat;
    float v1;
    float v2;
    float v3;
    uint8_t controlSum;

    bool getReady() const noexcept { return !(ready & 0x01); }
    bool csOk() const noexcept {
        uint8_t cs = 0;
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(this);
        for(size_t K = 1; K < sizeof(RawAdcData) - 1; ++K)
            cs = cs + ptr[K];
        cs = ~cs;
        return (cs == controlSum);
    }
    friend auto operator-(const RawAdcData& l, const RawAdcData& r) {
        return RawAdcData{{}, {}, l.v1 - r.v1, l.v2 - r.v2, l.v3 - r.v3, {}};
    }
    friend auto operator*(const RawAdcData& l, double r) {
        return RawAdcData{{}, {}, static_cast<float>(l.v1 * r), static_cast<float>(l.v2 * r), static_cast<float>(l.v3 * r), {}};
    }
};
#pragma pack(pop)

class Irt5501 final : public Elemer::Device {
    Q_OBJECT

public:
    Irt5501(QObject* parent = nullptr);
    Elemer::DeviceType type() const override { return Elemer::IRT_5501; }

    bool getAdcRawData();

signals:
    void rawAdcData(const RawAdcData&);
};

int inline id1 = qRegisterMetaType<RawAdcData>("RawAdcData");
int inline id2 = qRegisterMetaType<RawAdcData*>("RawAdcData*");
