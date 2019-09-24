#ifndef SATELLITESCHEDULING_H
#define SATELLITESCHEDULING_H

#include <QMainWindow>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QtCharts/QScatterSeries>
#include <QTreeWidgetItem>
#include "Utility/chartview.h"
#include "Utility/callout.h"
#include "Utility/qtutil.h"
#include "Utility/multicolumnsortfilterproxymodel.h"

#include "../VieSchedpp/Satellite/SatelliteMain.h"

namespace Ui {
class SatelliteScheduling;
}

class SatelliteScheduling : public QMainWindow
{
    Q_OBJECT

public:
    SatelliteScheduling(const QString &pathAntenna, const QString &pathEquip,
                        const QString &pathPosition, const QString &pathMask,
                        QDateTime startTime, QDateTime endTime,
                        const QStringList &stations,
                        boost::property_tree::ptree *settings,
                        QWidget *parent = nullptr);

    ~SatelliteScheduling();

private slots:
    void on_actionSatellite_triggered();

    void on_actionScan_triggered();

    void on_actionInfo_triggered();

    void on_pushButton_inputBrowse_clicked();

    void on_pushButton_inputReload_clicked();

    void on_lineEdit_availableFilter_textChanged(const QString &arg1);

    void on_treeView_available_clicked(const QModelIndex &index);

    void on_dateTimeEdit_showTime_dateTimeChanged(const QDateTime &dateTime);

    void on_horizontalSlider_adjustTime_valueChanged(int value);

    void worldmap_hovered(QPointF point, bool state);

    void on_treeWidget_selected_itemClicked(QTreeWidgetItem *item, int column);

    void on_pushButton_process_clicked();

    void on_pushButton_showAll_clicked();

    void on_pushButton_showConstant_clicked();

    void on_treeWidget_template_itemClicked(QTreeWidgetItem *item, int column);

    void on_pushButton_checkAndSave_clicked();

    void on_pushButton_savePath_clicked();

private:
    Ui::SatelliteScheduling *ui;
    ChartView *worldmap;

    std::vector<std::tuple<std::string,std::string,VieVS::Scan>> satellitefile_name_scan;
    std::vector<VieVS::Scan> scheduledScans;

    std::vector<VieVS::Satellite> satellites;
    VieVS::SatelliteMain satelliteScheduler;

    boost::property_tree::ptree *settings_;

};

#endif // SATELLITESCHEDULING_H
