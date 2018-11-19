#ifndef MODEL_IF_H
#define MODEL_IF_H

#include <QAbstractTableModel>
#include <QFont>
#include "../VieSchedpp/ObservingMode/If.h"

class Model_If : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit Model_If(QObject *parent = nullptr);

    void setIf(const std::shared_ptr<VieVS::If> &data);

    int getNrOfItems(){
        return data_->getIf_defs().size();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Header data
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;


    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    static const QVector<QString> &getPolarizations(){
        return polarizations_;
    }

    static const QVector<QString> &getNetSidebands(){
        return netSidebans_;
    }

    void updateNames();

    int nrItems(){
        if(data_ == nullptr){
            return 0;
        }else{
            return data_->getIf_defs().size();
        }
    }

signals:
    void idChanged();

private:
    std::shared_ptr<VieVS::If> data_;

    static QVector<QString> polarizations_;
    static QVector<QString> netSidebans_;

};

#endif // MODEL_IF_H
