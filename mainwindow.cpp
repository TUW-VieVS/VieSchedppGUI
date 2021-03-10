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

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QApplication::setWindowIcon(QIcon(":/icons/icons/VieSchedppGUI_logo.png"));
    this->setWindowTitle("VieSched++");

    ui->main_stacked->setCurrentIndex(0);


    allSourcePlusGroupModel_combined = new QStandardItemModel();
    allSourcePlusGroupModel_combined->appendRow(new QStandardItem(QIcon(":/icons/icons/all_source_types.png"),"__all__"));
    allSourcePlusGroupModel_combined->appendRow(new QStandardItem(QIcon(":/icons/icons/source_group.png"),"__AGNs__"));
    allSourcePlusGroupModel_combined->appendRow(new QStandardItem(QIcon(":/icons/icons/satellite_group.png"),"__satellites__"));
    allSourcePlusGroupModel_combined->appendRow(new QStandardItem(QIcon(":/icons/icons/spacecraft_group.png"),"__spacecrafts__"));



    allSourcePlusGroupModel = new QStandardItemModel();
    allSourcePlusGroupModel->appendRow(new QStandardItem(QIcon(":/icons/icons/source_group.png"),"__all__"));

    allStationPlusGroupModel = new QStandardItemModel();
    allStationPlusGroupModel->appendRow(new QStandardItem(QIcon(":/icons/icons/station_group.png"),"__all__"));

    allBaselinePlusGroupModel = new QStandardItemModel();
    allBaselinePlusGroupModel->appendRow(new QStandardItem(QIcon(":/icons/icons/baseline_group.png"),"__all__"));

    allSatellitePlusGroupModel = new QStandardItemModel();
    allSatellitePlusGroupModel->appendRow(new QStandardItem(QIcon(":/icons/icons/satellite_group.png"),"__all__"));

    allSpacecraftPlusGroupModel = new QStandardItemModel();
    allSpacecraftPlusGroupModel->appendRow(new QStandardItem(QIcon(":/icons/icons/spacecraft_group.png"),"__all__"));



    QCoreApplication::setOrganizationName("TU Wien");
    QCoreApplication::setApplicationName("VieSched++ GUI");
    QCoreApplication::setApplicationVersion(GIT_COMMIT_HASH);

    QSettings settings("TU Wien","VieSched++ GUI");
    restoreGeometry(settings.value("myWidget/geometry").toByteArray());
    restoreState(settings.value("myWidget/windowState").toByteArray());

    QString schedVersion = GIT_SCHEDULER_COMMIT_HASH;
    if(QCoreApplication::applicationVersion().length() == 40){
        ui->lineEdit_gui_version->setText(QCoreApplication::applicationVersion().left(8));
    }
    if(schedVersion.length() == 40){
        ui->lineEdit_scheduler_version->setText(schedVersion.left(8));
    }

    QCoreApplication::setOrganizationName("TU Wien");
    QCoreApplication::setOrganizationDomain("http://hg.geo.tuwien.ac.at/");

    mainPath = QCoreApplication::applicationFilePath();
    QStringList mainPathSplit = mainPath.split("/");
    ui->pathToGUILineEdit->setText(mainPath);
    mainPathSplit.removeLast();
    mainPath = mainPathSplit.join("/");
    ui->defaultSettingsFileLineEdit->setText(mainPath+"/settings.xml");
    QLabel *il = new QLabel;
    QPixmap ic(":/icons/icons/emblem-important-4.png");
    ic = ic.scaled(16,16);
    il->setPixmap(ic);
    ui->horizontalLayout->insertWidget(0,il);

    worldmap = new ChartView(this);
    qtUtil::worldMap(worldmap);
    worldMapCallout = new Callout(worldmap->chart());
    worldMapCallout->hide();
    ui->horizontalLayout_worldmap->insertWidget(0,worldmap,10);


    skymap = new ChartView();
    qtUtil::skyMap(skymap);
    skyMapCallout = new Callout(skymap->chart());
    skyMapCallout->hide();
    ui->horizontalLayout_skymap->insertWidget(0,skymap,10);

    QFile file;
    file.setFileName("settings.xml");
    if(!file.exists()){
        createDefaultParameterSettings();
    }

    connect(ui->lineEdit_outputPath, SIGNAL(textChanged(QString)), ui->lineEdit_sessionPath, SLOT(setText(QString)));
    std::ifstream iSettings("settings.xml");
    boost::property_tree::read_xml(iSettings,settings_,boost::property_tree::xml_parser::trim_whitespace);

    MulitSchedulingWidget *ms_widget = new MulitSchedulingWidget(allSourcePlusGroupModel,
                                                                 allStationPlusGroupModel,
                                                                 allBaselinePlusGroupModel,
                                                                 ui->checkBox_weightCoverage,
                                                                 ui->doubleSpinBox_weightSkyCoverage,
                                                                 ui->checkBox_weightNobs,
                                                                 ui->doubleSpinBox_weightNumberOfObservations,
                                                                 ui->checkBox_weightDuration,
                                                                 ui->doubleSpinBox_weightDuration,
                                                                 ui->checkBox_weightAverageSources,
                                                                 ui->doubleSpinBox_weightAverageSources,
                                                                 ui->checkBox_weightAverageStations,
                                                                 ui->doubleSpinBox_weightAverageStations,
                                                                 ui->checkBox_weightAverageBaselines,
                                                                 ui->doubleSpinBox_weightAverageBaselines,
                                                                 ui->checkBox_weightIdleTime,
                                                                 ui->doubleSpinBox_weightIdleTime,
                                                                 ui->checkBox_weightLowDeclination,
                                                                 ui->doubleSpinBox_weightLowDec,
                                                                 ui->checkBox_weightLowElevation,
                                                                 ui->doubleSpinBox_weightLowEl,
                                                                 ui->groupBox_multiScheduling,
                                                                 ui->groupBox_simulator,
                                                                 groupSta,
                                                                 groupSrc,
                                                                 groupBl,
                                                                 ui->groupBox_multiScheduling);

    connect(ms_widget->newStationGroup,SIGNAL(clicked(bool)),this,SLOT(addGroupStation()));
    connect(ms_widget->newSourceGroup,SIGNAL(clicked(bool)),this,SLOT(addGroupSource()));
    connect(ms_widget->newBaselineGroup,SIGNAL(clicked(bool)),this,SLOT(addGroupBaseline()));
    connect(ms_widget->saveMultiCoreSetup, SIGNAL(clicked(bool)), this, SLOT(saveMultiCoreSetup()));

    ms_widget->setObjectName("MultiScheduling_Widged");
    QVBoxLayout *vbox_ms = new QVBoxLayout;
    vbox_ms->addWidget(ms_widget,1);
    ui->groupBox_multiScheduling->setLayout(vbox_ms);

    readSettings();

    if(ui->pathToSchedulerLineEdit->text().isEmpty()){

        QFileInfo check_file1("../VieSchedpp/Release/VieSchedpp");
        QFileInfo check_file2("../../VieSchedpp/Release/VieSchedpp");
        QFileInfo check_file3("../VieSchedpp/cmake-build-release/VieSchedpp");
        QFileInfo check_file4("../../VieSchedpp/cmake-build-release/VieSchedpp");
        QFileInfo check_file5("./VieSchedpp");
        if(check_file1.exists() && check_file1.isFile() && check_file1.isExecutable()){
            ui->pathToSchedulerLineEdit->setText("../VieSchedpp/Release/VieSchedpp");
            ui->pushButton_17->click();
        }else if(check_file2.exists() && check_file2.isFile() && check_file2.isExecutable()){
            ui->pathToSchedulerLineEdit->setText("../../VieSchedpp/Release/VieSchedpp");
            ui->pushButton_17->click();
        }else if(check_file3.exists() && check_file3.isFile() && check_file3.isExecutable()){
            ui->pathToSchedulerLineEdit->setText("../VieSchedpp/cmake-build-release/VieSchedpp");
            ui->pushButton_17->click();
        }else if(check_file4.exists() && check_file4.isFile() && check_file4.isExecutable()){
            ui->pathToSchedulerLineEdit->setText("../../VieSchedpp/cmake-build-release/VieSchedpp");
            ui->pushButton_17->click();
        }else if(check_file5.exists() && check_file5.isFile() && check_file5.isExecutable()){
            ui->pathToSchedulerLineEdit->setText("./VieSchedpp");
            ui->pushButton_17->click();
        }else{
            QMessageBox mb;
            QString txt = "Please make sure to set the path to the VieSchedpp executable.<br>"
                          "After clicking \"ok\" the GUI schould open. Browse to the settings page <img src=\":/icons/icons/emblem-system-2.png\" height=\"30\" width=\"30\"/>, "
                          "add the path to the VieSchedpp executable and press save "
                          "<img src=\":/icons/icons/document-export.png\" height=\"30\" width=\"30\"/>.<br>"
                          "You can test your connection by clicking the <img src=\":/icons/icons/help.png\" height=\"30\" width=\"30\"/> button right next to it.";
            mb.information(this,"First start!",txt);
        }

    }

    plotSkyCoverageTemplate = false;

    ui->statusBar->addPermanentWidget(new QLabel("no schedules started"));

    QPushButton *savePara = new QPushButton(QIcon(":/icons/icons/document-export.png"),"",this);
    savePara->setToolTip("save parameter file");
    savePara->setStatusTip("save parameter file");
    connect(savePara,SIGNAL(clicked(bool)),this,SLOT(writeXML()));
    savePara->setMinimumSize(30,30);
    ui->statusBar->addPermanentWidget(savePara);

    QPushButton *createSchedule = new QPushButton(QIcon(":/icons/icons/arrow-right-3.png"),"",this);
    createSchedule->setToolTip("save parameter file and create schedule");
    createSchedule->setStatusTip("save parameter file and create schedule");
    connect(createSchedule,SIGNAL(clicked(bool)),this,SLOT(on_actionRun_triggered()));
    createSchedule->setMinimumSize(30,30);
    ui->statusBar->addPermanentWidget(createSchedule);


    allStationModel = new QStandardItemModel(0,19,this);
    allStationModel->setHeaderData(0, Qt::Horizontal, QObject::tr("name"));
    allStationModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Id"));
    allStationModel->setHeaderData(2, Qt::Horizontal, QObject::tr("lat [deg]"));
    allStationModel->setHeaderData(3, Qt::Horizontal, QObject::tr("lon [deg]"));
    allStationModel->setHeaderData(4, Qt::Horizontal, QObject::tr("diam [m]"));
    allStationModel->setHeaderData(5, Qt::Horizontal, QObject::tr("SEFD X [Jy]"));
    allStationModel->setHeaderData(6, Qt::Horizontal, QObject::tr("SEFD S [Jy]"));
    allStationModel->setHeaderData(7, Qt::Horizontal, QObject::tr("axis offset [m]"));
    allStationModel->setHeaderData(8, Qt::Horizontal, QObject::tr("slew rate1 [deg/min]"));
    allStationModel->setHeaderData(9, Qt::Horizontal, QObject::tr("constant overhead1 [sec]"));
    allStationModel->setHeaderData(10, Qt::Horizontal, QObject::tr("lower axis limit1 [deg]"));
    allStationModel->setHeaderData(11, Qt::Horizontal, QObject::tr("upper axis limit1 [deg]"));
    allStationModel->setHeaderData(12, Qt::Horizontal, QObject::tr("slew rate2 [deg/min]"));
    allStationModel->setHeaderData(13, Qt::Horizontal, QObject::tr("constant overhead2 [sec]"));
    allStationModel->setHeaderData(14, Qt::Horizontal, QObject::tr("lower axis limit2 [deg]"));
    allStationModel->setHeaderData(15, Qt::Horizontal, QObject::tr("upper axis limit2 [deg]"));
    allStationModel->setHeaderData(16, Qt::Horizontal, QObject::tr("x [m]"));
    allStationModel->setHeaderData(17, Qt::Horizontal, QObject::tr("y [m]"));
    allStationModel->setHeaderData(18, Qt::Horizontal, QObject::tr("z [m]"));

    selectedStationModel = new QStandardItemModel(0,19,this);
    selectedStationModel->setHeaderData(0, Qt::Horizontal, QObject::tr("name"));
    selectedStationModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Id"));
    selectedStationModel->setHeaderData(2, Qt::Horizontal, QObject::tr("lat [deg]"));
    selectedStationModel->setHeaderData(3, Qt::Horizontal, QObject::tr("lon [deg]"));
    selectedStationModel->setHeaderData(4, Qt::Horizontal, QObject::tr("diam [m]"));
    selectedStationModel->setHeaderData(5, Qt::Horizontal, QObject::tr("SEFD X [Jy]"));
    selectedStationModel->setHeaderData(6, Qt::Horizontal, QObject::tr("SEFD S [Jy]"));
    selectedStationModel->setHeaderData(7, Qt::Horizontal, QObject::tr("axis offset [m]"));
    selectedStationModel->setHeaderData(8, Qt::Horizontal, QObject::tr("slew rate1 [deg/min]"));
    selectedStationModel->setHeaderData(9, Qt::Horizontal, QObject::tr("constant overhead1 [sec]"));
    selectedStationModel->setHeaderData(10, Qt::Horizontal, QObject::tr("lower axis limit1 [deg]"));
    selectedStationModel->setHeaderData(11, Qt::Horizontal, QObject::tr("upper axis limit1 [deg]"));
    selectedStationModel->setHeaderData(12, Qt::Horizontal, QObject::tr("slew rate2 [deg/min]"));
    selectedStationModel->setHeaderData(13, Qt::Horizontal, QObject::tr("constant overhead2 [sec]"));
    selectedStationModel->setHeaderData(14, Qt::Horizontal, QObject::tr("lower axis limit2 [deg]"));
    selectedStationModel->setHeaderData(15, Qt::Horizontal, QObject::tr("upper axis limit2 [deg]"));
    selectedStationModel->setHeaderData(16, Qt::Horizontal, QObject::tr("x [m]"));
    selectedStationModel->setHeaderData(17, Qt::Horizontal, QObject::tr("y [m]"));
    selectedStationModel->setHeaderData(18, Qt::Horizontal, QObject::tr("z [m]"));

    allStationProxyModel = new MultiColumnSortFilterProxyModel(this);
    allStationProxyModel->setSourceModel(allStationModel);
    allStationProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    allStationProxyModel->setFilterKeyColumns({0,1});

    ui->treeView_allAvailabeStations->setModel(allStationProxyModel);
    ui->treeView_allAvailabeStations->setRootIsDecorated(false);
    ui->treeView_allAvailabeStations->setSortingEnabled(true);
    ui->treeView_allAvailabeStations->sortByColumn(0, Qt::AscendingOrder);
    ui->treeView_allAvailabeStations->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->treeView_allSelectedStations->setModel(selectedStationModel);
    ui->treeView_allSelectedStations->setRootIsDecorated(false);
    ui->treeView_allSelectedStations->setSortingEnabled(true);
    ui->treeView_allSelectedStations->sortByColumn(0, Qt::AscendingOrder);
    ui->treeView_allSelectedStations->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->lineEdit_allStationsFilter, &QLineEdit::textChanged, allStationProxyModel, &MultiColumnSortFilterProxyModel::addFilterFixedString);


    // ----------------------

    allSourceModel = new QStandardItemModel(0,3,this);
    allSourceModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    allSourceModel->setHeaderData(1, Qt::Horizontal, QObject::tr("RA [deg]"));
    allSourceModel->setHeaderData(2, Qt::Horizontal, QObject::tr("DC [deg]"));

    selectedSourceModel = new QStandardItemModel(0,3,this);
    selectedSourceModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    selectedSourceModel->setHeaderData(1, Qt::Horizontal, QObject::tr("RA [deg]"));
    selectedSourceModel->setHeaderData(2, Qt::Horizontal, QObject::tr("DC [deg]"));

    allSourceProxyModel = new MultiColumnSortFilterProxyModel(this);
    allSourceProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    allSourceProxyModel->setSourceModel(allSourceModel);
    allSourceProxyModel->setFilterKeyColumns({0});

    ui->treeView_allAvailabeSources->setModel(allSourceProxyModel);
    ui->treeView_allAvailabeSources->setRootIsDecorated(false);
    ui->treeView_allAvailabeSources->setSortingEnabled(true);
    ui->treeView_allAvailabeSources->sortByColumn(0, Qt::AscendingOrder);
    ui->treeView_allAvailabeSources->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->treeView_allSelectedSources->setModel(selectedSourceModel);
    ui->treeView_allSelectedSources->setRootIsDecorated(false);
    ui->treeView_allSelectedSources->setSortingEnabled(true);
    ui->treeView_allSelectedSources->sortByColumn(0, Qt::AscendingOrder);
    ui->treeView_allSelectedSources->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->lineEdit_allSourceFilter, &QLineEdit::textChanged, allSourceProxyModel, &MultiColumnSortFilterProxyModel::addFilterFixedString);


    // ----------------------

    allSatelliteModel = new QStandardItemModel(0,3,this);
    allSatelliteModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    allSatelliteModel->setHeaderData(1, Qt::Horizontal, QObject::tr("line 1"));
    allSatelliteModel->setHeaderData(2, Qt::Horizontal, QObject::tr("line 2"));

    selectedSatelliteModel = new QStandardItemModel(0,3,this);
    selectedSatelliteModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    selectedSatelliteModel->setHeaderData(1, Qt::Horizontal, QObject::tr("line 1"));
    selectedSatelliteModel->setHeaderData(2, Qt::Horizontal, QObject::tr("line 2"));

    allSatelliteProxyModel = new MultiColumnSortFilterProxyModel(this);
    allSatelliteProxyModel->setSourceModel(allSatelliteModel);
    allSatelliteProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    allSatelliteProxyModel->setFilterKeyColumns({0});

    ui->treeView_allAvailabeSatellites->setModel(allSatelliteProxyModel);
    ui->treeView_allAvailabeSatellites->setRootIsDecorated(false);
    ui->treeView_allAvailabeSatellites->setSortingEnabled(true);
    ui->treeView_allAvailabeSatellites->sortByColumn(0, Qt::AscendingOrder);
    ui->treeView_allAvailabeSatellites->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->treeView_allSelectedSatellites->setModel(selectedSatelliteModel);
    ui->treeView_allSelectedSatellites->setRootIsDecorated(false);
    ui->treeView_allSelectedSatellites->setSortingEnabled(true);
    ui->treeView_allSelectedSatellites->sortByColumn(0, Qt::AscendingOrder);
    ui->treeView_allSelectedSatellites->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->lineEdit_allSatelliteFilter, &QLineEdit::textChanged, allSatelliteProxyModel, &MultiColumnSortFilterProxyModel::addFilterFixedString);


    // ----------------------

    allSpacecraftModel = new QStandardItemModel(0,1,this);
    allSpacecraftModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));

    selectedSpacecraftModel = new QStandardItemModel(0,1,this);
    selectedSpacecraftModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));

    allSpacecraftProxyModel = new MultiColumnSortFilterProxyModel(this);
    allSpacecraftProxyModel->setSourceModel(allSpacecraftModel);
    allSpacecraftProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    allSpacecraftProxyModel->setFilterKeyColumns({0});

    ui->treeView_allAvailabeSpacecrafts->setModel(allSpacecraftProxyModel);
    ui->treeView_allAvailabeSpacecrafts->setRootIsDecorated(false);
    ui->treeView_allAvailabeSpacecrafts->setSortingEnabled(true);
    ui->treeView_allAvailabeSpacecrafts->sortByColumn(0, Qt::AscendingOrder);
    ui->treeView_allAvailabeSpacecrafts->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->treeView_allSelectedSpacecrafts->setModel(selectedSpacecraftModel);
    ui->treeView_allSelectedSpacecrafts->setRootIsDecorated(false);
    ui->treeView_allSelectedSpacecrafts->setSortingEnabled(true);
    ui->treeView_allSelectedSpacecrafts->sortByColumn(0, Qt::AscendingOrder);
    ui->treeView_allSelectedSpacecrafts->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->lineEdit_allSpacecraftFilter, &QLineEdit::textChanged, allSpacecraftProxyModel, &MultiColumnSortFilterProxyModel::addFilterFixedString);


    // ----------------------

    selectedBaselineModel = new QStandardItemModel(0,2,this);
    selectedBaselineModel->setHeaderData(0, Qt::Horizontal, QObject::tr("name"));
    selectedBaselineModel->setHeaderData(1, Qt::Horizontal, QObject::tr("distance [km]"));

    ui->treeView_allSelectedBaselines->setModel(selectedBaselineModel);
    ui->treeView_allSelectedBaselines->setRootIsDecorated(false);
    ui->treeView_allSelectedBaselines->setSortingEnabled(true);
    ui->treeView_allSelectedBaselines->sortByColumn(0, Qt::AscendingOrder);
    auto hv3 = ui->treeView_allSelectedBaselines->header();
    hv3->setSectionResizeMode(QHeaderView::ResizeToContents);
    createBaselines = true;



    allSkedModesModel = new QStringListModel();

    connect(selectedStationModel,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(networkSizeChanged()));
    connect(selectedStationModel,SIGNAL(rowsRemoved(QModelIndex,int,int)),this,SLOT(networkSizeChanged()));

    connect(selectedSourceModel,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(sourceListChanged()));
    connect(selectedSourceModel,SIGNAL(rowsRemoved(QModelIndex,int,int)),this,SLOT(sourceListChanged()));

    connect(selectedBaselineModel,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(baselineListChanged()));
    connect(selectedBaselineModel,SIGNAL(rowsRemoved(QModelIndex,int,int)),this,SLOT(baselineListChanged()));

    connect(selectedSatelliteModel,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(satelliteListChanged()));
    connect(selectedSatelliteModel,SIGNAL(rowsRemoved(QModelIndex,int,int)),this,SLOT(satelliteListChanged()));

    connect(selectedSpacecraftModel,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(spacecraftListChanged()));
    connect(selectedSpacecraftModel,SIGNAL(rowsRemoved(QModelIndex,int,int)),this,SLOT(spacecraftListChanged()));




    ui->comboBox_skedObsModes->setModel(allSkedModesModel);
    ui->comboBox_skedObsModes_advanced->setModel(allSkedModesModel);


    ui->comboBox_calibratorBlock_calibratorSources->setModel(allSourcePlusGroupModel);
    ui->comboBox_idleToObserving_stations->setModel(allStationPlusGroupModel);
    connect(ui->pushButton_idleToObserving_addStationGroup, SIGNAL(clicked(bool)), this, SLOT(addGroupStation()));

    deleteModeMapper = new QSignalMapper(this);
    connect (deleteModeMapper, SIGNAL(mapped(QString)), this, SLOT(deleteModesCustomLine(QString))) ;


    ui->comboBox_stationSettingMember_axis->setModel(allStationPlusGroupModel);
    stationSetupWidget = new setupWidget(setupWidget::Type::station,
                                         settings_,
                                         ui->tableWidget_ModesPolicy,
                                         allStationModel,
                                         allSourceModel,
                                         allSatelliteModel,
                                         allSpacecraftModel,
                                         selectedStationModel,
                                         selectedSourceModel,
                                         selectedBaselineModel,
                                         selectedSatelliteModel,
                                         selectedSpacecraftModel,
                                         allSourcePlusGroupModel,
                                         allStationPlusGroupModel,
                                         allBaselinePlusGroupModel,
                                         allSatellitePlusGroupModel,
                                         allSpacecraftPlusGroupModel,
                                         groupSta,
                                         groupSrc,
                                         groupBl,
                                         groupSat,
                                         groupSpace);
    ui->verticalLayout_stationSetupWidget->addWidget(stationSetupWidget);

    sourceSetupWidget = new setupWidget(setupWidget::Type::source,
                                        settings_,
                                        ui->tableWidget_ModesPolicy,
                                        allStationModel,
                                        allSourceModel,
                                        allSatelliteModel,
                                        allSpacecraftModel,
                                        selectedStationModel,
                                        selectedSourceModel,
                                        selectedBaselineModel,
                                        selectedSatelliteModel,
                                        selectedSpacecraftModel,
                                        allSourcePlusGroupModel,
                                        allStationPlusGroupModel,
                                        allBaselinePlusGroupModel,
                                        allSatellitePlusGroupModel,
                                        allSpacecraftPlusGroupModel,
                                        groupSta,
                                        groupSrc,
                                        groupBl,
                                        groupSat,
                                        groupSpace);
    ui->verticalLayout_sourceSetupWidget->addWidget(sourceSetupWidget);

    baselineSetupWidget = new setupWidget(setupWidget::Type::baseline,
                                          settings_,
                                          ui->tableWidget_ModesPolicy,
                                          allStationModel,
                                          allSourceModel,
                                          allSatelliteModel,
                                          allSpacecraftModel,
                                          selectedStationModel,
                                          selectedSourceModel,
                                          selectedBaselineModel,
                                          selectedSatelliteModel,
                                          selectedSpacecraftModel,
                                          allSourcePlusGroupModel,
                                          allStationPlusGroupModel,
                                          allBaselinePlusGroupModel,
                                          allSatellitePlusGroupModel,
                                          allSpacecraftPlusGroupModel,
                                          groupSta,
                                          groupSrc,
                                          groupBl,
                                          groupSat,
                                          groupSpace);
    ui->verticalLayout_baselineSetupWidget->addWidget(baselineSetupWidget);

    satelliteSetupWidget = new setupWidget(setupWidget::Type::satellite,
                                          settings_,
                                          ui->tableWidget_ModesPolicy,
                                          allStationModel,
                                          allSourceModel,
                                          allSatelliteModel,
                                          allSpacecraftModel,
                                          selectedStationModel,
                                          selectedSourceModel,
                                          selectedBaselineModel,
                                          selectedSatelliteModel,
                                          selectedSpacecraftModel,
                                          allSourcePlusGroupModel,
                                          allStationPlusGroupModel,
                                          allBaselinePlusGroupModel,
                                          allSatellitePlusGroupModel,
                                          allSpacecraftPlusGroupModel,
                                          groupSta,
                                          groupSrc,
                                          groupBl,
                                          groupSat,
                                          groupSpace);
    ui->verticalLayout_satelliteSetupWidget->addWidget(satelliteSetupWidget);

    spacecraftSetupWidget = new setupWidget(setupWidget::Type::spacecraft,
                                          settings_,
                                          ui->tableWidget_ModesPolicy,
                                          allStationModel,
                                          allSourceModel,
                                          allSatelliteModel,
                                          allSpacecraftModel,
                                          selectedStationModel,
                                          selectedSourceModel,
                                          selectedBaselineModel,
                                          selectedSatelliteModel,
                                          selectedSpacecraftModel,
                                          allSourcePlusGroupModel,
                                          allStationPlusGroupModel,
                                          allBaselinePlusGroupModel,
                                          allSatellitePlusGroupModel,
                                          allSpacecraftPlusGroupModel,
                                          groupSta,
                                          groupSrc,
                                          groupBl,
                                          groupSat,
                                          groupSpace);
    ui->verticalLayout_spacecraftSetupWidget->addWidget(spacecraftSetupWidget);

    ui->dateTimeEdit_sessionStart->setDate(QDate::currentDate());

    connect(stationSetupWidget->addGroupButton(),  SIGNAL(clicked(bool)), this, SLOT(addGroupStation()));
    connect(sourceSetupWidget->addGroupButton(),   SIGNAL(clicked(bool)), this, SLOT(addGroupSource()));
    connect(baselineSetupWidget->addGroupButton(), SIGNAL(clicked(bool)),this, SLOT(addGroupBaseline()));
    connect(satelliteSetupWidget->addGroupButton(), SIGNAL(clicked(bool)),this, SLOT(addGroupSatellite()));
    connect(spacecraftSetupWidget->addGroupButton(), SIGNAL(clicked(bool)),this, SLOT(addGroupSpacecraft()));


    connect(stationSetupWidget->horizontal_splitter(), SIGNAL(splitterMoved(int,int)), this, SLOT(splitterMoved()));
    connect(sourceSetupWidget->horizontal_splitter(), SIGNAL(splitterMoved(int,int)), this, SLOT(splitterMoved()));
    connect(baselineSetupWidget->horizontal_splitter(), SIGNAL(splitterMoved(int,int)), this, SLOT(splitterMoved()));
    connect(satelliteSetupWidget->horizontal_splitter(), SIGNAL(splitterMoved(int,int)), this, SLOT(splitterMoved()));
    connect(spacecraftSetupWidget->horizontal_splitter(), SIGNAL(splitterMoved(int,int)), this, SLOT(splitterMoved()));

    connect(stationSetupWidget->vertical_splitter(), SIGNAL(splitterMoved(int,int)), this, SLOT(splitterMoved()));
    connect(sourceSetupWidget->vertical_splitter(), SIGNAL(splitterMoved(int,int)), this, SLOT(splitterMoved()));
    connect(baselineSetupWidget->vertical_splitter(), SIGNAL(splitterMoved(int,int)), this, SLOT(splitterMoved()));
    connect(satelliteSetupWidget->vertical_splitter(), SIGNAL(splitterMoved(int,int)), this, SLOT(splitterMoved()));
    connect(spacecraftSetupWidget->vertical_splitter(), SIGNAL(splitterMoved(int,int)), this, SLOT(splitterMoved()));


    connect(ui->pushButton_addGroupStationCable, SIGNAL(clicked(bool)), this, SLOT(addGroupStation()));

    connect(ui->pushButton_addSourceGroup_Calibrator,SIGNAL(clicked(bool)), this, SLOT(addGroupSource()));
    connect(ui->pushButton_addSourceGroup_Sequence,SIGNAL(clicked(bool)), this, SLOT(addGroupSource()));
    connect(ui->pushButton_addSatelliteGroup_Sequence,SIGNAL(clicked(bool)), this, SLOT(addGroupSatellite()));
    connect(ui->pushButton_addSpacecraftGroup_Sequence,SIGNAL(clicked(bool)), this, SLOT(addGroupSpacecraft()));
    connect(ui->pushButton_calibration_addSrcGroup,SIGNAL(clicked(bool)), this, SLOT(addGroupSource()));


    readAllSkedObsModes();

    readSkedCatalogs();

    SimulatorWidget *simulator = new SimulatorWidget(selectedStationModel);
    simulator->setObjectName("Simulation_Widged");
    ui->tabWidget_simAna->addTab(simulator, "Simulation");
    connect(selectedStationModel, SIGNAL(itemChanged(QStandardItem *)), simulator, SLOT(addStations(QStandardItem *)));
    connect(selectedStationModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), simulator, SLOT(addStations()));

    SolverWidget *solver = new SolverWidget(selectedStationModel, selectedSourceModel);
    solver->setObjectName("Solver_Widged");
    ui->tabWidget_simAna->addTab(solver, "Solve");
    connect(selectedStationModel, SIGNAL(itemChanged(QStandardItem *)), solver, SLOT(addStations(QStandardItem *)));
    connect(selectedStationModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), solver, SLOT(addStations()));

    connect(selectedSourceModel, SIGNAL(itemChanged(QStandardItem *)), solver, SLOT(addSources(QStandardItem *)));
    connect(selectedSourceModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), solver, SLOT(addSources()));

    Priorities *priorities = new Priorities(selectedStationModel);
    priorities->setObjectName("Priorities_Widged");
    ui->tabWidget_simAna->addTab(priorities, "Priority");
    connect(selectedStationModel, SIGNAL(itemChanged(QStandardItem *)), priorities, SLOT(addStations(QStandardItem *)));
    connect(selectedStationModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), priorities, SLOT(addStations()));

    readSources();
    readSatellites();
    readStations();

    createModesPolicyTable();
    createModesCustonBandTable();

    defaultParameters();


    ui->treeWidget_2->expandAll();


    ui->splitter_4->setSizes(QList<int>({INT_MAX, INT_MAX}));
    ui->splitter_6->setSizes(QList<int>({INT_MAX, INT_MAX}));
    ui->splitter_statistics->setSizes({1000,5000});

    ui->spinBox_fontSize->setValue(QApplication::font().pointSize());
    ui->iconSizeSpinBox->setValue(ui->fileToolBar->iconSize().width());

    auto hv7 = ui->treeWidget_setupStationAxis->header();
    hv7->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget_calibrationBlock->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->spinBox_NCalibrationBlocks->setValue(7);

    connect(ui->pushButton_setupAxisAdd,SIGNAL(clicked(bool)),this,SLOT(setupStationAxisBufferAddRow()));

    setupStationAxisBufferAddRow();


    connect(ui->lineEdit_faqSearch,SIGNAL(textChanged(QString)),this,SLOT(faqSearch()));

    ui->spinBox_scanSequenceCadence->setValue(1);
    ui->spinBox_scanSequenceCadence->setMinimum(1);

    statistics = new Statistics(ui->treeWidget_statisticGeneral,
                                ui->treeWidget_statisticHovered,
                                ui->label_statistics_hoveredItem_title,
                                ui->verticalLayout_statisticPlot,
                                ui->horizontalScrollBar_statistics,
                                ui->spinBox_statistics_show,
                                ui->listWidget_statistics,
                                ui->checkBox_statistics_removeMinimum,
                                ui->radioButton_statistics_relative,
                                ui->radioButton_statistics_absolute,
                                ui->lineEdit_outputPath);

    connect(ui->pushButton_addStatistic,SIGNAL(clicked()),statistics,SLOT(on_pushButton_addStatistic_clicked()));
    connect(ui->pushButton_removeStatistic,SIGNAL(clicked()),statistics,SLOT(on_pushButton_removeStatistic_clicked()));
    connect(ui->horizontalScrollBar_statistics,SIGNAL(valueChanged(int)),statistics,SLOT(on_horizontalScrollBar_statistics_valueChanged(int)));
    connect(ui->spinBox_statistics_show,SIGNAL(valueChanged(int)),statistics,SLOT(on_spinBox_statistics_show_valueChanged(int)));
    connect(ui->treeWidget_statisticGeneral,SIGNAL(itemChanged(QTreeWidgetItem *, int)),statistics,SLOT(on_treeWidget_statisticGeneral_itemChanged(QTreeWidgetItem *, int)));

    statistics->setupStatisticView();
    setupSkyCoverageTemplatePlot();

    ui->comboBox_calibration_sources->setModel(allSourcePlusGroupModel);
    ui->comboBox_calibration_sources2->setModel(allSourcePlusGroupModel);
    ui->comboBox_calibration_sources3->setModel(allSourcePlusGroupModel);
    ui->comboBox_conditions_members->setModel(allSourcePlusGroupModel);
    connect(ui->pushButton_addSourceGroup_conditions,SIGNAL(clicked(bool)), this, SLOT(addGroupSource()));

    ui->comboBox_highImpactStation->setModel(allStationPlusGroupModel);
    connect(ui->pushButton_addGroupStationHighImpactAzEl,SIGNAL(clicked(bool)), this, SLOT(addGroupStation()));

    connect(ui->lineEdit_ivsMaster,SIGNAL(returnPressed()),this,SLOT(on_pushButton_clicked()));
    connect(ui->horizontalSlider_markerSizeWorldmap,SIGNAL(valueChanged(int)),this,SLOT(markerWorldmap()));
    connect(ui->horizontalSlider_markerSkymap,SIGNAL(valueChanged(int)),this,SLOT(markerSkymap()));

    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    ui->plainTextEdit_notes->setFont(fixedFont);


    connect(ui->sampleBitsSpinBox,SIGNAL(valueChanged(int)),this,SLOT(gbps()));
    connect(ui->sampleRateDoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(gbps()));
    gbps();

    ui->dateTimeEdit_sessionStart->setDisplayFormat("dd.MM.yyyy HH:mm");

    Model_Mode *modelModes = new Model_Mode(QVector<QString>(), this);
    ui->tableView_observingMode_mode->setModel(modelModes);
    auto mhv1 = ui->tableView_observingMode_mode->horizontalHeader();
    mhv1->setSectionResizeMode(QHeaderView::ResizeToContents);

    Model_Freq *modelFreq = new Model_Freq(this);
    ui->tableView_observingMode_freq->setModel(modelFreq);
    auto mhv2 = ui->tableView_observingMode_freq->horizontalHeader();
    mhv2->setSectionResizeMode(QHeaderView::ResizeToContents);

    Model_Bbc *modelBbc = new Model_Bbc(this);
    ui->tableView_observingMode_bbc->setModel(modelBbc);
    auto mhv3 = ui->tableView_observingMode_bbc->horizontalHeader();
    mhv3->setSectionResizeMode(QHeaderView::ResizeToContents);

    Model_If *modelIf = new Model_If(this);
    ui->tableView_observingMode_if->setModel(modelIf);
    auto mhv4 = ui->tableView_observingMode_if->horizontalHeader();
    mhv4->setSectionResizeMode(QHeaderView::ResizeToContents);

    Model_Tracks *modelTracks = new Model_Tracks(this);
    ui->tableView_observingMode_tracks->setModel(modelTracks);
    auto mhv5 = ui->tableView_observingMode_tracks->horizontalHeader();
    mhv5->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->comboBox_observingMode_mode,   SIGNAL(currentIndexChanged(int)), this, SLOT(changeObservingModeSelection(int)));
    connect(ui->comboBox_observingMode_freq,   SIGNAL(currentIndexChanged(int)), this, SLOT(changeObservingModeSelection(int)));
    connect(ui->comboBox_observingMode_bbc,    SIGNAL(currentIndexChanged(int)), this, SLOT(changeObservingModeSelection(int)));
    connect(ui->comboBox_observingMode_if,     SIGNAL(currentIndexChanged(int)), this, SLOT(changeObservingModeSelection(int)));
    connect(ui->comboBox_observingMode_tracks, SIGNAL(currentIndexChanged(int)), this, SLOT(changeObservingModeSelection(int)));

    ui->tableWidget_contact->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->checkBox_weightCoverage, SIGNAL(stateChanged(int)), this, SLOT(updateWeightFactorSliders()));
    connect(ui->checkBox_weightNobs, SIGNAL(stateChanged(int)), this, SLOT(updateWeightFactorSliders()));
    connect(ui->checkBox_weightDuration, SIGNAL(stateChanged(int)), this, SLOT(updateWeightFactorSliders()));
    connect(ui->checkBox_weightAverageSources, SIGNAL(stateChanged(int)), this, SLOT(updateWeightFactorSliders()));
    connect(ui->checkBox_weightAverageStations, SIGNAL(stateChanged(int)), this, SLOT(updateWeightFactorSliders()));
    connect(ui->checkBox_weightAverageBaselines, SIGNAL(stateChanged(int)), this, SLOT(updateWeightFactorSliders()));
    connect(ui->checkBox_weightIdleTime, SIGNAL(stateChanged(int)), this, SLOT(updateWeightFactorSliders()));
    connect(ui->checkBox_weightLowDeclination, SIGNAL(stateChanged(int)), this, SLOT(updateWeightFactorSliders()));
    connect(ui->checkBox_weightLowElevation, SIGNAL(stateChanged(int)), this, SLOT(updateWeightFactorSliders()));


    connect(ui->doubleSpinBox_weightSkyCoverage, SIGNAL(valueChanged(double)), this, SLOT(updateWeightFactorSliders()));
    connect(ui->doubleSpinBox_weightNumberOfObservations, SIGNAL(valueChanged(double)), this, SLOT(updateWeightFactorSliders()));
    connect(ui->doubleSpinBox_weightDuration, SIGNAL(valueChanged(double)), this, SLOT(updateWeightFactorSliders()));
    connect(ui->doubleSpinBox_weightAverageSources, SIGNAL(valueChanged(double)), this, SLOT(updateWeightFactorSliders()));
    connect(ui->doubleSpinBox_weightAverageStations, SIGNAL(valueChanged(double)), this, SLOT(updateWeightFactorSliders()));
    connect(ui->doubleSpinBox_weightAverageBaselines, SIGNAL(valueChanged(double)), this, SLOT(updateWeightFactorSliders()));
    connect(ui->doubleSpinBox_weightIdleTime, SIGNAL(valueChanged(double)), this, SLOT(updateWeightFactorSliders()));
    connect(ui->doubleSpinBox_weightLowDec, SIGNAL(valueChanged(double)), this, SLOT(updateWeightFactorSliders()));
    connect(ui->doubleSpinBox_weightLowEl, SIGNAL(valueChanged(double)), this, SLOT(updateWeightFactorSliders()));

    updateWeightFactorSliders();




    try {
        download();
    } catch ( ... ) {

    }

}

