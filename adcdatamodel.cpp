#include "adcdatamodel.h"

#include <QColor>

AdcDataModel::AdcDataModel(QObject* parent)
    : QAbstractTableModel(parent) {
}

int AdcDataModel::rowCount(const QModelIndex&) const { return RowCount; }

int AdcDataModel::columnCount(const QModelIndex&) const { return ColumnCount; }

QVariant AdcDataModel::data(const QModelIndex& index, int role) const {
    if(role == Qt::DisplayRole)
        switch(index.row()) {
        case Ready:
            return QChar{m_rawAdcData[0].getReady() ? '+' : '-'};
        case ControlSum:
            return QChar{m_rawAdcData[0].csOk() ? '+' : '-'};
        case AdcStatus: {
            QString str = QString::number(m_rawAdcData[index.column()].stat, 2);
            str = QString("00000000").left(8 - str.length()) + str + '(' + QString::number(m_rawAdcData[index.column()].stat) + ')';
            return str; //m_rawAdcData.stat;
        }
        case AdcVoltage1:
            return QString::number(m_rawAdcData[index.column()].v1, 'f', 6).replace('.', ',');
            //            return !index.column() ? QVariant { m_rawAdcData[index.column()].v1 }
            //                                   : QVariant { QString::number(m_rawAdcData[index.column()].v1, 'f', 6).replace('.', ',') };
        case AdcVoltage2:
            return QString::number(m_rawAdcData[index.column()].v2, 'f', 6).replace('.', ',');
            //            return !index.column() ? QVariant { m_rawAdcData[index.column()].v2 }
            //                                   : QVariant { QString::number(m_rawAdcData[index.column()].v2, 'f', 6).replace('.', ',') };
        case AdcVoltage3:
            return QString::number(m_rawAdcData[index.column()].v3, 'f', 6).replace('.', ',');
            //            return !index.column() ? QVariant { m_rawAdcData[index.column()].v3 }
            //                                   : QVariant { QString::number(m_rawAdcData[index.column()].v3, 'f', 6).replace('.', ',') };
        case Voltage:
            return QString::number(m_voltage[index.column()], 'f', 5).replace('.', ',');
            //            return !index.column() ? QVariant { m_voltage[index.column()] }
            //                                   : QVariant { QString::number(m_voltage[index.column()], 'f', 5).replace('.', ',') };
        case Current:
            return QString::number(m_current[index.column()], 'f', 5).replace('.', ',');
            //            return !index.column() ? QVariant { m_current[index.column()] }
            //                                   : QVariant { QString::number(m_current[index.column()], 'f', 4).replace('.', ',') };
        default:
            return {};
        }
    else if(role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;
    else if(role == Qt::BackgroundColorRole)
        switch(index.row()) {
        case AdcVoltage1:
            return (0.1 > abs(m_rawAdcData[index.column()].v1) && abs(m_rawAdcData[index.column()].v1) > threshold) ? QColor{255, 127, 127}
                                                                                                                    : QColor{127, 255, 127};
        case AdcVoltage2:
            return (0.1 > abs(m_rawAdcData[index.column()].v1) && abs(m_rawAdcData[index.column()].v2) > threshold) ? QColor{255, 127, 127}
                                                                                                                    : QColor{127, 255, 127};
        case AdcVoltage3:
            return (0.1 > abs(m_rawAdcData[index.column()].v3) && abs(m_rawAdcData[index.column()].v3) > threshold) ? QColor{255, 127, 127}
                                                                                                                    : QColor{127, 255, 127};
        }
    return {};
}

QVariant AdcDataModel::headerData(int section, Qt::Orientation orientation, int role) const {
    static const QStringList vData{
        "Готов",
        "Контрольная сумма",
        "Статус",
        "Измеренное значение канала 1",
        "Измеренное значение канала 2",
        "Измеренное значение канала 3",
        "Выходгое напряжение, В",
        "Ток потребления, мА",
    };
    static const QStringList hData{
        "Значения",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
    };
    if(role == Qt::DisplayRole) {
        return orientation == Qt::Horizontal ? hData[section]
                                             : vData[section];
    } else if(role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;
    return {};
}

Qt::ItemFlags AdcDataModel::flags(const QModelIndex&) const {
    return Qt::ItemIsEnabled;
}

void AdcDataModel::setVoltage(double voltage) {
    //    m_voltage[1] = (m_voltage[0] - voltage) * 1000.0;
    m_voltage[0] = voltage;
    emit dataChanged(createIndex(Voltage, 0), createIndex(Voltage, 0), {Qt::DisplayPropertyRole});
}

void AdcDataModel::setCurrent(double current) {
    //    m_current[1] = (m_current[0] - current) * 1000.0;
    m_current[0] = current;
    emit dataChanged(createIndex(Current, 0), createIndex(Current, 0), {Qt::DisplayPropertyRole});
}

void AdcDataModel::setRawAdcData(const RawAdcData& rawAdcData) {
    //    m_rawAdcData[1] = (m_rawAdcData[0] - rawAdcData) * 1000.0;
    m_rawAdcData[0] = rawAdcData;
    emit dataChanged(createIndex(AdcStatus, 0), createIndex(AdcVoltage3, 0), {Qt::DisplayPropertyRole});
}

RawAdcData AdcDataModel::rawAdcData() const { return m_rawAdcData[0]; }

void AdcDataModel::setCurrentTest(int currentTest) {
    m_currentTest = currentTest;
    m_rawAdcData[m_currentTest] = m_rawAdcData[0];
    m_current[m_currentTest] = m_current[0];
    m_voltage[m_currentTest] = m_voltage[0];
    emit dataChanged(createIndex(AdcStatus, m_currentTest), createIndex(AdcVoltage3, m_currentTest), {Qt::DisplayPropertyRole});
}

double AdcDataModel::voltage() const { return m_voltage[0]; }

double AdcDataModel::current() const { return m_current[0]; }
