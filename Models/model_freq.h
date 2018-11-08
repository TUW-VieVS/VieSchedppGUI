#ifndef MODEL_FREQ_H
#define MODEL_FREQ_H

#include <QAbstractItemModel>
#include <QFont>
#include "../VieSchedpp/Freq.h"

class Model_Freq : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit Model_Freq(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void setFreq(const std::shared_ptr<VieVS::Freq> &data);

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

};

#endif // MODEL_FREQ_H