MainWindow::~MainWindow()
{
    delete ui;
}


// ########################################### NAVIGATION AND GUI SETUP ###########################################

void MainWindow::on_actionWelcome_triggered()
{
    ui->main_stacked->setCurrentIndex(0);
}

void MainWindow::on_actionSettings_triggered()
{
    ui->main_stacked->setCurrentIndex(1);
}

void MainWindow::on_actionInput_triggered()
{
    ui->main_stacked->setCurrentIndex(2);
}

void MainWindow::on_actionMode_triggered()
{
    ui->main_stacked->setCurrentIndex(3);
}

void MainWindow::on_actionGeneral_triggered()
{
    ui->main_stacked->setCurrentIndex(4);
}

void MainWindow::on_actionNetwork_triggered()
{
    ui->main_stacked->setCurrentIndex(5);
}

void MainWindow::on_actionSource_List_triggered()
{
    ui->main_stacked->setCurrentIndex(6);
}

void MainWindow::on_actionStation_triggered()
{
    ui->main_stacked->setCurrentIndex(7);
}

void MainWindow::on_actionSource_triggered()
{
    ui->main_stacked->setCurrentIndex(8);
}

void MainWindow::on_actionBaseline_triggered()
{
    ui->main_stacked->setCurrentIndex(9);
}

void MainWindow::on_actionWeight_factors_triggered()
{
    ui->main_stacked->setCurrentIndex(10);
}

void MainWindow::on_actionOutput_triggered()
{
    ui->main_stacked->setCurrentIndex(11);
}

void MainWindow::on_actionSimulator_triggered()
{
    ui->main_stacked->setCurrentIndex(12);
}

void MainWindow::on_actionRules_triggered()
{
    ui->main_stacked->setCurrentIndex(13);
}

void MainWindow::on_actionMulti_Scheduling_triggered()
{
    ui->main_stacked->setCurrentIndex(14);
}

void MainWindow::on_actionSky_Coverage_triggered()
{
    ui->main_stacked->setCurrentIndex(15);
}

void MainWindow::on_actionConditions_triggered()
{
    ui->main_stacked->setCurrentIndex(16);
}

void MainWindow::on_actionadvanced_triggered()
{
    ui->main_stacked->setCurrentIndex(17);
}

void MainWindow::on_actionsummary_triggered()
{
    ui->main_stacked->setCurrentIndex(18);
}

void MainWindow::on_actionSkd_Parser_triggered()
{
    ui->main_stacked->setCurrentIndex(19);
}

void MainWindow::on_actionLog_parser_triggered()
{
    ui->main_stacked->setCurrentIndex(20);
}

void MainWindow::on_actionFAQ_triggered()
{
    ui->main_stacked->setCurrentIndex(21);
}

void MainWindow::on_actionCurrent_Release_triggered()
{
    ui->main_stacked->setCurrentIndex(22);
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

void MainWindow::on_actionAbout_triggered()
{
    ui->main_stacked->setCurrentIndex(23);
}


void MainWindow::on_actionWhat_is_this_triggered()
{
    QWhatsThis::enterWhatsThisMode();
}

QString MainWindow::on_actionSave_triggered()
{
    QString txt = "Do you want to save xml file?";

    QString result;
    if (QMessageBox::Yes == QMessageBox::question(this, "Save?", txt, QMessageBox::Yes | QMessageBox::No)){
        result = writeXML();
    }
    return result;
}

void MainWindow::on_actionOpen_triggered()
{
    QString startPath = ui->lineEdit_sessionPath->text();
    QString path = QFileDialog::getOpenFileName(this, "Browse to xml file", startPath, tr("xml files (*.xml)"));
    if( !path.isEmpty() ){
        try{
            loadXML(path);
        }catch(...){
            QMessageBox::warning(this, "Error", "Error while loading "+path);
        }

    }
}

void MainWindow::on_pushButton_2_clicked()
{
    QProcess *start = new QProcess(this);

    QString pathToExe = ui->pathToSchedulerLineEdit->text();
    QDir dirToExe(pathToExe);
    QString absolutePathToExe = dirToExe.absolutePath();

    #ifdef Q_OS_WIN
    QString program = absolutePathToExe;
    //program = "\""+program+"\"";
    start->start("cmd.exe",
                 QStringList() << "/c" << program);
    #else
    QString program = absolutePathToExe;
    //program = "\""+program+"\"";
    QStringList arguments;
    start->start(program);
    #endif

    start->waitForFinished();
    QString output(start->readAllStandardOutput());
    if(output.isEmpty()){
        output = "Connection was not successul. Check your path to executable.\n"
                 "In case you have whitspaces in your path consider moving VieSched++ to a location without whitespaces in its path.";
    }

    QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    fixedFont.setPointSize(8);
    QMessageBox *msg = new QMessageBox(QMessageBox::Information, "Test connection", output, QMessageBox::Ok, this);
    msg->setFont(fixedFont);
    msg->exec();
}


void MainWindow::on_actionRun_triggered()
{
    QString path = on_actionSave_triggered();

    QDir mydir(path);
    QString fullPath = mydir.absolutePath();
    if(!path.isEmpty()){
        QDockWidget *dw = new QDockWidget(this);
//        dw->setStyleSheet("background-color:gray;");

        dw->setWindowTitle("VieSchedpp log");
        QWidget * base = new QWidget();

        QHBoxLayout *header = new QHBoxLayout(dw);
        QPushButton *d = new QPushButton(dw);
        d->setText("terminate");
        d->setIcon(QIcon(":/icons/icons/edit-delete-6.png"));
        header->insertWidget(0,new QLabel("processing file: "+path,dw),1);
        header->insertWidget(1,d,0);

        QVBoxLayout *l1 = new QVBoxLayout(dw);

        myTextBrowser *tb = new myTextBrowser(dw);

        l1->insertLayout(0,header);
        l1->insertWidget(1,tb);

        base->setLayout(l1);
        dw->setWidget(base);


        QList<QDockWidget *> dockWidgets = this->findChildren<QDockWidget *>();

        if(dockWidgets.size() == 1){
            addDockWidget(Qt::BottomDockWidgetArea,dw);
        }else{
            tabifyDockWidget(dockWidgets.at(0),dw);
        }

        QProcess *start = new QProcess(this);
        QString pathToExe = ui->pathToSchedulerLineEdit->text();
        QDir dirToExe(pathToExe);
        QString absolutePathToExe = dirToExe.absolutePath();
        #ifdef Q_OS_WIN
            QString program = absolutePathToExe;
            //program = "\""+program+"\"";
            start->start("cmd.exe",
                         QStringList() << "/c" << program << fullPath);
        #else
            QString program = absolutePathToExe;
            QStringList arguments;
            arguments << fullPath;
            start->start(program, arguments);
        #endif


        QPushButton *sb = new QPushButton(dw);
        sb->setText("terminate");
        sb->setIcon(QIcon(":/icons/icons/edit-delete-6.png"));
        sb->setToolTip("session: "+path);
        sb->setStatusTip("session: "+path);
        sb->setMinimumSize(30,30);
        ui->statusBar->insertPermanentWidget(1,sb);

        for(auto &any: ui->statusBar->children()){
            QLabel *l = qobject_cast<QLabel *>(any);
            if(l){
                QString txt = l->text();
                double i = 1;
                if(txt == "no schedules started" || txt == "everything finished"){
                    l->setText("1 process running:");
                }else{
                    i = txt.split(" ").at(0).toDouble() +1;
                    l->setText(QString("%1 processes running:").arg(i));
                }

            }
        }
        connect(d,SIGNAL(pressed()),start,SLOT(kill()));
        connect(d,SIGNAL(pressed()),dw,SLOT(close()));
        connect(start,SIGNAL(readyReadStandardOutput()),tb,SLOT(readyReadStandardOutput()));
        connect(start,SIGNAL(readyReadStandardError()),tb,SLOT(readyReadStandardError()));
        connect(start,SIGNAL(finished(int)),d,SLOT(hide()));
        connect(start,SIGNAL(finished(int)),this,SLOT(processFinished()));
        connect(start,SIGNAL(finished(int)),sb,SLOT(deleteLater()));
        connect(sb,SIGNAL(pressed()),d,SLOT(hide()));
        connect(sb,SIGNAL(pressed()),start,SLOT(kill()));
        connect(sb,SIGNAL(pressed()),dw,SLOT(close()));

        if(start->waitForStarted()){
//            QMessageBox::information(this,"Scheduling started!","Starting scheduling " + fullPath +"!");
        }else{
            QMessageBox::warning(this,"Scheduling failed to start!","Could not start process:\n" + program +"\nwith arguments:\n" + fullPath);
        }
    }
}

void MainWindow::processFinished(){
    for(auto &any: ui->statusBar->children()){
        QLabel *l = qobject_cast<QLabel *>(any);
        if(l){
            QString txt = l->text();
            if(txt.isEmpty()){
                l->setText(" ");
            }else{
                double i = txt.split(" ").at(0).toDouble();
                if(i==1){
                    l->setText(QString("everything finished"));
                }else if(i==2){
                    l->setText(QString("1 process running"));
                }else{
                    l->setText(QString("%1 processes running").arg(i-1));
                }
            }
        }
    }
}


void MainWindow::on_actionExit_triggered()
{
    if (QMessageBox::Yes == QMessageBox::question(this, "Exit?", "Do you really want to exit?", QMessageBox::Yes | QMessageBox::No))
    {
        QSettings settings("TU Wien","VieSched++ GUI");
        settings.setValue("geometry", saveGeometry());
        settings.setValue("windowState", saveState());
        QApplication::quit();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)  // show prompt when user wants to close app
{
    event->ignore();
    if (QMessageBox::Yes == QMessageBox::question(this, "Exit?", "Do you really want to exit?", QMessageBox::Yes | QMessageBox::No))
    {
        QSettings settings("TU Wien","VieSched++ GUI");
        settings.setValue("geometry", saveGeometry());
        event->accept();
    }

}

void MainWindow::on_pushButton_18_clicked()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->pathToGUILineEdit->text());
}

void MainWindow::on_pushButton_19_clicked()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->defaultSettingsFileLineEdit->text());
}

void MainWindow::on_actionNew_triggered()
{
    if (QMessageBox::Yes == QMessageBox::question(this, "Restart?", "Do you really want to restart?", QMessageBox::Yes | QMessageBox::No)){
        QApplication::exit(1000);
    }
}



void MainWindow::on_spinBox_fontSize_valueChanged(int arg1)
{
    QFont myFont = ui->fontComboBox_font->currentFont();
    myFont.setPointSize(arg1);
    worldMapCallout->setFont(myFont);
    skyMapCallout->setFont(myFont);
    QApplication::setFont(myFont);
}

void MainWindow::on_fontComboBox_font_currentFontChanged(const QFont &f)
{
    QFont myFont = f;
    myFont.setPointSize(ui->spinBox_fontSize->value());
    worldMapCallout->setFont(myFont);
    skyMapCallout->setFont(myFont);
    QApplication::setFont(myFont);
}

void MainWindow::on_iconSizeSpinBox_valueChanged(int arg1)
{
    ui->fileToolBar->setIconSize(QSize(arg1,arg1));
    ui->basicToolBar->setIconSize(QSize(arg1,arg1));
    ui->advancedToolBar->setIconSize(QSize(arg1,arg1));
    ui->helpToolBar->setIconSize(QSize(arg1,arg1));
    ui->analysisToolBar->setIconSize(QSize(arg1,arg1));
}

void MainWindow::on_treeWidget_2_itemChanged(QTreeWidgetItem *item, int column)
{
    for(int i = 0; i<item->childCount(); ++i){
        if(item->checkState(0) == Qt::Checked){
            item->child(i)->setDisabled(false);
        }else{
            item->child(i)->setDisabled(true);
        }
    }

    if(item->text(0) == "Files"){
        if(item->checkState(0) == Qt::Checked){
            ui->fileToolBar->show();
        }else{
            ui->fileToolBar->hide();
        }
    } else if(item->text(0) == "Basic"){
        if(item->checkState(0) == Qt::Checked){
            ui->basicToolBar->show();
        }else{
            ui->basicToolBar->hide();
        }
    } else if(item->text(0) == "Advanced"){
        if(item->checkState(0) == Qt::Checked){
            ui->advancedToolBar->show();
        }else{
            ui->advancedToolBar->hide();
        }
    } else if(item->text(0) == "Help"){
        if(item->checkState(0) == Qt::Checked){
            ui->helpToolBar->show();
        }else{
            ui->helpToolBar->hide();
        }
    } else if (item->text(0) == "Analysis"){
        if(item->checkState(0) == Qt::Checked){
            ui->analysisToolBar->show();
        }else{
            ui->analysisToolBar->hide();
        }
    } else if(item->text(0) == "Welcome"){
        auto actions = ui->fileToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Welcome"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Settings"){
        auto actions = ui->fileToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Settings"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "New"){
        auto actions = ui->fileToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "New"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Open"){
        auto actions = ui->fileToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Open"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Save"){
        auto actions = ui->fileToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Save"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Run"){
        auto actions = ui->fileToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Run"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Exit"){
        auto actions = ui->fileToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Exit"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Input"){
        auto actions = ui->basicToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Input"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Mode"){
        auto actions = ui->basicToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Mode"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "General"){
        auto actions = ui->basicToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "General"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Network"){
        auto actions = ui->basicToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Network"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Source List"){
        auto actions = ui->basicToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Source List"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Parameter Stations"){
        auto actions = ui->basicToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Station"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Parameter Sources"){
        auto actions = ui->basicToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Source"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Parameter Baselines"){
        auto actions = ui->basicToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Baseline"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Weight Factors"){
        auto actions = ui->basicToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Weight Factors"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Output"){
        auto actions = ui->basicToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Output"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Rules"){
        auto actions = ui->advancedToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Rules"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Multi Scheduling"){
        auto actions = ui->advancedToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Multi Scheduling"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Sky-Coverage"){
        auto actions = ui->advancedToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Sky-Coverage"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Conditions"){
        auto actions = ui->advancedToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Conditions"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "What is this?"){
        auto actions = ui->helpToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "What is this?"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "FAQ"){
        auto actions = ui->helpToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "FAQ"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Current Reference"){
        auto actions = ui->helpToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Current Reference"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "About"){
        auto actions = ui->helpToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "About"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "About Qt"){
        auto actions = ui->helpToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "About Qt"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Statistics"){
        auto actions = ui->analysisToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Statistics"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Log Parser"){
        auto actions = ui->analysisToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Log Parser"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    } else if(item->text(0) == "Sched Analyser"){
        auto actions = ui->analysisToolBar->actions();
        for(const auto &any:actions){
            if(any->text() == "Skd Parser"){
                if(item->checkState(0) == Qt::Checked){
                    any->setVisible(true);
                }else{
                    any->setVisible(false);
                }
            }
        }
    }
}

void MainWindow::on_pushButton_browseExecutable_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Browse to VieSchedpp executable", "");
    if( !path.isEmpty() ){
        ui->pathToSchedulerLineEdit->setText(path);
    }

}


// ########################################### READ AND WRITE XML DOCUMENT ###########################################

