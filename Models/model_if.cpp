#include "model_if.h"

QVector<QString> Model_If::polarizations_ = { QString::fromStdString(VieVS::If::toString(VieVS::If::Polarization::R)),
                                              QString::fromStdString(VieVS::If::toString(VieVS::If::Polarization::L)),
                                              QString::fromStdString(VieVS::If::toString(VieVS::If::Polarization::X)),
                                              QString::fromStdString(VieVS::If::toString(VieVS::If::Polarization::Y)),
                                              QString::fromStdString(VieVS::If::toString(VieVS::If::Polarization::H)),
                                              QString::fromStdString(VieVS::If::toString(VieVS::If::Polarization::V))};

QVector<QString> Model_If::netSidebans_ = {QString::fromStdString(VieVS::If::toString(VieVS::If::Net_sidband::U)),
                                           QString::fromStdString(VieVS::If::toString(VieVS::If::Net_sidband::L)),
                                           QString::fromStdString(VieVS::If::toString(VieVS::If::Net_sidband::D))
                                           };

Model_If::Model_If(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void Model_If::setIf(const std::shared_ptr<VieVS::If> &data)
{
    data_ = data;
    layoutChanged();
}

int Model_If::rowCount(const QModelIndex &parent) const
{
    if(data_ == nullptr){
        return 0;
    }
    return data_->getIf_defs().size();
}

int Model_If::columnCount(const QModelIndex &parent) const
{
    return 7;
}

QVariant Model_If::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("IF_ID");
            case 1:
                return QString("Physical IF");
            case 2:
                return QString("Polarization");
            case 3:
                return QString("Total LO");
            case 4:
                return QString("Net SB");
            case 5:
                return QString("P-cal freq spacing");
            case 6:
                return QString("P-cal base freq");
            }
        }

    }
    if(role == Qt::FontRole){
        if (orientation == Qt::Horizontal) {
            if (section == 0){
                QFont boldFont;
                boldFont.setBold(true);
                return boldFont;
            }
        }
    }

    return QVariant();
}

QVariant Model_If::data(const QModelIndex &index, int role) const
{
    if(data_ == nullptr){
        return QVariant();
    }

    int row = index.row();
    int col = index.column();
    const auto &d = data_->getIf_defs();

    if (role == Qt::DisplayRole){
        switch (col) {
        case 0:
            return QString::fromStdString(d.at(row).getName());
        case 1:
            return QString::fromStdString(d.at(row).physical_name_);
        case 2:
            return QString::fromStdString(VieVS::If::toString(d.at(row).polarization_));
        case 3:
            return QString("%1 [MHz]").arg(d.at(row).total_lo_);
        case 4:
            return QString::fromStdString(VieVS::If::toString(d.at(row).net_sidband_));
        case 5:
            return QString("%1 [MHz]").arg(d.at(row).phase_cal_freq_spacing_);
        case 6:
            return QString("%1 [Hz]").arg(d.at(row).phase_cal_base_frequency_);
        }
    }
    if(role == Qt::FontRole){
        if (col == 0){
            QFont boldFont;
            boldFont.setBold(true);
            return boldFont;
        }
    }
    if(role == Qt::TextAlignmentRole){
        if (col == 3 || col == 5 || col == 6){
            return Qt::AlignRight + Qt::AlignVCenter;
        }
        if (col == 2 || col == 4){
            return Qt::AlignCenter;
        }
    }

    return QVariant();
}

bool Model_If::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(data_ == nullptr){
        return false;
    }

    int row = index.row();
    int col = index.column();
    auto &d = data_->refIf_defs();

    if (data(index, role) != value) {
        switch (col) {
        case 0:{

            return true;
        }
        case 1:{
            if(value.toString().isEmpty()){
                break;
            }
            d.at(row).physical_name_ = value.toString().toStdString();
            d.at(row).changeName(std::string("&IF_")+d.at(row).physical_name_);
            emit idChanged();
            break;
        }
        case 2:{
            int idx =value.toInt();

            switch(idx){
            case 0:
                d.at(row).polarization_ = VieVS::If::Polarization::R;
                break;
            case 1:
                d.at(row).polarization_ = VieVS::If::Polarization::L;
                break;
            case 2:
                d.at(row).polarization_ = VieVS::If::Polarization::X;
                break;
            case 3:
                d.at(row).polarization_ = VieVS::If::Polarization::Y;
                break;
            case 4:
                d.at(row).polarization_ = VieVS::If::Polarization::H;
                break;
            case 5:
                d.at(row).polarization_ = VieVS::If::Polarization::V;
                break;
            }
            break;
        }
        case 3:{
            d.at(row).total_lo_ = value.toDouble();
            break;
        }
        case 4:{
            int idx =value.toInt();

            switch(idx){
            case 0:
                d.at(row).net_sidband_ = VieVS::If::Net_sidband::U;
                break;
            case 1:
                d.at(row).net_sidband_ = VieVS::If::Net_sidband::L;
                break;
            case 2:
                d.at(row).net_sidband_ = VieVS::If::Net_sidband::D;
                break;
            }
            break;
        }
        case 5:{
            d.at(row).phase_cal_freq_spacing_ = value.toDouble();
            break;
        }
        case 6:{
            d.at(row).phase_cal_base_frequency_ = value.toDouble();
            break;
        }
        }


        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags Model_If::flags(const QModelIndex &index) const
{

    int col = index.column();
    switch (col) {
    case 0:
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    case 1:
        return  Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    case 2:
        return  Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    case 3:
        return  Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    case 4:
        return  Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    case 5:
        return  Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    case 6:
        return  Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }

    return Qt::NoItemFlags;

}

bool Model_If::insertRows(int row, int count, const QModelIndex &parent)
{
    if(data_ == nullptr){
        return false;
    }
    auto &d = data_->refIf_defs();

    bool noSelection = false;
    if(row == -1){
        noSelection = true;
        row = d.size();
    }

    beginInsertRows(parent, row, row + count - 1);
    if(noSelection){
        std::string name = "&IF_" + std::to_string(row);
        d.emplace_back(name,"",VieVS::If::Polarization::R, 0, VieVS::If::Net_sidband::U, 1, 0);
    }else{
        const auto &before = d.at(row);
        d.insert(d.begin()+row+1, before);
    }
    endInsertRows();
//    updateNames();
    return true;
}

bool Model_If::removeRows(int row, int count, const QModelIndex &parent)
{
    if(data_ == nullptr){
        return false;
    }
    beginRemoveRows(parent, row, row + count - 1);
    auto &d = data_->refIf_defs();
    d.erase(d.begin()+row);
    endRemoveRows();
//    updateNames();
    layoutChanged();

    emit idChanged();
    return true;
}

void Model_If::updateNames()
{

    auto &d = data_->refIf_defs();
    for(int i = 0; i<d.size(); ++i){
        std::string name = "&IF_" + std::to_string(i);
        d[i].changeName(name);
    }
}
