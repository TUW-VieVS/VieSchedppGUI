#ifndef SIMULATORWIDGET_H
#define SIMULATORWIDGET_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <boost/property_tree/ptree.hpp>
#include <QStandardItemModel>
#include <QTreeWidgetItem>
#include <QDialog>
#include <cmath>

namespace Ui {
class SimulatorWidget;
}

class SimulatorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SimulatorWidget(QStandardItemModel *model, QWidget *parent = nullptr);
    ~SimulatorWidget();
    boost::property_tree::ptree toXML();
    void fromXML(const boost::property_tree::ptree &tree);

public slots:
    void addStations(QStandardItem * = nullptr);

private slots:
    void toggleAll(QTreeWidgetItem *, int column);

    void on_pushButton_wnTable_clicked();

private:
    Ui::SimulatorWidget *ui;
    QStandardItemModel *model_;
};

#endif // SIMULATORWIDGET_H