void MainWindow::defaultParameters()
{
    VieVS::ParameterSettings::ParametersStations sta;
    sta.available = true;
    sta.availableForFillinmode = true;
    sta.maxScan = 600;
    sta.minScan = 30;
    sta.minSlewtime = 0;
    sta.maxSlewtime = 600;
    sta.maxSlewDistance = 175;
    sta.minSlewDistance = 0;
    sta.maxWait = 600;
    sta.maxTotalObsTime = 999999;
    sta.maxNumberOfScans = 9999;
    sta.weight = 1;
    sta.minElevation = 5;
    sta.preob = 10;
    sta.midob = 3;
    sta.systemDelay = 6;

    auto stationTree = settings_.get_child_optional("settings.station.parameters");
    if(stationTree.is_initialized()){
        for(const auto& it: *stationTree){
            std::string parameterName = it.second.get_child("<xmlattr>.name").data();
            if(parameterName == "default"){
                for (auto &it2: it.second) {
                    std::string paraName = it2.first;
                    if (paraName == "<xmlattr>") {
                        continue;
                    } else if (paraName == "weight") {
                        sta.weight = it2.second.get_value<double>();
                    } else if (paraName == "minScan") {
                        sta.minScan = it2.second.get_value < unsigned int > ();
                    } else if (paraName == "maxScan") {
                        sta.maxScan = it2.second.get_value < unsigned int > ();
                    } else if (paraName == "minSlewtime") {
                        sta.minSlewtime = it2.second.get_value < unsigned int > ();
                    } else if (paraName == "maxSlewtime") {
                        sta.maxSlewtime = it2.second.get_value < unsigned int > ();
                    } else if (paraName == "maxSlewDistance") {
                        sta.maxSlewDistance = it2.second.get_value < double > ();
                    } else if (paraName == "minSlewDistance") {
                        sta.minSlewDistance = it2.second.get_value < double > ();
                    } else if (paraName == "maxWait") {
                        sta.maxWait = it2.second.get_value < unsigned int > ();
                    } else if (paraName == "dataWriteSpeed") {
                        sta.dataWriteRate = it2.second.get_value < double > ();
                    } else if (paraName == "minElevation") {
                        sta.minElevation = it2.second.get_value < unsigned int > ();
                    } else if (paraName == "maxNumberOfScans") {
                        sta.maxNumberOfScans = it2.second.get_value < unsigned int > ();
                    } else if (paraName == "maxTotalObsTime") {
                        sta.maxTotalObsTime = it2.second.get_value < unsigned int > ();
                    } else if (paraName == "preob") {
                        sta.preob = it2.second.get_value < unsigned int > ();
                    } else if (paraName == "midob") {
                        sta.midob = it2.second.get_value < unsigned int > ();
                    } else if (paraName == "systemDelay") {
                        sta.systemDelay = it2.second.get_value < unsigned int > ();
                    } else {
                        QString txt = "Ignoring parameter: ";
                        txt.append(QString::fromStdString(paraName)).append(" in station default parameters!\nCheck settings.xml file!");
                        QMessageBox::warning(this,"Wrong default setting!",txt,QMessageBox::Ok);
                    }
                }
            }
        }
    }else{
        QMessageBox::warning(this,"Missing default parameters!","You have no entry for your default station parameters in settings.xml file! Internal backup values are used!",QMessageBox::Ok);
    }

    VieVS::ParameterSettings::ParametersSources src;
    src.available = true;
    src.availableForFillinmode = true;
    src.minRepeat = 1800;
    src.minScan = 0;
    src.maxScan = 9999;
    src.weight = 1;
    src.minFlux = 0.05;
    src.maxNumberOfScans = 999;
    src.minNumberOfStations = 3;
    src.minElevation = 0;
    src.minSunDistance = 4;

    auto sourceTree = settings_.get_child_optional("settings.source.parameters");
    if(sourceTree.is_initialized()){
        for(const auto& it: *sourceTree){
            std::string parameterName = it.second.get_child("<xmlattr>.name").data();
            if(parameterName == "default"){
                for (auto &it2: it.second) {
                    std::string paraName = it2.first;
                    if (paraName == "<xmlattr>") {
                        continue;
                    } else if (paraName == "weight") {
                        src.weight = it2.second.get_value<double>();
                    } else if (paraName == "minScan") {
                        src.minScan = it2.second.get_value < unsigned int > ();
                    } else if (paraName == "maxScan") {
                        src.maxScan = it2.second.get_value < unsigned int > ();
                    } else if (paraName == "minRepeat") {
                        src.minRepeat = it2.second.get_value < unsigned int > ();
                    } else if (paraName == "minFlux"){
                        src.minFlux = it2.second.get_value<double>();
                    } else if (paraName == "minElevation"){
                        src.minElevation = it2.second.get_value<double>();
                    } else if (paraName == "minSunDistance"){
                        src.minSunDistance = it2.second.get_value<double>();
                    } else if (paraName == "maxNumberOfScans"){
                        src.maxNumberOfScans = it2.second.get_value<double>();
                    } else if (paraName == "minNumberOfStations"){
                        src.minNumberOfStations = it2.second.get_value<double>();
                    } else {
                        QString txt = "Ignoring parameter: ";
                        txt.append(QString::fromStdString(paraName)).append(" in source default parameters!\nCheck settings.xml file!");
                        QMessageBox::warning(this,"Wrong default setting!",txt,QMessageBox::Ok);
                    }
                }
            }
        }
    }else{
        QMessageBox::warning(this,"Missing default parameters!","You have no entry for your default source parameters in settings.xml file! Internal backup values are used!",QMessageBox::Ok);
    }
    VieVS::ParameterSettings::ParametersBaselines bl;
    bl.ignore = false;
    bl.maxScan = 9999;
    bl.minScan = 0;
    bl.weight = 1;
    auto baselineTree = settings_.get_child_optional("settings.baseline.parameters");
    if(baselineTree.is_initialized()){
        for(const auto& it: *baselineTree){
            std::string parameterName = it.second.get_child("<xmlattr>.name").data();
            if(parameterName == "default"){
                for (auto &it2: it.second) {
                    std::string paraName = it2.first;
                    if (paraName == "<xmlattr>") {
                        continue;
                    } else if (paraName == "weight") {
                        bl.weight = it2.second.get_value<double>();
                    } else if (paraName == "minScan") {
                        bl.minScan = it2.second.get_value < unsigned int > ();
                    } else if (paraName == "maxScan") {
                        bl.maxScan = it2.second.get_value < unsigned int > ();
                    } else {
                        QString txt = "Ignoring parameter: ";
                        txt.append(QString::fromStdString(paraName)).append(" in baseline default parameters!\nCheck settings.xml file!");
                        QMessageBox::warning(this,"Wrong default setting!",txt,QMessageBox::Ok);
                    }
                }
            }
        }
    }else{
        QMessageBox::warning(this,"Missing default parameters!","You have no entry for your default baseline parameters in settings.xml file! Internal backup values are used!",QMessageBox::Ok);
    }

    stationSetupWidget->addParameter("default", sta);
    sourceSetupWidget->addParameter("default", src);
    baselineSetupWidget->addParameter("default", bl);

    satelliteSetupWidget->addParameter("default", src);
    spacecraftSetupWidget->addParameter("default", src);

    VieVS::ParameterSettings::ParametersStations down;
    down.available = false;
    stationSetupWidget->addParameter("down", down);

    clearSetup(true,true,true);


    boost::optional<double> ax1low = settings_.get_optional<double>("settings.station.cableWrapBuffers.axis1LowOffset");
    if(ax1low.is_initialized()){
//        ui->DoubleSpinBox_axis1low->setValue(*ax1low);
    }
    boost::optional<double> ax1up = settings_.get_optional<double>("settings.station.cableWrapBuffers.axis1UpOffset");
    if(ax1up.is_initialized()){
//        ui->DoubleSpinBox_axis1up->setValue(*ax1up);
    }
    boost::optional<double> ax2low = settings_.get_optional<double>("settings.station.cableWrapBuffers.axis2LowOffset");
    if(ax2low.is_initialized()){
//        ui->DoubleSpinBox_axis2low->setValue(*ax2low);
    }
    boost::optional<double> ax2up = settings_.get_optional<double>("settings.station.cableWrapBuffers.axis2UpOffset");
    if(ax2up.is_initialized()){
//        ui->DoubleSpinBox_axis2up->setValue(*ax2up);
    }

    boost::optional<std::string> skdMode = settings_.get_optional<std::string>("settings.mode.skdMode");
    if(skdMode.is_initialized()){
        ui->comboBox_skedObsModes->setCurrentText(QString::fromStdString(*skdMode));
    }

    boost::optional<bool> fillinmodeDuringScanSelection = settings_.get_optional<bool>("settings.general.fillinmodeDuringScanSelection");
    if(fillinmodeDuringScanSelection.is_initialized()){
        ui->checkBox_fillinmode_duringscan->setChecked(*fillinmodeDuringScanSelection);
    }
    boost::optional<bool> fillinmodeInfluenceOnSchedule = settings_.get_optional<bool>("settings.general.fillinmodeInfluenceOnSchedule");
    if(fillinmodeInfluenceOnSchedule.is_initialized()){
        ui->checkBox_fillinModeInfluence->setChecked(*fillinmodeInfluenceOnSchedule);
    }

    if(settings_.get("settings.general.fillinmodeAPosteriori_minNumberOfStations_checked",false)){
        ui->checkBox_fi_apost_minSta->setChecked(true);
        ui->spinBox_fi_apost_minSta->setEnabled(true);
        ui->spinBox_fi_apost_minSta->setValue(settings_.get("settings.general.fillinmodeAPosteriori_minNumberOfStations",2));
    }else{
        ui->checkBox_fi_apost_minSta->setChecked(false);
        ui->spinBox_fi_apost_minSta->setEnabled(false);
    }
    if(settings_.get("settings.general.fillinmodeAPosteriori_minRepeat_checked",false)){
        ui->checkBox_fi_apost_minRepeat->setChecked(true);
        ui->spinBox_fi_apost_minRepeat->setEnabled(true);
        ui->spinBox_fi_apost_minRepeat->setValue(settings_.get("settings.general.fillinmodeAPosteriori_minRepeat",180));
    }else{
        ui->checkBox_fi_apost_minRepeat->setChecked(false);
        ui->spinBox_fi_apost_minRepeat->setEnabled(false);
    }
    boost::optional<bool> fillinmodeAPosteriori = settings_.get_optional<bool>("settings.general.fillinmodeAPosteriori");
    ui->groupBox_fillinmode_aposteriori->setChecked(true);
    if(fillinmodeAPosteriori.is_initialized()){
        ui->groupBox_fillinmode_aposteriori->setChecked(*fillinmodeAPosteriori);
    }else{
        ui->groupBox_fillinmode_aposteriori->setChecked(false);
    }

    boost::optional<double> subnettingMinAngle = settings_.get_optional<double>("settings.general.subnettingMinAngle");
    if(subnettingMinAngle.is_initialized()){
        ui->doubleSpinBox_subnettingDistance->setValue(*subnettingMinAngle);
    }
    boost::optional<double> subnettingMinNSta = settings_.get_optional<double>("settings.general.subnettingMinNStaPercent");
    if(subnettingMinNSta.is_initialized()){
        ui->doubleSpinBox_subnettingMinStations->setValue(*subnettingMinNSta);
    }
    boost::optional<double> subnettingMinNStaAllBut = settings_.get_optional<double>("settings.general.subnettingMinNStaAllBut");
    if(subnettingMinNStaAllBut.is_initialized()){
        ui->spinBox_subnetting_min_sta->setValue(*subnettingMinNStaAllBut);
    }
    boost::optional<bool> subnettingFlag = settings_.get_optional<bool>("settings.general.subnettingMinNstaPercent_otherwiseAllBut");
    if(subnettingFlag.is_initialized()){
        if(*subnettingFlag){
            ui->radioButton_subnetting_percent->setChecked(true);
        }else{
            ui->radioButton_subnetting_allBut->setChecked(true);
        }
    }
    boost::optional<bool> subnetting = settings_.get_optional<bool>("settings.general.subnetting");
    if(subnetting.is_initialized()){
        ui->groupBox_subnetting->setChecked(*subnetting);
    }
    boost::optional<bool> idleToObserving = settings_.get_optional<bool>("settings.general.idleToObservingTime");
    if(idleToObserving.is_initialized()){
        if(*idleToObserving){
            ui->radioButton_idleToObservingTime_yes->setChecked(true);
        }else{
            ui->radioButton_idleToObservingTime_no->setChecked(true);
        }
    }

    boost::optional<std::string> alignObservingTime = settings_.get_optional<std::string>("settings.general.alignObservingTime");
    if(alignObservingTime.is_initialized()){
        if(*alignObservingTime == "start"){
            ui->radioButton_alignStart->setChecked(true);
        }else if(*alignObservingTime == "end"){
            ui->radioButton_alignEnd->setChecked(true);
        }else if(*alignObservingTime == "individual"){
            ui->radioButton_alignIndividual->setChecked(true);
        }
    }

    boost::optional<double> influenceDistance = settings_.get_optional<double>("settings.skyCoverage.influenceDistance");
    if(influenceDistance.is_initialized()){
        ui->influenceDistanceDoubleSpinBox->setValue(*influenceDistance);
    }
    boost::optional<int> influenceInterval = settings_.get_optional<int>("settings.skyCoverage.influenceInterval");
    if(influenceInterval.is_initialized()){
        ui->influenceTimeSpinBox->setValue(*influenceInterval);
    }
    boost::optional<double> maxTwinTelecopeDistance = settings_.get_optional<double>("settings.skyCoverage.maxTwinTelecopeDistance");
    if(maxTwinTelecopeDistance.is_initialized()){
        ui->maxDistanceForCombiningAntennasDoubleSpinBox->setValue(*maxTwinTelecopeDistance);
    }
    boost::optional<std::string> distanceType = settings_.get_optional<std::string>("settings.skyCoverage.distanceType");
    if(distanceType.is_initialized()){
        ui->comboBox_skyCoverageDistanceType->setCurrentText(QString::fromStdString(*distanceType));
    }
    boost::optional<std::string> timeType = settings_.get_optional<std::string>("settings.skyCoverage.timeType");
    if(timeType.is_initialized()){
        ui->comboBox_skyCoverageTimeType->setCurrentText(QString::fromStdString(*timeType));
    }

    boost::optional<bool> skyCoverageChecked = settings_.get_optional<bool>("settings.weightFactor.skyCoverageChecked");
    if(skyCoverageChecked.is_initialized()){
        ui->checkBox_weightCoverage->setChecked(*skyCoverageChecked);
//        ui->doubleSpinBox_weightSkyCoverage->setEnabled(*skyCoverageChecked);
    }
    boost::optional<bool> numberOfObservationsChecked = settings_.get_optional<bool>("settings.weightFactor.numberOfObservationsChecked");
    if(numberOfObservationsChecked.is_initialized()){
        ui->checkBox_weightNobs->setChecked(*numberOfObservationsChecked);
//        ui->doubleSpinBox_weightNumberOfObservations->setEnabled(*numberOfObservationsChecked);
    }
    boost::optional<bool> durationChecked = settings_.get_optional<bool>("settings.weightFactor.durationChecked");
    if(durationChecked.is_initialized()){
        ui->checkBox_weightDuration->setChecked(*durationChecked);
//        ui->doubleSpinBox_weightDuration->setEnabled(*durationChecked);
    }
    boost::optional<bool> averageSourcesChecked = settings_.get_optional<bool>("settings.weightFactor.averageSourcesChecked");
    if(averageSourcesChecked.is_initialized()){
        ui->checkBox_weightAverageSources->setChecked(*averageSourcesChecked);
//        ui->doubleSpinBox_weightAverageSources->setEnabled(*averageSourcesChecked);
    }
    boost::optional<bool> averageStationsChecked = settings_.get_optional<bool>("settings.weightFactor.averageStationsChecked");
    if(averageStationsChecked.is_initialized()){
        ui->checkBox_weightAverageStations->setChecked(*averageStationsChecked);
//        ui->doubleSpinBox_weightAverageStations->setEnabled(*averageStationsChecked);
    }
    boost::optional<bool> idleTimeChecked = settings_.get_optional<bool>("settings.weightFactor.idleTimeChecked");
    if(idleTimeChecked.is_initialized()){
        ui->checkBox_weightIdleTime->setChecked(*idleTimeChecked);
    }
    boost::optional<bool> weightDeclinationChecked = settings_.get_optional<bool>("settings.weightFactor.weightDeclinationChecked");
    if(weightDeclinationChecked.is_initialized()){
        ui->checkBox_weightLowDeclination->setChecked(*weightDeclinationChecked);
//        ui->doubleSpinBox_weightLowDec->setEnabled(*weightDeclinationChecked);
//        ui->doubleSpinBox_weightLowDecStart->setEnabled(*weightDeclinationChecked);
//        ui->doubleSpinBox_weightLowDecEnd->setEnabled(*weightDeclinationChecked);
//        ui->label_weightLowDecEnd->setEnabled(*weightDeclinationChecked);
//        ui->label_weightLowDecStart->setEnabled(*weightDeclinationChecked);
    }
    boost::optional<bool> weightLowElevationChecked = settings_.get_optional<bool>("settings.weightFactor.weightLowElevationChecked");
    if(weightLowElevationChecked.is_initialized()){
        ui->checkBox_weightLowElevation->setChecked(*weightLowElevationChecked);
//        ui->doubleSpinBox_weightLowEl->setEnabled(*weightLowElevationChecked);
//        ui->doubleSpinBox_weightLowElStart->setEnabled(*weightLowElevationChecked);
//        ui->doubleSpinBox_weightLowElEnd->setEnabled(*weightLowElevationChecked);
//        ui->label_weightLowElStart->setEnabled(*weightLowElevationChecked);
//        ui->label_weightLowElEnd->setEnabled(*weightLowElevationChecked);
    }

    boost::optional<double> skyCoverage = settings_.get_optional<double>("settings.weightFactor.skyCoverage");
    if(skyCoverage.is_initialized()){
        ui->doubleSpinBox_weightSkyCoverage->setValue(*skyCoverage);
    }
    boost::optional<double> numberOfObservations = settings_.get_optional<double>("settings.weightFactor.numberOfObservations");
    if(numberOfObservations.is_initialized()){
        ui->doubleSpinBox_weightNumberOfObservations->setValue(*numberOfObservations);
    }
    boost::optional<double> duration = settings_.get_optional<double>("settings.weightFactor.duration");
    if(duration.is_initialized()){
        ui->doubleSpinBox_weightDuration->setValue(*duration);
    }
    boost::optional<double> averageSources = settings_.get_optional<double>("settings.weightFactor.averageSources");
    if(averageSources.is_initialized()){
        ui->doubleSpinBox_weightAverageSources->setValue(*averageSources);
    }
    boost::optional<double> averageStations = settings_.get_optional<double>("settings.weightFactor.averageStations");
    if(averageStations.is_initialized()){
        ui->doubleSpinBox_weightAverageStations->setValue(*averageStations);
    }
    boost::optional<double> weightIdleTime = settings_.get_optional<double>("settings.weightFactor.weightIdleTime");
    if(weightIdleTime.is_initialized()){
        ui->doubleSpinBox_weightIdleTime->setValue(*weightIdleTime);
    }
    boost::optional<double> idleTimeInterval = settings_.get_optional<double>("settings.weightFactor.idleTimeInterval");
    if(idleTimeInterval.is_initialized()){
        ui->spinBox_idleTimeInterval->setValue(*idleTimeInterval);
    }
    boost::optional<double> weightDeclination = settings_.get_optional<double>("settings.weightFactor.weightDeclination");
    if(weightDeclination.is_initialized()){
        ui->doubleSpinBox_weightLowDec->setValue(*weightDeclination);
    }
    boost::optional<double> declinationStartWeight = settings_.get_optional<double>("settings.weightFactor.declinationStartWeight");
    if(declinationStartWeight.is_initialized()){
        ui->doubleSpinBox_weightLowDecStart->setValue(*declinationStartWeight);
    }
    boost::optional<double> declinationFullWeight = settings_.get_optional<double>("settings.weightFactor.declinationFullWeight");
    if(declinationFullWeight.is_initialized()){
        ui->doubleSpinBox_weightLowDecEnd->setValue(*declinationFullWeight);
    }
    boost::optional<double> weightLowElevation = settings_.get_optional<double>("settings.weightFactor.weightLowElevation");
    if(weightLowElevation.is_initialized()){
        ui->doubleSpinBox_weightLowEl->setValue(*weightLowElevation);
    }
    boost::optional<double> lowElevationStartWeight = settings_.get_optional<double>("settings.weightFactor.lowElevationStartWeight");
    if(lowElevationStartWeight.is_initialized()){
        ui->doubleSpinBox_weightLowElStart->setValue(*lowElevationStartWeight);
    }
    boost::optional<double> lowElevationFullWeight = settings_.get_optional<double>("settings.weightFactor.lowElevationFullWeight");
    if(lowElevationFullWeight.is_initialized()){
        ui->doubleSpinBox_weightLowElEnd->setValue(*lowElevationFullWeight);
    }

    boost::optional<bool> initializer_log = settings_.get_optional<bool>("settings.output.initializer_log");
    if(initializer_log.is_initialized()){
        ui->checkBox_outputInitializer->setChecked(*initializer_log);
    }
    boost::optional<bool> iteration_log = settings_.get_optional<bool>("settings.output.iteration_log");
    if(iteration_log.is_initialized()){
        ui->checkBox_outputIteration->setChecked(*iteration_log);
    }
    boost::optional<bool> createSummary = settings_.get_optional<bool>("settings.output.createSummary");
    if(createSummary.is_initialized()){
        ui->checkBox_outputStatisticsFile->setChecked(*createSummary);
    }
    boost::optional<bool> createNGS = settings_.get_optional<bool>("settings.output.createNGS");
    if(createNGS.is_initialized()){
        ui->checkBox_outputNGSFile->setChecked(*createNGS);
    }
    boost::optional<bool> redirectNGS = settings_.get_optional<bool>("settings.output.redirectNGS");
    if(redirectNGS.is_initialized()){
        ui->checkBox_redirectNGS->setChecked(*redirectNGS);
    }
    boost::optional<std::string> outputNGS = settings_.get_optional<std::string>("settings.output.NGS_directory");
    if(outputNGS.is_initialized()){
        ui->lineEdit_outputNGS->setText(QString::fromStdString(*outputNGS));
    }
    boost::optional<bool> createSKD = settings_.get_optional<bool>("settings.output.createSKD");
    if(createSKD.is_initialized()){
        ui->checkBox_outputSkdFile->setChecked(*createSKD);
    }
    boost::optional<bool> createVEX = settings_.get_optional<bool>("settings.output.createVEX");
    if(createVEX.is_initialized()){
        ui->checkBox_outputVex->setChecked(*createVEX);
    }
    boost::optional<bool> createSnrTable = settings_.get_optional<bool>("settings.output.createSnrTable");
    if(createSnrTable.is_initialized()){
        ui->checkBox_outputSnrTable->setChecked(*createSnrTable);
    }
    boost::optional<bool> createOperationsNotes = settings_.get_optional<bool>("settings.output.createOperationsNotes");
    if(createOperationsNotes.is_initialized()){
        ui->checkBox_outputOperationsNotes->setChecked(*createOperationsNotes);
    }
    boost::optional<bool> createSourceGroupStatistics = settings_.get_optional<bool>("settings.output.createSourceGroupStatistics");
    if(createSourceGroupStatistics.is_initialized()){
        ui->checkBox_outputSourceGroupStatFile->setChecked(*createSourceGroupStatistics);
    }
    boost::optional<bool> addTimestamps = settings_.get_optional<bool>("settings.output.addTimestamps");
    if(addTimestamps.is_initialized()){
        ui->checkBox_outputAddTimestamp->setChecked(*addTimestamps);
    }
}


// ########################################### MODE ###########################################

void MainWindow::createModesPolicyTable()
{
    QHeaderView *hv = ui->tableWidget_ModesPolicy->horizontalHeader();
    hv->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void MainWindow::addModesPolicyTable(QString name){

    ui->tableWidget_ModesPolicy->insertRow(ui->tableWidget_ModesPolicy->rowCount());
    ui->tableWidget_ModesPolicy->setVerticalHeaderItem(ui->tableWidget_ModesPolicy->rowCount()-1,new QTableWidgetItem(name));
    QDoubleSpinBox *dsp = new QDoubleSpinBox(this);
    if(name == "S"){
        dsp->setValue(15.);
    }else{
        dsp->setValue(20.);
    }
    dsp->setMaximum(10000);

    QComboBox *psta = new QComboBox(this);
    psta->addItem("required");
    psta->addItem("optional");
    QComboBox *bsta = new QComboBox(this);
//    bsta->addItem("none");
    bsta->addItem("value");
    bsta->addItem("min value times");
    bsta->addItem("max value times");
    bsta->setEnabled(false);
    QDoubleSpinBox *vsta = new QDoubleSpinBox(this);
    vsta->setMinimum(0);
    vsta->setMaximum(100000);
    vsta->setSingleStep(.1);
    vsta->setValue(0);
    vsta->setEnabled(false);
    connect(psta, QOverload<int>::of(&QComboBox::currentIndexChanged), [psta, bsta, vsta](){
        if( psta->currentText() == "required" ){
            bsta->setEnabled(false);
            vsta->setEnabled(false);
        }else{
            bsta->setEnabled(true);
            emit bsta->currentIndexChanged(bsta->currentIndex());
        }
    });
    connect(bsta, QOverload<int>::of(&QComboBox::currentIndexChanged), [bsta, vsta](){
        if( bsta->currentText() == "internal model" || bsta->currentText() == "none" ){
            vsta->setEnabled(false);
            vsta->setValue(0.0);
        }else{
            vsta->setEnabled(true);
            if ( bsta->currentText() == "value" ){
                vsta->setValue(1000.0);
            }else{
                vsta->setValue(1.0);
            }
        }

    });






    QComboBox *psrc = new QComboBox(this);
    psrc->addItem("required");
    psrc->addItem("optional");
    QComboBox *bsrc = new QComboBox(this);
//    bsrc->addItem("none");
    bsrc->addItem("internal model");
    bsrc->addItem("value");
    bsrc->addItem("min value times");
    bsrc->addItem("max value times");
    bsrc->setEnabled(false);
    QDoubleSpinBox *vsrc = new QDoubleSpinBox(this);
    vsrc->setMinimum(0);
    vsrc->setMaximum(100000);
    vsrc->setSingleStep(.1);
    vsrc->setValue(0);
    vsrc->setEnabled(false);
    connect(psrc, QOverload<int>::of(&QComboBox::currentIndexChanged), [psrc, bsrc, vsrc](){
        if( psrc->currentText() == "required" ){
            bsrc->setEnabled(false);
            vsrc->setEnabled(false);
        }else{
            bsrc->setEnabled(true);
            emit bsrc->currentIndexChanged(bsrc->currentIndex());
        }
    });
    connect(bsrc, QOverload<int>::of(&QComboBox::currentIndexChanged), [bsrc, vsrc](){
        if( bsrc->currentText() == "internal model" || bsrc->currentText() == "none" ){
            vsrc->setEnabled(false);
            vsrc->setValue(0.0);
        }else{
            vsrc->setEnabled(true);
            if ( bsrc->currentText() == "value" ){
                vsrc->setValue(0.25);
            }else{
                vsrc->setValue(1.0);
            }
        }
    });


    ui->tableWidget_ModesPolicy->setCellWidget(ui->tableWidget_ModesPolicy->rowCount()-1,0,dsp);
    ui->tableWidget_ModesPolicy->setCellWidget(ui->tableWidget_ModesPolicy->rowCount()-1,1,psta);
    ui->tableWidget_ModesPolicy->setCellWidget(ui->tableWidget_ModesPolicy->rowCount()-1,2,bsta);
    ui->tableWidget_ModesPolicy->setCellWidget(ui->tableWidget_ModesPolicy->rowCount()-1,3,vsta);
    ui->tableWidget_ModesPolicy->setCellWidget(ui->tableWidget_ModesPolicy->rowCount()-1,4,psrc);
    ui->tableWidget_ModesPolicy->setCellWidget(ui->tableWidget_ModesPolicy->rowCount()-1,5,bsrc);
    ui->tableWidget_ModesPolicy->setCellWidget(ui->tableWidget_ModesPolicy->rowCount()-1,6,vsrc);
}

void MainWindow::createModesCustonBandTable()
{
    ui->groupBox_modeCustom->setChecked(Qt::Checked);
    addModesCustomTable("A", 3.2564,  1);
    addModesCustomTable("B", 5.4964,  1);
    addModesCustomTable("C", 6.6164,  1);
    addModesCustomTable("D", 10.4564, 1);

    ui->tableWidget_modeCustonBand->resizeColumnsToContents();
    ui->tableWidget_modeCustonBand->verticalHeader()->show();

    QHeaderView *hv = ui->tableWidget_modeCustonBand->horizontalHeader();
    hv->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->groupBox_modeSked->setChecked(Qt::Checked);
}

void MainWindow::gbps()
{
    int bits = ui->sampleBitsSpinBox->value();
    double mhz = ui->sampleRateDoubleSpinBox->value();
    int channels = 0;
    for(int i=0; i<ui->tableWidget_modeCustonBand->rowCount(); ++i){
        channels += qobject_cast<QSpinBox *>(ui->tableWidget_modeCustonBand->cellWidget(i,1))->value();
    }
    double mb = bits * mhz * channels;
    ui->label_gbps->setText(QString("%1 [Mbps]").arg(mb));
}


void MainWindow::addModesCustomTable(QString name, double freq, int nChannel){
    name = name.trimmed();

    if(name.isEmpty()){
        QMessageBox warning;
        warning.warning(this,"missing band name","Band name is missing!");
        return;
    }
    for(int i = 0; i<ui->tableWidget_modeCustonBand->rowCount(); ++i ){
        QString tmp = ui->tableWidget_modeCustonBand->verticalHeaderItem(i)->text();
        if(name == tmp ){
            QMessageBox warning;
            warning.warning(this,"already used band name","Band name " + tmp + " is already used!");
            return;
        }
    }


    ui->tableWidget_modeCustonBand->insertRow(ui->tableWidget_modeCustonBand->rowCount());
    ui->tableWidget_modeCustonBand->setVerticalHeaderItem(ui->tableWidget_modeCustonBand->rowCount()-1,new QTableWidgetItem(name));

    QDoubleSpinBox *freqSB = new QDoubleSpinBox(this);
    freqSB->setMinimum(0);
    freqSB->setMaximum(100);
    freqSB->setSingleStep(.1);
    freqSB->setDecimals(4);
    freqSB->setValue(freq);
    freqSB->setSuffix(" [GHz]");

    QSpinBox *nChannelSB = new QSpinBox(this);
    nChannelSB->setMinimum(1);
    nChannelSB->setMaximum(100);
    nChannelSB->setValue(nChannel);
    connect(nChannelSB,SIGNAL(valueChanged(int)),this,SLOT(gbps()));

    QPushButton *d = new QPushButton("delete",this);
    d->setIcon(QIcon(":/icons/icons/edit-delete-6.png"));
    connect(d,SIGNAL(clicked(bool)),deleteModeMapper,SLOT(map()));
    deleteModeMapper->setMapping(d,name);

    ui->tableWidget_modeCustonBand->setCellWidget(ui->tableWidget_modeCustonBand->rowCount()-1,0,freqSB);
    ui->tableWidget_modeCustonBand->setCellWidget(ui->tableWidget_modeCustonBand->rowCount()-1,1,nChannelSB);
    ui->tableWidget_modeCustonBand->setCellWidget(ui->tableWidget_modeCustonBand->rowCount()-1,2,d);
    addModesPolicyTable(name);

    gbps();
}

void MainWindow::deleteModesCustomLine(QString name)
{
    int row;
    for(int i = 0; i<ui->tableWidget_modeCustonBand->rowCount(); ++i ){
        QString tmp = ui->tableWidget_modeCustonBand->verticalHeaderItem(i)->text();
        if(name == tmp ){
            row = i;
            break;
        }
    }

    ui->tableWidget_modeCustonBand->removeRow(row);
    ui->tableWidget_ModesPolicy->removeRow(row);
    gbps();
}

void MainWindow::on_pushButton_modeCustomAddBAnd_clicked()
{
    addBandDialog *dial = new addBandDialog(settings_,this);
    int result = dial->exec();

    if(result == QDialog::Accepted){
        QString name = dial->getBandName();
        double freq = dial->getFrequency();
        int channels = dial->getChannels();
        addModesCustomTable(name,freq,channels);
    }

    delete(dial);
}


void MainWindow::on_pushButton_startAdvancedMode_clicked()
{

    VieVS::SkdCatalogReader skd;
    std::vector<std::string> station_names;
    QVector<QString> qstation_names;
    for(int i=0; i<selectedStationModel->rowCount(); ++i){
        station_names.push_back(selectedStationModel->item(i)->text().toStdString());
        qstation_names.append(selectedStationModel->item(i)->text());
    }

    if(station_names.empty()){
        QMessageBox::information(this,"select stations first","Please select your station network before your start creating an observing mode");
        return;
    }

    skd.setStationNames(station_names);
    skd.setCatalogFilePathes(ui->lineEdit_pathAntenna->text().toStdString(),
                             ui->lineEdit_pathEquip->text().toStdString(),
                             "",
                             ui->lineEdit_pathFreq->text().toStdString(),
                             "",
                             ui->lineEdit_pathLoif->text().toStdString(),
                             ui->lineEdit_pathMask->text().toStdString(),
                             ui->lineEdit_pathModes->text().toStdString(),
                             ui->lineEdit_pathPosition->text().toStdString(),
                             ui->lineEdit_pathRec->text().toStdString(),
                             ui->lineEdit_pathRx->text().toStdString(),
                             "",
                             ui->lineEdit_pathTracks->text().toStdString());

    skd.initializeStationCatalogs();

    std::string obsName = ui->comboBox_skedObsModes_advanced->currentText().toStdString();
    skd.initializeModesCatalogs(obsName);

    VieVS::ObservingMode obsModeStart;
    obsModeStart.readFromSkedCatalogs(skd);

    obsModeStart.setStationNames(station_names);

    ObsModeDialog *obsModeDial = new ObsModeDialog(obsModeStart, this);
    int result = obsModeDial->exec();
    if(result == QDialog::Accepted){
        advancedObservingMode_ = obsModeDial->getObservingMode();
        updateAdvancedObservingMode();

    }

    delete(obsModeDial);
}

void MainWindow::updateAdvancedObservingMode()
{
//    ui->tableWidget_ModesPolicy->clear();
    ui->tableWidget_ModesPolicy->setRowCount(0);

    if(!advancedObservingMode_.is_initialized()){

        ui->comboBox_observingMode_mode->clear();
        ui->comboBox_observingMode_freq->clear();
        ui->comboBox_observingMode_bbc->clear();
        ui->comboBox_observingMode_if->clear();
        ui->comboBox_observingMode_tracks->clear();
        ui->comboBox_observingMode_trackFrameFormat->clear();

        qobject_cast<Model_Mode *>(ui->tableView_observingMode_mode->model())->setMode(nullptr);
        qobject_cast<Model_Freq *>(ui->tableView_observingMode_freq->model())->setFreq(nullptr);
        qobject_cast<Model_Bbc *>(ui->tableView_observingMode_bbc->model())->setBbc(nullptr);
        qobject_cast<Model_If *>(ui->tableView_observingMode_if->model())->setIf(nullptr);
        qobject_cast<Model_Tracks *>(ui->tableView_observingMode_tracks->model())->setTracks(nullptr);
        return;
    }

    QVector<QString> qstation_names;
    for(int i=0; i<selectedStationModel->rowCount(); ++i){
        qstation_names.append(selectedStationModel->item(i)->text());
    }

    qobject_cast<Model_Mode *>(ui->tableView_observingMode_mode->model())->setStations(qstation_names);
    ui->comboBox_observingMode_mode->clear();
    for(const auto & any : advancedObservingMode_->getModes()){
        ui->comboBox_observingMode_mode->addItem(QString::fromStdString(any->getName()));
    }
    qobject_cast<Model_Mode *>(ui->tableView_observingMode_mode->model())->setMode(std::make_shared< VieVS::Mode >(*advancedObservingMode_->getModePerIndex(0)));

    ui->comboBox_observingMode_freq->clear();
    for(const auto & any : advancedObservingMode_->getFreqs()){
        ui->comboBox_observingMode_freq->addItem(QString::fromStdString(any->getName()));
    }
    qobject_cast<Model_Freq *>(ui->tableView_observingMode_freq->model())->setFreq(std::make_shared< VieVS::Freq >(*advancedObservingMode_->getFreqPerIndex(0)));

    ui->comboBox_observingMode_bbc->clear();
    for(const auto & any : advancedObservingMode_->getBbcs()){
        ui->comboBox_observingMode_bbc->addItem(QString::fromStdString(any->getName()));
    }
    qobject_cast<Model_Bbc *>(ui->tableView_observingMode_bbc->model())->setBbc(std::make_shared< VieVS::Bbc >(*advancedObservingMode_->getBbcPerIndex(0)));

    ui->comboBox_observingMode_if->clear();
    for(const auto & any : advancedObservingMode_->getIfs()){
        ui->comboBox_observingMode_if->addItem(QString::fromStdString(any->getName()));
    }
    qobject_cast<Model_If *>(ui->tableView_observingMode_if->model())->setIf(std::make_shared< VieVS::If >(*advancedObservingMode_->getIfPerIndex(0)));

    ui->comboBox_observingMode_tracks->clear();
    for(const auto & any : advancedObservingMode_->getTracks()){
        ui->comboBox_observingMode_tracks->addItem(QString::fromStdString(any->getName()));
    }
    qobject_cast<Model_Tracks *>(ui->tableView_observingMode_tracks->model())->setTracks(std::make_shared< VieVS::Track >(*advancedObservingMode_->getTracksPerIndex(0)));

    ui->comboBox_observingMode_trackFrameFormat->clear();
    for(const auto & any : advancedObservingMode_->getTrackFrameFormats()){
        ui->comboBox_observingMode_trackFrameFormat->addItem(QString::fromStdString(*any));
    }

    for(const auto &band : advancedObservingMode_->bands){
        addModesPolicyTable(QString::fromStdString(band));
    }
}


void MainWindow::changeObservingModeSelection(int idx){
    auto s = sender();
    if(idx == -1){
        return;
    }

    if(s == ui->comboBox_observingMode_mode){
        qobject_cast<Model_Mode *>(ui->tableView_observingMode_mode->model())->setMode(std::make_shared< VieVS::Mode >(*advancedObservingMode_->getModePerIndex(idx)));
    }else if(s == ui->comboBox_observingMode_freq){
        qobject_cast<Model_Freq *>(ui->tableView_observingMode_freq->model())->setFreq(std::make_shared< VieVS::Freq >(*advancedObservingMode_->getFreqPerIndex(idx)));
    }else if(s == ui->comboBox_observingMode_bbc){
        qobject_cast<Model_Bbc *>(ui->tableView_observingMode_bbc->model())->setBbc(std::make_shared< VieVS::Bbc >(*advancedObservingMode_->getBbcPerIndex(idx)));
    }else if(s == ui->comboBox_observingMode_if){
        qobject_cast<Model_If *>(ui->tableView_observingMode_if->model())->setIf(std::make_shared< VieVS::If >(*advancedObservingMode_->getIfPerIndex(idx)));
    }else if(s == ui->comboBox_observingMode_tracks){
        qobject_cast<Model_Tracks *>(ui->tableView_observingMode_tracks->model())->setTracks(std::make_shared< VieVS::Track >(*advancedObservingMode_->getTracksPerIndex(idx)));
    }

}

void MainWindow::on_pushButton_loadAdvancedMode_clicked()
{
    auto tmodes= settings_.get_child_optional("settings.mode");
    if(!tmodes.is_initialized()){
        QMessageBox::warning(this,"No observing mode found!","There is no observing mode saved previously.");
        return;
    }
    QVector<QString> names;
    QVector<QVector<QString>> freqs;
    QVector<QVector<QString>> bbcs;
    QVector<QVector<QString>> ifs;
    QVector<QVector<QString>> tracks;
    QVector<QVector<QString>> trackFrameFormats;
    QVector<QVector<QString>> modes;

    for(const auto &mode : *tmodes){

        if(mode.first == "custom"){
            QVector<QString> tfreqs;
            QVector<QString> tbbcs;
            QVector<QString> tifs;
            QVector<QString> ttracks;
            QVector<QString> ttrackFrameFormats;
            QVector<QString> tmodes;

            names.append(QString::fromStdString(mode.second.get("<xmlattr>.name","unknown")));
            for(const auto &any : mode.second){
                if(any.first == "FREQ"){
                    QString name = QString::fromStdString(any.second.get<std::string>("<xmlattr>.name"));
                    tfreqs.append(name);
                }
                if(any.first == "BBC"){
                    QString name = QString::fromStdString(any.second.get<std::string>("<xmlattr>.name"));
                    tbbcs.append(name);
                }
                if(any.first == "IF"){
                    QString name = QString::fromStdString(any.second.get<std::string>("<xmlattr>.name"));
                    tifs.append(name);
                }
                if(any.first == "TRACKS"){
                    QString name = QString::fromStdString(any.second.get<std::string>("<xmlattr>.name"));
                    ttracks.append(name);
                }
                if(any.first == "track_frame_format"){
                    QString name = QString::fromStdString(any.second.get<std::string>("<xmlattr>.name"));
                    ttrackFrameFormats.append(name);
                }
                if(any.first == "MODE"){
                    QString name = QString::fromStdString(any.second.get<std::string>("<xmlattr>.name"));
                    tmodes.append(name);
                }
            }

            freqs.append(tfreqs);
            bbcs.append(tbbcs);
            ifs.append(tifs);
            tracks.append(ttracks);
            trackFrameFormats.append(ttrackFrameFormats);
            modes.append(tmodes);
        }
    }

    if(names.isEmpty()){
        QMessageBox::warning(this,"No observing mode found!","There is no observing mode saved previously.");
        return;
    }

    settingsLoadWindow *dial = new settingsLoadWindow(this);

    dial->setModes(names,freqs,bbcs,ifs,tracks,trackFrameFormats,modes);

    int result = dial->exec();
    if(result == QDialog::Accepted){

        QString itm = dial->selectedItem();
        int idx = dial->selectedIdx();

        int counter = 0;
        for(const auto &mode : *tmodes){
            if(mode.first == "custom"){
                if(counter == idx){
                    std::vector<std::string> station_names;
                    for(int i=0; i<selectedStationModel->rowCount(); ++i){
                        station_names.push_back(selectedStationModel->item(i)->text().toStdString());
                    }

                    advancedObservingMode_ = VieVS::ObservingMode(mode.second, station_names);
                    updateAdvancedObservingMode();

                    break;
                }else{
                    counter++;
                }
            }
        }
    }
}

void MainWindow::on_pushButton_saveAdvancedMode_clicked()
{
    if(!advancedObservingMode_.is_initialized()){
        QMessageBox::warning(this,"No observing mode!","Please create or load observing mode first!");
        return;
    }

    bool ok;
    QString text = QInputDialog::getText(this, "observing mode name:", "name: ", QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty()){
        boost::property_tree::ptree t = advancedObservingMode_->toPropertytree();
        t.add("<xmlattr>.name",text.toStdString());

        settings_.add_child("settings.mode.custom",t);

        std::ofstream os;
        os.open("settings.xml");
        boost::property_tree::xml_parser::write_xml(os, settings_,
                                                    boost::property_tree::xml_writer_make_settings<std::string>('\t', 1));
        os.close();

    }

}

void MainWindow::on_pushButton_changeCurrentAdvancedMode_clicked()
{
    if(!advancedObservingMode_.is_initialized()){
        QMessageBox::warning(this,"No observing mode!","Please create or load observing mode first!");
        return;
    }

    QVector<QString> qstation_names;
    for(int i=0; i<selectedStationModel->rowCount(); ++i){
        qstation_names.append(selectedStationModel->item(i)->text());
    }

    ObsModeDialog *obsModeDial = new ObsModeDialog(*advancedObservingMode_, this);
    int result = obsModeDial->exec();
    if(result == QDialog::Accepted){
        advancedObservingMode_ = obsModeDial->getObservingMode();

        qobject_cast<Model_Mode *>(ui->tableView_observingMode_mode->model())->setStations(qstation_names);
        ui->comboBox_observingMode_mode->clear();
        for(const auto & any : advancedObservingMode_->getModes()){
            ui->comboBox_observingMode_mode->addItem(QString::fromStdString(any->getName()));
        }
        qobject_cast<Model_Mode *>(ui->tableView_observingMode_mode->model())->setMode(std::make_shared< VieVS::Mode >(*advancedObservingMode_->getModePerIndex(0)));

        ui->comboBox_observingMode_freq->clear();
        for(const auto & any : advancedObservingMode_->getFreqs()){
            ui->comboBox_observingMode_freq->addItem(QString::fromStdString(any->getName()));
        }
        qobject_cast<Model_Freq *>(ui->tableView_observingMode_freq->model())->setFreq(std::make_shared< VieVS::Freq >(*advancedObservingMode_->getFreqPerIndex(0)));

        ui->comboBox_observingMode_bbc->clear();
        for(const auto & any : advancedObservingMode_->getBbcs()){
            ui->comboBox_observingMode_bbc->addItem(QString::fromStdString(any->getName()));
        }
        qobject_cast<Model_Bbc *>(ui->tableView_observingMode_bbc->model())->setBbc(std::make_shared< VieVS::Bbc >(*advancedObservingMode_->getBbcPerIndex(0)));

        ui->comboBox_observingMode_if->clear();
        for(const auto & any : advancedObservingMode_->getIfs()){
            ui->comboBox_observingMode_if->addItem(QString::fromStdString(any->getName()));
        }
        qobject_cast<Model_If *>(ui->tableView_observingMode_if->model())->setIf(std::make_shared< VieVS::If >(*advancedObservingMode_->getIfPerIndex(0)));

        ui->comboBox_observingMode_tracks->clear();
        for(const auto & any : advancedObservingMode_->getTracks()){
            ui->comboBox_observingMode_tracks->addItem(QString::fromStdString(any->getName()));
        }
        qobject_cast<Model_Tracks *>(ui->tableView_observingMode_tracks->model())->setTracks(std::make_shared< VieVS::Track >(*advancedObservingMode_->getTracksPerIndex(0)));

        ui->comboBox_observingMode_trackFrameFormat->clear();
        for(const auto & any : advancedObservingMode_->getTrackFrameFormats()){
            ui->comboBox_observingMode_trackFrameFormat->addItem(QString::fromStdString(*any));
        }

    }

    delete(obsModeDial);
}


void MainWindow::readAllSkedObsModes()
{
    QString modesPath = ui->lineEdit_pathModes->text();
    QFile modesFile(modesPath);
    QStringList modes;
    if (modesFile.open(QIODevice::ReadOnly)){
        QTextStream in(&modesFile);
        while (!in.atEnd()){
            QString line = in.readLine();
            if(line.isEmpty() || line[0] == "*" || line[0] == "!" || line[0] == "&"){
                continue;
            }
            QStringList split = line.split(" ",QString::SplitBehavior::SkipEmptyParts);
            QString obsModeName = split[0];

            modes << obsModeName;
        }
        modesFile.close();
    }
    allSkedModesModel->setStringList(modes);
    if(ui->comboBox_skedObsModes->count()>0){
        ui->comboBox_skedObsModes->setCurrentIndex(0);
    }
}

void MainWindow::on_groupBox_modeSked_toggled(bool arg1)
{
    if(arg1){
        ui->groupBox_modeCustom->setChecked(!arg1);
        ui->groupBox_modeAdvanced->setChecked(!arg1);

//        ui->tableWidget_ModesPolicy->clear();
        ui->tableWidget_ModesPolicy->setRowCount(0);
        addModesPolicyTable("X");
        addModesPolicyTable("S");
    }
}

void MainWindow::on_groupBox_modeCustom_toggled(bool arg1)
{
    if(arg1){
        ui->groupBox_modeSked->setChecked(!arg1);
        ui->groupBox_modeAdvanced->setChecked(!arg1);

        ui->tableWidget_ModesPolicy->setRowCount(0);
        for(int i=0; i<ui->tableWidget_modeCustonBand->rowCount(); ++i){
            QString name = ui->tableWidget_modeCustonBand->verticalHeaderItem(i)->text();
            addModesPolicyTable(name);
        }
    }
}

void MainWindow::on_groupBox_modeAdvanced_toggled(bool arg1)
{
    if(arg1){
        ui->groupBox_modeCustom->setChecked(!arg1);
        ui->groupBox_modeSked->setChecked(!arg1);

        updateAdvancedObservingMode();
    }
}


void MainWindow::on_pushButton_saveMode_clicked()
{
    int bits = ui->sampleBitsSpinBox->value();
    double srate = ui->sampleRateDoubleSpinBox->value();
    QVector<QString> bands;
    QVector<double> freqs;
    QVector<int> chans;

    for(int i = 0; i<ui->tableWidget_modeCustonBand->rowCount(); ++i){
        QString band = ui->tableWidget_modeCustonBand->verticalHeaderItem(i)->text();
        bands.push_back(band);
        double freq = qobject_cast<QDoubleSpinBox*>(ui->tableWidget_modeCustonBand->cellWidget(i,0))->value();
        freqs.push_back(freq);
        int chan = qobject_cast<QSpinBox*>(ui->tableWidget_modeCustonBand->cellWidget(i,1))->value();
        chans.push_back(chan);
    }
    saveToSettingsDialog *dial = new saveToSettingsDialog(settings_,this);
    dial->setType(saveToSettingsDialog::Type::modes);
    dial->setMode(bits,srate,bands,freqs,chans);

    dial->exec();

}

void MainWindow::on_pushButton_loadMode_clicked()
{
    auto modes= settings_.get_child_optional("settings.modes");
    if(!modes.is_initialized()){
        QMessageBox::warning(this,"No modes list found!","There were no modes saved in settings.xml file\nCheck settings.modes");
        return;
    }

    QVector<QString> names;
    QVector<int> bits;
    QVector<double> srates;
    QVector<QVector<QString> > bands;
    QVector<QVector<int> > channels;
    QVector<QVector<double> > freqs;

    for(const auto &it:*modes){
        QString name = QString::fromStdString(it.second.get_child("<xmlattr>.name").data());
        int bit;
        double srate;
        QVector<QString> band;
        QVector<int> channel;
        QVector<double> freq;

        for(const auto &it2:it.second){
            if(it2.first == "bits"){
                bit = it2.second.get_value<int>();
            }else if(it2.first == "sampleRate"){
                srate = it2.second.get_value<double>();
            }else if(it2.first == "band"){

                channel.push_back(it2.second.get<int>("channels"));
                freq.push_back(it2.second.get<double>("frequency"));
                band.push_back(QString::fromStdString(it2.second.get<std::string>("<xmlattr>.name")));

            }
        }
        names.push_back(name);
        bits.push_back(bit);
        srates.push_back(srate);
        bands.push_back(band);
        channels.push_back(channel);
        freqs.push_back(freq);
    }

    settingsLoadWindow *dial = new settingsLoadWindow(this);
    dial->setModes(names,bits,srates,bands,channels,freqs);

    int result = dial->exec();
    if(result == QDialog::Accepted){
        QString itm = dial->selectedItem();
        int idx = dial->selectedIdx();

        ui->sampleBitsSpinBox->setValue(bits.at(idx));
        ui->sampleRateDoubleSpinBox->setValue(srates.at(idx));
        ui->tableWidget_modeCustonBand->setRowCount(0);
        ui->tableWidget_ModesPolicy->setRowCount(0);
        for(int i=0; i<bands.at(idx).size(); ++i){
            QString bName = bands.at(idx).at(i);
            double bFreq = freqs.at(idx).at(i);
            int bChannels = channels.at(idx).at(i);
            addModesCustomTable(bName, bFreq, bChannels);
        }
    }

}

// ########################################### CATALOGS ###########################################

void MainWindow::readSkedCatalogs()
{
    skdCatalogReader.setCatalogFilePathes(ui->lineEdit_pathAntenna->text().toStdString(), ui->lineEdit_pathEquip->text().toStdString(),
                                          ui->lineEdit_pathFlux->text().toStdString(), ui->lineEdit_pathFreq->text().toStdString(),
                                          ui->lineEdit_pathHdpos->text().toStdString(), ui->lineEdit_pathLoif->text().toStdString(),
                                          ui->lineEdit_pathMask->text().toStdString(), ui->lineEdit_pathModes->text().toStdString(),
                                          ui->lineEdit_pathPosition->text().toStdString(), ui->lineEdit_pathRec->text().toStdString(),
                                          ui->lineEdit_pathRx->text().toStdString(), ui->lineEdit_pathSource->text().toStdString(),
                                          ui->lineEdit_pathTracks->text().toStdString());

    QFileInfo bant(ui->lineEdit_pathAntenna->text());
    QFileInfo bequ(ui->lineEdit_pathEquip->text());
    QFileInfo bflu(ui->lineEdit_pathFlux->text());
    QFileInfo bfre(ui->lineEdit_pathFreq->text());
    QFileInfo bhdp(ui->lineEdit_pathHdpos->text());
    QFileInfo bloi(ui->lineEdit_pathLoif->text());
    QFileInfo bmas(ui->lineEdit_pathMask->text());
    QFileInfo bmod(ui->lineEdit_pathModes->text());
    QFileInfo bpos(ui->lineEdit_pathPosition->text());
    QFileInfo brec(ui->lineEdit_pathRec->text());
    QFileInfo brx(ui->lineEdit_pathRx->text());
    QFileInfo bsrc(ui->lineEdit_pathSource->text());
    QFileInfo btra(ui->lineEdit_pathTracks->text());

    bool sta = false;
    if(bant.exists() && bequ.exists() && bmas.exists() && bpos.exists() ){
        skdCatalogReader.initializeStationCatalogs();
        sta = true;
    }
    bool src = false;
    if( bflu.exists() && bsrc.exists()){
        skdCatalogReader.initializeSourceCatalogs();
        src = true;
    }
    bool mode = false;
    if( bfre.exists() && bhdp.exists() && bloi.exists() && bmod.exists() && brec.exists() && brx.exists() && btra.exists()){
        skdCatalogReader.initializeModesCatalogs(ui->comboBox_skedObsModes->currentText().toStdString());
        mode = true;
    }


    if(!sta || !src || !mode) {
        QString txt = "<b>First start?</b><br>Wait for the <b>automatic downloads</b> to finish and restart VieSched++.<br>"
                      "One or multiple catalog files not found:<br><ul>";

        auto f=[&txt](QLineEdit *le, QString name){
            if(le->text().isEmpty()){
                txt.append("<li>").append(name).append(" not found!</li>");
            }else{
                txt.append("<li>").append(name).append(": ").append(le->text()).append(" not found!</li>");
            }
        };

        if(!bant.exists() ){
            f(ui->lineEdit_pathAntenna,"antenna.cat");
        }
        if(!bequ.exists()){
            f(ui->lineEdit_pathEquip,"equip.cat");
        }
        if(!bflu.exists()){
            f(ui->lineEdit_pathFlux,"flux.cat");
        }
        if(!bfre.exists()){
            f(ui->lineEdit_pathFreq,"freq.cat");
        }
        if(!bhdp.exists()){
            f(ui->lineEdit_pathHdpos,"hdpos.cat");
        }
        if(!bloi.exists()){
            f(ui->lineEdit_pathLoif,"loif.cat");
        }
        if(!bmas.exists()){
            f(ui->lineEdit_pathMask,"mask.cat");
        }
        if(!bmod.exists()){
            f(ui->lineEdit_pathModes,"modes.cat");
        }
        if(!bpos.exists()){
            f(ui->lineEdit_pathPosition,"position.cat");
        }
        if(!brec.exists()){
            f(ui->lineEdit_pathRec,"rec.cat");
        }
        if(!brx.exists()){
            f(ui->lineEdit_pathRx,"rx.cat");
        }
        if(!bsrc.exists()){
            f(ui->lineEdit_pathSource,"source.cat");
        }
        if(!btra.exists()){
            f(ui->lineEdit_pathTracks,"tracks.cat");
        }

        txt.append("</ul>Either:<ul><li>copy the catalogs to the required position</li><li>wait for the automatic download to finish and restart VieSched++ or reload the catalogs</li><li>change the path in the catalogs menu</li></ul>");

        QDialog *dial = new QDialog(this);
        dial->setWindowTitle("Catalog files not found!");
        QLabel *label = new QLabel(txt);
        QVBoxLayout *l = new QVBoxLayout();
        l->addWidget(label);
        dial->setLayout(l);
        dial->show();
        //QMessageBox::information(this, "CATALOG files", txt);
    }




}

void MainWindow::on_pushButton_browseAntenna_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Browse to catalog", ui->lineEdit_pathAntenna->text());
    if( !path.isEmpty() ){
        ui->lineEdit_pathAntenna->setText(path);
    }
}

