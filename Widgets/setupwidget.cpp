#include "setupwidget.h"
#include "ui_setupwidget.h"

setupWidget::setupWidget(Type type,
                         boost::property_tree::ptree &settings,
                         QTableWidget *tableWidget_ModesPolicy,
                         QStandardItemModel *allStationModel,
                         QStandardItemModel *allSourceModel,
                         QStandardItemModel *allSatelliteModel,
                         QStandardItemModel *allSpacecraftModel,
                         QStandardItemModel *selectedStationModel,
                         QStandardItemModel *selectedSourceModel,
                         QStandardItemModel *selectedBaselineModel,
                         QStandardItemModel *selectedSatelliteModel,
                         QStandardItemModel *selectedSpacecraftModel,
                         QStandardItemModel *allSourcePlusGroupModel,
                         QStandardItemModel *allStationPlusGroupModel,
                         QStandardItemModel *allBaselinePlusGroupModel,
                         QStandardItemModel *allSatellitePlusGroupModel,
                         QStandardItemModel *allSpacecraftPlusGroupModel,
                         std::map<std::string, std::vector<std::string>> *groupSta,
                         std::map<std::string, std::vector<std::string>> *groupSrc,
                         std::map<std::string, std::vector<std::string>> *groupBl,
                         std::map<std::string, std::vector<std::string>> *groupSat,
                         std::map<std::string, std::vector<std::string>> *groupSpace,
                         QWidget *parent) :
    QWidget(parent),
    type{type},
    settings{settings},
    tableWidget_ModesPolicy{tableWidget_ModesPolicy},
    allStationModel{allStationModel},
    allSourceModel{allSourceModel},
    allSatelliteModel{allSatelliteModel},
    allSpacecraftModel{allSpacecraftModel},
    selectedStationModel{selectedStationModel},
    selectedSourceModel{selectedSourceModel},
    selectedBaselineModel{selectedBaselineModel},
    selectedSatelliteModel{selectedSatelliteModel},
    selectedSpacecraftModel{selectedSpacecraftModel},
    allSourcePlusGroupModel{allSourcePlusGroupModel},
    allStationPlusGroupModel{allStationPlusGroupModel},
    allBaselinePlusGroupModel{allBaselinePlusGroupModel},
    allSatellitePlusGroupModel{allSatellitePlusGroupModel},
    allSpacecraftPlusGroupModel{allSpacecraftPlusGroupModel},
    groupSta{groupSta},
    groupSrc{groupSrc},
    groupBl{groupBl},
    groupSat{groupSat},
    groupSpace{groupSpace},
    ui(new Ui::setupWidget)
{
    ui->setupUi(this);

    switch (type) {
    case Type::station:{
        icon_single = QIcon(":/icons/icons/station.png");
        icon_group = QIcon(":/icons/icons/station_group.png");
        icon_add_group = QIcon(":/icons/icons/station_group_plus.png");
        selectedModel = selectedStationModel;
        allPlusGroupModel = allStationPlusGroupModel;
        allModel = allStationModel;
        groups = groupSta;

        break;
    }
    case Type::baseline:{
        icon_single = QIcon(":/icons/icons/baseline.png");
        icon_group = QIcon(":/icons/icons/baseline_group.png");
        icon_add_group = QIcon(":/icons/icons/baseline_group_plus.png");
        ui->pushButton_IvsDownTime->setVisible(false);
        ui->pushButton_parseDownTime->setVisible(false);
        selectedModel = selectedBaselineModel;
        allPlusGroupModel = allBaselinePlusGroupModel;
        groups = groupBl;

        break;
    }
    case Type::source:{
        icon_single = QIcon(":/icons/icons/source.png");
        icon_group = QIcon(":/icons/icons/source_group.png");
        icon_add_group = QIcon(":/icons/icons/source_group_plus.png");
        ui->pushButton_IvsDownTime->setVisible(false);
        ui->pushButton_parseDownTime->setVisible(false);
        selectedModel = selectedSourceModel;
        allPlusGroupModel = allSourcePlusGroupModel;
        allModel = allSourceModel;
        groups = groupSrc;

        break;
    }
    case Type::satellite:{
        icon_single = QIcon(":/icons/icons/satellite.png");
        icon_group = QIcon(":/icons/icons/satellite.png");
        icon_add_group = QIcon(":/icons/icons/satellite.png");
        ui->pushButton_IvsDownTime->setVisible(false);
        ui->pushButton_parseDownTime->setVisible(false);
        selectedModel = selectedSatelliteModel;
        allPlusGroupModel = allSatellitePlusGroupModel;
        allModel = allSatelliteModel;
        groups = groupSat;

        break;
    }
    case Type::spacecraft:{
        icon_single = QIcon(":/icons/icons/spacecraft.png");
        icon_group = QIcon(":/icons/icons/spacecraft.png");
        icon_add_group = QIcon(":/icons/icons/spacecraft.png");
        ui->pushButton_IvsDownTime->setVisible(false);
        ui->pushButton_parseDownTime->setVisible(false);
        selectedModel = selectedSpacecraftModel;
        allPlusGroupModel = allSpacecraftPlusGroupModel;
        allModel = allSpacecraftModel;
        groups = groupSpace;

        break;
    }
    }
    ui->pushButton_addGroup->setIcon(icon_add_group);
    ui->comboBox_members->setModel(allPlusGroupModel);
    ui->comboBox_setup->setModel(selectedModel);

    ui->splitter_horizontal->setSizes({2000,5000});
    ui->splitter_vertical->setSizes({3000,5000});

    ui->comboBox_members->installEventFilter(this);
    ui->ComboBox_parameters->installEventFilter(this);
    ui->comboBox_setup->installEventFilter(this);
    ui->DateTimeEdit_start->setDisplayFormat("dd.MM.yyyy HH:mm");
    ui->DateTimeEdit_end->setDisplayFormat("dd.MM.yyyy HH:mm");

    prepareSetupPlot();
}

setupWidget::~setupWidget()
{
    delete ui;
}

