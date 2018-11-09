#ifndef MODEL_TRACKS_H
#define MODEL_TRACKS_H

#include <QAbstractTableModel>
#include <QFont>
#include "../VieSchedpp/Track.h"


class Model_Tracks : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit Model_Tracks(QObject *parent = nullptr);

    void setTracks(const std::shared_ptr<VieVS::Track> &data);

    int getNrOfItems(){
        return data_->getFanout_defs().size();
    }

    void setFanout(int i){
        fanout_ = i;
        layoutChanged();
    }

    int getFanout() const{
        return fanout_;
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

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    static const QVector<QString> &getBitstreams(){
        return bitstream_;
    }

signals:
    void idChanged();

private:
    std::shared_ptr<VieVS::Track> data_;
    int fanout_ = 1;

    static QVector<QString> bitstream_;

};

#endif // MODEL_TRACKS_H