void MainWindow::on_pushButton_browseEquip_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Browse to catalog", ui->lineEdit_pathEquip->text());
    if( !path.isEmpty() ){
        ui->lineEdit_pathEquip->setText(path);
    }
}

void MainWindow::on_pushButton_browsePosition_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Browse to catalog", ui->lineEdit_pathPosition->text());
    if( !path.isEmpty() ){
        ui->lineEdit_pathPosition->setText(path);
    }
}

void MainWindow::on_pushButton_browseMask_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Browse to catalog", ui->lineEdit_pathMask->text());
    if( !path.isEmpty() ){
        ui->lineEdit_pathMask->setText(path);
    }
}

void MainWindow::on_pushButton_browseSource_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Browse to catalog", ui->lineEdit_pathSource->text());
    if( !path.isEmpty() ){
        ui->lineEdit_pathSource->setText(path);
    }
}

void MainWindow::on_pushButton_browseSatellite_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Browse to catalog", ui->lineEdit_pathSource->text());
    if( !path.isEmpty() ){
        ui->lineEdit_pathSatellite->setText(path);
    }
}

void MainWindow::on_pushButton_browseSpacecraft_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Browse to catalog", ui->lineEdit_pathSource->text());
    if( !path.isEmpty() ){
        ui->lineEdit_pathSpacecraft->setText(path);
    }
}

void MainWindow::on_pushButton_browseFlux_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Browse to catalog", ui->lineEdit_pathFlux->text());
    if( !path.isEmpty() ){
        ui->lineEdit_pathFlux->setText(path);
    }
}

void MainWindow::on_pushButton_browsModes_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Browse to catalog", ui->lineEdit_pathModes->text());
    if( !path.isEmpty() ){
        ui->lineEdit_pathModes->setText(path);
    }
}

void MainWindow::on_pushButton_browseFreq_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Browse to catalog", ui->lineEdit_pathFreq->text());
    if( !path.isEmpty() ){
        ui->lineEdit_pathFreq->setText(path);
    }
}

void MainWindow::on_pushButton_browseTracks_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Browse to catalog", ui->lineEdit_pathTracks->text());
    if( !path.isEmpty() ){
        ui->lineEdit_pathTracks->setText(path);
    }
}

void MainWindow::on_pushButton_browseLoif_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Browse to catalog", ui->lineEdit_pathLoif->text());
    if( !path.isEmpty() ){
        ui->lineEdit_pathLoif->setText(path);
    }
}

void MainWindow::on_pushButton_browseRec_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Browse to catalog", ui->lineEdit_pathRec->text());
    if( !path.isEmpty() ){
        ui->lineEdit_pathRec->setText(path);
    }
}

void MainWindow::on_pushButton_browseRx_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Browse to catalog", ui->lineEdit_pathRx->text());
    if( !path.isEmpty() ){
        ui->lineEdit_pathRx->setText(path);
    }
}

void MainWindow::on_pushButton_browseHdpos_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Browse to catalog", ui->lineEdit_pathHdpos->text());
    if( !path.isEmpty() ){
        ui->lineEdit_pathHdpos->setText(path);
    }
}

void MainWindow::on_pushButton_stations_clicked()
{
    allStationModel->removeRows(0,allStationModel->rowCount());

    selectedStationModel->removeRows(0,selectedStationModel->rowCount());
    selectedBaselineModel->removeRows(0,selectedBaselineModel->rowCount());

    allStationPlusGroupModel->removeRows(0,allStationPlusGroupModel->rowCount());
    allStationPlusGroupModel->insertRow(0,new QStandardItem(QIcon(":/icons/icons/station_group.png"),"__all__"));

    allBaselinePlusGroupModel->removeRows(0,allStationPlusGroupModel->rowCount());
    allBaselinePlusGroupModel->insertRow(0,new QStandardItem(QIcon(":/icons/icons/baseline_group.png"),"__all__"));


    availableStations->clear();
    selectedStations->clear();
    auto series = worldmap->chart()->series();
    for(auto &s : series){
        if(s->name().right(4) == "[km]"){
            worldmap->chart()->removeSeries(s);
            delete(s);
        }
    }

    readStations();

    QString warnings;
//    if(ui->treeWidget_setupStation->topLevelItem(0)->child(0)->childCount() != 0){
//        warnings.append("station setup cleared\n");
//        clearSetup(true,false,false);
//    }
//    if(ui->treeWidget_setupBaseline->topLevelItem(0)->child(0)->childCount() != 0){
//        warnings.append("baseline setup cleared\n");
//        clearSetup(false,false,true);
//    }
    bool flag = clearSetup(true,false,true);
    if (flag){
        warnings.append("parameter setup cleared");
    }

    if(!groupSta->empty()){
        groupSta->clear();
        warnings.append("station groups cleared\n");
    }
    if(!groupBl->empty()){
        groupBl->clear();
        warnings.append("baseline groups cleared\n");
    }
    if(ui->treeWidget_highImpactAzEl->topLevelItemCount() != 0){
        ui->treeWidget_highImpactAzEl->clear();
        warnings.append("high impact scans setup cleared\n");
    }

    bool mssta = false;
    bool msbl = false;
    auto *tmp_ms = ui->groupBox_multiScheduling->findChild<QWidget *>("MultiScheduling_Widged");
    MulitSchedulingWidget *ms = qobject_cast<MulitSchedulingWidget *>(tmp_ms);
    ms->resetStationParameters(mssta, msbl);

    if(mssta){
        warnings.append("station multi scheduling parameters cleared\n");
    }
    if(msbl){
        warnings.append("baseline multi scheduling parameters cleared\n");
    }

    if(!warnings.isEmpty()){
        QMessageBox::warning(this,"reload stations",warnings);
    }else{
        QMessageBox::information(this, "reload stations", "stations successfully reloaded");
    }

}

void MainWindow::on_pushButton_reloadsources_clicked()
{
    allSourceModel->removeRows(0,allSourceModel->rowCount());

    selectedSourceModel->removeRows(0,selectedSourceModel->rowCount());

    allSourcePlusGroupModel->removeRows(0,allSourcePlusGroupModel->rowCount());
    allSourcePlusGroupModel->insertRow(0,new QStandardItem(QIcon(":/icons/icons/source_group.png"),"__all__"));

    allSourcePlusGroupModel_combined->removeRows(0,allSourcePlusGroupModel_combined->rowCount());
    allSourcePlusGroupModel_combined->insertRow(0,new QStandardItem(QIcon(":/icons/icons/all_source_types.png"),"__all__"));
    allSourcePlusGroupModel_combined->insertRow(1,new QStandardItem(QIcon(":/icons/icons/source_group.png"),"__AGNs__"));
    allSourcePlusGroupModel_combined->insertRow(2,new QStandardItem(QIcon(":/icons/icons/satellite_group.png"),"__satellites__"));
    allSourcePlusGroupModel_combined->insertRow(3,new QStandardItem(QIcon(":/icons/icons/spacecraft_group.png"),"__spacecrafts__"));

    availableSources->clear();
    selectedSources->clear();

    readSources();
    readSatellites();
    readSpacecraft();

    QString warnings;
//    if(ui->treeWidget_setupSource->topLevelItem(0)->child(0)->childCount() != 0){
//        warnings.append("source setup cleared\n");
//        clearSetup(false,true,false);
//    }
    bool flag = clearSetup(false,true,false);
    if (flag){
        warnings.append("parameter setup cleared");
    }

    if(!groupSrc->empty()){
        groupSrc->clear();
        warnings.append("source groups cleared\n");
    }

    bool mssrc = false;
    auto *tmp_ms = ui->groupBox_multiScheduling->findChild<QWidget *>("MultiScheduling_Widged");
    MulitSchedulingWidget *ms = qobject_cast<MulitSchedulingWidget *>(tmp_ms);
    ms->resetSourceParameters(mssrc);
    if(mssrc){
        warnings.append("source multi scheduling parameters cleared\n");
    }

    ui->treeWidget_srcGroupForStatistics->clear();
    QTreeWidgetItem *itm = new QTreeWidgetItem();
    itm->setText(0,"__all__");
    itm->setCheckState(0,Qt::Unchecked);
    ui->treeWidget_srcGroupForStatistics->addTopLevelItem(itm);

    if(ui->spinBox_scanSequenceCadence->value() != 1){
        ui->spinBox_scanSequenceCadence->setValue(0);
        warnings.append("scan sequence cleared\n");
    }

    if(ui->comboBox_calibratorBlock_calibratorSources->currentText() != "__all__"){
        ui->comboBox_calibratorBlock_calibratorSources->setCurrentIndex(0);
        warnings.append("calibrator sources selection cleared\n");
    }

    if(ui->treeWidget_conditions->topLevelItemCount() != 0){
        ui->treeWidget_conditions->clear();
        warnings.append("optimisation conditions cleared\n");
    }

    if(!warnings.isEmpty()){
        QMessageBox::warning(this,"reload sources",warnings);
    }else{
        QMessageBox::information(this, "reload sources", "sources successfully reloaded");
    }

}

void MainWindow::on_pushButton_reloadcatalogs_clicked()
{
    readAllSkedObsModes();
    QMessageBox::information(this, "reload modes", "modes successfully reloaded\ncheck current selection");
}


void MainWindow::on_pushButton_howAreSkedCatalogsLinked_clicked()
{
    SkedCatalogInfo dial(this);
    dial.setFonts();
    dial.exec();
}

// ########################################### GENERAL ###########################################

void MainWindow::on_dateTimeEdit_sessionStart_dateTimeChanged(const QDateTime &dateTime)
{
    QDateTime dateTimeEnd = dateTime.addSecs(ui->doubleSpinBox_sessionDuration->value()*3600);

    stationSetupWidget->setDateTimeLimits(dateTime,dateTimeEnd);
    sourceSetupWidget->setDateTimeLimits(dateTime,dateTimeEnd);
    baselineSetupWidget->setDateTimeLimits(dateTime,dateTimeEnd);
    satelliteSetupWidget->setDateTimeLimits(dateTime,dateTimeEnd);
    spacecraftSetupWidget->setDateTimeLimits(dateTime,dateTimeEnd);

    bool flag = clearSetup(true,true,true);
    if(flag){
        QMessageBox::warning(this,"Setup deleted!","Setup was deleted due to session time change!");
    }

}

void MainWindow::on_doubleSpinBox_sessionDuration_valueChanged(double arg1)
{
    QDateTime dateTime = ui->dateTimeEdit_sessionStart->dateTime();
    QDateTime dateTimeEnd = ui->dateTimeEdit_sessionStart->dateTime().addSecs(arg1*3600);

    stationSetupWidget->setDateTimeLimits(dateTime,dateTimeEnd);
    sourceSetupWidget->setDateTimeLimits(dateTime,dateTimeEnd);
    baselineSetupWidget->setDateTimeLimits(dateTime,dateTimeEnd);
    satelliteSetupWidget->setDateTimeLimits(dateTime,dateTimeEnd);
    spacecraftSetupWidget->setDateTimeLimits(dateTime,dateTimeEnd);

    bool flag = clearSetup(true,true,true);
    if(flag){
        QMessageBox::warning(this,"Setup deleted!","Setup was deleted due to session time change!");
    }
}

void MainWindow::on_dateTimeEdit_sessionStart_dateChanged(const QDate &date)
{
    int doy = date.dayOfYear();
    ui->spinBox_doy->setValue(doy);
}

void MainWindow::on_spinBox_doy_valueChanged(int arg1)
{
    QDate x = ui->dateTimeEdit_sessionStart->date();
    int y = x.year();
    x.setDate(y,1,1);
    x = x.addDays(arg1-1);
    ui->dateTimeEdit_sessionStart->setDate(x);
}


void MainWindow::on_pushButton_clicked()
{
    QString errorText = "";
    QString txt = ui->lineEdit_ivsMaster->text().simplified();


    QString description;
    QString sessionName;
    QDateTime start;
    double dur = -1;
    QStringList stas;
    QString sked;
    QString corr;

    bool found = false;

    if(txt.length() <= 6){
        auto c = qtUtil::searchSessionCodeInMasterFile(txt);
        if(c.is_initialized()){
            std::tie(description, sessionName, start, dur, stas, sked, corr) = *c;

            found = true;
        }else{
            QMessageBox::warning(this,"session code not found","The session was not found in the master files located in AUTO_DOWNLOAD_MASTER!");
        }


    } else {

        QRegularExpression reg("([\\w\\&-]+)\\s(\\w+)\\s(\\d{4}-\\d{2}-\\d{2}\\s\\d{1,2}:\\d{2})\\s\\w+\\s(\\d{1,2}:\\d{2})\\s(.*?)(XA|XE|XH|XN|XU|XK|VG)\\s(\\w*)\\s(\\w*)");
        QRegularExpressionMatch match = reg.match(txt);
        if(match.hasMatch()){
            description = match.captured(1);
            sessionName = match.captured(2);
            QString date = match.captured(3);
            start = QDateTime::fromString(date,"yyyy-MM-dd hh:mm");
            QString dur = match.captured(4);
            int h = dur.split(":").at(0).toDouble();
            int min = dur.split(":").at(1).toDouble();
            dur = h+min/60.;
            QString stations = match.captured(5);
            stas = stations.split(" ",QString::SkipEmptyParts);
            sked = match.captured(7);
            corr = match.captured(8);
            found = true;
        } else {
            QMessageBox::warning(this,"errors while reading from session master","Not possible to parse input!");
        }
    }


    QMap<QString,QString> tlc2station;
    if (found){

        ui->lineEdit_experimentDescription->setText(description);
        ui->experimentNameLineEdit->setText(sessionName);
        ui->dateTimeEdit_sessionStart->setDateTime(start);
        if(dur < 0){
            errorText.append("session duration (and maybe start time) is unknown\n");
        }else{
            ui->doubleSpinBox_sessionDuration->setValue(dur);
        }


        createBaselines = false;
        selectedStationModel->blockSignals(true);

        if(stas.size() >= 2){
            int n = selectedStationModel->rowCount();
            for(int i=0; i<n; ++i){
                QModelIndex index = selectedStationModel->index(0,0);
                on_treeView_allSelectedStations_clicked(index);
            }

            allStationProxyModel->addFilterFixedString("");
            for(int i=0; i<stas.size(); ++i){
                QString sta = stas.at(i).toUpper();
                if(i==stas.size()-1){
                    selectedStationModel->blockSignals(false);
                }
                bool found = false;
                for(int j=0; j<allStationProxyModel->rowCount(); ++j){
                    QString itsta = allStationProxyModel->index(j,1).data().toString().toUpper();
                    if(itsta == sta){
                        tlc2station[sta] = allStationProxyModel->index(j,0).data().toString();
                        QModelIndex index = allStationProxyModel->index(j,0);
                        on_treeView_allAvailabeStations_clicked(index);
                        found = true;
                        break;
                    }
                }
                if(!found){
                    errorText.append(QString("unknown station %1\n").arg(sta));
                }
            }
        }else{
            errorText.append("error while reading stations\n");
        }
        createBaselines = true;
        networkSizeChanged();
        auto *tmp = ui->tabWidget_simAna->findChild<QWidget *>("Simulation_Widged");
        SimulatorWidget *sim = qobject_cast<SimulatorWidget *>(tmp);
        sim->addStations();

        auto *tmp2 = ui->tabWidget_simAna->findChild<QWidget *>("Solver_Widged");
        SolverWidget *solver = qobject_cast<SolverWidget *>(tmp2);
        solver->addStations();

        auto *tmp3 = ui->tabWidget_simAna->findChild<QWidget *>("Priorities_Widged");
        Priorities *priorities = qobject_cast<Priorities *>(tmp3);
        priorities->addStations();

        createBaselineModel();


        ui->schedulerLineEdit->setText(sked);
        ui->correlatorLineEdit->setText(corr);

        if(errorText.size() != 0){
            QMessageBox::warning(this,"errors while reading session master line",errorText);
        }

        auto downtimes = qtUtil::getDownTimes(start, start.addSecs(dur*3600), stas);
        if(!downtimes.isEmpty()){

            for(const auto any : downtimes){
                QString downTLC = any.first.toUpper();
                std::string station = tlc2station[downTLC].toStdString();
                unsigned int downStart = any.second.first;
                unsigned int downEnd = any.second.second;

                stationSetupWidget->addDownTime(station, downStart, downEnd);
            }
        }
    }
}

void MainWindow::on_experimentNameLineEdit_textChanged(const QString &arg1)
{
    if(arg1.length() >6){
        QPalette p = ui->experimentNameLineEdit->palette();
        p.setColor(QPalette::Base, Qt::red);
        ui->experimentNameLineEdit->setPalette(p);
    }else{
        QPalette p = ui->experimentNameLineEdit->palette();
        p.setColor(QPalette::Base, Qt::white);
        ui->experimentNameLineEdit->setPalette(p);
    }
}

void MainWindow::on_comboBox_log_file_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "trace" || arg1 == "debug"){
        ui->label_log->setText("high log level can slow down application");
    }else{
        if(ui->comboBox_log_console->currentText() == "trace" || ui->comboBox_log_console->currentText() == "debug"){
            ui->label_log->setText("heavy logging can slow down application");
        }else{
            ui->label_log->setText("");
        }
    }
}

void MainWindow::on_comboBox_log_console_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "trace" || arg1 == "debug"){
        ui->label_log->setText("high log level can slow down application");
    }else{
        if(ui->comboBox_log_file->currentText() == "trace" || ui->comboBox_log_file->currentText() == "debug"){
            ui->label_log->setText("heavy logging can slow down application");
        }else{
            ui->label_log->setText("");
        }
    }
}

// ########################################### SETUP ###########################################


