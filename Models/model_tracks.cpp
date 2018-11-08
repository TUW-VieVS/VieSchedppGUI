#include "model_tracks.h"

QVector<QString> Model_Tracks::bitstream_ = {QString::fromStdString(VieVS::Track::toString(VieVS::Track::Bitstream::sign)),
                                             QString::fromStdString(VieVS::Track::toString(VieVS::Track::Bitstream::mag))
                                             };


Model_Tracks::Model_Tracks(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void Model_Tracks::setTracks(const std::shared_ptr<VieVS::Track> &data)
{
    data_ = data;
    int max = 1;
    for(const auto &any : data_->getFanout_defs()){
        if(any.second_multiplex_track_>=0){
            max = std::max({max,2});
        }
        if(any.third_multiplex_track_>=0){
            max = std::max({max,3});
        }
        if(any.fourth_multiplex_track_>=0){
            max = std::max({max,4});
        }
    }
    fanout_ = max;
    layoutChanged();
}

QVariant Model_Tracks::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("subpass");
            case 1:
                return QString("trks ID");
            case 2:
                return QString("bitstream");
            case 3:
                return QString("headstack number");
            case 4:
                return QString("trk 1");
            case 5:
                return QString("trk 2");
            case 6:
                return QString("trk 3");
            case 7:
                return QString("trk 4");
            }
        }
    }
    return QVariant();
}


int Model_Tracks::rowCount(const QModelIndex &parent) const
{
    if(data_ == nullptr){
        return 0;
    }
    return data_->getFanout_defs().size();
}

int Model_Tracks::columnCount(const QModelIndex &parent) const
{
    return 4+fanout_;
}

QVariant Model_Tracks::data(const QModelIndex &index, int role) const
{
    if(data_ == nullptr){
        return QVariant();
    }
    int row = index.row();
    int col = index.column();
    const auto &d = data_->getFanout_defs();

    if (role == Qt::DisplayRole){
        switch (col) {
        case 0:
            return QString::fromStdString(d.at(row).subpass_);
        case 1:
            return QString::fromStdString(d.at(row).trksid_);
        case 2:
            return QString::fromStdString(VieVS::Track::toString(d.at(row).bitstream_));
        case 3:
            return d.at(row).headstack_number_;
        case 4:
            if(d.at(row).first_multiplex_track_<0){
                return "";
            }else{
                return d.at(row).first_multiplex_track_;
            }
        case 5:
            if(d.at(row).second_multiplex_track_<0){
                return "";
            }else{
                return d.at(row).second_multiplex_track_;
            }
        case 6:
            if(d.at(row).third_multiplex_track_<0){
                return "";
            }else{
                return d.at(row).third_multiplex_track_;
            }
        case 7:
            if(d.at(row).fourth_multiplex_track_<0){
                return "";
            }else{
                return d.at(row).fourth_multiplex_track_;
            }
        }
    }
    if(role == Qt::TextAlignmentRole){
        if (col == 0 || col == 2 || col == 3 || col == 4 || col == 5 || col == 6 || col == 7){
            return Qt::AlignCenter;
        }
    }

    return QVariant();
}

bool Model_Tracks::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(data_ == nullptr){
        return false;
    }

    int row = index.row();
    int col = index.column();
    auto &d = data_->refFanout_defs();

    if (data(index, role) != value) {
        switch (col) {
        case 0:{
            if(value.toString().isEmpty()){
                break;
            }
            d.at(row).subpass_ = value.toString().toStdString();
            break;
        }
        case 1:{
            if(value.toString().isEmpty()){
                break;
            }
            d.at(row).trksid_ = value.toString().toStdString();
            break;
        }
        case 2:{
            int idx =value.toInt();

            switch(idx){
            case 0:
                d.at(row).bitstream_ = VieVS::Track::Bitstream::sign;
                break;
            case 1:
                d.at(row).bitstream_ = VieVS::Track::Bitstream::mag;
                break;
            }
            break;
        }
        case 3:{
            d.at(row).headstack_number_ = value.toInt();
            break;
        }
        case 4:{
            d.at(row).first_multiplex_track_ = value.toInt();
            break;
        }
        case 5:{
            d.at(row).second_multiplex_track_ = value.toInt();
            break;
        }
        case 6:{
            d.at(row).third_multiplex_track_ = value.toInt();
            break;
        }
        case 7:{
            d.at(row).fourth_multiplex_track_ = value.toInt();
            break;
        }
        }

        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags Model_Tracks::flags(const QModelIndex &index) const
{
    return  Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool Model_Tracks::insertRows(int row, int count, const QModelIndex &parent)
{
    if(data_ == nullptr){
        return false;
    }
    auto &d = data_->refFanout_defs();

    bool noSelection = false;
    if(row == -1){
        noSelection = true;
        row = d.size();
    }

    beginInsertRows(parent, row, row + count - 1);
    if(noSelection){
        d.emplace_back("A", "", VieVS::Track::Bitstream::sign, 1, -1, -1, -1, -1);
    }else{
        const auto &before = d.at(row);
        d.insert(d.begin()+row+1, before);
    }
    endInsertRows();
    return true;
}

bool Model_Tracks::removeRows(int row, int count, const QModelIndex &parent)
{
    if(data_ == nullptr){
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);
    auto &d = data_->refFanout_defs();
    d.erase(d.begin()+row);
    endRemoveRows();
    layoutChanged();
    return true;
}
