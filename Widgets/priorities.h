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
    explicit Priorities(QStandardItemModel *model, QWidget *parent = nullptr);
    ~Priorities();
    boost::property_tree::ptree toXML();
    void fromXML(const boost::property_tree::ptree &tree);

public slots:
    void addStations(QStandardItem * = nullptr);

private:
    Ui::Priorities *ui;
    QStandardItemModel *model_;

    void addRow(QString name);

private slots:
    void paintBars();
    void setup();
};

#endif // PRIORITIES_H