void MainWindow::clearGroup(bool sta, bool src, bool bl, QString name)
{
    bool anyMapCleared = false;
    if(sta){
        int i=0;
        bool mapCleared = false;
        while(i<allStationPlusGroupModel->rowCount()){
            QString txt = allStationPlusGroupModel->item(i)->text();
            if(txt == "__all__"){
                ++i;
                continue;
            }
            if(groupSta->find(txt.toStdString()) != groupSta->end()){
                auto vec = (*groupSta)[txt.toStdString()];
                auto it = std::find(vec.begin(),vec.end(),name.toStdString());
                if(it != vec.end()){
                    vec.erase(it);
                    (*groupSta)[txt.toStdString()] = vec;
                }
                if(vec.empty()){
                    allStationPlusGroupModel->removeRow(i);
                    groupSta->erase(txt.toStdString());
                    mapCleared = true;
                }else{
                    ++i;
                }
            }else{
                break;
            }
        }
        anyMapCleared = anyMapCleared || mapCleared;
        if(mapCleared){
            clearSetup(true,false,false);
        }
    }
    if(src){
        int i=0;
        bool mapCleared = false;
        while(i<allSourcePlusGroupModel->rowCount()){
            QString txt = allSourcePlusGroupModel->item(i)->text();
            if(txt == "__all__"){
                ++i;
                continue;
            }
            if(groupSrc->find(txt.toStdString()) != groupSrc->end()){
                auto vec = (*groupSrc)[txt.toStdString()];
                auto it = std::find(vec.begin(),vec.end(),name.toStdString());
                if(it != vec.end()){
                    vec.erase(it);
                    (*groupSrc)[txt.toStdString()] = vec;
                }
                if(vec.empty()){
                    if(ui->comboBox_calibratorBlock_calibratorSources->currentText() == txt){
                        QMessageBox::warning(this,"Calibration block error!","A source group was deleted and this group was choosen as calibrator source group!\nCheck calibrator block!");
                        ui->comboBox_calibratorBlock_calibratorSources->setCurrentIndex(0);
                    }
                    for(int i=0; i<ui->tableWidget_scanSequence->rowCount(); ++i){
                        QComboBox* cb = qobject_cast<QComboBox*>(ui->tableWidget_scanSequence->cellWidget(i,0));
                        if(cb->currentText() == txt){
                            QMessageBox::warning(this,"Scan sequence error!","A source group was deleted and this group was choosen in scan sequence!\nCheck scan sequence!");
                            cb->setCurrentIndex(0);
                        }
                    }
                    allSourcePlusGroupModel->removeRow(i);
                    groupSrc->erase(txt.toStdString());
                    mapCleared = true;
                }else{
                    ++i;
                }
            }else{
                break;
            }
        }
        anyMapCleared = anyMapCleared || mapCleared;
        if(mapCleared){
            clearSetup(false,true,false);
        }
    }
    if(bl){
        int i=0;
        bool mapCleared = false;
        while(i<allBaselinePlusGroupModel->rowCount()){
            QString txt = allBaselinePlusGroupModel->item(i)->text();
            if(txt == "__all__"){
                ++i;
                continue;
            }
            if(groupBl->find(txt.toStdString()) != groupBl->end()){
                auto vec = (*groupBl)[txt.toStdString()];
                int j = 0;
                while(j<vec.size()){
                    QString itm = QString::fromStdString(vec.at(j));
                    auto stations = itm.split("-");
                    if(stations.indexOf(name) != -1){
                        vec.erase(vec.begin()+j);
                    }else{
                        ++j;
                    }
                    (*groupBl)[txt.toStdString()] = vec;
                }
                if(vec.empty()){
                    allBaselinePlusGroupModel->removeRow(i);
                    groupBl->erase(txt.toStdString());
                    mapCleared = true;
                }else{
                    ++i;
                }
            }else{
                break;
            }
        }
        anyMapCleared = anyMapCleared || mapCleared;
        if(mapCleared){
            clearSetup(false,false,true);
        }
    }

    if(anyMapCleared){
        QMessageBox::warning(this,"Group deleted!","At least one group became empty!\nSetup might got removed... please check setup again!");
    }

}

bool MainWindow::clearSetup(bool sta, bool src, bool bl)
{
    bool flag = false;
    if(sta){
        flag = flag | stationSetupWidget->reset();
    }

    if(src){
        flag = flag | sourceSetupWidget->reset();
        flag = flag | satelliteSetupWidget->reset();
        flag = flag | spacecraftSetupWidget->reset();
    }

    if(bl){
        flag = flag | baselineSetupWidget->reset();
    }
    return flag;
}

// ########################################### STATION AND BASELINE ###########################################

void MainWindow::readStations()
{
    selectedStationModel->blockSignals(true);

    QString antennaPath = ui->lineEdit_pathAntenna->text();
    QString equipPath = ui->lineEdit_pathEquip->text();
    QString positionPath = ui->lineEdit_pathPosition->text();
    QMap<QString,QStringList > antennaMap;
    QMap<QString,QStringList > equipMap;
    QMap<QString,QStringList > positionMap;

    QFile antennaFile(antennaPath);
    if (antennaFile.open(QIODevice::ReadOnly)){
        QTextStream in(&antennaFile);
        while (!in.atEnd()){
            QString line = in.readLine();
            if(line.isEmpty() || line[0] == "*" || line[0] == "!" || line[0] == "&"){
                continue;
            }
            QStringList split = line.split(" ",QString::SplitBehavior::SkipEmptyParts);
            QString antennaName = split[1];
            antennaMap.insert(antennaName,split);
        }
        antennaFile.close();
    }

    QFile equipFile(equipPath);
    if (equipFile.open(QIODevice::ReadOnly)){
        QTextStream in(&equipFile);
        while (!in.atEnd()){
            QString line = in.readLine();
            if(line.isEmpty() || line[0] == "*" || line[0] == "!" || line[0] == "&"){
                continue;
            }
            QStringList split = line.split(" ",QString::SplitBehavior::SkipEmptyParts);
            QString equipName = split[1] + "|" + split[0];
            equipName = equipName.toUpper();
            equipMap.insert(equipName,split);
        }
        equipFile.close();
    }

    QFile positionFile(positionPath);
    if (positionFile.open(QIODevice::ReadOnly)){
        QTextStream in(&positionFile);
        while (!in.atEnd()){
            QString line = in.readLine();
            if(line.isEmpty() || line[0] == "*" || line[0] == "!" || line[0] == "&"){
                continue;
            }
            QStringList split = line.split(" ",QString::SplitBehavior::SkipEmptyParts);
            QString positionName = split[0];
            positionName = positionName;
            positionMap.insert(positionName,split);
        }
        positionFile.close();
    }

    QMap<QString, QStringList>::iterator i;
    for (i = antennaMap.begin(); i != antennaMap.end(); ++i){
        try{
            QString antName = i.key();
            QStringList antList = i.value();

            QString antId,eqKey,posKey;
            double offset, rate1, con1, axis1_low, axis1_up, rate2, con2, axis2_low, axis2_up, diam;
            if(antList.size()>14){
                antId = antList.at(13);

                offset = antList.at(3).toDouble();
                rate1 = antList.at(4).toDouble();
                con1 = antList.at(5).toDouble();
                axis1_low = antList.at(6).toDouble();
                axis1_up = antList.at(7).toDouble();
                rate2 = antList.at(8).toDouble();
                con2 = antList.at(9).toDouble();
                axis2_low = antList.at(10).toDouble();
                axis2_up = antList.at(11).toDouble();
                diam = antList.at(12).toDouble();

                eqKey = antList.at(14) + "|" +antName;
                eqKey = eqKey.toUpper();
                posKey = antList.at(13);
//                posKey = posKey.toUpper();
            }else{
                continue;
            }

            QStringList eqList = equipMap.value(eqKey);
            double SEFD_X, SEFD_S;
            if(eqList.size()>8){
                SEFD_X = 0;
                if(eqList.at(5) == "X"){
                    SEFD_X = eqList.at(6).toDouble();
                }

                SEFD_S = 0;
                if(eqList.at(7) == "S"){
                    SEFD_S = eqList.at(8).toDouble();
                }
            }else{
                continue;
            }

            QStringList posList = positionMap.value(posKey);
            double x, y, z, lon, lat, h;
            if(posList.size()>8){
                x = posList.at(2).toDouble();
                y = posList.at(3).toDouble();
                z = posList.at(4).toDouble();

                double a = 6378136.6;
                double f = 1/298.25642;
                double e2 = 2*f-f*f;

                lon = atan2(y,x);
                double r = sqrt(x*x+y*y);
                lat = atan2(z,r);

                for(int i=0; i<6; ++i){
                    double N=a/sqrt(1-e2*sin(lat)*sin(lat));
                    h=r/cos(lat)-N;
                    lat=atan2(z*(N+h),r*((1-e2)*N+h));
                }
            }else{
                continue;
            }

            allStationModel->insertRow(0);
            allStationModel->setData(allStationModel->index(0, 0), antName);
            allStationModel->item(0,0)->setIcon(QIcon(":/icons/icons/station.png"));
            allStationModel->setData(allStationModel->index(0, 1), antId);
            allStationModel->setData(allStationModel->index(0, 2), (double)((int)(qRadiansToDegrees(lat)*100 +0.5))/100.0);
            allStationModel->setData(allStationModel->index(0, 3), (double)((int)(qRadiansToDegrees(lon)*100 +0.5))/100.0);
            allStationModel->setData(allStationModel->index(0, 4), (double)((int)(diam*10 +0.5))/10.0);

            allStationModel->setData(allStationModel->index(0, 5), SEFD_X);
            allStationModel->setData(allStationModel->index(0, 6), SEFD_S);

            allStationModel->setData(allStationModel->index(0, 7), offset);

            allStationModel->setData(allStationModel->index(0, 8), rate1);
            allStationModel->setData(allStationModel->index(0, 9), con1);
            allStationModel->setData(allStationModel->index(0, 10), axis1_low);
            allStationModel->setData(allStationModel->index(0, 11), axis1_up);

            allStationModel->setData(allStationModel->index(0, 12), rate2);
            allStationModel->setData(allStationModel->index(0, 13), con2);
            allStationModel->setData(allStationModel->index(0, 14), axis2_low);
            allStationModel->setData(allStationModel->index(0, 15), axis2_up);


            allStationModel->setData(allStationModel->index(0, 16), x);
            allStationModel->setData(allStationModel->index(0, 17), y);
            allStationModel->setData(allStationModel->index(0, 18), z);

        }catch(...){

        }
    }

    for(int i=0; i<19; ++i){
        ui->treeView_allAvailabeStations->resizeColumnToContents(i);
    }

    selectedStationModel->blockSignals(false);
    networkSizeChanged();
    auto *tmp = ui->tabWidget_simAna->findChild<QWidget *>("Simulation_Widged");
    SimulatorWidget *sim = qobject_cast<SimulatorWidget *>(tmp);
    sim->addStations();

    auto *tmp2 = ui->tabWidget_simAna->findChild<QWidget *>("Solver_Widged");
    SolverWidget *solver = qobject_cast<SolverWidget *>(tmp2);
    solver->addStations();

    auto *tmp3 = ui->tabWidget_simAna->findChild<QWidget *>("Priorities_Widged");
    Priorities *priorities = qobject_cast<Priorities *>(tmp3);
    priorities->addStations();

    plotWorldMap();

}

void MainWindow::on_treeView_allSelectedStations_clicked(const QModelIndex &index)
{

    QString name = selectedStationModel->item(index.row())->text();
    selectedStationModel->removeRow(index.row());
    clearGroup(true,false,true, name);

    int row;
    double x;
    double y;
    for(int i = 0; i < allStationModel->rowCount(); ++i){
        if (allStationModel->index(i,0).data().toString() == name){
            row = i;
            x = allStationModel->index(row,3).data().toDouble();
            y = allStationModel->index(row,2).data().toDouble();
            break;
        }
    }

    for(int i = 0; i<selectedStations->count(); ++i){
        double xn = selectedStations->at(i).x();
        double yn = selectedStations->at(i).y();
        if( (x-xn)*(x-xn) + (y-yn)*(y-yn) < 1e-3 ){
            selectedStations->remove(i);
            break;
        }
    }

    for(int i = 0; i<allStationPlusGroupModel->rowCount(); ++i){
        if (allStationPlusGroupModel->index(i,0).data().toString() == name) {
            allStationPlusGroupModel->removeRow(i);
            break;
        }
    }


    if(createBaselines){
        createBaselineModel();
    }

    auto *tmp_ms = ui->groupBox_multiScheduling->findChild<QWidget *>("MultiScheduling_Widged");
    MulitSchedulingWidget *ms = qobject_cast<MulitSchedulingWidget *>(tmp_ms);
    ms->clear();
}

void MainWindow::on_treeView_allAvailabeStations_clicked(const QModelIndex &index)
{
    int row = index.row();
    QString name = allStationProxyModel->index(row,0).data().toString();

    if(selectedStationModel->findItems(name).isEmpty()){
        selectedStationModel->insertRow(0);

        int nrow = allStationModel->findItems(name).at(0)->row();
        for(int i=0; i<allStationModel->columnCount(); ++i){
            selectedStationModel->setItem(0, i, allStationModel->item(nrow,i)->clone() );
        }

        selectedStationModel->sort(0);
        selectedStations->append(allStationProxyModel->index(row,3).data().toDouble(),
                                 allStationProxyModel->index(row,2).data().toDouble());

        int r = 0;
        for(int i = 0; i<allStationPlusGroupModel->rowCount(); ++i){
            QString txt = allStationPlusGroupModel->item(i)->text();
            if(groupSta->find(txt.toStdString()) != groupSta->end() || txt == "__all__"){
                ++r;
                continue;
            }
            if(txt>name){
                break;
            }else{
                ++r;
            }
        }

        allStationPlusGroupModel->insertRow(r,new QStandardItem(QIcon(":/icons/icons/station.png"),name));
        if(createBaselines){
            createBaselineModel();
        }
    }
    ui->lineEdit_allStationsFilter->setFocus();
    ui->lineEdit_allStationsFilter->selectAll();
}

//void MainWindow::on_lineEdit_allStationsFilter_textChanged(const QString &arg1)
//{
//    allStationProxyModel->addFilterFixedString(arg1);
//}

void MainWindow::on_treeView_allAvailabeStations_entered(const QModelIndex &index)
{
    int row = index.row();
    QString name = allStationProxyModel->index(row,0).data().toString();
    QString id = allStationProxyModel->index(row,1).data().toString();

    double x = allStationProxyModel->index(row,3).data().toDouble();
    double y = allStationProxyModel->index(row,2).data().toDouble();

    QString text = QString("%1 (%2) \nlat: %3 [deg] \nlon: %4 [deg] ").arg(name).arg(id).arg(y).arg(x);
    worldMapCallout->setText(text);
    worldMapCallout->setAnchor(QPointF(x,y));
    worldMapCallout->setZValue(11);
    worldMapCallout->updateGeometry();
    worldMapCallout->show();
}

void MainWindow::on_treeView_allSelectedBaselines_entered(const QModelIndex &index)
{
    int row = index.row();
    QString txt = selectedBaselineModel->index(row,0).data().toString();
    QString txt2 = selectedBaselineModel->index(row,1).data().toString();
    txt.append("\n").append(txt2).append(" [km]");

    double x,y;
    for(int i=0; i<worldmap->chart()->series().count(); ++i){
        if(worldmap->chart()->series().at(i)->name() == txt){
            auto s = qobject_cast<QLineSeries *>(worldmap->chart()->series().at(i));
            x = (s->at(0).x()+s->at(1).x())/2;
            y = (s->at(0).y()+s->at(1).y())/2;
            break;
        }
    }


    worldMapCallout->setText(txt);
    worldMapCallout->setAnchor(QPointF(x,y));
    worldMapCallout->setZValue(11);
    worldMapCallout->updateGeometry();
    worldMapCallout->show();
}

void MainWindow::on_treeView_allSelectedStations_entered(const QModelIndex &index)
{
    int row = index.row();
    QString name = selectedStationModel->index(row,0).data().toString();
    QString id = selectedStationModel->index(row,1).data().toString();

    for(int i = 0; i < allStationModel->rowCount(); ++i){
        QString newName = allStationModel->index(i,0).data().toString();
        if (newName == name){
            double x = allStationModel->index(i,3).data().toDouble();;
            double y = allStationModel->index(i,2).data().toDouble();;
            QString text = QString("%1 (%2) \nlat: %3 [deg] \nlon: %4 [deg] ").arg(name).arg(id).arg(y).arg(x);
            worldMapCallout->setText(text);
            worldMapCallout->setAnchor(QPointF(x,y));
            worldMapCallout->setZValue(11);
            worldMapCallout->updateGeometry();
            worldMapCallout->show();
            break;
        }
    }
}

void MainWindow::plotWorldMap()
{
    QChart *worldChart = worldmap->chart();

    availableStations = new QScatterSeries(worldChart);
    availableStations->setColor(Qt::red);
    availableStations->setMarkerSize(10);
    availableStations->setName("availableStations");

    selectedStations = new QScatterSeries(worldChart);
    selectedStations->setName("selectedStations");
    markerWorldmap();

    worldChart->addSeries(availableStations);
    worldChart->addSeries(selectedStations);

    connect(availableStations,SIGNAL(hovered(QPointF,bool)),this,SLOT(worldmap_hovered(QPointF,bool)));
    connect(selectedStations,SIGNAL(hovered(QPointF,bool)),this,SLOT(worldmap_hovered(QPointF,bool)));


    for(int row = 0; row<allStationModel->rowCount(); ++row){
        double lat = allStationModel->index(row,2).data().toDouble();
        double lon = allStationModel->index(row,3).data().toDouble();
        availableStations->append(lon,lat);
    }

    availableStations->attachAxis(worldChart->axisX());
    availableStations->attachAxis(worldChart->axisY());
    selectedStations->attachAxis(worldChart->axisX());
    selectedStations->attachAxis(worldChart->axisY());

}

void MainWindow::worldmap_hovered(QPointF point, bool state)
{
    if (state) {
        QString sta;
        int scans;
        int obs;
        for(int i = 0; i<allStationModel->rowCount();++i){
            double x = allStationModel->index(i,3).data().toDouble();
            double y = allStationModel->index(i,2).data().toDouble();
            QString name = allStationModel->index(i,0).data().toString();
            QString id = allStationModel->index(i,1).data().toString();

            auto dx = x-point.x();
            auto dy = y-point.y();
            if(dx*dx+dy*dy < 1e-3){
                if(sta.size()==0){
                    sta.append(QString("%1 (%2)").arg(name).arg(id));
                }else{
                    sta.append(",").append(QString("%1 (%2)").arg(name).arg(id));
                }
            }

        }

        QString text = QString("%1 \nlat: %2 [deg] \nlon: %3 [deg] ").arg(sta).arg(point.y()).arg(point.x());
        worldMapCallout->setText(text);
        worldMapCallout->setAnchor(point);
        worldMapCallout->setZValue(11);
        worldMapCallout->updateGeometry();
        worldMapCallout->show();
    } else {
        worldMapCallout->hide();
    }

}

void MainWindow::addGroupStation()
{
    AddGroupDialog *dial = new AddGroupDialog(settings_,AddGroupDialog::Type::station,this);
    dial->addModel(selectedStationModel, *groupSta);
    int result = dial->exec();
    if(result == QDialog::Accepted){
        std::vector<std::string> stdlist = dial->getSelection();
        std::string stdname = dial->getGroupName();
        VieVS::ParameterGroup newGroup(stdname, stdlist);

        int r = 0;
        for(int i = 0; i<allStationPlusGroupModel->rowCount(); ++i){
            QString txt = allStationPlusGroupModel->item(i)->text();
            if(txt == "__all__"){
                ++r;
                continue;
            }
            if(groupSta->find(txt.toStdString()) == groupSta->end()){
                break;
            }
            if(txt>QString::fromStdString(stdname)){
                break;
            }else{
                ++r;
            }
        }
        (*groupSta)[stdname] = stdlist;

        allStationPlusGroupModel->insertRow(r,new QStandardItem(QIcon(":/icons/icons/station_group.png"),QString::fromStdString(stdname) ));
        if(sender() == stationSetupWidget->addGroupButton()){
            stationSetupWidget->memberComboBox()->setCurrentIndex(r);
        }
    }
    delete(dial);
}

void MainWindow::addGroupBaseline()
{
    AddGroupDialog *dial = new AddGroupDialog(settings_,AddGroupDialog::Type::baseline,this);
    dial->addModel(selectedBaselineModel, *groupBl);
    int result = dial->exec();
    if(result == QDialog::Accepted){
        std::vector<std::string> stdlist = dial->getSelection();
        std::string stdname = dial->getGroupName();
        VieVS::ParameterGroup newGroup(stdname, stdlist);

        int r = 0;
        for(int i = 0; i<allBaselinePlusGroupModel->rowCount(); ++i){
            QString txt = allBaselinePlusGroupModel->item(i)->text();
            if(txt == "__all__"){
                ++r;
                continue;
            }
            if(groupBl->find(txt.toStdString()) == groupBl->end()){
                break;
            }
            if(txt>QString::fromStdString(stdname)){
                break;
            }else{
                ++r;
            }
        }

        (*groupBl)[stdname] = stdlist;

        allBaselinePlusGroupModel->insertRow(r,new QStandardItem(QIcon(":/icons/icons/baseline_group.png"),QString::fromStdString(stdname) ));
        if(sender() == baselineSetupWidget->addGroupButton()){
            baselineSetupWidget->memberComboBox()->setCurrentIndex(r);
        }
    }
    delete(dial);
}


void MainWindow::createBaselineModel()
{
    selectedBaselineModel->removeRows(0,selectedBaselineModel->rowCount());

    allBaselinePlusGroupModel->setRowCount(1);
    for(const auto& any:*groupBl){
        allBaselinePlusGroupModel->appendRow(new QStandardItem(QIcon(":/icons/icons/baseline_group.png"),QString::fromStdString(any.first)));
    }

    int n = selectedStationModel->rowCount();
    for(int i = 0; i<n; ++i){
        for(int j = i+1; j<n; ++j){
            QString bl = selectedStationModel->index(i,1).data().toString();
            bl.append("-").append(selectedStationModel->index(j,1).data().toString());
            allBaselinePlusGroupModel->appendRow(new QStandardItem(QIcon(":/icons/icons/baseline.png"),bl));
            int row = selectedBaselineModel->rowCount();

            selectedBaselineModel->insertRow(row);
            selectedBaselineModel->setItem(row,new QStandardItem(QIcon(":/icons/icons/baseline.png"),bl));
        }
    }

    auto series = worldmap->chart()->series();
    QAbstractSeries *tmpSel;
    QAbstractSeries *tmpAva;
    int nn = series.count();
    for(int i=0; i<nn; ++i){
        QString name = series.at(i)->name();
        if(name.size() >=5 && name.left(5) == "coast"){
            continue;
        }
        if(name == "selectedStations"){
            tmpSel = series.at(i);
            worldmap->chart()->removeSeries(series.at(i));
            continue;
        }
        if(name == "availableStations"){
            tmpAva = series.at(i);
            worldmap->chart()->removeSeries(series.at(i));
            continue;
        }
        worldmap->chart()->removeSeries(series.at(i));
        delete(series.at(i));
    }

    for(int i=0; i<selectedBaselineModel->rowCount(); ++i){
        QString txt = selectedBaselineModel->item(i,0)->text();
        QStringList stas = txt.split("-");
        double lat1, lat2, lon1, lon2, x1,y1,z1,x2,y2,z2;

        bool found1 = false;
        bool found2 = false;
        for(int j=0; j<selectedStationModel->rowCount(); ++j){
            auto thisSta = selectedStationModel->item(j,1)->text();
            if(thisSta == stas.at(0)){
                lon1 = selectedStationModel->index(j,3).data().toDouble();
                lat1 = selectedStationModel->index(j,2).data().toDouble();
                x1 = selectedStationModel->index(j, 16).data().toDouble();
                y1 = selectedStationModel->index(j, 17).data().toDouble();
                z1 = selectedStationModel->index(j, 18).data().toDouble();

                found1 = true;
            }else if(thisSta == stas.at(1)){
                lon2 = selectedStationModel->index(j,3).data().toDouble();
                lat2 = selectedStationModel->index(j,2).data().toDouble();
                x2 = selectedStationModel->index(j, 16).data().toDouble();
                y2 = selectedStationModel->index(j, 17).data().toDouble();
                z2 = selectedStationModel->index(j, 18).data().toDouble();

                found2 = true;
            }

            if(found1 && found2){
                break;
            }
        }
        double dist = qRound(qSqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)+(z2-z1)*(z2-z1))/1000);
//        selectedBaselineModel->setItem(i,1,new QStandardItem());
        selectedBaselineModel->setData(selectedBaselineModel->index(i, 1), dist);

        if(lon1>lon2){
            auto tmp1 = lon1;
            lon1 = lon2;
            lon2 = tmp1;
            auto tmp2 = lat1;
            lat1 = lat2;
            lat2 = tmp2;
        }

        if(qAbs(lon2-lon1)<180){
            QLineSeries *bl = new QLineSeries(worldmap->chart());
            bl->setPen(QPen(QBrush(Qt::darkGreen),1.5,Qt::DashLine));
            bl->append(lon1,lat1);
            bl->append(lon2,lat2);
            bl->setName(txt.append(QString("\n%1 [km]").arg(dist)));
            connect(bl,SIGNAL(hovered(QPointF,bool)),this,SLOT(baselineHovered(QPointF,bool)));
            worldmap->chart()->addSeries(bl);
        }else{

            double dx = 180-qAbs(lon1)+180-qAbs(lon2);
            double dy = lat2-lat1;

            QLineSeries *bl1 = new QLineSeries(worldmap->chart());
            bl1->setPen(QPen(QBrush(Qt::darkGreen),1.5,Qt::DashLine));
            bl1->append(lon1,lat1);
            double fracx = (180-qAbs(lon1))/dx;
            double fracy = dy*fracx;
            bl1->append(-180,lat1+fracy);
            bl1->setName(txt.append(QString("\n%1 [km]").arg(dist)));
            connect(bl1,SIGNAL(hovered(QPointF,bool)),this,SLOT(baselineHovered(QPointF,bool)));

            QLineSeries *bl2 = new QLineSeries(worldmap->chart());
            bl2->setPen(QPen(QBrush(Qt::darkGreen),1.5,Qt::DashLine));
            bl2->append(lon2,lat2);
            bl2->append(180,lat2-(dy-fracy));
            bl2->setName(txt.append(QString("\n%1 [km]").arg(dist)));
            connect(bl2,SIGNAL(hovered(QPointF,bool)),this,SLOT(baselineHovered(QPointF,bool)));

            if(qAbs(lon1)>qAbs(lon2)){
                worldmap->chart()->addSeries(bl2);
                worldmap->chart()->addSeries(bl1);
            }else{
                worldmap->chart()->addSeries(bl1);
                worldmap->chart()->addSeries(bl2);
            }
        }
    }
    worldmap->chart()->addSeries(tmpAva);
    worldmap->chart()->addSeries(tmpSel);

    worldmap->chart()->createDefaultAxes();
}


void MainWindow::setupStationAxisBufferAddRow()
{
    auto t = ui->treeWidget_setupStationAxis;
    int row = t->topLevelItemCount();
    QString name = ui->comboBox_stationSettingMember_axis->currentText();
    QIcon ic;
    bool inGroup = groupSta->find(name.toStdString()) != groupSta->end();
    if( inGroup || name == "__all__"){
        ic = QIcon(":/icons/icons/station_group.png");
    }else{
        ic = QIcon(":/icons/icons/station.png");
    }

    QString errorStation;
    bool valid = true;
    for(int i = 0; i<row; ++i){
        QString itmName = t->topLevelItem(i)->text(0);
        if(name == "__all__" || itmName == "__all__"){
            valid = false;
            errorStation = QString("__all__");
            break;
        }

        if(groupSta->find(itmName.toStdString()) != groupSta->end()){
            std::vector<std::string> itmMembers = groupSta->at(itmName.toStdString());
            if(inGroup){
                std::vector<std::string> members = groupSta->at(name.toStdString());
                for(const auto &any:members){
                    if(std::find(itmMembers.begin(),itmMembers.end(),any) != itmMembers.end()){
                        valid = false;
                        errorStation = QString::fromStdString(any);
                        break;
                    }
                }
            }else{
                if(std::find(itmMembers.begin(),itmMembers.end(),name.toStdString()) != itmMembers.end()){
                    valid = false;
                    errorStation = name;
                    break;
                }
            }
        }else{
            if(inGroup){
                std::vector<std::string> members = groupSta->at(name.toStdString());
                if(std::find(members.begin(),members.end(),itmName.toStdString()) != members.end()){
                    valid = false;
                    errorStation = itmName;
                    break;
                }
            }else{
                if(itmName.toStdString() == name.toStdString()){
                    valid = false;
                    errorStation = name;
                    break;
                }
            }
        }
    }

    if(valid){
        t->insertTopLevelItem(row,new QTreeWidgetItem());
        t->topLevelItem(row)->setText(0,name);
        t->topLevelItem(row)->setIcon(0,ic);
        t->topLevelItem(row)->setText(1,QString::number(ui->DoubleSpinBox_axis1low->value()).append(" [deg]"));
        t->topLevelItem(row)->setText(2,QString::number(ui->DoubleSpinBox_axis1up->value()).append(" [deg]"));
        t->topLevelItem(row)->setText(3,QString::number(ui->DoubleSpinBox_axis2low->value()).append(" [deg]"));
        t->topLevelItem(row)->setText(4,QString::number(ui->DoubleSpinBox_axis2up->value()).append(" [deg]"));
    }else{
        QMessageBox *ms = new QMessageBox(this);
        QString txt;
        if(errorStation != "__all__"){
            txt = QString("Setup for station %1 already defined!").arg(errorStation);
        }else{
            txt = QString("Setup for all stations is already defined! \nRemove selection and try again!");
        }
        ms->warning(this,"Multiple setup for station",txt);
        delete(ms);
    }
}

void MainWindow::on_pushButton_16_clicked()
{
    auto t = ui->treeWidget_setupStationAxis;
    auto sel = t->selectedItems();
    for(int i=0; i<sel.count(); ++i){
        delete(sel.at(i));
    }
}

void MainWindow::on_pushButton_saveNetwork_clicked()
{
    QVector<QString> selSta;
    for(int i = 0; i<selectedStationModel->rowCount(); ++i){
        selSta.append(selectedStationModel->item(i)->text());
    }
    saveToSettingsDialog *dial = new saveToSettingsDialog(settings_,this);
    dial->setType(saveToSettingsDialog::Type::stationNetwork);
    dial->setNetwork(selSta);

    dial->exec();
}

void MainWindow::on_pushButton_loadNetwork_clicked()
{
    auto network= settings_.get_child_optional("settings.networks");
    if(!network.is_initialized()){
        QMessageBox::warning(this,"No network found!","There was no network saved in settings.xml file\nCheck settings.networks");
        return;
    }

    QVector<QString> names;
    QVector<QVector<QString> > networks;

    for(const auto &it:*network){
        QString name = QString::fromStdString(it.second.get_child("<xmlattr>.name").data());
        QVector<QString> network;

        for(const auto &it2:it.second){
            if(it2.first == "member"){
                network.push_back(QString::fromStdString(it2.second.data()));
            }
        }

        names.push_back(name);
        networks.push_back(network);
    }
    settingsLoadWindow *dial = new settingsLoadWindow(this);

    dial->setNetwork(names,networks);

    int result = dial->exec();
    if(result == QDialog::Accepted){

        for(int i=0; i<selectedStationModel->rowCount(); ++i){
            QModelIndex idx = selectedStationModel->index(0,0);
            on_treeView_allSelectedStations_clicked(idx);
        }

        QString warningTxt;

        QString itm = dial->selectedItem();
        int idx = dial->selectedIdx();
        QVector<QString> members = networks.at(idx);

        for(const auto&any:members){
            auto list = allStationModel->findItems(any);
            if(list.size() == 1){
                for(int i = 0; i<allStationProxyModel->rowCount(); ++i){
                    QModelIndex idx = allStationProxyModel->index(i,0);
                    if(idx.data().toString() == any){
                        ui->treeView_allAvailabeStations->clicked(idx);
                    }
                }
//                selectedStationModel->appendRow(new QStandardItem(QIcon(":icons/icons/station.png"),list.at(0)->text()));
            }else{
                warningTxt.append("    unknown station: ").append(any).append("!\n");
            }
        }
        if(!warningTxt.isEmpty()){
            QString txt = "The following errors occurred while loading the network:\n";
            txt.append(warningTxt).append("These stations were ignored!\nPlease double check stations again!");
            QMessageBox::warning(this,"Unknown network stations!",txt);
        }
    }
}

void MainWindow::networkSizeChanged()
{
    int size = selectedStationModel->rowCount();
    ui->label_network_selected->setText(QString("selected: %1").arg(size));

    if(advancedObservingMode_.is_initialized()){
        QMessageBox::warning(this,"Custom observing mode deleted!","Your custom observing mode was deleted due to change in station network!");
        advancedObservingMode_ = boost::none;
        updateAdvancedObservingMode();
    }
}

void MainWindow::baselineListChanged()
{
    int size = selectedBaselineModel->rowCount();
    ui->label_network_baselines->setText(QString("baselines: %1").arg(size));
}

void MainWindow::markerWorldmap()
{
    if(ui->radioButton_marker_worldmap->isChecked()){
        selectedStations->setMarkerSize(ui->horizontalSlider_markerSizeWorldmap->value());
        selectedStations->setBrush(QBrush(Qt::darkGreen,Qt::SolidPattern));
        selectedStations->setPen(QColor(Qt::white));
    }else{
        QImage img(":/icons/icons/station_white.png");
        img = img.scaled(ui->horizontalSlider_markerSizeWorldmap->value(),ui->horizontalSlider_markerSizeWorldmap->value());
        selectedStations->setBrush(QBrush(img));
        selectedStations->setMarkerSize(ui->horizontalSlider_markerSizeWorldmap->value());
        selectedStations->setPen(QColor(Qt::transparent));
    }
}

