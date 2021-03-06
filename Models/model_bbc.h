#ifndef MODEL_BBC_H
#define MODEL_BBC_H

#include <QAbstractTableModel>
#include <QFont>
#include <QColor>
#include <QStringListModel>
#include "../VieSchedpp/ObservingMode/Bbc.h"

class Model_Bbc : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit Model_Bbc(QObject *parent = nullptr);

    Model_Bbc(QStringListModel *ifs, QObject *parent = nullptr);

    void setBbc(const std::shared_ptr<VieVS::Bbc> &data);

    int getNrOfItems(){
        return data_->getBbc_assigns().size();
    }

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void updateNames();

    int nrItems(){
        if(data_ == nullptr){
            return 0;
        }else{
            return data_->getBbc_assigns().size();
        }
    }

signals:
    void idChanged();

private:
    std::shared_ptr<VieVS::Bbc> data_;

    QStringListModel *ifs_;

};

#endif // MODEL_BBC_H
