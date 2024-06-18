/* 
 *  VieSched++ Very Long Baseline Interferometry (VLBI) Scheduling Software
 *  Copyright (C) 2018  Matthias Schartner
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH "unknown"
#endif

#ifndef MAINWINDOW_H
#define MAINWINDOW_H



#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QFileDialog>
#include <QListWidget>
#include <QWhatsThis>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QTreeWidgetItem>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QDesktopServices>
#include <QDockWidget>
//#include <QTextBrowser>
#include <Utility/mytextbrowser.h>
#include <QRegularExpression>
#include <QFontDatabase>
#include <QInputDialog>
#include <QSlider>

#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QPolarChart>
#include <QtCharts/QAreaSeries>

#include "Widgets/calibratorblockwidget.h"
#include "Utility/chartview.h"
#include "Utility/callout.h"
#include "secondaryGUIs/addgroupdialog.h"
#include "Parameters/baselineparametersdialog.h"
#include "Parameters/stationparametersdialog.h"
#include "Parameters/sourceparametersdialog.h"
#include "secondaryGUIs/addbanddialog.h"
#include "secondaryGUIs/savetosettingsdialog.h"
#include "../VieSchedpp/VieSchedpp.h"
#include "SatelliteGUI/satellitescheduling.h"
#include "secondaryGUIs/textfileviewer.h"
#include "secondaryGUIs/vieschedpp_analyser.h"
#include "../VieSchedpp/Input/SkdParser.h"
#include "Utility/qtutil.h"
#include "secondaryGUIs/skedcataloginfo.h"
#include "Utility/multicolumnsortfilterproxymodel.h"
#include "secondaryGUIs/obsmodedialog.h"
#include "Utility/statistics.h"
#include "secondaryGUIs/mastersessionviewer.h"
#include "secondaryGUIs/parsedowntimes.h"
#include "secondaryGUIs/tleformat.h"
#include "Widgets/simulatorwidget.h"
#include "Widgets/solverwidget.h"
#include "Widgets/priorities.h"
#include "Widgets/mulitschedulingwidget.h"
#include "Widgets/setupwidget.h"
// #include "Widgets/skycoveragewidget.h"
#include "Widgets/skycovwidget.h"
#include "Widgets/satelliteavoidancewidget.h"


#include "Utility/downloadmanager.h"
QT_CHARTS_USE_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:    
    void closeEvent(QCloseEvent *event);

    QString writeXML();

    void loadXML(QString path);

    void readSettings();

    void on_actionMode_triggered();

    void on_actionWelcome_triggered();

    void on_actionGeneral_triggered();

    void on_actionStation_triggered();

    void on_actionSource_triggered();

    void on_actionBaseline_triggered();

    void on_actionRules_triggered();

    void on_actionOutput_triggered();

    void on_actionSettings_triggered();

    void on_actionWeight_factors_triggered();

    void on_actionMulti_Scheduling_triggered();

    void on_pushButton_browseAntenna_clicked();

    void on_pushButton_browseEquip_clicked();

    void on_pushButton_browsePosition_clicked();

    void on_pushButton_browseMask_clicked();

    void on_pushButton_browseSource_clicked();

    void on_pushButton_browseFlux_clicked();

    void on_pushButton_browsModes_clicked();

    void on_pushButton_browseFreq_clicked();

    void on_pushButton_browseTracks_clicked();

    void on_pushButton_browseLoif_clicked();

    void on_pushButton_browseRec_clicked();

    void on_pushButton_browseRx_clicked();

    void on_pushButton_browseHdpos_clicked();

    void on_treeView_allSelectedStations_clicked(const QModelIndex &index);

    void on_groupBox_modeSked_toggled(bool arg1);

    void on_groupBox_modeCustom_toggled(bool arg1);

    // void on_lineEdit_allStationsFilter_textChanged(const QString &arg1);

    void on_treeView_allAvailabeStations_clicked(const QModelIndex &index);

    void on_actionInput_triggered();

    void on_doubleSpinBox_weightLowDecStart_valueChanged(double arg1);

    void on_doubleSpinBox_weightLowDecEnd_valueChanged(double arg1);

    void on_doubleSpinBox_weightLowElStart_valueChanged(double arg1);

    void on_doubleSpinBox_weightLowElEnd_valueChanged(double arg1);

    void on_spinBox_scanSequenceCadence_valueChanged(int arg1);

    void createModesPolicyTable();

    void addModesPolicyTable(QString name);

    void createModesCustonBandTable();

    void addModesCustomTable(QString name, double freq, int nChannel);

    void deleteModesCustomLine(QString name);

    void on_actionWhat_is_this_triggered();

    void on_spinBox_fontSize_valueChanged(int arg1);

    void on_fontComboBox_font_currentFontChanged(const QFont &f);

    void on_treeView_allAvailabeStations_entered(const QModelIndex &index);

    void worldmap_hovered(QPointF point, bool state);

    void skymap_hovered(QPointF point, bool state);

    void on_treeView_allSelectedStations_entered(const QModelIndex &index);

    void on_actionSky_Coverage_triggered();

    void on_actionExit_triggered();

    void on_iconSizeSpinBox_valueChanged(int arg1);

    void on_treeWidget_2_itemChanged(QTreeWidgetItem *item, int column);

    void addGroupStation();

    void addGroupSource();

    void addGroupSatellite();

    void addGroupSpacecraft();

    void addGroupBaseline();

    void on_dateTimeEdit_sessionStart_dateTimeChanged(const QDateTime &dateTime);

    void on_doubleSpinBox_sessionDuration_valueChanged(double arg1);

    void createBaselineModel();

    void setupStationAxisBufferAddRow();

    void on_pushButton_16_clicked();

    void on_treeView_allAvailabeSources_clicked(const QModelIndex &index);

    void on_treeView_allSelectedSources_clicked(const QModelIndex &index);

    void on_pushButton_13_clicked();

    void on_pushButton_15_clicked();

    void on_treeView_allAvailabeSources_entered(const QModelIndex &index);

    void on_treeView_allSelectedSources_entered(const QModelIndex &index);

    void on_pushButton_18_clicked();

    void on_pushButton_19_clicked();

    void on_actionNew_triggered();

    QString on_actionSave_triggered();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_saveCatalogPathes_clicked();

    void on_pushButton_26_clicked();

    void changeDefaultSettings(QStringList path, QStringList value, QString name);

    void on_pushButton_23_clicked();

    void on_pushButton_22_clicked();

    void createDefaultParameterSettings();

    void on_pushButton_saveNetwork_clicked();

    void on_pushButton_loadNetwork_clicked();

    void on_pushButton_saveSourceList_clicked();

    void on_pushButton_loadSourceList_clicked();

    void on_pushButton_saveMode_clicked();

    void on_pushButton_loadMode_clicked();

    void clearGroup(bool sta, bool src, bool bl, QString name);

    bool clearSetup(bool sta, bool src, bool bl);

    void splitterMoved();

    void faqSearch();

    void on_actionFAQ_triggered();

    void on_actionRun_triggered();

    void processFinished();

    void networkSizeChanged();

    void sourceListChanged();

    void satelliteListChanged();

    void spacecraftListChanged();

    void baselineListChanged();

    void on_actionsummary_triggered();


    void on_actionConditions_triggered();

    void on_pushButton_addCondition_clicked();

    void on_pushButton_removeCondition_clicked();

    // void on_lineEdit_allStationsFilter_3_textChanged(const QString &arg1);

    void on_actionNetwork_triggered();

    void on_actionSource_List_triggered();

    void on_dateTimeEdit_sessionStart_dateChanged(const QDate &date);

    void on_spinBox_doy_valueChanged(int arg1);

    void on_pushButton_clicked();

    void markerWorldmap();

    void markerSkymap();

    void on_radioButton_imageSkymap_toggled(bool checked);

    void on_radioButton_imageWorldmap_toggled(bool checked);

    void on_checkBox_showEcliptic_clicked(bool checked);

    void baselineHovered(QPointF point ,bool flag);

    void on_treeView_allSelectedBaselines_entered(const QModelIndex &index);

    void on_checkBox_showBaselines_clicked(bool checked);

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    // void on_pushButton_10_clicked();

    void on_pushButton_12_clicked();

    void on_spinBox_maxNumberOfIterations_valueChanged(int arg1);

    void on_experimentNameLineEdit_textChanged(const QString &arg1);

    void on_pushButton_41_clicked();

    void on_pushButton_addHighImpactAzEl_clicked();

    void on_pushButton_removeHighImpactAzEl_clicked();

    void on_pushButton_readLogFile_read_clicked();

    void on_pushButton_readSkdFile_read_clicked();

    void on_actionLog_parser_triggered();

    void on_actionSkd_Parser_triggered();

    void on_actionAbout_Qt_triggered();

    void on_pushButton_sessionBrowse_clicked();

    void on_pushButton_sessionAnalyser_clicked();

    void on_comboBox_log_file_currentIndexChanged(const QString &arg1);

    void on_comboBox_log_console_currentIndexChanged(const QString &arg1);

    void on_pushButton_stations_clicked();

    void on_pushButton_reloadsources_clicked();

    void on_pushButton_reloadcatalogs_clicked();

    void on_actionOpen_triggered();

    void gbps();

    void on_pushButton_modeCustomAddBAnd_clicked();

    void on_pushButton_browseExecutable_clicked();

    void on_pushButton_howAreSkedCatalogsLinked_clicked();

    void on_actionCurrent_Release_triggered();

    void on_actionAbout_triggered();

    void on_pushButton_2_clicked();

    void on_groupBox_modeAdvanced_toggled(bool arg1);

    void on_pushButton_startAdvancedMode_clicked();

    void changeObservingModeSelection(int idx);

    void on_pushButton_loadAdvancedMode_clicked();

    void on_pushButton_saveAdvancedMode_clicked();

    void on_pushButton_changeCurrentAdvancedMode_clicked();

    void on_pushButton_20_clicked();

    void on_checkBox_outputNGSFile_stateChanged(int arg1);

    void on_pushButton_parse_clicked();

    void on_pushButton_outputNgsFild_clicked();

    void on_pushButton_outputSnrTable_2_clicked();

    void on_pushButton_outputSnrTable_clicked();

    void on_pushButton_contact_add_clicked();

    void on_pushButton_contact_save_clicked();

    void on_pushButton_contactlist_save_clicked();

    void on_pushButton_21_clicked();

    void on_pushButton_contact_load_clicked();

    void on_pushButton_autoSetupIntensive_weightFactor_clicked();

    void on_pushButton_autoSetupIntensive_minStations_clicked();

    void on_pushButton_autoSetupIntensive_maxScan120_clicked();

    void on_pushButton_autoSetupIntensive_masScan200_clicked();

    void on_pushButton_minTimeBetweenScans_clicked();

    void on_pushButton_mulitScheduling_clicked();

    void on_groupBox_34_toggled(bool arg1);

    void on_groupBox_35_toggled(bool arg1);

    void on_groupBox_scanSequence_toggled(bool arg1);

    void on_groupBox_CalibratorBlock_toggled(bool arg1);

    void on_pushButton_autoSetupIntensive_masScan300_clicked();

    void masterDownloadFinished();

    void downloadFinished();

    void on_sampleBitsSpinBox_valueChanged(int arg1);

    void on_pushButton_viewNext_clicked();

    void on_groupBox_highImpactAzEl_toggled(bool arg1);

    void on_pushButton_11_clicked();

    void on_actionadvanced_triggered();

    void updateWeightFactorSliders();

    void updateWeightFactorValue();

    void on_actionSimulator_triggered();

    void on_pushButton_simulator_save_clicked();

    void on_pushButton_simulator_load_clicked();

    void saveMultiCoreSetup();

    void on_pushButton_browseSpacecraft_clicked();

    void on_pushButton_browseSatellite_clicked();

    void on_treeView_allAvailabeSatellites_clicked(const QModelIndex &index);

    void on_treeView_allAvailabeSpacecrafts_clicked(const QModelIndex &index);

    void on_treeView_allSelectedSatellites_clicked(const QModelIndex &index);

    void on_treeView_allSelectedSpacecrafts_clicked(const QModelIndex &index);

    void on_groupBox_6_toggled(bool arg1);

    void on_pushButton_3_clicked();

    void on_pushButton_satellite_select_all_clicked();

    void on_pushButton_satellite_select_none_clicked();

    void on_pushButton_tle_info_clicked();

    void on_pushButton_browseStp_clicked();

    void on_groupBox_a_priori_satellite_scans_toggled(bool arg1);

    void on_doubleSpinBox_calibratorLowElStart_valueChanged(double arg1);

    void on_doubleSpinBox_calibratorLowElEnd_valueChanged(double arg1);

    void on_doubleSpinBox_calibratorHighElStart_valueChanged(double arg1);

    void on_doubleSpinBox_calibratorHighElEnd_valueChanged(double arg1);

    void on_groupBox_5_toggled(bool arg1);

    void on_groupBox_9_toggled(bool arg1);

    void on_pushButton_setupSEFD_add_clicked();

    void on_pushButton_setupSEFD_remove_clicked();

    void on_pushButton_browseSatellite_2_clicked();

    void on_pushButton_tle_info_2_clicked();

    void on_actionSatellite_Avoidance_triggered();

    void on_pushButton_save_int_downtime_clicked();

    void on_spinBox_int_downtime_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QString mainPath;
    boost::property_tree::ptree settings_;

    boost::optional<VieVS::ObservingMode> advancedObservingMode_;

    VieVS::ParameterSettings para;

    QStandardItemModel *allStationModel;
    QStandardItemModel *allSourceModel;
    QStandardItemModel *allSatelliteModel;
    QStandardItemModel *allSpacecraftModel;
    MultiColumnSortFilterProxyModel *allStationProxyModel;
    MultiColumnSortFilterProxyModel *allSourceProxyModel;
    MultiColumnSortFilterProxyModel *allSatelliteProxyModel;
    MultiColumnSortFilterProxyModel *allSpacecraftProxyModel;

    QStandardItemModel *selectedStationModel;
    QStandardItemModel *selectedSourceModel;
    QStandardItemModel *selectedBaselineModel;
    QStandardItemModel *selectedSatelliteModel;
    QStandardItemModel *selectedSpacecraftModel;
    bool createBaselines;

    QStandardItemModel *allSourcePlusGroupModel_combined;
    QStandardItemModel *allSourcePlusGroupModel;
    QStandardItemModel *allStationPlusGroupModel;
    QStandardItemModel *allBaselinePlusGroupModel;
    QStandardItemModel *allSatellitePlusGroupModel;
    QStandardItemModel *allSpacecraftPlusGroupModel;

    QStringListModel *allSkedModesModel;

    VieVS::SkdCatalogReader skdCatalogReader;

    ChartView *worldmap;
    ChartView *skymap;

    QScatterSeries *availableStations;
    QScatterSeries *selectedStations;
    QScatterSeries *availableSources;
    QScatterSeries *selectedSources;

    Callout *worldMapCallout;
    Callout *skyMapCallout;

    QSignalMapper *deleteModeMapper;

    std::map<std::string, std::vector<std::string>> *groupSta = new std::map<std::string, std::vector<std::string>>();
    std::map<std::string, std::vector<std::string>> *groupSrc = new std::map<std::string, std::vector<std::string>>();
    std::map<std::string, std::vector<std::string>> *groupBl = new std::map<std::string, std::vector<std::string>>();
    std::map<std::string, std::vector<std::string>> *groupSat = new std::map<std::string, std::vector<std::string>>();
    std::map<std::string, std::vector<std::string>> *groupSpace = new std::map<std::string, std::vector<std::string>>();

    setupWidget *stationSetupWidget;
    setupWidget *sourceSetupWidget;
    setupWidget *baselineSetupWidget;
    setupWidget *satelliteSetupWidget;
    setupWidget *spacecraftSetupWidget;

    SkyCovWidget *skyCoverageWidget;
    CalibratorBlockWidget *calibratorWidget;
    SatelliteAvoidanceWidget *satelliteAvoidanceWidget;

    Statistics *statistics;
    boost::optional<VieVS::Scheduler> parsedSchedule;
    std::map<std::string, std::vector<double>> parsedFreq;

    boost::property_tree::ptree a_priori_satellite_scans;

    void updateBands();

    void readSkedCatalogs();

    void readStations();

    void readSources();

    void readSatellites();

    void highlightSatelliteEpoch();

    void readSpacecraft();

    void readAllSkedObsModes();

    void plotWorldMap();

    void plotSkyMap();

    void defaultParameters();

    void displayStationSetupMember(QString name);

    void displaySourceSetupMember(QString name);

    void displayBaselineSetupMember(QString name);

    void displayStationSetupParameter(QString name);

    void displaySourceSetupParameter(QString name);

    void displayBaselineSetupParameter(QString name);

    void displaySetupCallout(QPointF,bool);

    int plotParameter(QChart* targetChart, QTreeWidgetItem *item, int level, int plot, QString target, const std::map<std::string, std::vector<std::string> > &map);


    void updateAdvancedObservingMode();

    DownloadManager *downloadManager = new DownloadManager();

    void download();


//    void searchSessionCodeInMasterFile(QString code);


};

#endif // MAINWINDOW_H