void MainWindow::on_radioButton_imageWorldmap_toggled(bool checked)
{
    if(checked){
        ui->horizontalSlider_markerSizeWorldmap->setValue(30);
    }else{
        ui->horizontalSlider_markerSizeWorldmap->setValue(15);
    }
    markerWorldmap();
}

void MainWindow::baselineHovered(QPointF point, bool flag)
{
    if (flag) {
        auto tmp = sender();
        auto x = qobject_cast<QLineSeries *>(tmp);
        QString name = x->name();
        QString text = QString("%1").arg(name);
        worldMapCallout->setText(text);
        worldMapCallout->setAnchor(point);
        worldMapCallout->setZValue(11);
        worldMapCallout->updateGeometry();
        worldMapCallout->show();
    } else {
        worldMapCallout->hide();
    }
}

void MainWindow::on_checkBox_showBaselines_clicked(bool checked)
{
    auto series = worldmap->chart()->series();
    for(int i=0; i<series.count(); ++i){
        QString name = series.at(i)->name();
        if(name.size() >=5 && name.left(5) == "coast"){
            continue;
        }
        if(name == "selectedStations"){
            continue;
        }
        if(name == "availableStations"){
            continue;
        }
        if(checked){
            series.at(i)->setVisible(true);
        }else{
            series.at(i)->setVisible(false);
        }
    }
}

// ########################################### SOURCE ###########################################

void MainWindow::readSources()
{
    sourceSetupWidget->blockSignal(true);
    selectedSourceModel->blockSignals(true);

    QString sourcePath = ui->lineEdit_pathSource->text();

    QFile sourceFile(sourcePath);
    if (sourceFile.open(QIODevice::ReadOnly)){
        QTextStream in(&sourceFile);
        while (!in.atEnd()){
            QString line = in.readLine();
            if(line.isEmpty() || line[0] == "*" || line[0] == "!" || line[0] == "&"){
                continue;
            }
            QStringList split = line.split(" ",QString::SplitBehavior::SkipEmptyParts);
            QString sourceName = split.at(0);
            QString rah = split.at(2);
            QString ram = split.at(3);
            QString ras = split.at(4);
            double ra = (rah.toDouble() + ram.toDouble()/60 + ras.toDouble()/3600)*15;
            QString ded = split.at(5);
            QString dem = split.at(6);
            QString des = split.at(7);
            double de = ded.toDouble() + dem.toDouble()/60 + des.toDouble()/3600;

            sourceSetupWidget->blockSignal(true);
            allSourceModel->insertRow(allSourceModel->rowCount());
            allSourceModel->setData(allSourceModel->index(allSourceModel->rowCount()-1,0), sourceName);
            allSourceModel->item(allSourceModel->rowCount()-1,0)->setIcon(QIcon(":/icons/icons/source.png"));
            allSourceModel->setData(allSourceModel->index(allSourceModel->rowCount()-1, 1), (double)((int)(ra*100 +0.5))/100.0);
            allSourceModel->setData(allSourceModel->index(allSourceModel->rowCount()-1, 2), (double)((int)(de*100 +0.5))/100.0);

            selectedSourceModel->insertRow(selectedSourceModel->rowCount());
            selectedSourceModel->setData(selectedSourceModel->index(selectedSourceModel->rowCount()-1,0), sourceName);
            selectedSourceModel->item(selectedSourceModel->rowCount()-1,0)->setIcon(QIcon(":/icons/icons/source.png"));
            selectedSourceModel->setData(selectedSourceModel->index(selectedSourceModel->rowCount()-1, 1), (double)((int)(ra*100 +0.5))/100.0);
            selectedSourceModel->setData(selectedSourceModel->index(selectedSourceModel->rowCount()-1, 2), (double)((int)(de*100 +0.5))/100.0);
            sourceSetupWidget->blockSignal(false);
            sourceSetupWidget->setupComboBox()->setCurrentIndex(0);

            {
                int r = 0;
                for(int i = 0; i<allSourcePlusGroupModel->rowCount(); ++i){
                    QString txt = allSourcePlusGroupModel->item(i)->text();
                    if(groupSrc->find(txt.toStdString()) != groupSrc->end() || txt == "__all__"){
                        ++r;
                        continue;
                    }
                    if(txt>sourceName){
                        break;
                    }else{
                        ++r;
                    }
                }
                allSourcePlusGroupModel->insertRow(r,new QStandardItem(QIcon(":/icons/icons/source.png"),sourceName));
            }

            {
                int r = 0;
                for(int i = 0; i<allSourcePlusGroupModel_combined->rowCount(); ++i){
                    QString txt = allSourcePlusGroupModel_combined->item(i)->text();
                    if(groupSrc->find(txt.toStdString()) != groupSrc->end() || groupSat->find(txt.toStdString()) != groupSat->end() || groupSpace->find(txt.toStdString()) != groupSpace->end() ){
                        ++r;
                        continue;
                    }
                    if(txt == "__all__" || txt == "__AGNs__" || txt == "__satellites__" || txt == "__spacecrafts__" ){
                        ++r;
                        continue;
                    }
                    if(txt>sourceName){
                        break;
                    }else{
                        ++r;
                    }
                }
                allSourcePlusGroupModel_combined->insertRow(r,new QStandardItem(QIcon(":/icons/icons/source.png"),sourceName));
            }

        }
        sourceFile.close();
    }
    selectedSourceModel->blockSignals(false);
    sourceSetupWidget->blockSignal(false);
    sourceSetupWidget->setupComboBox()->setCurrentIndex(0);
    sourceListChanged();
    auto *tmp2 = ui->tabWidget_simAna->findChild<QWidget *>("Solver_Widged");
    SolverWidget *solver = qobject_cast<SolverWidget *>(tmp2);
    solver->addSources();

    plotSkyMap();
}

void MainWindow::readSatellites()
{
    satelliteSetupWidget->blockSignal(true);
    QString pathToTle = ui->lineEdit_pathSatellite->text();
    if(pathToTle.isEmpty()){
        return;
    }
    QFile sourceFile(pathToTle);
    if (sourceFile.open(QIODevice::ReadOnly)){
        QTextStream in(&sourceFile);
        QString header;
        QString line1;
        QString line2;
        int flag = 0;
        while (!in.atEnd()){
            QString line = in.readLine();
            line = line.simplified();
            if(line.isEmpty() || line[0] == "*" || line[0] == "!" || line[0] == "&"){
                continue;
            }
            switch ( flag ) {
                case 0: {
                    header = line;
                    ++flag;
                    break;
                }
                case 1: {
                    line1 = line;
                    ++flag;
                    break;
                }
                case 2: {
                    line2 = line;
                    ++flag;
                    break;
                }
                default: {
                    break;
                }
            }
            if ( flag == 3 ) {
                flag = 0;
                allSatelliteModel->insertRow(allSatelliteModel->rowCount());
                allSatelliteModel->setData(allSatelliteModel->index(allSatelliteModel->rowCount()-1,0), header);
                allSatelliteModel->item(allSatelliteModel->rowCount()-1,0)->setIcon(QIcon(":/icons/icons/satellite.png"));
                allSatelliteModel->setData(allSatelliteModel->index(allSatelliteModel->rowCount()-1, 1), line1);
                allSatelliteModel->setData(allSatelliteModel->index(allSatelliteModel->rowCount()-1, 2), line2);
            }
        }
    }
    satelliteSetupWidget->blockSignal(false);
}

void MainWindow::readSpacecraft()
{
    spacecraftSetupWidget->blockSignal(true);

    spacecraftSetupWidget->blockSignal(false);
}

void MainWindow::on_treeView_allSelectedSources_clicked(const QModelIndex &index)
{
    QString name = selectedSourceModel->item(index.row())->text();
    if(ui->comboBox_calibratorBlock_calibratorSources->currentText() == name){
        QMessageBox::warning(this,"Calibration block error!","Deleted source was choosen as calibrator source!\nCheck calibrator block!");
        ui->comboBox_calibratorBlock_calibratorSources->setCurrentIndex(0);
    }
    for(int i=0; i<ui->tableWidget_scanSequence->rowCount(); ++i){
        QComboBox* cb = qobject_cast<QComboBox*>(ui->tableWidget_scanSequence->cellWidget(i,0));
        if(cb->currentText() == name){
            QMessageBox::warning(this,"Scan sequence error!","Deleted source was in scan sequence!\nCheck scan sequence!");
            cb->setCurrentIndex(0);
        }
    }
    selectedSourceModel->removeRow(index.row());

    clearGroup(false,true,false,name);

    int row;
    double x;
    double y;
    for(int i = 0; i < allSourceModel->rowCount(); ++i){
        if (allSourceModel->index(i,0).data().toString() == name){
            row = i;
            double ra = allSourceModel->index(i,1).data().toDouble();
            double dc = allSourceModel->index(i,2).data().toDouble();
            ra -=180;

            double lambda = qDegreesToRadians(ra);
            double phi = qDegreesToRadians(dc);
            double hn = qSqrt( 1 + qCos(phi)*qCos(lambda/2) );

            x = (2 * qSqrt(2) *qCos(phi) *qSin(lambda/2) ) / hn;
            y = (qSqrt(2) *qSin(phi) ) / hn;
            break;
        }
    }

    for(int i = 0; i<selectedSources->count(); ++i){
        double xn = selectedSources->at(i).x();
        double yn = selectedSources->at(i).y();
        if( (x-xn)*(x-xn) + (y-yn)*(y-yn) < 1e-3 ){
            selectedSources->remove(i);
            break;
        }
    }

    for(int i = 0; i<allSourcePlusGroupModel->rowCount(); ++i){
        if (allSourcePlusGroupModel->index(i,0).data().toString() == name) {
            allSourcePlusGroupModel->removeRow(i);
            break;
        }
    }
    for(int i = 0; i<allSourcePlusGroupModel_combined->rowCount(); ++i){
        if (allSourcePlusGroupModel_combined->index(i,0).data().toString() == name) {
            allSourcePlusGroupModel_combined->removeRow(i);
            break;
        }
    }
    auto *tmp_ms = ui->groupBox_multiScheduling->findChild<QWidget *>("MultiScheduling_Widged");
    MulitSchedulingWidget *ms = qobject_cast<MulitSchedulingWidget *>(tmp_ms);
    ms->clear();
}


void MainWindow::on_treeView_allSelectedSatellites_clicked(const QModelIndex &index)
{
    QString name = selectedSatelliteModel->item(index.row())->text();
    selectedSatelliteModel->removeRow(index.row());
    for(int i = 0; i<allSatellitePlusGroupModel->rowCount(); ++i){
        if (allSatellitePlusGroupModel->index(i,0).data().toString() == name) {
            allSatellitePlusGroupModel->removeRow(i);
            break;
        }
    }
}

void MainWindow::on_treeView_allSelectedSpacecrafts_clicked(const QModelIndex &index)
{
    QString name = selectedSpacecraftModel->item(index.row())->text();
    selectedSpacecraftModel->removeRow(index.row());
    for(int i = 0; i<allSpacecraftPlusGroupModel->rowCount(); ++i){
        if (allSpacecraftPlusGroupModel->index(i,0).data().toString() == name) {
            allSpacecraftPlusGroupModel->removeRow(i);
            break;
        }
    }
}

void MainWindow::on_treeView_allAvailabeSources_clicked(const QModelIndex &index)
{
    int row = index.row();
    QString name = allSourceProxyModel->index(row,0).data().toString();

    if(selectedSourceModel->findItems(name).isEmpty()){


        selectedSourceModel->insertRow(0);

        int nrow = allSourceModel->findItems(name).at(0)->row();
        for(int i=0; i<allSourceModel->columnCount(); ++i){
            selectedSourceModel->setItem(0, i, allSourceModel->item(nrow,i)->clone() );
        }

        selectedSourceModel->sort(0);

        double ra = allSourceProxyModel->index(row,1).data().toDouble();
        double dc = allSourceProxyModel->index(row,2).data().toDouble();
        ra -=180;

        double lambda = qDegreesToRadians(ra);
        double phi = qDegreesToRadians(dc);
        double hn = qSqrt( 1 + qCos(phi)*qCos(lambda/2) );

        double x = (2 * qSqrt(2) *qCos(phi) *qSin(lambda/2) ) / hn;
        double y = (qSqrt(2) *qSin(phi) ) / hn;
        selectedSources->append(x,y);

        {
            int r = 0;
            for(int i = 0; i<allSourcePlusGroupModel->rowCount(); ++i){
                QString txt = allSourcePlusGroupModel->item(i)->text();
                if(groupSrc->find(txt.toStdString()) != groupSrc->end() || txt == "__all__"){
                    ++r;
                    continue;
                }
                if(txt>name){
                    break;
                }else{
                    ++r;
                }
            }

            allSourcePlusGroupModel->insertRow(r,new QStandardItem(QIcon(":/icons/icons/source.png"),name));
        }

        {
            int r = 0;
            for(int i = 0; i<allSourcePlusGroupModel_combined->rowCount(); ++i){
                QString txt = allSourcePlusGroupModel_combined->item(i)->text();
                if(groupSrc->find(txt.toStdString()) != groupSrc->end() || groupSat->find(txt.toStdString()) != groupSat->end() || groupSpace->find(txt.toStdString()) != groupSpace->end() ){
                    ++r;
                    continue;
                }
                if(txt == "__all__" || txt == "__AGNs__" || txt == "__satellites__" || txt == "__spacecrafts__" ){
                    ++r;
                    continue;
                }
                if(txt>name){
                    break;
                }else{
                    ++r;
                }
            }
            allSourcePlusGroupModel_combined->insertRow(r,new QStandardItem(QIcon(":/icons/icons/source.png"),name));
        }
    }
    ui->lineEdit_allSourceFilter->setFocus();
    ui->lineEdit_allSourceFilter->selectAll();
}

void MainWindow::on_treeView_allAvailabeSatellites_clicked(const QModelIndex &index)
{
    int row = index.row();
    QString name = allSatelliteProxyModel->index(row,0).data().toString();

    if(selectedSatelliteModel->findItems(name).isEmpty()){

        selectedSatelliteModel->insertRow(0);

        int nrow = allSatelliteModel->findItems(name).at(0)->row();
        for(int i=0; i<allSatelliteModel->columnCount(); ++i){
            selectedSatelliteModel->setItem(0, i, allSatelliteModel->item(nrow,i)->clone() );
        }

        selectedSatelliteModel->sort(0);

        {
            int r = 0;
            for(int i = 0; i<allSatellitePlusGroupModel->rowCount(); ++i){
                QString txt = allSatellitePlusGroupModel->item(i)->text();
                if(groupSat->find(txt.toStdString()) != groupSat->end() || txt == "__all__"){
                    ++r;
                    continue;
                }
                if(txt>name){
                    break;
                }else{
                    ++r;
                }
            }
            allSatellitePlusGroupModel->insertRow(r,new QStandardItem(QIcon(":/icons/icons/satellite.png"),name));
        }

        {
            int r = 0;
            for(int i = 0; i<allSourcePlusGroupModel_combined->rowCount(); ++i){
                QString txt = allSourcePlusGroupModel_combined->item(i)->text();
                if(groupSrc->find(txt.toStdString()) != groupSrc->end() || groupSat->find(txt.toStdString()) != groupSat->end() || groupSpace->find(txt.toStdString()) != groupSpace->end() ){
                    ++r;
                    continue;
                }
                if(txt == "__all__" || txt == "__AGNs__" || txt == "__satellites__" || txt == "__spacecrafts__" ){
                    ++r;
                    continue;
                }
                if(txt>name){
                    break;
                }else{
                    ++r;
                }
            }
            allSourcePlusGroupModel_combined->insertRow(r,new QStandardItem(QIcon(":/icons/icons/satellite.png"),name));
        }

    }
    ui->lineEdit_allSatelliteFilter->setFocus();
    ui->lineEdit_allSatelliteFilter->selectAll();
}


void MainWindow::on_treeView_allAvailabeSpacecrafts_clicked(const QModelIndex &index)
{
    int row = index.row();
    QString name = allSpacecraftProxyModel->index(row,0).data().toString();

    if(selectedSpacecraftModel->findItems(name).isEmpty()){

        selectedSpacecraftModel->insertRow(0);

        int nrow = allSpacecraftModel->findItems(name).at(0)->row();
        for(int i=0; i<allSpacecraftModel->columnCount(); ++i){
            selectedSpacecraftModel->setItem(0, i, allSpacecraftModel->item(nrow,i)->clone() );
        }

        selectedSpacecraftModel->sort(0);

        {
            int r = 0;
            for(int i = 0; i<allSpacecraftPlusGroupModel->rowCount(); ++i){
                QString txt = allSpacecraftPlusGroupModel->item(i)->text();
                if(groupSpace->find(txt.toStdString()) != groupSpace->end() || txt == "__all__"){
                    ++r;
                    continue;
                }
                if(txt>name){
                    break;
                }else{
                    ++r;
                }
            }
            allSpacecraftPlusGroupModel->insertRow(r,new QStandardItem(QIcon(":/icons/icons/spacecraft.png"),name));
        }

        {
            int r = 0;
            for(int i = 0; i<allSourcePlusGroupModel_combined->rowCount(); ++i){
                QString txt = allSourcePlusGroupModel_combined->item(i)->text();
                if(groupSrc->find(txt.toStdString()) != groupSrc->end() || groupSat->find(txt.toStdString()) != groupSat->end() || groupSpace->find(txt.toStdString()) != groupSpace->end() ){
                    ++r;
                    continue;
                }
                if(txt == "__all__" || txt == "__AGNs__" || txt == "__satellites__" || txt == "__spacecrafts__" ){
                    ++r;
                    continue;
                }
                if(txt>name){
                    break;
                }else{
                    ++r;
                }
            }
            allSourcePlusGroupModel_combined->insertRow(r,new QStandardItem(QIcon(":/icons/icons/spacecraft.png"),name));
        }

    }
    ui->lineEdit_allSpacecraftFilter->setFocus();
    ui->lineEdit_allSpacecraftFilter->selectAll();
}


//void MainWindow::on_lineEdit_allStationsFilter_3_textChanged(const QString &arg1)
//{
//    allSourceProxyModel->addFilterFixedString(arg1);
//}

void MainWindow::on_treeView_allAvailabeSources_entered(const QModelIndex &index)
{
    int row = index.row();
    QString name = allSourceProxyModel->index(row,0).data().toString();

    double ra = allSourceProxyModel->index(row,1).data().toDouble();
    ra -=180;
    double dc = allSourceProxyModel->index(row,2).data().toDouble();

    double lambda = qDegreesToRadians(ra);
    double phi = qDegreesToRadians(dc);
    double hn = qSqrt( 1 + qCos(phi)*qCos(lambda/2) );

    double x = (2 * qSqrt(2) *qCos(phi) *qSin(lambda/2) ) / hn;
    double y = (qSqrt(2) *qSin(phi) ) / hn;

    QString text = QString("%1 \nra: %2 [deg] \ndec: %3 [deg] ").arg(name).arg(ra+180).arg(dc);
    skyMapCallout->setText(text);
    skyMapCallout->setAnchor(QPointF(x,y));
    skyMapCallout->setZValue(11);
    skyMapCallout->updateGeometry();
    skyMapCallout->show();
}

void MainWindow::on_treeView_allSelectedSources_entered(const QModelIndex &index)
{

    int row = index.row();
    QString name = selectedSourceModel->index(row,0).data().toString();

    for(int i = 0; i < allSourceModel->rowCount(); ++i){
        QString newName = allSourceModel->index(i,0).data().toString();

        if (newName == name){
            double ra = allSourceModel->index(i,1).data().toDouble();
            ra -=180;
            double dc = allSourceModel->index(i,2).data().toDouble();

            double lambda = qDegreesToRadians(ra);
            double phi = qDegreesToRadians(dc);
            double hn = qSqrt( 1 + qCos(phi)*qCos(lambda/2) );

            double x = (2 * qSqrt(2) *qCos(phi) *qSin(lambda/2) ) / hn;
            double y = (qSqrt(2) *qSin(phi) ) / hn;


            QString text = QString("%1 \nra: %2 [deg] \ndec: %3 [deg] ").arg(name).arg(ra+180).arg(dc);
            skyMapCallout->setText(text);
            skyMapCallout->setAnchor(QPointF(x,y));
            skyMapCallout->setZValue(11);
            skyMapCallout->updateGeometry();
            skyMapCallout->show();
            break;
        }
    }
}

void MainWindow::plotSkyMap(){

    QChart *skyChart = skymap->chart();


    availableSources = new QScatterSeries(skyChart);
    availableSources->setName("available");
    availableSources->setColor(Qt::red);
    availableSources->setMarkerSize(10);

    selectedSources = new QScatterSeries(skyChart);
    selectedSources->setName("selected");
    markerSkymap();

    skyChart->addSeries(availableSources);
    skyChart->addSeries(selectedSources);

    connect(availableSources,SIGNAL(hovered(QPointF,bool)),this,SLOT(skymap_hovered(QPointF,bool)));
    connect(selectedSources,SIGNAL(hovered(QPointF,bool)),this,SLOT(skymap_hovered(QPointF,bool)));

    for(int i = 0; i< allSourceModel->rowCount(); ++i){
        double ra = allSourceModel->item(i,1)->text().toDouble();
        double lambda = qDegreesToRadians(ra);

        double dc = allSourceModel->item(i,2)->text().toDouble();
        double phi = qDegreesToRadians(dc);

        auto xy = qtUtil::radec2xy(lambda, phi);

        availableSources->append(xy.first, xy.second);
        selectedSources->append(xy.first, xy.second);
    }

    availableSources->attachAxis(skyChart->axisX());
    availableSources->attachAxis(skyChart->axisY());
    selectedSources->attachAxis(skyChart->axisX());
    selectedSources->attachAxis(skyChart->axisY());

}

void MainWindow::skymap_hovered(QPointF point, bool state){

    if (state) {

        double px = point.x();
        double py = point.y();


        double z = qSqrt(1 - (1./4.*px)*(1./4.*px) - (1./2.*py)*(1./2.*py));
        double pra = qRadiansToDegrees(2* qAtan( (z*px) / (2*(2*z*z-1)) ));
        pra +=180;
        double pde = qRadiansToDegrees(qAsin(z*py));

        QString src;
        for(int i = 0; i<allSourceModel->rowCount();++i){
            double ra = allSourceModel->index(i,1).data().toDouble();
            double de = allSourceModel->index(i,2).data().toDouble();

            auto dra = ra-pra;
            auto dde = de-pde;
            if(dra*dra+dde*dde < 10){
                if(src.size()==0){
                    src.append(allSourceModel->index(i,0).data().toString());
                }else{
                    src.append(","+allSourceModel->index(i,0).data().toString());
                }
            }
        }

        QString text = QString("%1 \nRA: %2 [deg] \nDC: %3 [deg] ").arg(src).arg(pra).arg(pde);
        skyMapCallout->setText(text);
        skyMapCallout->setAnchor(point);
        skyMapCallout->setZValue(11);
        skyMapCallout->updateGeometry();
        skyMapCallout->show();
    } else {
        skyMapCallout->hide();
    }

}

void MainWindow::addGroupSource()
{
    AddGroupDialog *dial = new AddGroupDialog(settings_,AddGroupDialog::Type::source,this);
    dial->addModel(selectedSourceModel, *groupSrc);
    int result = dial->exec();
    if(result == QDialog::Accepted){
        std::vector<std::string> stdlist = dial->getSelection();
        std::string stdname = dial->getGroupName();
        VieVS::ParameterGroup newGroup(stdname, stdlist);

        {
            int r = 0;
            for(int i = 0; i<allSourcePlusGroupModel->rowCount(); ++i){
                QString txt = allSourcePlusGroupModel->item(i)->text();
                if(txt == "__all__"){
                    ++r;
                    continue;
                }
                if(groupSrc->find(txt.toStdString()) == groupSrc->end()){
                    break;
                }
                if(txt>QString::fromStdString(stdname)){
                    break;
                }else{
                    ++r;
                }
            }
            (*groupSrc)[stdname] = stdlist;

            allSourcePlusGroupModel->insertRow(r,new QStandardItem(QIcon(":/icons/icons/source_group.png"),QString::fromStdString(stdname) ));
            if(sender() == sourceSetupWidget->addGroupButton()){
                sourceSetupWidget->memberComboBox()->setCurrentIndex(r);
            }
            if(sender() == ui->pushButton_addSourceGroup_Calibrator){
                ui->comboBox_calibratorBlock_calibratorSources->setCurrentIndex(r);
            }
        }
        {
            int r = 0;
            for(int i = 0; i<allSourcePlusGroupModel_combined->rowCount(); ++i){
                QString txt = allSourcePlusGroupModel_combined->item(i)->text();
                if(txt == "__all__" || txt == "__AGNs__" || txt == "__satellites__" || txt == "__spacecrafts__" ){
                    ++r;
                    continue;
                }
                if(groupSrc->find(txt.toStdString()) == groupSrc->end() && groupSat->find(txt.toStdString()) == groupSat->end() && groupSpace->find(txt.toStdString()) == groupSpace->end() ){
                    break;
                }
                if(txt>QString::fromStdString(stdname)){
                    break;
                }else{
                    ++r;
                }
            }
            allSourcePlusGroupModel_combined->insertRow(r,new QStandardItem(QIcon(":/icons/icons/source_group.png"),QString::fromStdString(stdname)));
        }

        QTreeWidgetItem *itm = new QTreeWidgetItem();
        itm->setText(0,QString::fromStdString(stdname));
        itm->setCheckState(0,Qt::Unchecked);
        ui->treeWidget_srcGroupForStatistics->addTopLevelItem(itm);
    }
    delete(dial);
}

void MainWindow::addGroupSatellite()
{
    AddGroupDialog *dial = new AddGroupDialog(settings_,AddGroupDialog::Type::satellite,this);
    dial->addModel(selectedSatelliteModel, *groupSat);
    int result = dial->exec();
    if(result == QDialog::Accepted){
        std::vector<std::string> stdlist = dial->getSelection();
        std::string stdname = dial->getGroupName();
        VieVS::ParameterGroup newGroup(stdname, stdlist);

        int r = 0;
        for(int i = 0; i<allSatellitePlusGroupModel->rowCount(); ++i){
            QString txt = allSatellitePlusGroupModel->item(i)->text();
            if(txt == "__all__"){
                ++r;
                continue;
            }
            if(groupSat->find(txt.toStdString()) == groupSat->end()){
                break;
            }
            if(txt>QString::fromStdString(stdname)){
                break;
            }else{
                ++r;
            }
        }

        (*groupSat)[stdname] = stdlist;

        allSatellitePlusGroupModel->insertRow(r,new QStandardItem(QIcon(":/icons/icons/satellite_group.png"),QString::fromStdString(stdname) ));
        if(sender() == satelliteSetupWidget->addGroupButton()){
            satelliteSetupWidget->memberComboBox()->setCurrentIndex(r);
        }
        QTreeWidgetItem *itm = new QTreeWidgetItem();
        itm->setText(0,QString::fromStdString(stdname));
        itm->setCheckState(0,Qt::Unchecked);
        ui->treeWidget_srcGroupForStatistics->addTopLevelItem(itm);
        {
            int r = 0;
            for(int i = 0; i<allSourcePlusGroupModel_combined->rowCount(); ++i){
                QString txt = allSourcePlusGroupModel_combined->item(i)->text();
                if(txt == "__all__" || txt == "__AGNs__" || txt == "__satellites__" || txt == "__spacecrafts__" ){
                    ++r;
                    continue;
                }
                if(groupSrc->find(txt.toStdString()) == groupSrc->end() && groupSat->find(txt.toStdString()) == groupSat->end() && groupSpace->find(txt.toStdString()) == groupSpace->end() ){
                    break;
                }
                if(txt>QString::fromStdString(stdname)){
                    break;
                }else{
                    ++r;
                }
            }
            allSourcePlusGroupModel_combined->insertRow(r,new QStandardItem(QIcon(":/icons/icons/satellite_group.png"),QString::fromStdString(stdname)));
        }
    }
    delete(dial);
}

void MainWindow::addGroupSpacecraft()
{
    AddGroupDialog *dial = new AddGroupDialog(settings_,AddGroupDialog::Type::spacecraft,this);
    dial->addModel(selectedSpacecraftModel, *groupSrc);
    int result = dial->exec();
    if(result == QDialog::Accepted){
        std::vector<std::string> stdlist = dial->getSelection();
        std::string stdname = dial->getGroupName();
        VieVS::ParameterGroup newGroup(stdname, stdlist);

        {
            int r = 0;
            for(int i = 0; i<allSpacecraftPlusGroupModel->rowCount(); ++i){
                QString txt = allSpacecraftPlusGroupModel->item(i)->text();
                if(txt == "__all__"){
                    ++r;
                    continue;
                }
                if(groupSpace->find(txt.toStdString()) == groupSpace->end()){
                    break;
                }
                if(txt>QString::fromStdString(stdname)){
                    break;
                }else{
                    ++r;
                }
            }

            (*groupSpace)[stdname] = stdlist;

            allSpacecraftPlusGroupModel->insertRow(r,new QStandardItem(QIcon(":/icons/icons/spacecraft_group.png"),QString::fromStdString(stdname) ));
            if(sender() == spacecraftSetupWidget->addGroupButton()){
                spacecraftSetupWidget->memberComboBox()->setCurrentIndex(r);
            }
        }
        QTreeWidgetItem *itm = new QTreeWidgetItem();
        itm->setText(0,QString::fromStdString(stdname));
        itm->setCheckState(0,Qt::Unchecked);
        ui->treeWidget_srcGroupForStatistics->addTopLevelItem(itm);
        {
            int r = 0;
            for(int i = 0; i<allSourcePlusGroupModel_combined->rowCount(); ++i){
                QString txt = allSourcePlusGroupModel_combined->item(i)->text();
                if(txt == "__all__" || txt == "__AGNs__" || txt == "__satellites__" || txt == "__spacecrafts__" ){
                    ++r;
                    continue;
                }
                if(groupSrc->find(txt.toStdString()) == groupSrc->end() && groupSat->find(txt.toStdString()) == groupSat->end() && groupSpace->find(txt.toStdString()) == groupSpace->end() ){
                    break;
                }
                if(txt>QString::fromStdString(stdname)){
                    break;
                }else{
                    ++r;
                }
            }
            allSourcePlusGroupModel_combined->insertRow(r,new QStandardItem(QIcon(":/icons/icons/spacecraft_group.png"),QString::fromStdString(stdname)));
        }
    }
    delete(dial);
}

void MainWindow::on_pushButton_13_clicked()
{
    ui->lineEdit_allSourceFilter->setText("");
    sourceSetupWidget->blockSignal(true);
    selectedSourceModel->blockSignals(true);

    for(int i=0; i<allSourceModel->rowCount()-1; ++i){
        on_treeView_allAvailabeSources_clicked(allSourceModel->index(i,0));
    }

    selectedSourceModel->blockSignals(false);
    sourceSetupWidget->blockSignal(false);
    on_treeView_allAvailabeSources_clicked(allSourceModel->index(allSourceModel->rowCount()-1,0));
    sourceSetupWidget->setupComboBox()->setCurrentIndex(0);
    sourceListChanged();
    auto *tmp2 = ui->tabWidget_simAna->findChild<QWidget *>("Solver_Widged");
    SolverWidget *solver = qobject_cast<SolverWidget *>(tmp2);
    solver->addSources();

}

void MainWindow::on_pushButton_15_clicked()
{
    selectedSourceModel->removeRows(0, selectedSourceModel->rowCount());
    selectedSources->clear();
    {
        int i = 0;
        while(i<allSourcePlusGroupModel->rowCount()){
            QString name = allSourcePlusGroupModel->item(i)->text();
            if(name != "__all__" && groupSrc->find(name.toStdString()) == groupSrc->end()){
                allSourcePlusGroupModel->removeRow(i);
            }else{
                ++i;
            }
        }
    }
    {
        int i = 0;
        while(i<allSourcePlusGroupModel_combined->rowCount()){
            QString name = allSourcePlusGroupModel_combined->item(i)->text();
            if(groupSrc->find(name.toStdString()) != groupSrc->end() || groupSat->find(name.toStdString()) != groupSat->end() || groupSpace->find(name.toStdString()) != groupSpace->end()){
                allSourcePlusGroupModel_combined->removeRow(i);
            }else if(name == "__all__" || name == "__AGNs__" || name == "__satellites__" || name == "__spacecrafts__" ){
                ++i;
            }else{
                ++i;
            }
        }
    }
    auto *tmp_ms = ui->groupBox_multiScheduling->findChild<QWidget *>("MultiScheduling_Widged");
    MulitSchedulingWidget *ms = qobject_cast<MulitSchedulingWidget *>(tmp_ms);
    ms->clear();
    ui->label_sourceList_selected->setText("selected: ");
}

