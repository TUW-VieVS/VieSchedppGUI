#include "model_bbc.h"

Model_Bbc::Model_Bbc(QObject *parent)
    : QAbstractTableModel(parent), ifs_{nullptr}
{
}

Model_Bbc::Model_Bbc(QStringListModel *ifs, QObject *parent)
    : QAbstractTableModel(parent), ifs_{ifs}
{
}

void Model_Bbc::setBbc(const std::shared_ptr<VieVS::Bbc> &data)
{
    data_ = data;
    layoutChanged();
}

QVariant Model_Bbc::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("BBC ID");
            case 1:
                return QString("Physical BBC#");
            case 2:
                return QString("IF ID");
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


int Model_Bbc::rowCount(const QModelIndex &parent) const
{
    if(data_ == nullptr){
        return 0;
    }
    return data_->getBbc_assigns().size();
}

int Model_Bbc::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant Model_Bbc::data(const QModelIndex &index, int role) const
{
    if(data_ == nullptr){
        return QVariant();
    }
    int row = index.row();
    int col = index.column();
    const auto &d = data_->getBbc_assigns();

    if (role == Qt::DisplayRole){
        switch (col) {
        case 0:
            return QString::fromStdString(d.at(row).getName());
        case 1:
            return d.at(row).physical_bbc_number_;
        case 2:{
            QString name = QString::fromStdString(d.at(row).if_name_);
            if(ifs_ != nullptr && ifs_->stringList().indexOf(name) == -1){
                return "undefined";
            }
            return QString::fromStdString(d.at(row).if_name_);
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
        if (col == 1){
            return Qt::AlignCenter;
        }
    }

    return QVariant();
}

bool Model_Bbc::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(data_ == nullptr){
        return false;
    }
    int row = index.row();
    int col = index.column();
    auto &d = data_->refBbc_assigns();

    if (data(index, role) != value) {
        switch (col) {
        case 0:{

            return true;
        }
        case 1:{
            d.at(row).physical_bbc_number_ = value.toInt();
            break;
        }
        case 2:{
            int idx =value.toInt();
            if(idx>=0){
                QString name = ifs_->stringList().at(idx);
                d.at(row).if_name_ = name.toStdString();
            }
            break;
        }
        }

        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags Model_Bbc::flags(const QModelIndex &index) const
{
    int col = index.column();
    switch (col) {
    case 0:
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    case 1:
        return  Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    case 2:
        return  Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

bool Model_Bbc::insertRows(int row, int count, const QModelIndex &parent)
{
    if(data_ == nullptr){
        return false;
    }
    auto &d = data_->refBbc_assigns();

    bool noSelection = false;
    if(row == -1){
        noSelection = true;
        row = d.size();
    }

    beginInsertRows(parent, row, row + count - 1);
    if(noSelection){
        std::string name = "$BBC" + std::to_string(row);
        d.emplace_back(name,0,"");
    }else{
        const auto &before = d.at(row);
        d.insert(d.begin()+row+1, before);
    }
    endInsertRows();
    updateNames();
    emit idChanged();
    return true;
}

bool Model_Bbc::removeRows(int row, int count, const QModelIndex &parent)
{
    if(data_ == nullptr){
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);
    auto &d = data_->refBbc_assigns();
    d.erase(d.begin()+row);
    endRemoveRows();
    updateNames();
    layoutChanged();
    emit idChanged();
    return true;
}

void Model_Bbc::updateNames()
{
    auto &d = data_->refBbc_assigns();
    for(int i = 0; i<d.size(); ++i){
        std::string name = QString("&BBC%1").arg(i+1,2,10,QChar('0')).toStdString();
        d[i].changeName(name);
    }
}
