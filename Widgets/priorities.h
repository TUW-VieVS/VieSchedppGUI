#ifndef PRIORITIES_H
#define PRIORITIES_H

#include <QWidget>
#include "QStandardItemModel"
#include <boost/property_tree/ptree.hpp>
#include "QProgressBar"
#include "QDoubleSpinBox"
#include <cmath>

namespace Ui {
class Priorities;
}

class Priorities : public QWidget
{
    Q_OBJECT

public:
    explicit Priorities(QStandardItemModel *stations, QStandardItemModel *sources, QWidget *parent = nullptr);
    ~Priorities();
    boost::property_tree::ptree toXML();
    void fromXML(const boost::property_tree::ptree &tree);

public slots:
    void addStations(QStandardItem * = nullptr);
    void addSources(QStandardItem * = nullptr);

private:
    Ui::Priorities *ui;
    QStandardItemModel *stations_;
    QStandardItemModel *sources_;

    bool blockPaint = false;
    void addRow(QString name, double val=1);

private slots:
    void paintBars();
    void setup();

    void averageEOP();
    void averageSta();
    void averageSrc();
};

#endif // PRIORITIES_H