void MainWindow::on_pushButton_saveSourceList_clicked()
{
    QVector<QString> selSrc;
    for(int i = 0; i<selectedSourceModel->rowCount(); ++i){
        selSrc.append(selectedSourceModel->item(i)->text());
    }
    saveToSettingsDialog *dial = new saveToSettingsDialog(settings_,this);
    dial->setType(saveToSettingsDialog::Type::sourceNetwork);
    dial->setNetwork(selSrc);

    dial->exec();
}

void MainWindow::on_pushButton_loadSourceList_clicked()
{
    auto network= settings_.get_child_optional("settings.source_lists");
    if(!network.is_initialized()){
        QMessageBox::warning(this,"No source list found!","There was no source list saved in settings.xml file\nCheck settings.source_list");
        return;
    }

    QVector<QString> names;
    QVector<QVector<QString> > source_lists;

    for(const auto &it:*network){
        QString name = QString::fromStdString(it.second.get_child("<xmlattr>.name").data());
        QVector<QString> source_list;

        for(const auto &it2:it.second){
            if(it2.first == "member"){
                source_list.push_back(QString::fromStdString(it2.second.data()));
            }
        }

        names.push_back(name);
        source_lists.push_back(source_list);
    }
    settingsLoadWindow *dial = new settingsLoadWindow(this);

    dial->setSourceList(names,source_lists);

    int result = dial->exec();
    if(result == QDialog::Accepted){
        QString warningTxt;

        QString itm = dial->selectedItem();
        int idx = dial->selectedIdx();
        QVector<QString> members = source_lists.at(idx);

        on_pushButton_15_clicked();
        for(const auto&any:members){
            auto list = allSourceModel->findItems(any);
            if(list.size() == 1){
                for(int i = 0; i<allSourceProxyModel->rowCount(); ++i){
                    QModelIndex idx = allSourceProxyModel->index(i,0);
                    if(idx.data().toString() == any){
                        ui->treeView_allAvailabeSources->clicked(idx);
                    }
                }
//                selectedSourceModel->appendRow(new QStandardItem(QIcon(":icons/icons/source.png"),list.at(0)->text()));
            }else{
                warningTxt.append("    unknown station: ").append(any).append("!\n");
            }
        }
        if(!warningTxt.isEmpty()){
            QString txt = "The following errors occurred while loading the source list:\n";
            txt.append(warningTxt).append("These sources were ignored!\nPlease double check sources again!");
            QMessageBox::warning(this,"Unknown source list source!",txt);
        }
    }
}

void MainWindow::sourceListChanged()
{
    int size = selectedSourceModel->rowCount();
    ui->label_sourceList_selected->setText(QString("selected: %1").arg(size));
}

void MainWindow::satelliteListChanged()
{
    int size = selectedSatelliteModel->rowCount();
    ui->label_satelliteList_selected->setText(QString("selected: %1").arg(size));
}

void MainWindow::spacecraftListChanged()
{
    int size = selectedSpacecraftModel->rowCount();
    ui->label_spacecraftList_selected->setText(QString("selected: %1").arg(size));
}

void MainWindow::markerSkymap()
{
    if(ui->radioButton_markerSkymap->isChecked()){
        selectedSources->setMarkerSize(ui->horizontalSlider_markerSkymap->value());
        selectedSources->setBrush(QBrush(Qt::darkGreen,Qt::SolidPattern));
        selectedSources->setPen(QColor(Qt::white));
    }else{
        QImage img(":/icons/icons/source_white.png");
        img = img.scaled(ui->horizontalSlider_markerSkymap->value(),ui->horizontalSlider_markerSkymap->value());
        selectedSources->setBrush(QBrush(img));
        selectedSources->setMarkerSize(ui->horizontalSlider_markerSkymap->value());
        selectedSources->setPen(QColor(Qt::transparent));
    }

}

void MainWindow::on_radioButton_imageSkymap_toggled(bool checked)
{
    if(checked){
        ui->horizontalSlider_markerSkymap->setValue(30);
        auto series = skymap->chart()->series();
        QScatterSeries *tmp;
        for(const auto&serie : series){
            if(serie->name() == "selected"){
                tmp = qobject_cast<QScatterSeries *>(serie);
            }
        }
        tmp->setPen(QPen(QBrush(Qt::darkGreen),3,Qt::DashLine));

    }else{
        ui->horizontalSlider_markerSkymap->setValue(15);
        auto series = skymap->chart()->series();
        QScatterSeries *tmp;
        for(const auto&serie : series){
            if(serie->name() == "selected"){
                tmp = qobject_cast<QScatterSeries *>(serie);
            }
        }
        tmp->setPen(QPen(QBrush(Qt::blue),3,Qt::DashLine));
        selectedSources->setPen(QColor(Qt::transparent));
    }
    markerSkymap();
}

void MainWindow::on_checkBox_showEcliptic_clicked(bool checked)
{
    if(checked){
        auto series = skymap->chart()->series();
        series.back()->setVisible(true);
    }else{
        auto series = skymap->chart()->series();
        series.back()->setVisible(false);
    }
}


// ########################################### WEIGHT FACTORS ###########################################

void MainWindow::on_doubleSpinBox_weightLowDecStart_valueChanged(double arg1)
{
    if(ui->doubleSpinBox_weightLowDecEnd->value() > arg1){
        ui->doubleSpinBox_weightLowDecEnd->setValue(arg1);
    }
}

void MainWindow::on_doubleSpinBox_weightLowDecEnd_valueChanged(double arg1)
{
    if(ui->doubleSpinBox_weightLowDecStart->value() < arg1){
        ui->doubleSpinBox_weightLowDecStart->setValue(arg1);
    }
}

void MainWindow::on_doubleSpinBox_weightLowElStart_valueChanged(double arg1)
{
    if(ui->doubleSpinBox_weightLowElEnd->value() > arg1){
        ui->doubleSpinBox_weightLowElEnd->setValue(arg1);
    }
}

void MainWindow::on_doubleSpinBox_weightLowElEnd_valueChanged(double arg1)
{
    if(ui->doubleSpinBox_weightLowElStart->value() < arg1){
        ui->doubleSpinBox_weightLowElStart->setValue(arg1);
    }
}

// ########################################### RULES ###########################################

void MainWindow::on_spinBox_scanSequenceCadence_valueChanged(int arg1)
{
    if(ui->tableWidget_scanSequence->rowCount()>arg1){
        while(ui->tableWidget_scanSequence->rowCount()>arg1){
            ui->tableWidget_scanSequence->removeRow(ui->tableWidget_scanSequence->rowCount()-1);
        }
    }else{
        while(ui->tableWidget_scanSequence->rowCount()<arg1){
            ui->tableWidget_scanSequence->insertRow(ui->tableWidget_scanSequence->rowCount());
            QComboBox *cBox = new QComboBox(this);
            cBox->setModel(allSourcePlusGroupModel_combined);
            ui->tableWidget_scanSequence->setCellWidget(ui->tableWidget_scanSequence->rowCount()-1,0, cBox);
        }
    }
}

void MainWindow::on_doubleSpinBox_calibratorLowElStart_valueChanged(double arg1)
{
    if(ui->doubleSpinBox_calibratorLowElEnd->value() > arg1){
        ui->doubleSpinBox_calibratorLowElEnd->setValue(arg1);
    }
}

void MainWindow::on_doubleSpinBox_calibratorLowElEnd_valueChanged(double arg1)
{
    if(ui->doubleSpinBox_calibratorLowElStart->value() < arg1){
        ui->doubleSpinBox_calibratorLowElStart->setValue(arg1);
    }
}

void MainWindow::on_doubleSpinBox_calibratorHighElStart_valueChanged(double arg1)
{
    if(ui->doubleSpinBox_calibratorHighElEnd->value() < arg1){
        ui->doubleSpinBox_calibratorHighElEnd->setValue(arg1);
    }
}

void MainWindow::on_doubleSpinBox_calibratorHighElEnd_valueChanged(double arg1)
{
    if(ui->doubleSpinBox_calibratorHighElStart->value() > arg1){
        ui->doubleSpinBox_calibratorHighElStart->setValue(arg1);
    }
}


// ########################################### GUI UTILITY ###########################################

void MainWindow::splitterMoved() {
  QSplitter* senderSplitter = static_cast<QSplitter*>(sender());



  QSplitter* receiverSplitter1;
  QSplitter* receiverSplitter2;
  QSplitter* receiverSplitter3;
  QSplitter* receiverSplitter4;

  if(senderSplitter == stationSetupWidget->horizontal_splitter()){
      receiverSplitter1 = sourceSetupWidget->horizontal_splitter();
      receiverSplitter2 = baselineSetupWidget->horizontal_splitter();
      receiverSplitter3 = satelliteSetupWidget->horizontal_splitter();
      receiverSplitter4 = spacecraftSetupWidget->horizontal_splitter();

  }else if(senderSplitter == sourceSetupWidget->horizontal_splitter()){
      receiverSplitter1 = stationSetupWidget->horizontal_splitter();
      receiverSplitter2 = baselineSetupWidget->horizontal_splitter();
      receiverSplitter3 = satelliteSetupWidget->horizontal_splitter();
      receiverSplitter4 = spacecraftSetupWidget->horizontal_splitter();

  }else if(senderSplitter == baselineSetupWidget->horizontal_splitter()){
      receiverSplitter1 = stationSetupWidget->horizontal_splitter();
      receiverSplitter2 = sourceSetupWidget->horizontal_splitter();
      receiverSplitter3 = satelliteSetupWidget->horizontal_splitter();
      receiverSplitter4 = spacecraftSetupWidget->horizontal_splitter();

  }else if(senderSplitter == satelliteSetupWidget->horizontal_splitter()){
      receiverSplitter1 = stationSetupWidget->horizontal_splitter();
      receiverSplitter2 = sourceSetupWidget->horizontal_splitter();
      receiverSplitter3 = baselineSetupWidget->horizontal_splitter();
      receiverSplitter4 = spacecraftSetupWidget->horizontal_splitter();

  }else if(senderSplitter == spacecraftSetupWidget->horizontal_splitter()){
      receiverSplitter1 = stationSetupWidget->horizontal_splitter();
      receiverSplitter2 = sourceSetupWidget->horizontal_splitter();
      receiverSplitter3 = baselineSetupWidget->horizontal_splitter();
      receiverSplitter4 = satelliteSetupWidget->horizontal_splitter();

  }else if(senderSplitter == stationSetupWidget->vertical_splitter()){
      receiverSplitter1 = sourceSetupWidget->vertical_splitter();
      receiverSplitter2 = baselineSetupWidget->vertical_splitter();
      receiverSplitter3 = satelliteSetupWidget->vertical_splitter();
      receiverSplitter4 = spacecraftSetupWidget->vertical_splitter();

  }else if(senderSplitter == sourceSetupWidget->vertical_splitter()){
      receiverSplitter1 = stationSetupWidget->vertical_splitter();
      receiverSplitter2 = baselineSetupWidget->vertical_splitter();
      receiverSplitter3 = satelliteSetupWidget->vertical_splitter();
      receiverSplitter4 = spacecraftSetupWidget->vertical_splitter();

  }else if(senderSplitter == baselineSetupWidget->vertical_splitter()){
      receiverSplitter1 = stationSetupWidget->vertical_splitter();
      receiverSplitter2 = sourceSetupWidget->vertical_splitter();
      receiverSplitter3 = satelliteSetupWidget->vertical_splitter();
      receiverSplitter4 = spacecraftSetupWidget->vertical_splitter();

  }else if(senderSplitter == satelliteSetupWidget->vertical_splitter()){
      receiverSplitter1 = stationSetupWidget->vertical_splitter();
      receiverSplitter2 = sourceSetupWidget->vertical_splitter();
      receiverSplitter3 = baselineSetupWidget->vertical_splitter();
      receiverSplitter4 = spacecraftSetupWidget->vertical_splitter();

  }else if(senderSplitter == spacecraftSetupWidget->vertical_splitter()){
      receiverSplitter1 = stationSetupWidget->vertical_splitter();
      receiverSplitter2 = sourceSetupWidget->vertical_splitter();
      receiverSplitter3 = baselineSetupWidget->vertical_splitter();
      receiverSplitter4 = satelliteSetupWidget->vertical_splitter();

  }

  receiverSplitter1->blockSignals(true);
  receiverSplitter1->setSizes(senderSplitter->sizes());
  receiverSplitter1->blockSignals(false);

  receiverSplitter2->blockSignals(true);
  receiverSplitter2->setSizes(senderSplitter->sizes());
  receiverSplitter2->blockSignals(false);

  receiverSplitter3->blockSignals(true);
  receiverSplitter3->setSizes(senderSplitter->sizes());
  receiverSplitter3->blockSignals(false);

  receiverSplitter4->blockSignals(true);
  receiverSplitter4->setSizes(senderSplitter->sizes());
  receiverSplitter4->blockSignals(false);
}

void MainWindow::faqSearch()
{
    QString searchString = ui->lineEdit_faqSearch->text();
    QTextDocument *document = ui->textEdit_faq->document();
    searchString = searchString.simplified();

    bool found = false;

    document->undo();

    if (searchString.isEmpty()) {

    } else {
        QTextCursor highlightCursor(document);
        QTextCursor cursor(document);

        cursor.beginEditBlock();

        QTextCharFormat plainFormat(highlightCursor.charFormat());
        QTextCharFormat colorFormat = plainFormat;
        colorFormat.setForeground(Qt::red);

        while (!highlightCursor.isNull() && !highlightCursor.atEnd()) {
            highlightCursor = document->find(searchString, highlightCursor);

            if (!highlightCursor.isNull()) {
                found = true;
                highlightCursor.mergeCharFormat(colorFormat);
            }
        }

        cursor.endEditBlock();

        if (found == false) {
            QMessageBox::information(this, tr("Not Found"),
                                     tr("Search text is not found on FAQ page.\nTry another keyword or write an email to matthias.schartner@geo.tuwien.ac.at"));
        }
    }

}

void MainWindow::on_checkBox_outputNGSFile_stateChanged(int arg1)
{
    if(arg1 == 0){
        ui->checkBox_redirectNGS->setEnabled(false);
        ui->lineEdit_outputNGS->setEnabled(false);
    }else{
        ui->checkBox_redirectNGS->setEnabled(true);
        if(ui->checkBox_redirectNGS->isChecked()){
            ui->lineEdit_outputNGS->setEnabled(true);
        }
    }
}

// ########################################### SKY COVERAGE ###########################################

void MainWindow::setupSkyCoverageTemplatePlot()
{
    plotSkyCoverageTemplate = true;
    skyCoverageTemplateView = new QChartView(this);
    skyCoverageTemplateView->setStatusTip("sky-coverage example");
    skyCoverageTemplateView->setToolTip("sky-coverage example");

    QPolarChart *chart = new QPolarChart();

    QValueAxis *angularAxis = new QValueAxis();
    angularAxis->setTickCount(13);
    angularAxis->setLabelFormat("%d");
    angularAxis->setShadesVisible(true);
    angularAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
    chart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);
    angularAxis->setRange(0, 361);


    QValueAxis *radialAxis = new QValueAxis();
    radialAxis->setTickCount(10);
    radialAxis->setLabelFormat(" ");
    chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);
    radialAxis->setRange(0, 90);


    chart->legend()->setVisible(false);
    chart->setTitle("possible sky-coverage example");
    skyCoverageTemplateView->setChart(chart);
    skyCoverageTemplateView->setRenderHint(QPainter::Antialiasing);
    ui->verticalLayout_54->insertWidget(0,skyCoverageTemplateView,2);

    connect(ui->spinBox_skyCoverageTemplateRandomObservations,SIGNAL(valueChanged(int)),this,SLOT(on_pushButton_skyCoverageTemplateRandom_clicked()));

    connect(ui->influenceDistanceDoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(skyCoverageTemplate()));
    connect(ui->horizontalSlider_skyCoverageMarkerSize,SIGNAL(valueChanged(int)),this,SLOT(skyCoverageTemplate()));
    connect(ui->horizontalSlider_skyCoverageMarkerDistance,SIGNAL(valueChanged(int)),this,SLOT(skyCoverageTemplate()));
    connect(ui->horizontalSlider_skyCoverageColorResultion,SIGNAL(valueChanged(int)),this,SLOT(skyCoverageTemplate()));

    connect(ui->comboBox_skyCoverageDistanceType,SIGNAL(currentIndexChanged(int)),this,SLOT(skyCoverageTemplate()));
    connect(ui->comboBox_skyCoverageTimeType,SIGNAL(currentIndexChanged(int)),this,SLOT(skyCoverageTemplate()));
    on_pushButton_skyCoverageTemplateRandom_clicked();

}

void MainWindow::skyCoverageTemplate()
{
    if(plotSkyCoverageTemplate){
        auto chart = skyCoverageTemplateView->chart();
        chart->removeAllSeries();

        QLineSeries *upper = new QLineSeries();
        QLineSeries *lower = new QLineSeries();
        double minElevation = 5;
        for(int az=0; az<=365; az+=5){
            upper->append(az,90);
            lower->append(az,90-minElevation);
        }

        QAreaSeries *area = new QAreaSeries();
        area->setName("minimum elevation");
        area->setUpperSeries(upper);
        area->setLowerSeries(lower);
        area->setBrush(QBrush(Qt::gray));
        area->setOpacity(0.7);

        chart->addSeries(area);

        area->attachAxis(chart->axisX());
        area->attachAxis(chart->axisY());

        QVector<double> V{0.00,0.05,0.10,0.15,0.20,0.25,0.30,0.35,0.40,0.45,0.50,0.55,0.60,0.65,0.70,0.75,0.80,0.85,0.90,0.95,1.00};
        QVector<double> R{62,68,71,72,67,52,45,37,28,4,18,48,72,113,159,200,234,254,250,245,249};
        QVector<double> G{38,51,67,85,103,122,140,156,170,182,190,197,203,205,201,193,186,193,212,232,251};
        QVector<double> B{168,204,231,246,253,253,243,231,223,206,185,162,134,100,66,41,48,58,46,37,21};
        int nColor = ui->horizontalSlider_skyCoverageColorResultion->value();
        double dist = 1.0/(nColor-1);

        QVector<double> Rq;
        QVector<double> Gq;
        QVector<double> Bq;
        for(int i=0; i<nColor; ++i){
            double vq = i*dist;
            Rq.append(interpolate(V,R,vq,false));
            Gq.append(interpolate(V,G,vq,false));
            Bq.append(interpolate(V,B,vq,false));
        }


        QVector<QScatterSeries*> ss;
        for(int i=0;i<nColor;++i){
            QScatterSeries *tss = new QScatterSeries();
            tss->setMarkerShape(QScatterSeries::MarkerShapeCircle);
            tss->setMarkerSize(ui->horizontalSlider_skyCoverageMarkerSize->value());
            tss->setBrush(QBrush(QColor(Rq.at(i),Gq.at(i),Bq.at(i))));
            tss->setBorderColor(QColor(Rq.at(i),Gq.at(i),Bq.at(i)));
            ss.append(tss);
        }

        double d = (double)ui->horizontalSlider_skyCoverageMarkerDistance->value()/10;
        for (double el = 0; el <= 90; el+=d) {
            if(el<= minElevation){
                continue;
            }
            double zd = 90-el;

            double deltaAz;
            if(el!=90){
                deltaAz = d/qCos(qDegreesToRadians(el));
            }else{
                deltaAz = 360;
            }

            for (double az = 0; az < 360; az+=deltaAz) {

                double score = 1;
                for(int i=0; i<obsTime.count(); ++i){
                    int deltaTime = obsTime.at(0)-obsTime.at(i);

                    double el1 = qDegreesToRadians(el);
                    double el2 = qDegreesToRadians(obsEl.at(i));
                    double az1 = qDegreesToRadians(az);
                    double az2 = qDegreesToRadians(obsAz.at(i));

                    double tmp = (qSin(el1) * qSin(el2) + qCos(el1) * qCos(el2) * qCos(az2-az1));
                    double deltaDistance = qRadiansToDegrees(qAcos(tmp));
                    double scoreDistance;
                    double scoreTime;

                    if(deltaDistance >= ui->influenceDistanceDoubleSpinBox->value()){
                        scoreDistance = 0;
                    }else if(ui->comboBox_skyCoverageDistanceType->currentText() == "cosine"){
                        scoreDistance = .5+.5*qCos(deltaDistance * M_PI / ui->influenceDistanceDoubleSpinBox->value());
                    }else if(ui->comboBox_skyCoverageDistanceType->currentText() == "linear"){
                        scoreDistance = 1-deltaDistance/ui->influenceDistanceDoubleSpinBox->value();
                    }else{
                        scoreDistance = 1;
                    }

                    if(deltaTime >= ui->influenceTimeSpinBox->value()){
                        scoreTime = 0;
                    }else if(ui->comboBox_skyCoverageTimeType->currentText() == "cosine"){
                        scoreTime = .5+.5*qCos(deltaTime * M_PI / ui->influenceTimeSpinBox->value());
                    }else if(ui->comboBox_skyCoverageTimeType->currentText() == "linear"){
                        scoreTime = 1-(double)deltaTime/(double)ui->influenceTimeSpinBox->value();
                    }else{
                        scoreTime = 1;
                    }

                    double thisScore = 1-(scoreDistance*scoreTime);
                    if(thisScore<score){
                        score=thisScore;
                    }
                }

                int idx = score*(nColor-1);
                ss.at(idx)->append(az,zd);
            }
        }

        for(int i=nColor-1; i>=0; --i){
            chart->addSeries(ss.at(i));
            ss.at(i)->attachAxis(chart->axisX());
            ss.at(i)->attachAxis(chart->axisY());
        }

        QScatterSeries *obs = new QScatterSeries();
        obs->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        obs->setMarkerSize(12);
        obs->setBrush(QBrush(Qt::red));
        obs->setBorderColor(Qt::red);
        for(int i=0;i<obsTime.count();++i){
            obs->append(obsAz.at(i),90-obsEl.at(i));
        }

        chart->addSeries(obs);
        obs->attachAxis(chart->axisX());
        obs->attachAxis(chart->axisY());
    }
}

void MainWindow::on_pushButton_skyCoverageTemplateRandom_clicked()
{
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
    obsAz.clear();
    obsEl.clear();
    obsTime.clear();
    double minElevation = 5;
    int nobs = ui->spinBox_skyCoverageTemplateRandomObservations->value() * (double)ui->influenceTimeSpinBox->value()/3600.;

    for(int i=0; i<nobs; ++i){
        obsAz.append(qrand() % ((360 + 1) - 0) + 0);

        double thisEl;
        double rn = (double)(qrand() % ((100 + 1) - 0) + 0);

        if(rn<58){
            thisEl = qrand() % ((40 + 1) - (int)minElevation) + (int)minElevation;
        }else{
            double u = (double)(qrand() % ((1000 + 1) - 0) + 0);
            u = u/1000;
            thisEl = 90-qSqrt((1-u)*(90-40)*(90-40));
        }
        obsEl.append(thisEl);
        obsTime.append((nobs-i)*ui->influenceTimeSpinBox->value()/nobs);
    }
    skyCoverageTemplate();
}

void MainWindow::on_influenceTimeSpinBox_valueChanged(int arg1)
{
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
    obsTime.clear();
    double minElevation = 5;
    int nobs = ui->spinBox_skyCoverageTemplateRandomObservations->value() * (double)ui->influenceTimeSpinBox->value()/3600.;

    if(nobs>obsAz.count()){
        for(int i=obsAz.count(); i<nobs; ++i){
            obsAz.append(qrand() % ((360 + 1) - 0) + 0);
            obsEl.append(qrand() % ((90 + 1) - (int)minElevation) + (int)minElevation);
        }
    }else{
        for(int i=nobs; i<obsAz.count(); ++i){
            obsAz.remove(obsAz.count()-1);
            obsEl.remove(obsEl.count()-1);
        }
    }

    for(int i=0; i<nobs; ++i){
        obsTime.append((nobs-i)*ui->influenceTimeSpinBox->value()/nobs);
    }
    skyCoverageTemplate();

}

double MainWindow::interpolate( QVector<double> &xData, QVector<double> &yData, double x, bool extrapolate )
{
   int size = xData.size();

   int i = 0;
   if ( x >= xData[size - 2] )
   {
      i = size - 2;
   }
   else
   {
      while ( x > xData[i+1] ) i++;
   }
   double xL = xData[i], yL = yData[i], xR = xData[i+1], yR = yData[i+1];
   if ( !extrapolate )
   {
      if ( x < xL ) yR = yL;
      if ( x > xR ) yL = yR;
   }

   double dydx = ( yR - yL ) / ( xR - xL );
   return yL + dydx * ( x - xL );
}

// ########################################### CONDITIONS ###########################################

void MainWindow::on_pushButton_addCondition_clicked()
{
    QString members = ui->comboBox_conditions_members->currentText();
    bool isGroup = groupSrc->find(members.toStdString() ) != groupSrc->end();

    int scans = ui->spinBox_condtionsMinNumScans->value();
    int bls = ui->spinBox_conditionsMinNumBaselines->value();

    QIcon ic;
    QTreeWidgetItem *c = new QTreeWidgetItem();
    if(isGroup || members == "__all__"){
        ic = QIcon(":/icons/icons/source_group.png");
    }else{
        ic = QIcon(":/icons/icons/source.png");
    }

    c->setText(0,QString("%1").arg(ui->treeWidget_conditions->topLevelItemCount()));
    c->setIcon(1,ic);
    c->setText(1,members);
    c->setText(2,QString("%1").arg(scans));
    c->setText(3,QString("%1").arg(bls));

    ui->treeWidget_conditions->addTopLevelItem(c);

}

void MainWindow::on_pushButton_removeCondition_clicked()
{
    auto list = ui->treeWidget_conditions->selectedItems();
    if(!list.empty()){
        auto itm = list.at(0);
        delete itm;
        for(int i=0; i<ui->treeWidget_conditions->topLevelItemCount(); ++i){
            auto titm = ui->treeWidget_conditions->topLevelItem(i);
            titm->setText(0,QString("%1").arg(i));
        }
    }
}

void MainWindow::on_spinBox_maxNumberOfIterations_valueChanged(int arg1)
{
    ui->spinBox_gentleSourceReduction->setMaximum(arg1);
}

// ########################################### HIGH IMPACT ###########################################

void MainWindow::on_pushButton_addHighImpactAzEl_clicked()
{
    QString members = ui->comboBox_highImpactStation->currentText();
    bool isGroup = groupSta->find(members.toStdString() ) != groupSrc->end();

    double az = ui->doubleSpinBox_highImpactAzimuth->value();
    double el = ui->doubleSpinBox_highImpactElevation->value();
    double margin = ui->doubleSpinBox_highImpactMargin->value();

    QIcon ic;
    QTreeWidgetItem *c = new QTreeWidgetItem();
    if(isGroup || members == "__all__"){
        ic = QIcon(":/icons/icons/station_group.png");
    }else{
        ic = QIcon(":/icons/icons/station.png");
    }

    c->setIcon(0,ic);
    c->setText(0,members);
    c->setText(1,QString("%1").arg(az));
    c->setText(2,QString("%1").arg(el));
    c->setText(3,QString("%1").arg(margin));

    ui->treeWidget_highImpactAzEl->addTopLevelItem(c);

}

void MainWindow::on_pushButton_removeHighImpactAzEl_clicked()
{
    auto list = ui->treeWidget_highImpactAzEl->selectedItems();
    if(!list.empty()){
        auto itm = list.at(0);
        delete itm;
    }
}

// ########################################### AUXILIARY FILES ###########################################

void MainWindow::on_pushButton_readLogFile_read_clicked()
{
    QString path = ui->lineEdit_logFilePath->text().trimmed();
    textfileViewer* myViewer = new textfileViewer(this);
    myViewer->setTextFile(path, textfileViewer::Type::log);
    myViewer->show();
}


void MainWindow::on_pushButton_readSkdFile_read_clicked()
{
    QString path = ui->lineEdit_skdFilePath->text().trimmed();
    textfileViewer* myViewer = new textfileViewer(this);
    if(path.right(3) == "skd"){
        myViewer->setTextFile(path, textfileViewer::Type::skd);
    }else if(path.right(3) == "vex"){
        myViewer->setTextFile(path, textfileViewer::Type::vex);
    }else{
        myViewer->setTextFile(path, textfileViewer::Type::undefined);
    }
    myViewer->show();
}


void MainWindow::on_pushButton_sessionBrowse_clicked()
{
    QString startPath = ui->lineEdit_sessionPath->text();
    QString path = QFileDialog::getOpenFileName(this, "Browse to skd file", startPath, tr("skd files (*.skd)"));
    if( !path.isEmpty() ){
        ui->lineEdit_sessionPath->setText(path);
        ui->lineEdit_sessionPath->setFocus();
        ui->pushButton_parse->click();
    }
}

void MainWindow::on_pushButton_parse_clicked()
{
    QString path = ui->lineEdit_sessionPath->text();
    if(path.length()>4){
        if(path.right(4) == ".skd"){
            try{
                VieVS::SkdParser mySkdParser(path.toStdString());
                mySkdParser.read();
                parsedSchedule = mySkdParser.createScheduler();
                parsedFreq = mySkdParser.getFrequencies();
                std::string start = VieVS::TimeSystem::time2string(VieVS::TimeSystem::startTime);
                std::string end = VieVS::TimeSystem::time2string(VieVS::TimeSystem::endTime);
                QDateTime qstart = QDateTime::fromString(QString::fromStdString(start),"yyyy.MM.dd HH:mm:ss");
                QDateTime qend   = QDateTime::fromString(QString::fromStdString(end),"yyyy.MM.dd HH:mm:ss");

                ui->lineEdit_parseExpName->setText(QString::fromStdString(parsedSchedule->getName()));
                ui->dateTimeEdit_parseSessionStart->setDateTime(qstart);
                ui->dateTimeEdit_parseSessionEnd->setDateTime(qend);
                ui->spinBox_parseStations->setValue(parsedSchedule->getNetwork().getStations().size());
                ui->spinBox_parseSources->setValue(parsedSchedule->getSourceList().getNSrc());
                ui->spinBox_parseScans->setValue(parsedSchedule->getScans().size());
                ui->spinBox_parseObs->setValue(parsedSchedule->getNumberOfObservations());

                const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
                const auto &sources = parsedSchedule->getSourceList().getSources();
                const auto &network = parsedSchedule->getNetwork();
                QString txt = " Source      Start      DURATIONS           \n";
                txt .append(" name     yyddd-hhmmss   ") ;
                for (const auto &any : network.getStations()){
                    txt.append(QString::fromStdString(any.getAlternativeName())).append("  ");
                }
                txt.append("\n");

                for(const auto &any : parsedSchedule->getScans()){
                    txt.append(QString::fromStdString(any.toSkedOutputTimes(sources[any.getSourceId()], network.getNSta())));
                }
                ui->plainTextEdit_parseOutput->setFont(fixedFont);
                ui->plainTextEdit_parseOutput->setPlainText(txt);

                const auto &obsMode = parsedSchedule->getObservingMode();
                const auto &mode = obsMode->getMode(0);
                const auto &bands = mode->getAllBands();
                double recRate = 0;
                double effRate = 0;
                for(const auto &band : bands){
                    recRate += mode->recordingRate(0,1,band);
                    effRate = mode->efficiency(0,1);
                }
                ui->doubleSpinBox_parseRecRate->setValue(recRate/1e6);
                ui->doubleSpinBox_parseEff->setValue(effRate);

                ui->groupBox_parsedSchedule->setEnabled(true);

            }catch(...){
                QString message = QString("Error reading session:\n").append(path);
                QMessageBox::critical(this, "error reading session", message);
            }

        }else{
            QString message = QString("Error reading session:\n").append(path);
            QMessageBox::critical(this, "error reading session", message);
        }
    }
}


void MainWindow::on_pushButton_sessionAnalyser_clicked()
{
    try {
        if(parsedSchedule.is_initialized()){

            QDateTime qstart = ui->dateTimeEdit_parseSessionStart->dateTime();
            QDateTime qend   = ui->dateTimeEdit_parseSessionEnd->dateTime();
            VieSchedpp_Analyser *analyser = new VieSchedpp_Analyser(*parsedSchedule,parsedFreq,qstart,qend, this);
            analyser->show();
        }
    } catch (...){
        QString message = QString("Error starting session analyzer\nCheck parsed schedule!");
        QMessageBox::critical(this, "error opening session analyzer", message);

    }

}


void MainWindow::on_pushButton_outputNgsFild_clicked()
{
    if(parsedSchedule.is_initialized()){
        QString startPath = ui->lineEdit_sessionPath->text();
        QString path = QFileDialog::getExistingDirectory(this, "Browse to folder", startPath);
        if( !path.isEmpty() ){
            if(path[path.size()-1] != '/'){
                path += '/';
            }

            VieVS::Scheduler copy = *parsedSchedule;

            std::string fname = parsedSchedule->getName();
            std::size_t found = fname.find_last_of( "/\\" );
            fname = fname.substr(found+1);
            std::size_t dot = fname.find_last_of( "." );
            fname = fname.substr( 0, dot );

            VieVS::Output out(copy);
            out.writeNGS();
            QString message = QString("NGS file has been written to:\n").append(path);
            QMessageBox mb;
            QMessageBox::StandardButton reply = mb.information(this, "NGS output", message, QMessageBox::Open,QMessageBox::Ok);
            if(reply == QMessageBox::Open){
                QDesktopServices::openUrl(path);
            }
        }

    }else{
        QString message = QString("Error writing NGS file\nCheck parsed schedule!");
        QMessageBox::critical(this, "Error writing NGS file", message);
    }
}