bool setupWidget::reset()
{
    bool flag = false;
    ui->DateTimeEdit_start->setDateTime(minDateTime);
    ui->DateTimeEdit_end->setDateTime(maxDateTime);

    QTreeWidgetItem *def = new QTreeWidgetItem();
    def->setText(0,"__all__");
    def->setText(1,"default");
    def->setText(2,ui->DateTimeEdit_start->dateTime().toString("dd.MM.yyyy hh:mm"));
    def->setText(3,ui->DateTimeEdit_end->dateTime().toString("dd.MM.yyyy hh:mm"));
    def->setText(4,"hard");
    def->setIcon(0,icon_group);

    QTreeWidgetItem *ms = new QTreeWidgetItem();
    ms->setText(0,"__all__");
    ms->setText(1,"multi scheduling");
    ms->setText(2,ui->DateTimeEdit_start->dateTime().toString("dd.MM.yyyy hh:mm"));
    ms->setText(3,ui->DateTimeEdit_end->dateTime().toString("dd.MM.yyyy hh:mm"));
    ms->setText(4,"hard");
    ms->setIcon(0,icon_group);
    def->addChild(ms);

    flag = false;
    if(ui->treeWidget_setup->topLevelItemCount() > 0){
        if(ui->treeWidget_setup->topLevelItem(0)->childCount()>0){
            if(ui->treeWidget_setup->topLevelItem(0)->child(0)->childCount()>0){
                flag = true;
            }
        }
    }
    ui->treeWidget_setup->clear();
    ui->treeWidget_setup->insertTopLevelItem(0,def);
    ui->treeWidget_setup->expandAll();
    QHeaderView * hvsta = ui->treeWidget_setup->header();
    hvsta->setSectionResizeMode(QHeaderView::ResizeToContents);
    setupTree = VieVS::ParameterSetup("default",
                                  "__all__",
                                  0,
                                  ui->DateTimeEdit_start->dateTime().secsTo(ui->DateTimeEdit_end->dateTime()),
                                  VieVS::ParameterSetup::Transition::hard);

    VieVS::ParameterSetup mss = VieVS::ParameterSetup("multi scheduling",
                                                      "__all__",
                                                      0,
                                                      ui->DateTimeEdit_start->dateTime().secsTo(ui->DateTimeEdit_end->dateTime()),
                                  VieVS::ParameterSetup::Transition::hard);
    setupTree.addChild(mss);

    drawSetupPlot();

    return flag;
}

void setupWidget::blockSignal(bool flag)
{
    ui->comboBox_setup->blockSignals(flag);
}

void setupWidget::setDateTimeLimits(QDateTime start, QDateTime end)
{
    minDateTime = start;
    maxDateTime = end;
}

void setupWidget::addDownTime(std::string station, unsigned int downStart, unsigned int downEnd)
{

    VieVS::ParameterSetup setupDown("down", station, downStart, downEnd, VieVS::ParameterSetup::Transition::hard);
    setupTree.refChildren().at(0).addChild(setupDown);

    ui->treeWidget_setup->clear();

    QTreeWidgetItem *c = new QTreeWidgetItem();
    VieVS::ParameterSettings::Type setupType = VieVS::ParameterSettings::Type::station;
    drawTable(setupTree, c);
    ui->treeWidget_setup->addTopLevelItem(c);
    ui->treeWidget_setup->expandAll();

    drawSetupPlot();

}


void setupWidget::prepareSetupPlot()
{
    QChartView *figure = new QChartView();
    figure->setToolTip("visual representation of setup");
    figure->setStatusTip("visual representation of setup");

    QChart *chart = new QChart();
    figure->setChart(chart);
    callout = new Callout(chart);
    callout->hide();


    QLineSeries *series = new QLineSeries();
    QDateTime start = ui->DateTimeEdit_start->dateTime();
    QDateTime end = ui->DateTimeEdit_end->dateTime();


    series->append(start.toMSecsSinceEpoch(),-5);
    series->append(end.toMSecsSinceEpoch(),-5);
    chart->addSeries(series);

    chart->setTitle("Setup");

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTitleText("time");
    axisX->setFormat("hh:mm");
    axisX->setRange(start,end);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("Parameters");
    axisY->setTickCount(1);
    axisY->setRange(-10,10);
    chart->addAxis(axisY,Qt::AlignLeft);
    series->attachAxis(axisY);

    figure->setRenderHint(QPainter::Antialiasing);
    ui->verticalLayout_setupChart->insertWidget(1,figure,1);
    axisY->hide();
    axisX->show();
    chart->legend()->hide();
}

bool setupWidget::eventFilter(QObject *watched, QEvent *event)
{
    bool valid = false;
    if(watched == ui->comboBox_members){
        if(event->type() == QEvent::Enter){
            QString name = ui->comboBox_members->currentText();
            displaySetupMember(name);
            valid = true;
        }
    }else if(watched == ui->ComboBox_parameters){
        if(event->type() == QEvent::Enter){
            QString name = ui->ComboBox_parameters->currentText();
            switch (type) {
            case Type::station: {
                displayStationSetupParameter(name);
                break;
            }
            case Type::baseline: {
                displayBaselineSetupParameter(name);
                break;
            }
            case Type::source: {
                displaySourceSetupParameter(name);
                break;
            }
            case Type::satellite: {
                displaySourceSetupParameter(name);
                break;
            }
            case Type::spacecraft: {
                displaySourceSetupParameter(name);
                break;
            }
            }

            valid = true;
        }
    }else if(watched == ui->comboBox_setup){
        if(event->type() == QEvent::Enter){
            QString name = ui->comboBox_setup->currentText();
            if(!name.isEmpty()){
                displaySetupMember(name);
                valid = true;
            }
        }
    }
    return valid;

}

void setupWidget::displaySetupMember(QString name)
{
    if(name.isEmpty()){
        return;
    }
    auto t = ui->tableWidget_hovered;
    t->clear();
    t->setColumnCount(1);
    t->verticalHeader()->show();
    if (name == "__all__"){
        t->setHorizontalHeaderItem(0,new QTableWidgetItem(icon_group, QString("Group: %1").arg(name)));
        t->setRowCount(selectedModel->rowCount());
        for(int i=0; i<selectedModel->rowCount(); ++i){
            QString txt = selectedModel->index(i,0).data().toString();
            t->setItem(i,0,new QTableWidgetItem(icon_single,txt));
        }
    }else if(groups->find(name.toStdString()) != groups->end()){
        t->setHorizontalHeaderItem(0,new QTableWidgetItem(icon_group,QString("Group: %1").arg(name)));
        auto members = groups->at(name.toStdString());
        t->setRowCount(members.size());
        for(int i=0; i<members.size(); ++i){
            QString txt = QString::fromStdString(members.at(i));
            t->setItem(i,0,new QTableWidgetItem(icon_single,txt));
        }
    }else{
        if(type == Type::baseline){
            t->setHorizontalHeaderItem(0,new QTableWidgetItem(QIcon(":/icons/icons/baseline.png"),QString("%1").arg(name)));
            t->setRowCount(3);
            QStringList list= name.split("-");

            double distance;
            for(int i=0; i<selectedBaselineModel->rowCount(); ++i){
                QString txt = selectedBaselineModel->index(i,0).data().toString();
                if(txt == name){
                    distance = selectedBaselineModel->index(i,1).data().toDouble();
                }
            }

            t->setItem(0,0,new QTableWidgetItem(QIcon(":/icons/icons/station.png"),list.at(0)));
            t->setItem(0,1,new QTableWidgetItem(QIcon(":/icons/icons/station.png"),list.at(1)));
            t->setItem(0,2,new QTableWidgetItem(QString("%1").arg(distance)));

            t->setVerticalHeaderItem(0,new QTableWidgetItem("Station 1"));
            t->setVerticalHeaderItem(1,new QTableWidgetItem("Station 2"));
            t->setVerticalHeaderItem(2,new QTableWidgetItem("distance [km]"));

        }else{
            t->setHorizontalHeaderItem(0,new QTableWidgetItem(icon_single,QString("%1").arg(name)));
            t->setRowCount(allModel->columnCount());
            QList<QStandardItem *> litm = allModel->findItems(name);
            QStandardItem * itm = litm.at(0);
            int row = itm->row();
            for(int i=0; i<allModel->columnCount(); ++i){
                QString txt = allModel->headerData(i,Qt::Horizontal).toString();
                QString txt2 = allModel->item(row,i)->text();
                t->setVerticalHeaderItem(i,new QTableWidgetItem(txt));
                t->setItem(i,0,new QTableWidgetItem(txt2));
            }
        }
    }
    QHeaderView *hv = t->verticalHeader();
    hv->setSectionResizeMode(QHeaderView::ResizeToContents);

}

