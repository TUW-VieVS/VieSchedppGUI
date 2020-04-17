#ifndef SOLVERWIDGET_H
#define SOLVERWIDGET_H

#include <QWidget>
#include <QWidget>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <boost/property_tree/ptree.hpp>
#include <QStandardItemModel>
#include <QTreeWidgetItem>
#include <QDialog>

namespace Ui {
class SolverWidget;
}

class SolverWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SolverWidget(QStandardItemModel *station_model_, QStandardItemModel *source_model_, QWidget *parent = nullptr);
    ~SolverWidget();
    boost::property_tree::ptree toXML();
    void fromXML(const boost::property_tree::ptree &tree);

public slots:
    void addStations(QStandardItem * = nullptr);
    void addSources(QStandardItem * = nullptr);

private slots:
    void toggleAll_sta_coord(QTreeWidgetItem *, int column);
    void toggleAll_src_coord(QTreeWidgetItem *, int column);

    void toggleAll_sta_clock(QTreeWidgetItem *, int column);
    void toggleAll_sta_tropo(QTreeWidgetItem *, int column);

    void on_pushButton_3_clicked();

    void checkDatum();

private:
    Ui::SolverWidget *ui;
    QStandardItemModel *station_model_;
    QStandardItemModel *source_model_;
};

#endif // SOLVERWIDGET_H