void MainWindow::on_pushButton_outputSnrTable_2_clicked()
{
    if(parsedSchedule.is_initialized()){
        QString startPath = ui->lineEdit_sessionPath->text();
        QString path = QFileDialog::getExistingDirectory(this, "Browse to folder", startPath);
        if( !path.isEmpty() ){
            if(path[path.size()-1] != '/'){
                path += '/';
            }

            VieVS::Scheduler copy = *parsedSchedule;

            std::string fname = parsedSchedule->getName();
            std::size_t found = fname.find_last_of( "/\\" );
            fname = fname.substr(found+1);
            std::size_t dot = fname.find_last_of( "." );
            fname = fname.substr( 0, dot );

            VieVS::Output out(copy);
            out.writeOperationsNotes();
            QString message = QString("Operation notes file has been written to:\n").append(path);
            QMessageBox mb;
            QMessageBox::StandardButton reply = mb.information(this, "Operation notes output", message, QMessageBox::Open,QMessageBox::Ok);
            if(reply == QMessageBox::Open){
                QDesktopServices::openUrl(path);
            }
        }

    }else{
        QString message = QString("Error writing NGS file\nCheck parsed schedule!");
        QMessageBox::critical(this, "Error writing NGS file", message);
    }

}

void MainWindow::on_pushButton_outputSnrTable_clicked()
{
    if(parsedSchedule.is_initialized()){
        QString startPath = ui->lineEdit_sessionPath->text();
        QString path = QFileDialog::getExistingDirectory(this, "Browse to folder", startPath);
        if( !path.isEmpty() ){
            if(path[path.size()-1] != '/'){
                path += '/';
            }

            VieVS::Scheduler copy = *parsedSchedule;

            std::string fname = parsedSchedule->getName();
            std::size_t found = fname.find_last_of( "/\\" );
            fname = fname.substr(found+1);
            std::size_t dot = fname.find_last_of( "." );
            fname = fname.substr( 0, dot );

            VieVS::Output out(copy);
            out.writeSnrTable();
            QString message = QString("SNR table has been written to:\n").append(path);

            QMessageBox mb;
            QMessageBox::StandardButton reply = mb.information(this, "SNR table output", message, QMessageBox::Open,QMessageBox::Ok);
            if(reply == QMessageBox::Open){
                QDesktopServices::openUrl(path);
            }
        }

    }else{
        QString message = QString("Error writing NGS file\nCheck parsed schedule!");
        QMessageBox::critical(this, "Error writing NGS file", message);
    }
}

void MainWindow::on_pushButton_contact_add_clicked()
{
    QString function = ui->lineEdit_PITask->text();
    QString name = ui->lineEdit_PIName->text();
    QString email = ui->lineEdit_PIEmail->text();
    QString phone = ui->lineEdit_pi_phone->text();
    QString affiliation = ui->lineEdit_pi_affiliation ->text();

    if(function.isEmpty() && name.isEmpty() && email.isEmpty() && phone.isEmpty() && affiliation.isEmpty()){
        return;
    }

    QTableWidget *tableWidget = ui->tableWidget_contact;
    tableWidget->insertRow (tableWidget->rowCount());
    tableWidget->setItem   (tableWidget->rowCount()-1, 0, new QTableWidgetItem(function));
    tableWidget->setItem   (tableWidget->rowCount()-1, 1, new QTableWidgetItem(name));
    tableWidget->setItem   (tableWidget->rowCount()-1, 2, new QTableWidgetItem(email));
    tableWidget->setItem   (tableWidget->rowCount()-1, 3, new QTableWidgetItem(phone));
    tableWidget->setItem   (tableWidget->rowCount()-1, 4, new QTableWidgetItem(affiliation));
}

void MainWindow::on_pushButton_21_clicked()
{
    QTableWidget *tableWidget = ui->tableWidget_contact;
    for (const auto &any : tableWidget->selectionModel()->selectedRows()){
        tableWidget->removeRow(any.row());
    }
}




void MainWindow::on_pushButton_contact_load_clicked()
{
    QDialog *dial = new QDialog(this);
    QVBoxLayout *mainLayout = new QVBoxLayout();

    QTableWidget *tw = new QTableWidget();
    tw->setColumnCount(5);
    tw->setHorizontalHeaderItem(0, new QTableWidgetItem("function"));
    tw->setHorizontalHeaderItem(1, new QTableWidgetItem("name"));
    tw->setHorizontalHeaderItem(2, new QTableWidgetItem("email"));
    tw->setHorizontalHeaderItem(3, new QTableWidgetItem("phone"));
    tw->setHorizontalHeaderItem(4, new QTableWidgetItem("affiliation"));

    tw->setSelectionBehavior(QAbstractItemView::SelectRows);
    tw->setSelectionMode(QAbstractItemView::SingleSelection);
    tw->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);


    const auto &t = settings_.get_child_optional("settings.contacts");
    if(t.is_initialized()){
        for (const auto &any : *t){
            std::string function = any.second.get("function","");
            std::string name = any.second.get("name","");
            std::string email = any.second.get("email","");
            std::string phone = any.second.get("phone","");
            std::string affiliation = any.second.get("affiliation","");

            tw->insertRow (tw->rowCount());
            tw->setItem   (tw->rowCount()-1, 0, new QTableWidgetItem(QString::fromStdString(function)));
            tw->setItem   (tw->rowCount()-1, 1, new QTableWidgetItem(QString::fromStdString(name)));
            tw->setItem   (tw->rowCount()-1, 2, new QTableWidgetItem(QString::fromStdString(email)));
            tw->setItem   (tw->rowCount()-1, 3, new QTableWidgetItem(QString::fromStdString(phone)));
            tw->setItem   (tw->rowCount()-1, 4, new QTableWidgetItem(QString::fromStdString(affiliation)));
        }
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, dial, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, dial, &QDialog::reject);

    mainLayout->addWidget(tw);
    mainLayout->addWidget(buttonBox);

    dial->setLayout(mainLayout);

    int result = dial->exec();

    if(result == QDialog::Accepted){
        for (const auto &any : tw->selectionModel()->selectedRows()){
            int i = any.row();
            QString function = tw->item(i,0)->text();;
            QString name = tw->item(i,1)->text();
            QString email = tw->item(i,2)->text();
            QString phone = tw->item(i,3)->text();
            QString affiliation = tw->item(i,4)->text();

            ui->lineEdit_PITask->setText(function);
            ui->lineEdit_PIName->setText(name);
            ui->lineEdit_PIEmail->setText(email);
            ui->lineEdit_pi_phone->setText(phone);
            ui->lineEdit_pi_affiliation ->setText(affiliation);

        }
    }


}

// #################################### Algorithm ####################################

void MainWindow::on_pushButton_autoSetupIntensive_weightFactor_clicked()
{
    ui->checkBox_weightNobs->setCheckState(Qt::Unchecked);
    ui->checkBox_weightCoverage->setCheckState(Qt::Checked);
    ui->doubleSpinBox_weightSkyCoverage->setValue(0.02);
    ui->checkBox_weightDuration->setCheckState(Qt::Checked);
    ui->doubleSpinBox_weightDuration->setValue(1);
    ui->checkBox_weightIdleTime->setCheckState(Qt::Unchecked);
    ui->checkBox_weightAverageStations->setCheckState(Qt::Unchecked);
    ui->checkBox_weightAverageSources->setCheckState(Qt::Unchecked);
    ui->checkBox_weightAverageBaselines->setCheckState(Qt::Unchecked);
    ui->checkBox_weightLowDeclination->setCheckState(Qt::Unchecked);
    ui->checkBox_weightLowElevation->setCheckState(Qt::Unchecked);
}

void MainWindow::on_pushButton_autoSetupIntensive_minStations_clicked()
{
    sourceSetupWidget->refSourceParameter("default").minNumberOfStations = std::max({selectedStationModel->rowCount(),2});
}

void MainWindow::on_pushButton_autoSetupIntensive_maxScan120_clicked()
{
    stationSetupWidget->refStationParameter("default").maxScan = 120;
}

void MainWindow::on_pushButton_autoSetupIntensive_masScan200_clicked()
{
    stationSetupWidget->refStationParameter("default").maxScan = 200;
}

void MainWindow::on_pushButton_autoSetupIntensive_masScan300_clicked()
{
    stationSetupWidget->refStationParameter("default").maxScan = 90;
}

void MainWindow::on_pushButton_minTimeBetweenScans_clicked()
{
    int val = ui->spinBox_intensiveBlockCadence->value();
    sourceSetupWidget->refSourceParameter("default").minRepeat = std::max({0,  2*val-120});
}

void MainWindow::on_pushButton_mulitScheduling_clicked()
{
    auto *tmp_ms = ui->groupBox_multiScheduling->findChild<QWidget *>("MultiScheduling_Widged");
    MulitSchedulingWidget *ms = qobject_cast<MulitSchedulingWidget *>(tmp_ms);
    ms->defaultIntensive();

}

void MainWindow::on_groupBox_35_toggled(bool arg1)
{
    if(!arg1){
        ui->tabWidget_4->setTabIcon(0,QIcon(":/icons/icons/edit-delete-6.png"));
    }else{
        ui->tabWidget_4->setTabIcon(0,QIcon(":/icons/icons/dialog-ok-2.png"));
    }
}

void MainWindow::on_groupBox_34_toggled(bool arg1)
{
    if(!arg1){
        ui->tabWidget_4->setTabIcon(1,QIcon(":/icons/icons/edit-delete-6.png"));
    }else{
        ui->tabWidget_4->setTabIcon(1,QIcon(":/icons/icons/dialog-ok-2.png"));
    }
}

void MainWindow::on_groupBox_scanSequence_toggled(bool arg1)
{
    if(!arg1){
        ui->tabWidget_4->setTabIcon(2,QIcon(":/icons/icons/edit-delete-6.png"));
    }else{
        ui->tabWidget_4->setTabIcon(2,QIcon(":/icons/icons/dialog-ok-2.png"));
    }
}

void MainWindow::on_groupBox_6_toggled(bool arg1)
{
    if(!arg1){
        ui->tabWidget_4->setTabIcon(3,QIcon(":/icons/icons/edit-delete-6.png"));
    }else{
        ui->tabWidget_4->setTabIcon(3,QIcon(":/icons/icons/dialog-ok-2.png"));
    }
}

void MainWindow::on_groupBox_CalibratorBlock_toggled(bool arg1)
{
    if(!arg1){
        ui->tabWidget_4->setTabIcon(4,QIcon(":/icons/icons/edit-delete-6.png"));
    }else{
        ui->tabWidget_4->setTabIcon(4,QIcon(":/icons/icons/dialog-ok-2.png"));
    }
}

void MainWindow::on_groupBox_highImpactAzEl_toggled(bool arg1)
{
    if(!arg1){
        ui->tabWidget_4->setTabIcon(5,QIcon(":/icons/icons/edit-delete-6.png"));
    }else{
        ui->tabWidget_4->setTabIcon(5,QIcon(":/icons/icons/dialog-ok-2.png"));
    }
}



// ############################### DOWNLOAD ###############################
void MainWindow::download(){

    QLabel *statusBarLabel;
    for(auto &any: ui->statusBar->children()){
        QLabel *l = qobject_cast<QLabel *>(any);
        if(l){
            l->setText(QString("downloading files..."));
            statusBarLabel = l;
        }
    }

    QDir folder = QDir("./AUTO_DOWNLOAD_MASTER");
    QString folderPath = folder.absolutePath();
    if( !folder.exists() ){
        folder.mkdir(folderPath);
    }

    QDateTime now = QDateTime::currentDateTimeUtc();
    int year = now.date().year();

    QStringList files;
    files << QString("ftp://ivs.bkg.bund.de/pub/vlbi/ivscontrol/master%1.txt").arg(year-2000);
    files << QString("ftp://ivs.bkg.bund.de/pub/vlbi/ivscontrol/master%1-int.txt").arg(year-2000);
//    files << QString("ftp://ivs.bkg.bund.de/pub/vlbi/ivscontrol/master%1-vgos.txt").arg(year-2000);

    if (now.date().month() >=11){
        files << QString("ftp://ivs.bkg.bund.de/pub/vlbi/ivscontrol/master%1.txt").arg(year+1-2000);
        files << QString("ftp://ivs.bkg.bund.de/pub/vlbi/ivscontrol/master%1-int.txt").arg(year+1-2000);
    }
//    files << QString("ftp://ivs.bkg.bund.de/pub/vlbi/ivscontrol/master%1-vgos.txt").arg(year+1-2000);


    // legacy SX
    for (int i = 79; i<=99; ++i){
        QString x = QString("./AUTO_DOWNLOAD_MASTER/master%1.txt").arg(i);
        if( !QFile::exists(x)){
            QString z = QString("ftp://ivs.bkg.bund.de/pub/vlbi/ivscontrol/master%1.txt").arg(i);
            files << z;
        }
    }
    for (int i = 00; i<=year-1-2000; ++i){
        QString x = QString("./AUTO_DOWNLOAD_MASTER/master%1.txt").arg(i,2,10,QChar('0'));
        if( !QFile::exists(x)){
            QString z = QString("ftp://ivs.bkg.bund.de/pub/vlbi/ivscontrol/master%1.txt").arg(i,2,10,QChar('0'));
            files << z;
        }
    }

    // intensives
    for (int i = 92; i<=99; ++i){
        QString x = QString("./AUTO_DOWNLOAD_MASTER/master%1-int.txt").arg(i);
        if( !QFile::exists(x)){
            QString z = QString("ftp://ivs.bkg.bund.de/pub/vlbi/ivscontrol/master%1-int.txt").arg(i);
            files << z;
        }
    }
    for (int i = 00; i<=year-1-2000; ++i){
        QString x = QString("./AUTO_DOWNLOAD_MASTER/master%1-int.txt").arg(i,2,10,QChar('0'));
        if( !QFile::exists(x)){
            QString z = QString("ftp://ivs.bkg.bund.de/pub/vlbi/ivscontrol/master%1-int.txt").arg(i,2,10,QChar('0'));
            files << z;
        }
    }

    // vgos
    QString x = QString("./AUTO_DOWNLOAD_MASTER/master%1-vgos.txt").arg(13,2,10,QChar('0'));
    if( !QFile::exists(x)){
        QString z = QString("ftp://ivs.bkg.bund.de/pub/vlbi/ivscontrol/master%1-vgos.txt").arg(13,2,10,QChar('0'));
        files << z;
    }
    for (int i = 15; i<=19; ++i){
        QString x = QString("./AUTO_DOWNLOAD_MASTER/master%1-vgos.txt").arg(i,2,10,QChar('0'));
        if( !QFile::exists(x)){
            QString z = QString("ftp://ivs.bkg.bund.de/pub/vlbi/ivscontrol/master%1-vgos.txt").arg(i,2,10,QChar('0'));
            files << z;
        }
    }

#if VieSchedppOnline

    downloadManager->execute(files,"AUTO_DOWNLOAD_MASTER", statusBarLabel);

    connect(downloadManager,SIGNAL(masterDownloadsFinished()),this,SLOT(masterDownloadFinished()));
    connect(downloadManager,SIGNAL(allDownloadsFinished()),this,SLOT(downloadFinished()));
#endif


}

void MainWindow::masterDownloadFinished(){

    QLabel *statusBarLabel;
    for(auto &any: ui->statusBar->children()){
        QLabel *l = qobject_cast<QLabel *>(any);
        if(l){
            l->setText(QString("downloading catalog files..."));
            statusBarLabel = l;
        }
    }

    QDir folder = QDir("./AUTO_DOWNLOAD_CATALOGS");
    QString folderPath = folder.absolutePath();
    if( !folder.exists() ){
        folder.mkdir(folderPath);
    }

    QStringList files;
    files << "https://raw.githubusercontent.com/nvi-inc/sked_catalogs/main/antenna.cat";
    files << "https://raw.githubusercontent.com/nvi-inc/sked_catalogs/main/equip.cat";
    files << "https://raw.githubusercontent.com/nvi-inc/sked_catalogs/main/flux.cat";
    files << "https://raw.githubusercontent.com/nvi-inc/sked_catalogs/main/freq.cat";
    files << "https://raw.githubusercontent.com/nvi-inc/sked_catalogs/main/hdpos.cat";
    files << "https://raw.githubusercontent.com/nvi-inc/sked_catalogs/main/loif.cat";
    files << "https://raw.githubusercontent.com/nvi-inc/sked_catalogs/main/mask.cat";
    files << "https://raw.githubusercontent.com/nvi-inc/sked_catalogs/main/modes.cat";
    files << "https://raw.githubusercontent.com/nvi-inc/sked_catalogs/main/position.cat";
    files << "https://raw.githubusercontent.com/nvi-inc/sked_catalogs/main/rec.cat";
    files << "https://raw.githubusercontent.com/nvi-inc/sked_catalogs/main/rx.cat";
    files << "https://raw.githubusercontent.com/nvi-inc/sked_catalogs/main/source.cat.geodetic.good";
    files << "https://raw.githubusercontent.com/nvi-inc/sked_catalogs/main/tracks.cat";
    files << "https://datacenter.iers.org/data/latestVersion/9_FINALS.ALL_IAU2000_V2013_019.txt";

    downloadManager->execute(files,"AUTO_DOWNLOAD_CATALOGS", statusBarLabel);
}

void MainWindow::downloadFinished(){
    QLabel *statusBarLabel;
    for(auto &any: ui->statusBar->children()){
        QLabel *l = qobject_cast<QLabel *>(any);
        if(l){
            statusBarLabel = l;
        }
    }

    if(!downloadManager->successful()){
        QStringList errorList = downloadManager->getErrorFileList();
        QStringList errorFiles;
        for( const auto &any: errorList ){
            if( QString::compare(any.left(6),"master", Qt::CaseInsensitive) == 0){
                QDateTime now = QDateTime::currentDateTimeUtc();
                int year = now.date().year();
                QString masterYearStr = any.mid(6,2);
                int masterYear = masterYearStr.toInt()+2000;

                if ( year+1 != masterYear){
                    errorFiles << any;
                }

            }
        }

        if( errorFiles.isEmpty() ){
            statusBarLabel->setText("all downloads finished");
        }else{
            QMessageBox::warning(this,"Error while downloading files", downloadManager->getErrorText());
            statusBarLabel->setText("Error while downloading files");
        }

    }else{
        statusBarLabel->setText("all downloads finished");
    }
    downloadManager->checkDownloads();
    QString errorMsg = downloadManager->getErrorText();
    if(!errorMsg.isEmpty()){
        errorMsg.append("Did you install OpenSSL?");
        QMessageBox::warning(this,"Error while downloading files", errorMsg);
    }
}

void MainWindow::on_sampleBitsSpinBox_valueChanged(int arg1)
{
    if (arg1 == 1){
        ui->doubleSpinBox_efficiencyFactor->setValue(0.6366 * 0.97);
    }else{
        ui->doubleSpinBox_efficiencyFactor->setValue(0.625 * 0.97);
    }
}

void MainWindow::on_pushButton_viewNext_clicked()
{
    auto tmp = qtUtil::getUpcomingSessions();

    std::string load = settings_.get("settings.filterMaster.text","");
    QString qload = QString::fromStdString(load);
    masterSessionViewer *m = new masterSessionViewer( qload, this);
    m->addSessions(tmp);

    int result = m->exec();
    if(result == QDialog::Accepted){
        QString code = m->getSessionCode();

        if(!code.isEmpty()){
            ui->lineEdit_ivsMaster->setText(code);
            on_pushButton_clicked();
        }
    }

    if(m->getSaveClicked()){
        QString txt = m->getSaveText();

        QStringList path;
        QStringList value;

        path << "settings.filterMaster.text";
        value << txt;

        QString name = "";
        changeDefaultSettings(path,value,name);
    }
    delete(m);
}


void MainWindow::on_checkBox_calibration_sessionStart_toggled(bool checked)
{
    auto *l = ui->gridLayout_10;
    for( int i =1; i<=8; ++i){
        l->itemAtPosition(0,i)->widget()->setEnabled(checked);
    }
}

void MainWindow::on_checkBox_calibration_sessionMid_toggled(bool checked)
{
    auto *l = ui->gridLayout_10;
    for( int i =1; i<=8; ++i){
        l->itemAtPosition(1,i)->widget()->setEnabled(checked);
    }
}

void MainWindow::on_checkBox_calibration_sessionEnd_toggled(bool checked)
{
    auto *l = ui->gridLayout_10;
    for( int i =1; i<=8; ++i){
        l->itemAtPosition(2,i)->widget()->setEnabled(checked);
    }
}



void MainWindow::on_spinBox_NCalibrationBlocks_valueChanged(int row)
{
    double session_dur = ui->doubleSpinBox_sessionDuration->value();
    double delta = session_dur/(row-1);
    auto *tab = ui->tableWidget_calibrationBlock;
    tab->setRowCount(row);
    for(int i_row = 0; i_row<row; ++i_row){
        QDoubleSpinBox *t = new QDoubleSpinBox();
        t->setMaximum(session_dur);
        t->setSingleStep(.5);
        double v = i_row * delta;
        if(v==0){
            v=.5;
        }else if(abs(v-session_dur)<.5){
            v = session_dur-.5;
        }
        t->setValue(v);
        t->setSuffix(" [hours]");

        QSpinBox *d = new QSpinBox();
        d->setMaximum(1200);
        d->setSingleStep(60);
        d->setValue(300);
        d->setSuffix(" [s]");

        QSpinBox *s = new QSpinBox();
        s->setMaximum(10);
        s->setValue(2);

        QComboBox *c = new QComboBox();
        c->setModel(allSourcePlusGroupModel);

        tab->setCellWidget(i_row,0,t);
        tab->setCellWidget(i_row,1,d);
        tab->setCellWidget(i_row,2,s);
        tab->setCellWidget(i_row,3,c);

    }
}


void MainWindow::updateWeightFactorSliders()
{

    double weightSkyCoverage = 0;
    if(ui->checkBox_weightCoverage->isChecked()){
        weightSkyCoverage = ui->doubleSpinBox_weightSkyCoverage->value();
    }
    double weightNumberOfObservations = 0;
    if(ui->checkBox_weightNobs->isChecked()){
        weightNumberOfObservations = ui->doubleSpinBox_weightNumberOfObservations->value();
    }
    double weightDuration = 0;
    if(ui->checkBox_weightDuration->isChecked()){
        weightDuration = ui->doubleSpinBox_weightDuration->value();
    }
    double weightAverageSources = 0;
    if(ui->checkBox_weightAverageSources->isChecked()){
        weightAverageSources = ui->doubleSpinBox_weightAverageSources->value();
    }
    double weightAverageStations = 0;
    if(ui->checkBox_weightAverageStations->isChecked()){
        weightAverageStations = ui->doubleSpinBox_weightAverageStations->value();
    }
    double weightAverageBaselines = 0;
    if(ui->checkBox_weightAverageBaselines->isChecked()){
        weightAverageBaselines = ui->doubleSpinBox_weightAverageBaselines->value();
    }
    double weightIdleTime = 0;
    if(ui->checkBox_weightIdleTime->isChecked()){
        weightIdleTime = ui->doubleSpinBox_weightIdleTime->value();
    }
    double weightDeclination = 0;
    if(ui->checkBox_weightLowDeclination->isChecked()){
        weightDeclination = ui->doubleSpinBox_weightLowDec->value();
    }
    double weightElevation = 0;
    if(ui->checkBox_weightLowElevation->isChecked()){
        weightElevation = ui->doubleSpinBox_weightLowEl->value();
    }
    double sum = weightSkyCoverage + weightNumberOfObservations + weightDuration + weightAverageSources + weightAverageStations + weightAverageBaselines + weightIdleTime + weightDeclination + weightElevation;
//    double sum = std::max({weightSkyCoverage, weightNumberOfObservations, weightDuration, weightAverageSources, weightAverageStations, weightAverageBaselines, weightIdleTime, weightDeclination, weightElevation});

    if(sum == 0){
        ui->progressBar_wSky->setValue(0);
        ui->progressBar_wObs->setValue(0);
        ui->progressBar_wDur->setValue(0);
        ui->progressBar_wIdle->setValue(0);
        ui->progressBar_Asrc->setValue(0);
        ui->progressBar_Asta->setValue(0);
        ui->progressBar_Abl->setValue(0);
        ui->progressBar_LowDec->setValue(0);
        ui->progressBar_LowEl->setValue(0);
        return;
    }

    weightSkyCoverage /= sum;
    weightNumberOfObservations /= sum;
    weightDuration /= sum;
    weightAverageSources /= sum;
    weightAverageStations /= sum;
    weightAverageBaselines /= sum;
    weightIdleTime /= sum;
    weightDeclination /= sum;
    weightElevation /= sum;


//    ui->progressBar_wSky->blockSignals(true);
//    ui->progressBar_wObs->blockSignals(true);
//    ui->progressBar_wDur->blockSignals(true);
//    ui->progressBar_wIdle->blockSignals(true);
//    ui->progressBar_Asrc->blockSignals(true);
//    ui->progressBar_Asta->blockSignals(true);
//    ui->progressBar_Abl->blockSignals(true);
//    ui->progressBar_LowDec->blockSignals(true);
//    ui->progressBar_LowEl->blockSignals(true);

    ui->progressBar_wSky->setValue(weightSkyCoverage*100);
    ui->progressBar_wObs->setValue(weightNumberOfObservations*100);
    ui->progressBar_wDur->setValue(weightDuration*100);
    ui->progressBar_wIdle->setValue(weightIdleTime*100);
    ui->progressBar_Asrc->setValue(weightAverageSources*100);
    ui->progressBar_Asta->setValue(weightAverageStations*100);
    ui->progressBar_Abl->setValue(weightAverageBaselines*100);
    ui->progressBar_LowDec->setValue(weightDeclination*100);
    ui->progressBar_LowEl->setValue(weightElevation*100);

//    ui->progressBar_wSky->blockSignals(false);
//    ui->progressBar_wObs->blockSignals(false);
//    ui->progressBar_wDur->blockSignals(false);
//    ui->progressBar_wIdle->blockSignals(false);
//    ui->progressBar_Asrc->blockSignals(false);
//    ui->progressBar_Asta->blockSignals(false);
//    ui->progressBar_Abl->blockSignals(false);
//    ui->progressBar_LowDec->blockSignals(false);
//    ui->progressBar_LowEl->blockSignals(false);
}

void MainWindow::updateWeightFactorValue()
{
    auto *s = qobject_cast<QProgressBar *>( sender() );

    QDoubleSpinBox *dsb;
    if(s == ui->progressBar_wSky){
        dsb = ui->doubleSpinBox_weightSkyCoverage;
    }
    if(s == ui->progressBar_wObs){
        dsb = ui->doubleSpinBox_weightNumberOfObservations;
    }
    if(s == ui->progressBar_wDur){
        dsb = ui->doubleSpinBox_weightDuration;
    }
    if(s == ui->progressBar_wIdle){
        dsb = ui->doubleSpinBox_weightIdleTime;
    }
    if(s == ui->progressBar_Asrc){
        dsb = ui->doubleSpinBox_weightAverageSources;
    }
    if(s == ui->progressBar_Asta){
        dsb = ui->doubleSpinBox_weightAverageStations;
    }
    if(s == ui->progressBar_Abl){
        dsb = ui->doubleSpinBox_weightAverageBaselines;
    }
    if(s == ui->progressBar_LowDec){
        dsb = ui->doubleSpinBox_weightLowDec;
    }
    if(s == ui->progressBar_LowEl){
        dsb = ui->doubleSpinBox_weightLowEl;
    }

    double point2val = 1;
    if(s != ui->progressBar_wSky && ui->progressBar_wSky->value() > 0){
        point2val = ui->doubleSpinBox_weightSkyCoverage->value() / ui->progressBar_wSky->value();
    } else  if(s != ui->progressBar_wObs && ui->progressBar_wObs->value() > 0){
        point2val = ui->doubleSpinBox_weightNumberOfObservations->value() / ui->progressBar_wObs->value();
    } else  if(s != ui->progressBar_wDur && ui->progressBar_wDur->value() > 0){
        point2val = ui->doubleSpinBox_weightDuration->value() / ui->progressBar_wDur->value();
    } else  if(s != ui->progressBar_wIdle && ui->progressBar_wIdle->value() > 0){
        point2val = ui->doubleSpinBox_weightIdleTime->value() / ui->progressBar_wIdle->value();
    } else  if(s != ui->progressBar_Asrc && ui->progressBar_Asrc->value() > 0){
        point2val = ui->doubleSpinBox_weightAverageSources->value() / ui->progressBar_Asrc->value();
    } else  if(s != ui->progressBar_Asta && ui->progressBar_Asta->value() > 0){
        point2val = ui->doubleSpinBox_weightAverageStations->value() / ui->progressBar_Asta->value();
    } else  if(s != ui->progressBar_Abl && ui->progressBar_Abl->value() > 0){
        point2val = ui->doubleSpinBox_weightAverageBaselines->value() / ui->progressBar_Abl->value();
    } else  if(s != ui->progressBar_LowDec && ui->progressBar_LowDec->value() > 0){
        point2val = ui->doubleSpinBox_weightLowDec->value() / ui->doubleSpinBox_weightLowDec->value();
    } else  if(s != ui->progressBar_LowEl && ui->progressBar_LowEl->value() > 0){
        point2val = ui->doubleSpinBox_weightLowEl->value() / ui->progressBar_LowEl->value();
    }
    double newVal = s->value() * point2val;

    dsb->setValue(newVal);
}




void MainWindow::on_pushButton_3_clicked()
{
    QString pathAntenna = ui->lineEdit_pathAntenna->text();
    QString pathEquip = ui->lineEdit_pathEquip->text();
    QString pathPosition = ui->lineEdit_pathPosition->text();
    QString pathMask = ui->lineEdit_pathMask->text();
    QString pathSat = ui->lineEdit_pathSatellite->text();

    QDateTime startTime = ui->dateTimeEdit_sessionStart->dateTime();
    double dur = ui->doubleSpinBox_sessionDuration->value();
    int sec = dur*3600;
    QDateTime endTime = startTime.addSecs(sec);

    QStringList stations;
    for(int i=0; i<selectedStationModel->rowCount(); ++i){
        QString txt = selectedStationModel->index(i,0).data().toString();
        stations.append(txt);
    }
    if (stations.isEmpty()){
        QMessageBox::information(this,"select stations first","Please select your station network before your start with satellite observations");
    }else{
        SatelliteScheduling *sat = new SatelliteScheduling(pathAntenna, pathEquip, pathPosition, pathMask, pathSat,
                                                           selectedSatelliteModel, allSatelliteModel, allSatellitePlusGroupModel, groupSat,
                                                           startTime, endTime, stations, &settings_, this);
        sat->show();

        connect(sat, &SatelliteScheduling::finished, [this, sat](){
            unsigned long nscans = sat->getNumberOfScans();
            ui->spinBox_a_priori_satellite_scans->setValue(nscans);

            a_priori_satellite_scans = sat->toPropertyTree();
            delete sat;
        });
    }
}

void MainWindow::on_pushButton_satellite_select_all_clicked()
{
    ui->lineEdit_allSatelliteFilter->setText("");
    satelliteSetupWidget->blockSignal(true);
    selectedSatelliteModel->blockSignals(true);

    for(int i=0; i<allSatelliteModel->rowCount()-1; ++i){
        on_treeView_allAvailabeSatellites_clicked(allSatelliteModel->index(i,0));
    }

    selectedSatelliteModel->blockSignals(false);
    satelliteSetupWidget->blockSignal(false);
    on_treeView_allAvailabeSatellites_clicked(allSatelliteModel->index(allSatelliteModel->rowCount()-1,0));
    satelliteSetupWidget->setupComboBox()->setCurrentIndex(0);
    satelliteListChanged();
}

void MainWindow::on_pushButton_satellite_select_none_clicked()
{
    selectedSatelliteModel->removeRows(0, selectedSatelliteModel->rowCount());
    int i = 0;
    while(i<allSatellitePlusGroupModel->rowCount()){
        QString name = allSatellitePlusGroupModel->item(i)->text();
        if(name != "__all__" && groupSrc->find(name.toStdString()) == groupSrc->end()){
            allSatellitePlusGroupModel->removeRow(i);
        }else{
            ++i;
        }
    }
    ui->label_sourceList_selected->setText("selected: ");
}
