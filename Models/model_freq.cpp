#include "model_freq.h"

QVector<QString> Model_Freq::netSidebans_ = {QString::fromStdString(VieVS::Freq::toString(VieVS::Freq::Net_sideband::U)),
                                             QString::fromStdString(VieVS::Freq::toString(VieVS::Freq::Net_sideband::L)),
                                             QString::fromStdString(VieVS::Freq::toString(VieVS::Freq::Net_sideband::UC)),
                                             QString::fromStdString(VieVS::Freq::toString(VieVS::Freq::Net_sideband::LC))
                                             };

Model_Freq::Model_Freq(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QVariant Model_Freq::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Band ID");
            case 1:
                return QString("Sky freq at 0Hz BBC");
            case 2:
                return QString("Net SB");
            case 3:
                return QString("Chan BW");
            case 4:
                return QString("Chan ID");
            case 5:
                return QString("BBC ID");
            case 6:
                return QString("Phase-cal ID");
            }
        }
    }
    return QVariant();
}

void Model_Freq::setFreq(const std::shared_ptr<VieVS::Freq> &data)
{
    data_=data;
    layoutChanged();
}


int Model_Freq::rowCount(const QModelIndex &parent) const
{
    if(data_ == nullptr){
        return 0;
    }
    return data_->getChan_defs().size();
}

int Model_Freq::columnCount(const QModelIndex &parent) const
{
    return 7;
}

QVariant Model_Freq::data(const QModelIndex &index, int role) const
{
    if(data_ == nullptr){
        return QVariant();
    }
    int row = index.row();
    int col = index.column();
    const auto &d = data_->getChan_defs();

    if (role == Qt::DisplayRole){
        switch (col) {
        case 0:
            return QString::fromStdString(d.at(row).bandId_);
        case 1:
            return QString("%1 [MHz]").arg(d.at(row).sky_freq_);
        case 2:
            return QString::fromStdString(VieVS::Freq::toString(d.at(row).net_sideband_));
        case 3:
            return QString("%1 [MHz]").arg(d.at(row).chan_bandwidth_);
        case 4:
            return QString::fromStdString(d.at(row).chan_id_);
        case 5:
            return QString::fromStdString(d.at(row).bbc_id_);
        case 6:
            return QString::fromStdString(d.at(row).phase_cal_id_);
        }
    }
    if(role == Qt::TextAlignmentRole){
        if (col == 1 || col == 3){
            return Qt::AlignRight + Qt::AlignVCenter;
        }
        if (col == 0 || col == 3){
            return Qt::AlignCenter;
        }
    }

    return QVariant();
}

bool Model_Freq::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(data_ == nullptr){
        return false;
    }
    int row = index.row();
    int col = index.column();
    auto &d = data_->refChan_defs();

    if (data(index, role) != value) {
        switch (col) {
        case 0:{
            d.at(row).bandId_ = value.toString().toStdString();
            break;
        }
        case 1:{
            if(value.toString().isEmpty()){
                break;
            }
            d.at(row).sky_freq_ = value.toDouble();
            break;
        }
        case 2:{
            int idx =value.toInt();

            switch(idx){
            case 0:
                d.at(row).net_sideband_ = VieVS::Freq::Net_sideband::U;
                break;
            case 1:
                d.at(row).net_sideband_ = VieVS::Freq::Net_sideband::L;
                break;
            case 2:
                d.at(row).net_sideband_ = VieVS::Freq::Net_sideband::UC;
                break;
            case 3:
                d.at(row).net_sideband_ = VieVS::Freq::Net_sideband::LC;
                break;
            }
            break;
        }
        case 3:{
            d.at(row).chan_bandwidth_ = value.toDouble();
            break;
        }
        case 4:{
            d.at(row).chan_id_ = value.toString().toStdString();
            break;
        }
        case 5:{
            d.at(row).bbc_id_ = value.toString().toStdString();
            break;
        }
        case 6:{
            d.at(row).phase_cal_id_ = value.toString().toStdString();
            break;
        }
        }

        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags Model_Freq::flags(const QModelIndex &index) const
{
    return  Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool Model_Freq::insertRows(int row, int count, const QModelIndex &parent)
{
    if(data_ == nullptr){
        return false;
    }
    auto &d = data_->refChan_defs();

    bool noSelection = false;
    if(row == -1){
        noSelection = true;
        row = d.size();
    }

    beginInsertRows(parent, row, row + count - 1);
    if(noSelection){
        d.emplace_back("", 0, VieVS::Freq::Net_sideband::U, 0, "", "", "");
    }else{
        const auto &before = d.at(row);
        d.insert(d.begin()+row+1, before);
    }
    endInsertRows();
    return true;
}

bool Model_Freq::removeRows(int row, int count, const QModelIndex &parent)
{
    if(data_ == nullptr){
        return false;
    }
    beginRemoveRows(parent, row, row + count - 1);
    auto &d = data_->refChan_defs();
    d.erase(d.begin()+row);
    endRemoveRows();
    layoutChanged();
    return true;
}