void setupWidget::displayStationSetupParameter(QString name)
{
    if(name.isEmpty()){
        return;
    }

    auto t = ui->tableWidget_hovered;
    if(name == "multi scheduling"){
        t->clear();
        t->setRowCount(0);
        t->setColumnCount(1);
        t->setHorizontalHeaderItem(0, new QTableWidgetItem("multi scheduling"));
        t->verticalHeader()->hide();
        t->insertRow(0);
        t->setItem(0,0,new QTableWidgetItem(QIcon(":/icons/icons/multi_sched.png"),"see multi scheduling setup"));
        return;
    }
    t->verticalHeader()->show();
    t->clear();
    t->setColumnCount(1);
    t->setHorizontalHeaderItem(0,new QTableWidgetItem(QString("Parameter: %1").arg(name)));
    VieVS::ParameterSettings::ParametersStations para = paraSta.at(name.toStdString());
    t->setRowCount(0);
    int r = 0;
    if(para.available.is_initialized()){
        t->insertRow(r);
        QString boolText = *para.available ? "true" : "false";
        t->setItem(r,0,new QTableWidgetItem(boolText));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("available"));
        ++r;
    }
    if(para.availableForFillinmode.is_initialized()){
        t->insertRow(r);
        QString boolText = *para.availableForFillinmode ? "true" : "false";
        t->setItem(r,0,new QTableWidgetItem(boolText));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("available for fillin mode"));
        ++r;
    }
    if(para.tagalong.is_initialized()){
        t->insertRow(r);
        QString boolText = *para.tagalong ? "true" : "false";
        t->setItem(r,0,new QTableWidgetItem(boolText));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("tagalong"));
        ++r;
    }
    if(para.maxScan.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.maxScan)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("max scan time [s]"));
        ++r;
     }
    if(para.minScan.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.minScan)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("min scan time [s]"));
        ++r;
    }
    if(para.minSlewtime.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.minSlewtime)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("min slew time [s]"));
        ++r;
    }
    if(para.maxSlewtime.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.maxSlewtime)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("max slew time [s]"));
        ++r;
    }
    if(para.maxSlewDistance.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.maxSlewDistance)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("max slew distance [deg]"));
        ++r;
    }
    if(para.minSlewDistance.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.minSlewDistance)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("min slew distance [deg]"));
        ++r;
    }
    if(para.maxWait.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.maxWait)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("max wait time [s]"));
        ++r;
    }
    if(para.dataWriteRate.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.dataWriteRate)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("data write speed to disk [Mbps]"));
        ++r;
    }
    if(para.maxNumberOfScans.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.maxNumberOfScans)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("max number of scans"));
        ++r;
    }
    if(para.minElevation.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.minElevation)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("min elevation [deg]"));
        ++r;
    }
    if(para.weight.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.weight)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("weight"));
        ++r;
    }
    if(para.maxTotalObsTime.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.maxTotalObsTime)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("max total obs time [s]"));
        ++r;
    }
    if(para.minSNR.size() >0 ){
        for(const auto& any: para.minSNR){
            t->insertRow(r);
            t->setItem(r,0,new QTableWidgetItem(QString::number(any.second)));
            QString txt = QString("min SNR %1").arg(QString::fromStdString(any.first));
            t->setVerticalHeaderItem(r,new QTableWidgetItem(txt));
            ++r;
        }
    }
    if(para.ignoreSourcesString.size() > 0){
          for(const auto &any: para.ignoreSourcesString){
              t->insertRow(r);
              if(groupSrc->find(any) != groupSrc->end() || any == "__all__"){
                  t->setVerticalHeaderItem(r,new QTableWidgetItem("ignore source group"));
                  t->setItem(r,0,new QTableWidgetItem(QIcon(":/icons/icons/source_group.png"),QString::fromStdString(any)));
              }else{
                  t->setVerticalHeaderItem(r,new QTableWidgetItem("ignore source"));
                  t->setItem(r,0,new QTableWidgetItem(QIcon(":/icons/icons/source.png"),QString::fromStdString(any)));
              }
              ++r;
          }
    }
    if(para.preob.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.preob)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("preob [s]"));
        ++r;
    }
    if(para.midob.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.midob)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("midob [s]"));
        ++r;
    }
    if(para.systemDelay.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.systemDelay)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("system delay [s]"));
        ++r;
    }

    QHeaderView *hv = t->verticalHeader();
    hv->setSectionResizeMode(QHeaderView::ResizeToContents);

}

