#include "adcswitch.h"
#include "my_protocol.h"
#include "mcc_generated_files/mcc.h"

static const uint8_t StepMotor[][8] = {
    {
        // forward
        0b00000001, // 0x01
        0b00000011, // 0x03
        0b00000010, // 0x02
        0b00000110, // 0x06
        0b00000100, // 0x04
        0b00001100, // 0x0C
        0b00001000, // 0x08
        0b00001001, // 0x09
    },
    {
        // reverse
        0b00001001, // 0x09
        0b00001000, // 0x08
        0b00001100, // 0x0C
        0b00000100, // 0x04
        0b00000110, // 0x06
        0b00000010, // 0x02
        0b00000011, // 0x03
        0b00000001, // 0x01
    }
};

static int8_t pos = 0;


static const uint8_t k[16]__at(0x100) = {
    37,
    69,
    102,
    139,
    187,
    255
};

void SetAdcSwitch(int8_t adcSwitch) {
    if (pos != adcSwitch) {
        int8_t tmp = adcSwitch - pos;
        pos = adcSwitch;

        uint8_t reverse = (tmp > 0)
                ? (abs(tmp) > 3 ? 0 : 1)
                : (abs(tmp) > 3 ? 8 : 0);

        uint8_t counter = 0;
        uint8_t i = 0;

        do {
            PORTC = reverse ? StepMotor[1][++i & 0x7] : StepMotor[0][++i & 0x7];
            __delay_ms(6);
            int16_t tmp = ADC_GetConversion2();
            if (abs(k[pos] - tmp) < 10)
                ++counter;
            else
                counter = 0;
            //uint8_t data[] = {counter, abs(k[pos] - tmp), tmp, k[pos]};
            //TransmitParcelWD(Text, data, sizeof (data));
        } while (counter < 50);
        PORTC = 0x00;
    }
}

uint8_t GetAdcSwitch(void) {
    return pos;
}

void TestAdcSwitch(void) {
    uint16_t counter = 0;
    while (counter < 1200) {
        PORTC = StepMotor[0][counter++ % 8];
        __delay_ms(8);
        uint8_t data[3];
        *(uint16_t*)(data) = counter;
        data[2] = ADC_GetConversion2();
        TransmitParcelWD(TestSwitch, data, 3);
        __delay_ms(8);
    }
    PORTC = 0x00;
}
