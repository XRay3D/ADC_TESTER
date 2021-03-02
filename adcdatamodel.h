#pragma once

#include <QAbstractTableModel>
#include <common_types.h>
#include <qglobal.h>

typedef struct ADC {
    ADC() { }

    //int status = 0;
    double U1 = 0.0;
    double U2 = 0.0;
    double U3 = 0.0;
    double U = 0.0;
    double I = 0.0;

    void reset()
    {
        //status = 0;
        U1 = 0.0;
        U2 = 0.0;
        U3 = 0.0;
        U = 0.0;
        I = 0.0;
    }

    ADC operator+=(const QVector<double>& v)
    {
        const double* p = v.data();
        U += *p++;
        I += *p++;
        return *this;
    }
    ADC operator+=(const Elemer::RawAdcData& data)
    {
        U1 += data.v1;
        U2 += data.v2;
        U3 += data.v3;
        return *this;
    }
    ADC operator/=(double v)
    {
        U1 /= v;
        U2 /= v;
        U3 /= v;
        U /= v;
        I /= v;
        return *this;
    }
} ADC;

class AdcDataModel : public QAbstractTableModel {
    Q_OBJECT
    Elemer::RawAdcData m_rawAdcData {};
    double m_voltage {};
    double m_current {};

public:
    explicit AdcDataModel(QObject* parent = nullptr);

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& = {}) const override;
    int columnCount(const QModelIndex& = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    void Raw(const Elemer::RawAdcData& raw);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    enum {
        AdcStatus,
        AdcVoltage1,
        AdcVoltage2,
        AdcVoltage3,
        Voltage,
        Current,
        RowCount
    };

    void setVoltage(double voltage);
    void setCurrent(double current);
    void setRawAdcData(const Elemer::RawAdcData& rawAdcData);

signals:
};