void setupWidget::displaySourceSetupParameter(QString name)
{
    if(name.isEmpty()){
        return;
    }

    auto t = ui->tableWidget_hovered;
    if(name == "multi scheduling"){
        t->clear();
        t->setRowCount(0);
        t->setColumnCount(1);
        t->setHorizontalHeaderItem(0, new QTableWidgetItem("multi scheduling"));
        t->verticalHeader()->hide();
        t->insertRow(0);
        t->setItem(0,0,new QTableWidgetItem(QIcon(":/icons/icons/multi_sched.png"),"see multi scheduling setup"));
        return;
    }
    t->verticalHeader()->show();
    t->clear();
    t->setColumnCount(1);
    t->setHorizontalHeaderItem(0,new QTableWidgetItem(QString("Parameter: %1").arg(name)));
    VieVS::ParameterSettings::ParametersSources para = paraSrc.at(name.toStdString());
    t->setRowCount(0);
    int r = 0;
    if(para.available.is_initialized()){
        t->insertRow(r);
        QString boolText = *para.available ? "true" : "false";
        t->setItem(r,0,new QTableWidgetItem(boolText));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("available"));
        ++r;
    }
    if(para.availableForFillinmode.is_initialized()){
        t->insertRow(r);
        QString boolText = *para.availableForFillinmode ? "true" : "false";
        t->setItem(r,0,new QTableWidgetItem(boolText));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("available for fillin mode"));
        ++r;
    }
    if(para.minNumberOfStations.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.minNumberOfStations)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("min number of stations"));
        ++r;
    }
    if(para.maxScan.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.maxScan)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("max scan time [s]"));
        ++r;
     }
    if(para.minScan.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.minScan)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("min scan time [s]"));
        ++r;
    }
    if(para.maxNumberOfScans.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.maxNumberOfScans)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("max number of scans"));
        ++r;
    }
    if(para.minFlux.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.minFlux)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("min flux density [Jy]"));
        ++r;
    }
    if(para.minRepeat.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.minRepeat)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("min time between two scans [s]"));
        ++r;
    }
    if(para.weight.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.weight)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("weight"));
        ++r;
    }
    if(para.minElevation.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.minElevation)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("minimum elevation [deg]"));
        ++r;
    }
    if(para.minSunDistance.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.minSunDistance)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("minimum sun distance [deg]"));
        ++r;
    }
    if(para.fixedScanDuration.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.fixedScanDuration)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("fixed scan duration"));
        ++r;
    }
    if(para.tryToFocusIfObservedOnce.is_initialized()){
        t->insertRow(r);
        QString boolText = *para.tryToFocusIfObservedOnce ? "true" : "false";
        t->setItem(r,0,new QTableWidgetItem(boolText));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("try to focus if observed once"));
        ++r;

        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.tryToFocusFactor)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("increase weight by factor"));
        ++r;

        t->insertRow(r);
        QString occurrencyText = *para.tryToFocusOccurrency == VieVS::ParameterSettings::TryToFocusOccurrency::once ? "once" : "per scan";
        t->setItem(r,0,new QTableWidgetItem(occurrencyText));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("increase weight"));
        ++r;

        t->insertRow(r);
        QString typeText = *para.tryToFocusType == VieVS::ParameterSettings::TryToFocusType::additive ? "additive" : "multiplicative";
        t->setItem(r,0,new QTableWidgetItem(typeText));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("increase weight"));
        ++r;
    }
    if(para.tryToObserveXTimesEvenlyDistributed.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.tryToObserveXTimesEvenlyDistributed)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("eavenly distributed scans over time"));
        ++r;

        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.tryToObserveXTimesMinRepeat)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("min time between two scans [s]"));
        ++r;

    }
    if(para.minSNR.size() >0 ){
        for(const auto& any: para.minSNR){
            t->insertRow(r);
            t->setItem(r,0,new QTableWidgetItem(QString::number(any.second)));
            QString txt = QString("min SNR %1").arg(QString::fromStdString(any.first));
            t->setVerticalHeaderItem(r,new QTableWidgetItem(txt));
            ++r;
        }
    }
    if(para.ignoreStationsString.size() > 0){
          for(const auto &any: para.ignoreStationsString){
              t->insertRow(r);
              if(groupSta->find(any) != groupSta->end() || any == "__all__"){
                  t->setVerticalHeaderItem(r,new QTableWidgetItem("ignore station group"));
                  t->setItem(r,0,new QTableWidgetItem(QIcon(":/icons/icons/station_group.png"),QString::fromStdString(any)));
              }else{
                  t->setVerticalHeaderItem(r,new QTableWidgetItem("ignore station"));
                  t->setItem(r,0,new QTableWidgetItem(QIcon(":/icons/icons/station.png"),QString::fromStdString(any)));
              }
              ++r;
          }
    }
    if(para.requiredStationsString.size() > 0){
          for(const auto &any: para.requiredStationsString){
              t->insertRow(r);
              if(groupSta->find(any) != groupSta->end() || any == "__all__"){
                  t->setVerticalHeaderItem(r,new QTableWidgetItem("required station group"));
                  t->setItem(r,0,new QTableWidgetItem(QIcon(":/icons/icons/station_group.png"),QString::fromStdString(any)));
              }else{
                  t->setVerticalHeaderItem(r,new QTableWidgetItem("required stations"));
                  t->setItem(r,0,new QTableWidgetItem(QIcon(":/icons/icons/station.png"),QString::fromStdString(any)));
              }
              ++r;
          }

    }
    if(para.ignoreBaselinesString.size() > 0){
          for(const auto &any: para.ignoreBaselinesString){
              t->insertRow(r);
              if(groupBl->find(any) != groupBl->end() || any == "__all__"){
                  t->setVerticalHeaderItem(r,new QTableWidgetItem("ignore baseline group"));
                  t->setItem(r,0,new QTableWidgetItem(QIcon(":/icons/icons/baseline_group.png"),QString::fromStdString(any)));
              }else{
                  t->setVerticalHeaderItem(r,new QTableWidgetItem("ignore baseline"));
                  t->setItem(r,0,new QTableWidgetItem(QIcon(":/icons/icons/baseline.png"),QString::fromStdString(any)));
              }
              ++r;
          }

    }
    QHeaderView *hv = t->verticalHeader();
    hv->setSectionResizeMode(QHeaderView::ResizeToContents);

}

void setupWidget::displayBaselineSetupParameter(QString name)
{
    if(name.isEmpty()){
        return;
    }

    auto t = ui->tableWidget_hovered;
    if(name == "multi scheduling"){
        t->clear();
        t->setRowCount(0);
        t->setColumnCount(1);
        t->setHorizontalHeaderItem(0, new QTableWidgetItem("multi scheduling"));
        t->verticalHeader()->hide();
        t->insertRow(0);
        t->setItem(0,0,new QTableWidgetItem(QIcon(":/icons/icons/multi_sched.png"),"see multi scheduling setup"));
        return;
    }
    t->verticalHeader()->show();
    t->clear();
    t->setColumnCount(1);
    t->setHorizontalHeaderItem(0,new QTableWidgetItem(QString("Parameter: %1").arg(name)));
    VieVS::ParameterSettings::ParametersBaselines para = paraBl.at(name.toStdString());
    t->setRowCount(0);
    int r = 0;
    if(para.ignore.is_initialized()){
        t->insertRow(r);
        QString boolText = *para.ignore ? "true" : "false";
        t->setItem(r,0,new QTableWidgetItem(boolText));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("ignore"));
        ++r;
    }
    if(para.maxScan.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.maxScan)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("max scan time [s]"));
        ++r;
     }
    if(para.minScan.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.minScan)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("min scan time [s]"));
        ++r;
    }
    if(para.weight.is_initialized()){
        t->insertRow(r);
        t->setItem(r,0,new QTableWidgetItem(QString::number(*para.weight)));
        t->setVerticalHeaderItem(r,new QTableWidgetItem("weight"));
        ++r;
    }
    if(para.minSNR.size() >0 ){
        for(const auto& any: para.minSNR){
            t->insertRow(r);
            t->setItem(r,0,new QTableWidgetItem(QString::number(any.second)));
            QString txt = QString("min SNR %1").arg(QString::fromStdString(any.first));
            t->setVerticalHeaderItem(r,new QTableWidgetItem(txt));
            ++r;
        }
    }
    QHeaderView *hv = t->verticalHeader();
    hv->setSectionResizeMode(QHeaderView::ResizeToContents);

}

