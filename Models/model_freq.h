#ifndef MODEL_FREQ_H
#define MODEL_FREQ_H

#include <QAbstractItemModel>
#include <QFont>
#include <QStringListModel>
#include "../VieSchedpp/Freq.h"

class Model_Freq : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit Model_Freq(QStringListModel *band, QStringListModel *channels, QStringListModel *bbcs, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void setFreq(const std::shared_ptr<VieVS::Freq> &data);

    int getNrOfItems(){
        return data_->getChan_defs().size();
    }

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

    static const QVector<QString> &getNetSidebands(){
        return netSidebans_;
    }

private:

    std::shared_ptr<VieVS::Freq> data_;
    static QVector<QString> netSidebans_;

    QStringListModel *band_;
    QStringListModel *channels_;
    QStringListModel *bbcs_;

};

#endif // MODEL_FREQ_H
