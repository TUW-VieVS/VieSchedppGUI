#ifndef SOLVER_H
#define SOLVER_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <boost/property_tree/ptree.hpp>
#include "QStandardItemModel"
#include <QTreeWidgetItem>
#include "QDialog"

namespace Ui {
class Solver;
}

class Solver : public QWidget
{
    Q_OBJECT

public:
    explicit Solver(QStandardItemModel *model, QWidget *parent = nullptr);
    ~Solver();
    boost::property_tree::ptree toXML();
    void fromXML(const boost::property_tree::ptree &tree);

public slots:
    void addStations(QStandardItem * = nullptr);

private slots:
    void toggleAll_sta_coord(QTreeWidgetItem *, int column);
    void toggleAll_sta_clock(QTreeWidgetItem *, int column);
    void toggleAll_sta_tropo(QTreeWidgetItem *, int column);

private:
    Ui::Solver *ui;
    QStandardItemModel *model_;
};

#endif // SOLVER_H
