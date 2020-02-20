#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <boost/property_tree/ptree.hpp>
#include "QStandardItemModel"
#include <QTreeWidgetItem>
#include "QDialog"

namespace Ui {
class Simulator;
}

class Simulator : public QWidget
{
    Q_OBJECT

public:
    explicit Simulator(QStandardItemModel *model, QWidget *parent = nullptr);
    ~Simulator();
    boost::property_tree::ptree toXML();
    void fromXML(const boost::property_tree::ptree &tree);

public slots:
    void addStations(QStandardItem * = nullptr);

private slots:
    void toggleAll(QTreeWidgetItem *, int column);

private:
    Ui::Simulator *ui;
    QStandardItemModel *model_;
};

#endif // SIMULATOR_H