void setupWidget::on_pushButton_editParameter_clicked()
{
    QStringList bands;
    for(int i = 0; i<tableWidget_ModesPolicy->rowCount(); ++i){
        bands << tableWidget_ModesPolicy->verticalHeaderItem(i)->text();
    }

    switch(type){
    case Type::station:{
        stationParametersDialog *dial = new stationParametersDialog(settings,this);
        dial->addBandNames(bands);
        dial->addSourceNames(allSourcePlusGroupModel);
        dial->addDefaultParameters(paraSta["default"]);
        dial->addSelectedParameters(paraSta[ui->ComboBox_parameters->currentText().toStdString()],
                                    ui->ComboBox_parameters->currentText());

        int result = dial->exec();
        if(result == QDialog::Accepted){
            std::pair<std::string, VieVS::ParameterSettings::ParametersStations> res = dial->getParameters();
            std::string name = res.first;
            VieVS::ParameterSettings::ParametersStations parameter = res.second;

            paraSta[name] = parameter;

        }
        delete(dial);

        break;
    }
    case Type::baseline:{
        baselineParametersDialog *dial = new baselineParametersDialog(settings, this);
        dial->addBandNames(bands);

        dial->addDefaultParameters(paraBl["default"]);
        dial->addSelectedParameters(paraBl[ui->ComboBox_parameters->currentText().toStdString()],ui->ComboBox_parameters->currentText());


        int result = dial->exec();
        if(result == QDialog::Accepted){
            std::pair<std::string, VieVS::ParameterSettings::ParametersBaselines> res = dial->getParameters();
            std::string name = res.first;
            VieVS::ParameterSettings::ParametersBaselines parameter = res.second;

            paraBl[name] = parameter;

        }
        delete(dial);

        break;
    }
    case Type::source:{
        sourceParametersDialog *dial = new sourceParametersDialog(settings,this);
        QStringList bands;
        for(int i = 0; i<tableWidget_ModesPolicy->rowCount(); ++i){
            bands << tableWidget_ModesPolicy->verticalHeaderItem(i)->text();
        }
        dial->addBandNames(bands);

        dial->addStationModel(allStationPlusGroupModel);
        dial->addBaselineModel(allBaselinePlusGroupModel);
        dial->addDefaultParameters(paraSrc["default"]);
        dial->addSelectedParameters(paraSrc[ui->ComboBox_parameters->currentText().toStdString()],ui->ComboBox_parameters->currentText());

        int result = dial->exec();
        if(result == QDialog::Accepted){
            std::pair<std::string, VieVS::ParameterSettings::ParametersSources> res = dial->getParameters();
            std::string name = res.first;
            VieVS::ParameterSettings::ParametersSources parameter = res.second;

            paraSrc[name] = parameter;

        }
        delete(dial);

        break;
    }
    case Type::satellite:{
        sourceParametersDialog *dial = new sourceParametersDialog(settings,this);
        QStringList bands;
        for(int i = 0; i<tableWidget_ModesPolicy->rowCount(); ++i){
            bands << tableWidget_ModesPolicy->verticalHeaderItem(i)->text();
        }
        dial->addBandNames(bands);

        dial->addStationModel(allStationPlusGroupModel);
        dial->addBaselineModel(allBaselinePlusGroupModel);
        dial->addDefaultParameters(paraSrc["default"]);
        dial->addSelectedParameters(paraSrc[ui->ComboBox_parameters->currentText().toStdString()],ui->ComboBox_parameters->currentText());

        int result = dial->exec();
        if(result == QDialog::Accepted){
            std::pair<std::string, VieVS::ParameterSettings::ParametersSources> res = dial->getParameters();
            std::string name = res.first;
            VieVS::ParameterSettings::ParametersSources parameter = res.second;

            paraSrc[name] = parameter;

        }
        delete(dial);

        break;
    }
    case Type::spacecraft:{
        sourceParametersDialog *dial = new sourceParametersDialog(settings,this);
        QStringList bands;
        for(int i = 0; i<tableWidget_ModesPolicy->rowCount(); ++i){
            bands << tableWidget_ModesPolicy->verticalHeaderItem(i)->text();
        }
        dial->addBandNames(bands);

        dial->addStationModel(allStationPlusGroupModel);
        dial->addBaselineModel(allBaselinePlusGroupModel);
        dial->addDefaultParameters(paraSrc["default"]);
        dial->addSelectedParameters(paraSrc[ui->ComboBox_parameters->currentText().toStdString()],ui->ComboBox_parameters->currentText());

        int result = dial->exec();
        if(result == QDialog::Accepted){
            std::pair<std::string, VieVS::ParameterSettings::ParametersSources> res = dial->getParameters();
            std::string name = res.first;
            VieVS::ParameterSettings::ParametersSources parameter = res.second;

            paraSrc[name] = parameter;

        }
        delete(dial);

        break;
    }
    }


}

void setupWidget::on_pushButton_addParameter_clicked()
{
    QStringList bands;
    for(int i = 0; i<tableWidget_ModesPolicy->rowCount(); ++i){
        bands << tableWidget_ModesPolicy->verticalHeaderItem(i)->text();
    }

    switch(type){
    case Type::station:{
        stationParametersDialog *dial = new stationParametersDialog(settings,this);
        dial->addBandNames(bands);
        dial->addSourceNames(allSourcePlusGroupModel);
        dial->addDefaultParameters(paraSta["default"]);

        int result = dial->exec();
        if(result == QDialog::Accepted){
            std::pair<std::string, VieVS::ParameterSettings::ParametersStations> res = dial->getParameters();
            std::string name = res.first;
            VieVS::ParameterSettings::ParametersStations parameter = res.second;

            paraSta[name] = parameter;

            ui->ComboBox_parameters->addItem(QString::fromStdString(name));
            ui->ComboBox_parameters->setCurrentIndex(ui->ComboBox_parameters->count()-1);

        }
        delete(dial);

    break;
    }
    case Type::baseline:{
        baselineParametersDialog *dial = new baselineParametersDialog(settings, this);
        dial->addDefaultParameters(paraBl["default"]);
        dial->addBandNames(bands);

        int result = dial->exec();
        if(result == QDialog::Accepted){
            std::pair<std::string, VieVS::ParameterSettings::ParametersBaselines> res = dial->getParameters();
            std::string name = res.first;
            VieVS::ParameterSettings::ParametersBaselines parameter = res.second;

            paraBl[name] = parameter;

            ui->ComboBox_parameters->addItem(QString::fromStdString(name));
            ui->ComboBox_parameters->setCurrentIndex(ui->ComboBox_parameters->count()-1);

        }
        delete(dial);

        break;
    }
    case Type::source:{
        sourceParametersDialog *dial = new sourceParametersDialog(settings,this);
        dial->addBandNames(bands);

        dial->addStationModel(allStationPlusGroupModel);
        dial->addBaselineModel(allBaselinePlusGroupModel);
        dial->addDefaultParameters(paraSrc["default"]);

        int result = dial->exec();
        if(result == QDialog::Accepted){
            std::pair<std::string, VieVS::ParameterSettings::ParametersSources> res = dial->getParameters();
            std::string name = res.first;
            VieVS::ParameterSettings::ParametersSources parameter = res.second;

            paraSrc[name] = parameter;

            ui->ComboBox_parameters->addItem(QString::fromStdString(name));
            ui->ComboBox_parameters->setCurrentIndex(ui->ComboBox_parameters->count()-1);
        }
        delete(dial);

        break;
    }
    case Type::satellite:{
        sourceParametersDialog *dial = new sourceParametersDialog(settings,this);
        dial->addBandNames(bands);

        dial->addStationModel(allStationPlusGroupModel);
        dial->addBaselineModel(allBaselinePlusGroupModel);
        dial->addDefaultParameters(paraSrc["default"]);

        int result = dial->exec();
        if(result == QDialog::Accepted){
            std::pair<std::string, VieVS::ParameterSettings::ParametersSources> res = dial->getParameters();
            std::string name = res.first;
            VieVS::ParameterSettings::ParametersSources parameter = res.second;

            paraSrc[name] = parameter;

            ui->ComboBox_parameters->addItem(QString::fromStdString(name));
            ui->ComboBox_parameters->setCurrentIndex(ui->ComboBox_parameters->count()-1);
        }
        delete(dial);

        break;
    }
    case Type::spacecraft:{
        sourceParametersDialog *dial = new sourceParametersDialog(settings,this);
        dial->addBandNames(bands);

        dial->addStationModel(allStationPlusGroupModel);
        dial->addBaselineModel(allBaselinePlusGroupModel);
        dial->addDefaultParameters(paraSrc["default"]);

        int result = dial->exec();
        if(result == QDialog::Accepted){
            std::pair<std::string, VieVS::ParameterSettings::ParametersSources> res = dial->getParameters();
            std::string name = res.first;
            VieVS::ParameterSettings::ParametersSources parameter = res.second;

            paraSrc[name] = parameter;

            ui->ComboBox_parameters->addItem(QString::fromStdString(name));
            ui->ComboBox_parameters->setCurrentIndex(ui->ComboBox_parameters->count()-1);
        }
        delete(dial);

        break;
    }
    }
}

