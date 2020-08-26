#ifndef SATELLITESCHEDULING_H
#define SATELLITESCHEDULING_H

#include <QMainWindow>
#include <QDateTime>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QComboBox>
#include <QGraphicsLayout>
#include <QStandardItemModel>

#include <QtCharts/QScatterSeries>
#include <QtCharts/QPolarChart>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QPercentBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegendMarker>
#include <QStackedBarSeries>

#include <secondaryGUIs/rendersetup.h>

#include <QTreeWidgetItem>
#include "Utility/chartview.h"
#include "Utility/callout.h"
#include "Utility/qtutil.h"
#include "Utility/multicolumnsortfilterproxymodel.h"

#include "SatelliteMain.h"
#include "setTimes.h"

namespace Ui {
class SatelliteScheduling;
}

class SatelliteScheduling : public QMainWindow
{
    Q_OBJECT

signals:
    void finished();

public:

    SatelliteScheduling(const QString &pathAntenna, const QString &pathEquip,
                        const QString &pathPosition, const QString &pathMask,
                        const QString &pathSat,
                        QStandardItemModel *selectedSatelliteModel,
                        QStandardItemModel *allSatelliteModel,
                        QStandardItemModel *allSatellitePlusGroupModel,
                        std::map<std::string, std::vector<std::string>> *groupSat,
                        QDateTime startTime, QDateTime endTime,
                        const QStringList &stations,
                        boost::property_tree::ptree *settings,
                        QWidget *parent = nullptr);

    ~SatelliteScheduling();

    boost::property_tree::ptree toPropertyTree();

    unsigned long getNumberOfScans() {return scheduledScans.size();}

private slots:
    void on_actionSatellite_triggered();
    void on_actionScan_triggered();
    void on_actionInfo_triggered();
    void on_actionStatistic_triggered();
    void on_actionElevation_triggered();
    void on_actionSkyPlots_triggered();

    void on_lineEdit_availableFilter_textChanged(const QString &arg1);
    void on_treeView_available_clicked(const QModelIndex &index);
    void on_checkBox_showTracks_clicked(bool checked);
    void on_checkBox_showStations_clicked(bool checked);

    void on_dateTimeEdit_showTime_dateTimeChanged(const QDateTime &dateTime);

    void on_horizontalSlider_adjustTime_valueChanged(int value);

    void worldmap_hovered(QPointF point, bool state);

    void sattrack_hovered(QPointF point,bool state);

    void on_pushButton_process_clicked();

    void writeLists();

    void on_pushButton_showConstant_clicked();

    void on_treeWidget_template_itemClicked(QTreeWidgetItem *item, int column);

    void on_pushButton_checkAndSave_clicked();
    bool checkScan(VieVS::Scan scan);

    void on_pushButton_removeScan_clicked();

    void on_pushButton_adjustStart_clicked();

    void ElevationSetup();
    void updateElevation();

    void satelliteStatisticsSetup();
    void updateSatelliteStatistics();

    void createTableVisibility();

    void StackedBarPlotSetup();
    void createBarPlotVisibilityStacked();

    void on_lineEdit_satelliteStatistics_textChanged(const QString &arg1);
    void on_lineEdit_satelliteElevation_textChanged(const QString &arg1);
    void on_lineEdit_satelliteStatistics_table_textChanged(const QString &arg1);

    void on_treeView_satelliteListStatistics_clicked(const QModelIndex &index);

    void on_horizontalScrollBar_stackedPlot_valueChanged(int value);

    //create SkyPlots and ElevationPlot for scan
    void SetupPlotsScan();
    int getIndexTopLevelItem();
    void createElevationPlotScan(int idxTopLevelItem);
    void createSkyPlotScan(int idxTopLevelItem);
    void updateSkyPlotScan(QString name);

    //Plots
    void on_pushButton_screenshot_worldmap_clicked();
    void on_pushButton_screenshot_SkyPlotScan_clicked();
    void on_pushButton_screenshot_ElevationPlotScan_clicked();
    void on_pushButton_screenshot_BarPlotVisibilitySingle_clicked();
    void on_pushButton_screenshot_BarPlotVisibilityStacked_clicked();
    void on_pushButton_screenshot_ElevationPlot_clicked();

    void on_actionFinish_triggered();
    void createXMLOutput(std::vector<VieVS::Scan> scanList,VieVS::Network network, std::vector<SatelliteForGUI>);

    void on_treeView_selected_clicked(const QModelIndex &index);

private:
    Ui::SatelliteScheduling *ui;

    ChartView *worldmap;

    QScatterSeries *selectedStations;

    Callout *worldMapCallout;

   // QVector<QScatterSeries> *SatelliteTracks;

    std::vector<std::tuple<std::string,std::string,VieVS::Scan>> satellitefile_name_scan;
    std::vector<VieVS::Scan> scheduledScans;

    std::vector<SatelliteForGUI> satellites;
    SatelliteMain satelliteScheduler;
    QStandardItemModel *allSatelliteModel;
    QStandardItemModel *selectedSatelliteModel;
    QStandardItemModel *allSatellitePlusGroupModel;
    std::map<std::string, std::vector<std::string>> *groupSat;


    boost::property_tree::ptree *settings_;

    QDateTime sessionStart_;
    QDateTime sessionEnd_;
};

#endif // SATELLITESCHEDULING_H
