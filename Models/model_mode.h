#ifndef Model_Mode_H
#define Model_Mode_H

#include <QAbstractTableModel>
#include <QFont>
#include <QColor>
#include "../VieSchedpp/Mode.h"

class Model_Mode : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit Model_Mode(const QVector<QString> &stations, QObject *parent = nullptr);

    void setMode(std::shared_ptr<VieVS::Mode> data);

    void setStations(const QVector<QString> &stations){
        stations_ = stations;
    }

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    QVector<QString> stations_;
    std::shared_ptr<VieVS::Mode> data_;
};

#endif // Model_Mode_H