void setupWidget::on_pushButton_addSetup_clicked()
{
    VieVS::ParameterSetup ps;

    unsigned int startt = minDateTime.secsTo(ui->DateTimeEdit_start->dateTime());
    unsigned int endt = minDateTime.secsTo(ui->DateTimeEdit_end->dateTime());
    VieVS::ParameterSetup::Transition trans;
    if(ui->comboBox_transition->currentText() == "smooth"){
        trans = VieVS::ParameterSetup::Transition::smooth;
    }else{
        trans = VieVS::ParameterSetup::Transition::hard;
    }

    std::map<std::string, std::vector<std::string>> groups;

    bool isGroup = groups.find(ui->comboBox_members->currentText().toStdString() ) != groups.end();
    if(isGroup){
        std::string parameterName = ui->ComboBox_parameters->currentText().toStdString();
        std::string groupName = ui->comboBox_members->currentText().toStdString();
        std::vector<std::string> groupMembers = groups.at(ui->comboBox_members->currentText().toStdString());
        ps = VieVS::ParameterSetup(parameterName,
                                      groupName,
                                      groupMembers,
                                      startt,
                                      endt,
                                      trans);
    }else{
        std::string parameterName = ui->ComboBox_parameters->currentText().toStdString();
        std::string stationName = ui->comboBox_members->currentText().toStdString();
        ps = VieVS::ParameterSetup(parameterName,
                                      stationName,
                                      startt,
                                      endt,
                                      trans);
    }

    int errorCode = setupTree.refChildren().at(0).addChild(ps);

    if (errorCode != 0) {
        QString txt;
        switch (errorCode) {
        case 1: txt = "Conflict with parent: child contains all stations but parent object does not! Always make sure that all stations in child are also part of parent."; break;
        case 2: txt = "Conflict with parent: time span of child is not part of time span of parent!"; break;
        case 3: txt = "Conflict with parent: at least one of the stations in child are not part of parent! Always make sure that all stations in child are also part of parent."; break;
        case 4: txt = "Conflict with sibling: overlapping time series with at least one sibling and at least one of the siblings or new setup contains all stations"; break;
        case 5: txt = "Conflict with sibling: overlapping time series with at least one sibling and somehow there are no members in at least one sibling or in the new setup... maybe error with a group."; break;
        case 6: txt = "Conflict with sibling: overpassing time series with at least one sibling and at least one station is part of a sibling! "; break;
        default: txt = "Child could not be added... wired error... please report to developers! This should not have happened :-) "; break;
        }

        QMessageBox ms;
        ms.warning(this,"Invalid child",txt);
    } else {
        ui->treeWidget_setup->clear();

        QTreeWidgetItem *c = new QTreeWidgetItem();
        drawTable(setupTree, c);
        ui->treeWidget_setup->addTopLevelItem(c);
        ui->treeWidget_setup->expandAll();


        drawSetupPlot();
    }
}

void setupWidget::drawTable(const VieVS::ParameterSetup &setup, QTreeWidgetItem *c)
{
    bool isGroup = groups->find(setup.getMemberName() ) != groups->end();
    QIcon ic;
    if(isGroup || setup.getMemberName() == "__all__"){
        ic = icon_group;
    }else{
        ic = icon_single;
    }

    c->setIcon(0,ic);
    c->setText(0,QString::fromStdString(setup.getMemberName()));
    c->setText(1,QString::fromStdString(setup.getParameterName()));
    QDateTime start = minDateTime.addSecs(setup.getStart());
    QDateTime end = minDateTime.addSecs(setup.getEnd());
    c->setText(2,start.toString("dd.MM.yyyy hh:mm"));
    c->setText(3,end.toString("dd.MM.yyyy hh:mm"));
    if(setup.getTransition() == VieVS::ParameterSetup::Transition::hard){
        c->setText(4,"hard");
    }else{
        c->setText(4,"smooth");
    }

    for(const auto any: setup.getChildren()){
        QTreeWidgetItem *c_new = new QTreeWidgetItem();
        drawTable(any, c_new);
        c->addChild(c_new);
    }
}

void setupWidget::drawSetupPlot()
{
    QChartView *cv = qobject_cast<QChartView *>(ui->verticalLayout_setupChart->itemAt(1)->widget());
    QChart * ss = cv->chart();
    ss->removeAllSeries();
    QString name = ui->comboBox_setup->currentText();
    ss->setTitle(QString("Setup %1").arg(name));


    QTreeWidgetItem *root = ui->treeWidget_setup->topLevelItem(0);
    plotParameter(ss, root, 0, 0, name);
    QDateTime start = QDateTime::fromString(root->text(2),"dd.MM.yyyy hh:mm");
    QDateTime end = QDateTime::fromString(root->text(3),"dd.MM.yyyy hh:mm");

    auto axes = ss->axes();
    axes.at(0)->setMin(start);
    axes.at(0)->setMax(end);
    axes.at(1)->setMin(-10);
    axes.at(1)->setMax(1);
}

