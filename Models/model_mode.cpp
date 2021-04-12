#include "model_mode.h"

Model_Mode::Model_Mode(const QVector<QString> &stations, QObject *parent)
    : QAbstractTableModel(parent), stations_{stations}
{
}

void Model_Mode::setMode(std::shared_ptr<VieVS::Mode> data)
{
    data_ = data;
    layoutChanged();
}

QVariant Model_Mode::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Station");
            case 1:
                return QString("FREQ");
            case 2:
                return QString("BBC");
            case 3:
                return QString("IF");
            case 4:
                return QString("TRACKS");
            case 5:
                return QString("PHASE_CAL_DETECT");
            case 6:
                return QString("Track Frame Format");
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


int Model_Mode::rowCount(const QModelIndex &parent) const
{
    if (data_ == nullptr){
        return 0;
    }
    return data_->getNSta();
}

int Model_Mode::columnCount(const QModelIndex &parent) const
{
    return 7;
}

QVariant Model_Mode::data(const QModelIndex &index, int role) const
{
    if(data_ == nullptr){
        return QVariant();
    }
    int row = index.row();
    int col = index.column();

    if (role == Qt::DisplayRole){
        switch (col) {
        case 0:
            return stations_.at(row);
        case 1:
            if(data_->getFreq(row).is_initialized()){
                return QString::fromStdString(data_->getFreq(row).get()->getName());
            }else{
                return "undefined";
            }
        case 2:
            if(data_->getBbc(row).is_initialized()){
                return QString::fromStdString(data_->getBbc(row).get()->getName());
            }else{
                return "undefined";
            }
        case 3:
            if(data_->getIf(row).is_initialized()){
                return QString::fromStdString(data_->getIf(row).get()->getName());
            }else{
                return "undefined";
            }
        case 4:
            if(data_->getTracks(row).is_initialized()){
                return QString::fromStdString(data_->getTracks(row).get()->getName());
            }else{
                return "undefined";
            }
        case 5:
            return "Standard";
        case 6:
            if(data_->getTrackFrameFormat(row).is_initialized()){
                try {
                    return QString::fromStdString(*data_->getTrackFrameFormat(row).get());
                } catch (...) {
                    return "undefined";
                }
            }else{
                return "undefined";
            }
        }
    }
    if(role == Qt::FontRole){
        if (col == 0){
            QFont boldFont;
            boldFont.setBold(true);
            return boldFont;
        }
        if(data(index).toString() == "undefined"){
            QFont boldFont;
            boldFont.setBold(true);
            return boldFont;
        }

    }
    if(role == Qt::ForegroundRole){
        if(data(index).toString() == "undefined"){
            return QVariant::fromValue(QColor(Qt::red));
        }
    }
    if(role == Qt::TextAlignmentRole){
        if (col > 0){
            return Qt::AlignRight + Qt::AlignVCenter;
        }
    }


    return QVariant();
}

bool Model_Mode::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(data_ == nullptr){
        return false;
    }

    int row = index.row();
    int col = index.column();

    if (data(index, role) != value) {
        switch (col) {
        case 0:{
            break;
        }
        case 1:{ // FREQ
            int idx =value.toInt();
            data_->changeFreq(idx,row);
            break;
        }
        case 2:{ // BBC
            int idx =value.toInt();
            data_->changeBbc(idx,row);
            break;
        }
        case 3:{ // IF
            int idx =value.toInt();
            data_->changeIf(idx,row);
            break;
        }
        case 4:{ // TRACKS
            int idx =value.toInt();
            data_->changeTracks(idx,row);
            break;
        }
        case 5:{ // PHASE CAL
            int idx =value.toInt();

            break;
        }
        case 6:{ // TRACK FRAME FORMAT
            int idx =value.toInt();
            data_->changeTrackFrameFormat(idx,row);
            break;
        }
        }

        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags Model_Mode::flags(const QModelIndex &index) const
{
    int col = index.column();
    if(col == 0){
        return  Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

    return  Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}
