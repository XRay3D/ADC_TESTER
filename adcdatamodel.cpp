#include "adcdatamodel.h"

AdcDataModel::AdcDataModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int AdcDataModel::rowCount(const QModelIndex&) const { return RowCount; }

int AdcDataModel::columnCount(const QModelIndex&) const { return 2; }

QVariant AdcDataModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole)
        switch (index.row()) {
        case AdcStatus: {
            QString str = QString::number(m_rawAdcData[index.column()].stat, 2);
            str = QString("00000000").left(8 - str.length()) + str;
            return str; //m_rawAdcData.stat;
        }
        case AdcVoltage1:
            return !index.column() ? QVariant { m_rawAdcData[index.column()].v1 }
                                   : QVariant { QString::number(m_rawAdcData[index.column()].v1, 'g', 4).replace('.', ',') };
        case AdcVoltage2:
            return !index.column() ? QVariant { m_rawAdcData[index.column()].v2 }
                                   : QVariant { QString::number(m_rawAdcData[index.column()].v2, 'g', 4).replace('.', ',') };
        case AdcVoltage3:
            return !index.column() ? QVariant { m_rawAdcData[index.column()].v3 }
                                   : QVariant { QString::number(m_rawAdcData[index.column()].v3, 'g', 4).replace('.', ',') };
        case Voltage:
            return !index.column() ? QVariant { m_voltage[index.column()] }
                                   : QVariant { QString::number(m_voltage[index.column()], 'g', 4).replace('.', ',') };
        case Current:
            return !index.column() ? QVariant { m_current[index.column()] }
                                   : QVariant { QString::number(m_current[index.column()], 'g', 4).replace('.', ',') };
        default:
            return {};
        }
    else if (role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;
    return {};
}

QVariant AdcDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static const QStringList vData {
        "Статус",
        "Измеренное значение канала 1",
        "Измеренное значение канала 2",
        "Измеренное значение канала 3",
        "Выходгое напряжение, В",
        "Ток потребления, мА",
    };
    static const QStringList hData {
        "Значения",
        "Δ, x 1000",
    };
    if (role == Qt::DisplayRole) {
        return orientation == Qt::Horizontal ? hData[section]
                                             : vData[section];
    } else if (role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;
    return {};
}

Qt::ItemFlags AdcDataModel::flags(const QModelIndex&) const
{
    return Qt::ItemIsEnabled;
}

void AdcDataModel::setVoltage(double voltage)
{
    m_voltage[1] = (m_voltage[0] - voltage) * 1000.0;
    m_voltage[0] = voltage;
    emit dataChanged(createIndex(Voltage, 0), createIndex(Voltage, 1), { Qt::DisplayPropertyRole });
}

void AdcDataModel::setCurrent(double current)
{
    m_current[1] = (m_current[0] - current) * 1000.0;
    m_current[0] = current;
    emit dataChanged(createIndex(Current, 0), createIndex(Current, 1), { Qt::DisplayPropertyRole });
}

auto operator-(const Elemer::RawAdcData& l, const Elemer::RawAdcData& r)
{
    return Elemer::RawAdcData { {}, {},
        l.v1 - r.v1,
        l.v2 - r.v2,
        l.v3 - r.v3, {} };
}
auto operator*(const Elemer::RawAdcData& l, double r)
{
    return Elemer::RawAdcData { {}, {},
        static_cast<float>(l.v1 * r),
        static_cast<float>(l.v2 * r),
        static_cast<float>(l.v3 * r), {} };
}
void AdcDataModel::setRawAdcData(const Elemer::RawAdcData& rawAdcData)
{
    m_rawAdcData[1] = (m_rawAdcData[0] - rawAdcData) * 1000.0;
    m_rawAdcData[0] = rawAdcData;
    emit dataChanged(createIndex(AdcStatus, 0), createIndex(AdcVoltage3, 1), { Qt::DisplayPropertyRole });
}

Elemer::RawAdcData AdcDataModel::rawAdcData() const { return m_rawAdcData[0]; }

double AdcDataModel::voltage() const { return m_voltage[0]; }

double AdcDataModel::current() const { return m_current[0]; }