int setupWidget::plotParameter(QChart* chart, QTreeWidgetItem *root, int level, int plot, QString target){
    QDateTime start = QDateTime::fromString(root->text(2),"dd.MM.yyyy hh:mm");
    QDateTime end = QDateTime::fromString(root->text(3),"dd.MM.yyyy hh:mm");

    QLineSeries *series = new QLineSeries();

//    connect(series,SIGNAL(clicked(QPointF)),this,SLOT(worldmap_clicked(QPointF)));
    series->setName(root->text(1));

    QColor c;
    switch (plot%9) {
    case 0: c = QColor(228,26,28); break;
    case 1: c = QColor(200,200,200); break;
    case 2: c = QColor(55,126,184); break;
    case 3: c = QColor(77,175,74); break;
    case 4: c = QColor(152,78,163); break;
    case 5: c = QColor(255,127,0); break;
    case 6: c = QColor(255,255,51); break;
    case 7: c = QColor(166,86,40); break;
    case 8: c = QColor(247,129,191); break;
    case 9: c = QColor(153,153,153); break;
    default:c = QColor(153,153,153);break;
    }
    root->setBackgroundColor(5,c);
    series->setPen(QPen(QBrush(c),10,Qt::SolidLine,Qt::RoundCap));

    QDateTime i = start;
    while( i <= end){
        series->append(i.toMSecsSinceEpoch(),0-level);
        i = i.addSecs(60);
    }
    chart->addSeries(series);
    auto axes = chart->axes();
    if(level>9){
        axes.at(1)->setMin(-(level+1));
    }
    series->attachAxis(axes.at(1));
    series->attachAxis(axes.at(0));


    connect(series,SIGNAL(hovered(QPointF,bool)),this,SLOT(displaySetupParameterFromPlot(QPointF,bool)));

    for(int i=0; i<root->childCount(); ++i ){
        auto itm = root->child(i);
        QString member = itm->text(0);
        bool inMap = false;
        if(groups->find(member.toStdString())!=groups->end()){
            auto members = groups->at(member.toStdString());
            if (std::find(members.begin(),members.end(),target.toStdString()) != members.end()){
                inMap = true;
            }
        }
        if(member == "__all__" || inMap || member == target){
            plot = plotParameter(chart,itm,level+1, plot+1,target);
        }else{
            itm->setBackgroundColor(5,Qt::white);
            setBackgroundColorOfChildrenWhite(itm);
        }
    }
    return plot;
}

void setupWidget::displaySetupParameterFromPlot(QPointF point, bool flag){
    QLineSeries* series = qobject_cast<QLineSeries*>(sender());
    QString name = series->name();
    switch(type){
    case Type::station:{
        displayStationSetupParameter(name);
        break;
    }
    case Type::source:{
        displaySourceSetupParameter(name);
        break;
    }
    case Type::baseline:{
        displayBaselineSetupParameter(name);
        break;
    }
    case Type::satellite:{
        displaySourceSetupParameter(name);
        break;
    }
    case Type::spacecraft:{
        displaySourceSetupParameter(name);
        break;
    }
    }

    if(flag){
        callout->setAnchor(point);
        QDateTime st = QDateTime::fromMSecsSinceEpoch(series->at(0).x());
        QDateTime et = QDateTime::fromMSecsSinceEpoch(series->at(series->count()-1).x());
        QString txt = QString("Parameter: ").append(name);
        txt.append("\nfrom: ").append(st.toString("dd.MM.yyyy hh:mm"));
        txt.append("\nuntil: ").append(et.toString("dd.MM.yyyy hh:mm"));
        callout->setText(txt);
        callout->setZValue(11);
        callout->updateGeometry();
        callout->show();
    }else{
        callout->hide();
    }
}

void setupWidget::setBackgroundColorOfChildrenWhite(QTreeWidgetItem *item)
{
    for(int i=0; i<item->childCount(); ++i){
        auto itm = item->child(i);
        itm->setBackgroundColor(5,Qt::white);
        setBackgroundColorOfChildrenWhite(itm);
    }
}

void setupWidget::on_pushButton_removeSetup_clicked()
{
    QList<QTreeWidgetItem *> sel = ui->treeWidget_setup->selectedItems();
    for(int i = 0; i<sel.size(); ++i){
        if(sel.at(0)->text(1) == "multi scheduling" || !sel.at(0)->parent()){
            QMessageBox *ms = new QMessageBox;
            ms->warning(this,"Wrong selection","You can not delete top level parameters!");
        }else{
            QString txt2 = sel.at(0)->text(2);
            QString txt3 = sel.at(0)->text(3);
            QDateTime start2 = QDateTime::fromString(txt2,"dd.MM.yyyy hh:mm");
            QDateTime start3 = QDateTime::fromString(txt3,"dd.MM.yyyy hh:mm");

            unsigned int startt2 = minDateTime.secsTo(start2);
            unsigned int endt2 = minDateTime.secsTo(start3);
            std::string parameterName2 = sel.at(0)->text(1).toStdString();
            std::string memberName2 = sel.at(0)->text(0).toStdString();
            std::vector<std::string> members2;
            if(groups->find(memberName2) != groups->end()){
                members2 = groups->at(memberName2);
            }else{
                members2.push_back(memberName2);
            }
            VieVS::ParameterSetup::Transition trans2;
            if(sel.at(0)->text(4) == "smooth"){
                trans2 = VieVS::ParameterSetup::Transition::smooth;
            }else{
                trans2 = VieVS::ParameterSetup::Transition::hard;
            }

            int level=0;
            QTreeWidgetItem * t = sel.at(0);
            while(t->parent()){
                t = t->parent();
                ++level;
            }

            bool successful = setupTree.deleteChild(0,level, parameterName2, memberName2, members2, trans2, startt2, endt2);

            delete(sel.at(0));
            drawSetupPlot();
        }
    }
}


void setupWidget::on_pushButton_parseDownTime_clicked()
{
    QVector<QString> sta;
    for(int i=0; i<selectedStationModel->rowCount(); ++i){
        sta.push_back(selectedStationModel->item(i)->text());
    }

    ParseDownTimes *dial = new ParseDownTimes(sta, minDateTime, maxDateTime, this);

    int result = dial->exec();

    if(result == QDialog::Accepted){
        auto downtimes = dial->getDownTimes();

        if(!downtimes.isEmpty()){

            for(const auto any : downtimes){
                QString station = any.first;
                unsigned int downStart = any.second.first;
                unsigned int downEnd = any.second.second;
                VieVS::ParameterSetup setupDown("down",station.toStdString(),downStart,downEnd,VieVS::ParameterSetup::Transition::hard);

                setupTree.refChildren().at(0).addChild(setupDown);


                ui->treeWidget_setup->clear();

                QTreeWidgetItem *c = new QTreeWidgetItem();
                VieVS::ParameterSettings::Type setupType = VieVS::ParameterSettings::Type::station;
                drawTable(setupTree, c);
                ui->treeWidget_setup->addTopLevelItem(c);
                ui->treeWidget_setup->expandAll();

                drawSetupPlot();
            }
        }
    }

    delete(dial);
}

