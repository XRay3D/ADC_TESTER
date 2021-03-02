#include "adcdatamodel.h"

void AdcDataModel::setVoltage(double voltage)
{
    m_voltage = voltage;
    emit dataChanged(createIndex(Voltage, 0), createIndex(Voltage, 0), { Qt::DisplayPropertyRole });
}

void AdcDataModel::setCurrent(double current)
{
    m_current = current;
    emit dataChanged(createIndex(Current, 0), createIndex(Current, 0), { Qt::DisplayPropertyRole });
}

void AdcDataModel::setRawAdcData(const Elemer::RawAdcData& rawAdcData)
{
    m_rawAdcData = rawAdcData;
    emit dataChanged(createIndex(AdcStatus, 0), createIndex(AdcVoltage3, 0), { Qt::DisplayPropertyRole });
}

AdcDataModel::AdcDataModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int AdcDataModel::rowCount(const QModelIndex&) const
{
    return RowCount;
}

int AdcDataModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QVariant AdcDataModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole)
        switch (index.row()) {
        case AdcStatus:
            return m_rawAdcData.stat;
        case AdcVoltage1:
            return m_rawAdcData.v1;
        case AdcVoltage2:
            return m_rawAdcData.v2;
        case AdcVoltage3:
            return m_rawAdcData.v3;
        case Voltage:
            return m_voltage;
        case Current:
            return m_current;
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
        "Статус АЦП",
        "измеренное напряжение\n"
        "канала 1 АЦП",
        "измеренное напряжение\n"
        "канала 3 АЦП",
        "измеренное напряжение\n"
        "канала 3 АЦП",
        "Выходгое\n"
        "напряжение АЦП, В.",
        "Ток потребления\n"
        "АЦП, мА.",
    };
    if (role == Qt::DisplayRole) {
        return orientation == Qt::Horizontal ? "Значения"
                                             : vData[section];
    } else if (role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;
    return {};
}

Qt::ItemFlags AdcDataModel::flags(const QModelIndex&) const
{
    return Qt::ItemIsEnabled;
}