void setupWidget::on_pushButton_IvsDownTime_clicked()
{

    QStringList stas;
    QMap<QString,QString> tlc2station;

    for(int i=0; i<selectedStationModel->rowCount(); ++i){
        QString name = selectedStationModel->index(i,0).data().toString();
        QString tlc = selectedStationModel->index(i,1).data().toString().toUpper();
        stas << tlc;
        tlc2station[tlc] = name;
    }

    auto downtimes = qtUtil::getDownTimes(minDateTime, maxDateTime, stas);
    if(!downtimes.isEmpty()){

        for(const auto any : downtimes){
            QString downTLC = any.first.toUpper();
            std::string station = tlc2station[downTLC].toStdString();
            unsigned int downStart = any.second.first;
            unsigned int downEnd = any.second.second;
            VieVS::ParameterSetup setupDown("down",station,downStart,downEnd,VieVS::ParameterSetup::Transition::hard);

            setupTree.refChildren().at(0).addChild(setupDown);

            ui->treeWidget_setup->clear();

            QTreeWidgetItem *c = new QTreeWidgetItem();
            VieVS::ParameterSettings::Type setupType = VieVS::ParameterSettings::Type::station;
            drawTable(setupTree, c);
            ui->treeWidget_setup->addTopLevelItem(c);
            ui->treeWidget_setup->expandAll();

            drawSetupPlot();
        }
    }

}

void setupWidget::on_treeWidget_setup_itemEntered(QTreeWidgetItem *item, int column)
{
    if(column == 0){
        displaySetupMember(item->text(column));
    }else if(column == 1){
        switch (type) {
        case Type::station: {
            displayStationSetupParameter(item->text(column));
            break;
        }
        case Type::baseline: {
            displayBaselineSetupParameter(item->text(column));
            break;
        }
        case Type::source: {
            displaySourceSetupParameter(item->text(column));
            break;
        }
        case Type::satellite: {
            displaySourceSetupParameter(item->text(column));
            break;
        }
        case Type::spacecraft: {
            displaySourceSetupParameter(item->text(column));
            break;
        }
        }
    }
}

void setupWidget::on_DateTimeEdit_start_dateTimeChanged(const QDateTime &dateTime)
{
    QDateTime processed = dateTime;
    if(processed < minDateTime){
        processed = minDateTime;
    }
    if(processed > maxDateTime){
        processed = maxDateTime;
    }
    if(processed > ui->DateTimeEdit_end->dateTime()){
        ui->DateTimeEdit_end->setDateTime(processed);
    }
    ui->DateTimeEdit_start->setDateTime(processed);
}

void setupWidget::on_DateTimeEdit_end_dateTimeChanged(const QDateTime &dateTime)
{
    QDateTime processed = dateTime;
    if(processed < minDateTime){
        processed = minDateTime;
    }
    if(processed > maxDateTime){
        processed = maxDateTime;
    }
    if(processed < ui->DateTimeEdit_start->dateTime()){
        ui->DateTimeEdit_start->setDateTime(processed);
    }
    ui->DateTimeEdit_end->setDateTime(processed);
}

void setupWidget::on_comboBox_setup_currentTextChanged(const QString &arg1)
{
    drawSetupPlot();
    if(!arg1.isEmpty()){
        displaySetupMember(arg1);
    }
}

QPushButton *setupWidget::addGroupButton() const
{
    return ui->pushButton_addGroup;
}

QComboBox *setupWidget::memberComboBox() const
{
    return ui->comboBox_members;
}

QComboBox *setupWidget::setupComboBox() const
{
    return ui->comboBox_setup;
}

QSplitter *setupWidget::vertical_splitter() const
{
    return ui->splitter_vertical;
}

QSplitter *setupWidget::horizontal_splitter() const
{
    return ui->splitter_horizontal;
}

void setupWidget::clearParameters()
{
    paraSta.clear();
    paraSrc.clear();
    paraBl.clear();
    ui->ComboBox_parameters->clear();
}

void setupWidget::addParameter(const std::string &name, const VieVS::ParameterSettings::ParametersStations &para)
{
    paraSta[name] = para;
    ui->ComboBox_parameters->addItem(QString::fromStdString(name));
}

void setupWidget::addParameter(const std::string &name, const VieVS::ParameterSettings::ParametersSources &para)
{
    paraSrc[name] = para;
    ui->ComboBox_parameters->addItem(QString::fromStdString(name));
}

void setupWidget::addParameter(const std::string &name, const VieVS::ParameterSettings::ParametersBaselines &para)
{
    paraBl[name] = para;
    ui->ComboBox_parameters->addItem(QString::fromStdString(name));
}

void setupWidget::addSetup(const boost::property_tree::ptree &setup)
{
    QDateTime start_time = minDateTime;
    QDateTime end_time = maxDateTime;
    QString parameter;
    QString member;
    QString transition = "smooth";


    for(const auto & any: setup){
        if(any.first == "group" || any.first == "member"){
            member = QString::fromStdString(any.second.get_value<std::string>());
        }else if(any.first == "parameter"){
            parameter = QString::fromStdString(any.second.get_value<std::string>());
        }else if(any.first == "start"){
            QString starTimeStr = QString::fromStdString(any.second.get_value<std::string>());
            start_time = QDateTime::fromString(starTimeStr,"yyyy.MM.dd HH:mm:ss");
        }else if(any.first == "end"){
            QString endTimeStr = QString::fromStdString(any.second.get_value<std::string>());
            end_time   = QDateTime::fromString(endTimeStr,"yyyy.MM.dd HH:mm:ss");
        }else if(any.first == "transition"){
            transition = QString::fromStdString(any.second.get_value<std::string>());
        }
    }

    ui->comboBox_members->setCurrentText(member);
    ui->ComboBox_parameters->setCurrentText(parameter);
    ui->DateTimeEdit_start->setDateTime(start_time);
    ui->DateTimeEdit_end->setDateTime(end_time);
    ui->comboBox_transition->setCurrentText(transition);

    ui->pushButton_addSetup->click();

    for(const auto & any: setup){
        if(any.first == "setup"){
            addSetup(setup);
        }
    }

}

void setupWidget::on_comboBox_members_currentTextChanged(const QString &arg1)
{
    displaySetupMember(arg1);
}

void setupWidget::on_ComboBox_parameters_currentTextChanged(const QString &arg1)
{
    switch (type) {
    case Type::station: {
        displayStationSetupParameter(arg1);
        break;
    }
    case Type::baseline: {
        displayBaselineSetupParameter(arg1);
        break;
    }
    case Type::source: {
        displaySourceSetupParameter(arg1);
        break;
    }
    case Type::satellite: {
        displaySourceSetupParameter(arg1);
        break;
    }
    case Type::spacecraft: {
        displaySourceSetupParameter(arg1);
        break;
    }
    }
}
