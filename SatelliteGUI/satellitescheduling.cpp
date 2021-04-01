#include "satellitescheduling.h"
#include "ui_satellitescheduling.h"
#include <limits>
SatelliteScheduling::SatelliteScheduling(const QString &pathAntenna, const QString &pathEquip,
                                         const QString &pathPosition, const QString &pathMask,
                                         const QString &pathSat,
                                         QStandardItemModel *selectedSatelliteModel,
                                         QStandardItemModel *allSatelliteModel,
                                         QStandardItemModel *allSatellitePlusGroupModel,
                                         std::map<std::string, std::vector<std::string>> *groupSat,
                                         QDateTime startTime, QDateTime endTime,
                                         const QStringList &stations,
                                         boost::property_tree::ptree *settings,
                                         QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SatelliteScheduling),
    allSatelliteModel{allSatelliteModel},
    selectedSatelliteModel{selectedSatelliteModel},
    allSatellitePlusGroupModel{allSatellitePlusGroupModel},  
    groupSat{groupSat}
{
    ui->setupUi(this);
    settings_ = settings;
    try {
        satellites = satelliteScheduler.readSatelliteFile(pathSat.toStdString());
    }
    catch(const char* msg)
    {
        QMessageBox::warning(this,"Error loading satellites!",msg);
        return;
    }
    if(satellites.empty()){
        QMessageBox::warning(this,"No satellites found!","There was no satellite information provided within the selected file!");
        return;
    }
    ui->stackedWidget->setCurrentIndex(0);
    ui->dateTimeEdit_sessionStart->setDateTime(startTime);
    ui->dateTimeEdit_sessionEnd->setDateTime(endTime);
    sessionStart_ = startTime;
    sessionEnd_ = endTime;

    boost::posix_time::ptime start(boost::gregorian::date( startTime.date().year(), startTime.date().month(), startTime.date().day()),
                                   boost::posix_time::time_duration( startTime.time().hour(), startTime.time().minute(), startTime.time().second()));
    boost::posix_time::ptime end(boost::gregorian::date( endTime.date().year(), endTime.date().month(), endTime.date().day()),
                                   boost::posix_time::time_duration( endTime.time().hour(), endTime.time().minute(), endTime.time().second()));

    std::vector<std::string> stationList;
    for(const auto &sta : stations){
        stationList.push_back(sta.toStdString());
        ui->listWidget_listOfStations->addItem(new QListWidgetItem(QIcon(":/icons/icons/station.png"),sta));
    }
    std::string antenna = pathAntenna.toStdString();
    std::string equip = pathEquip.toStdString();
    std::string position = pathPosition.toStdString();
    std::string mask = pathMask.toStdString();

    satelliteScheduler.initialize(antenna,equip,position,mask,start,end,stationList);
    ui->treeView_selected->setModel(selectedSatelliteModel);
    ui->treeView_selected->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    MultiColumnSortFilterProxyModel *allSatelliteProxyModel = new MultiColumnSortFilterProxyModel(this);
    allSatelliteProxyModel->setSourceModel(allSatelliteModel);
    allSatelliteProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    allSatelliteProxyModel->setFilterKeyColumns({0});
    ui->treeView_available->setModel(allSatelliteProxyModel);
    ui->treeView_available->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    MultiColumnSortFilterProxyModel *pModelElevation = new MultiColumnSortFilterProxyModel(this);
    pModelElevation->setSourceModel(allSatelliteModel);
    pModelElevation->setFilterCaseSensitivity(Qt::CaseInsensitive);
    pModelElevation->setFilterKeyColumns({0});
    ui->treeView_satellites->setModel(pModelElevation);
    ui->treeView_satellites->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    MultiColumnSortFilterProxyModel *pModelObsSingle = new MultiColumnSortFilterProxyModel(this);
    pModelObsSingle->setSourceModel(allSatelliteModel);
    pModelObsSingle->setFilterCaseSensitivity(Qt::CaseInsensitive);
    pModelObsSingle->setFilterKeyColumns({0});
    ui->treeView_satelliteListStatistics->setModel(pModelObsSingle);
    ui->treeView_satelliteListStatistics->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ElevationSetup();
    satelliteStatisticsSetup();
    StackedBarPlotSetup();

    // worldmap
    worldmap = new ChartView(this);
    qtUtil::worldMap(worldmap);
    worldMapCallout = new Callout(worldmap->chart());
    worldMapCallout->hide();
    ui->horizontalLayout_map->insertWidget(0,worldmap,10);

    QChart *worldChart = worldmap->chart();
    selectedStations = new QScatterSeries(worldChart);
    QImage img(":/icons/icons/station_white.png");
    img = img.scaled(35,35);
    selectedStations->setBrush(QBrush(img));
    selectedStations->setMarkerSize(35);
    selectedStations->setPen(QColor(Qt::transparent));
    selectedStations->setName("selectedStations");

    worldChart->addSeries(selectedStations);

    connect(selectedStations,SIGNAL(hovered(QPointF,bool)),this,SLOT(worldmap_hovered(QPointF,bool)));

    for(const auto &any : satelliteScheduler.refNetwork().getStations()){
        double lat = any.getPosition()->getLat()*180/3.141592653589793;
        double lon = any.getPosition()->getLon()*180/3.141592653589793;
        selectedStations->append(lon,lat);
    }

    selectedStations->attachAxis(worldChart->axes(Qt::Horizontal).back());
    selectedStations->attachAxis(worldChart->axes(Qt::Vertical).back());

    ui->dateTimeEdit_showTime->setDateTime(startTime);
    ui->dateTimeEdit_showTime->setDisplayFormat("dd.MM.yyyy HH:mm");
    auto dt = startTime.secsTo(endTime);
    ui->horizontalSlider_adjustTime->setMinimum(0);
    ui->horizontalSlider_adjustTime->setMaximum(dt);

    ui->tableWidget_adjust->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeWidget_template->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeWidget_selectedScanPlots->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeWidget_listOfSelectedScans->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

SatelliteScheduling::~SatelliteScheduling()
{
    delete ui;
}

boost::property_tree::ptree SatelliteScheduling::toPropertyTree()
{
    boost::property_tree::ptree tree;

    for(const auto &scan : scheduledScans){
        unsigned long srcid = scan.getSourceId();
        std::string sourceName = satellites[srcid].getName();
        boost::property_tree::ptree scan_tree = scan.toPropertyTree(satelliteScheduler.refNetwork(), sourceName);
        tree.add_child("a_priori_scans.scan", scan_tree.get_child("scan"));
    }
    return tree;
}

void SatelliteScheduling::on_actionSatellite_triggered()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void SatelliteScheduling::on_actionScan_triggered()
{
    on_pushButton_process_clicked();
}

void SatelliteScheduling::on_lineEdit_availableFilter_textChanged(const QString &arg1)
{
    MultiColumnSortFilterProxyModel *proxy = qobject_cast<MultiColumnSortFilterProxyModel *>(ui->treeView_available->model());
    proxy->addFilterFixedString(arg1);
}

void SatelliteScheduling::on_treeView_available_clicked(const QModelIndex &index)
{
    int row = index.row();
    MultiColumnSortFilterProxyModel *proxy = qobject_cast<MultiColumnSortFilterProxyModel *>(ui->treeView_available->model());

    QString name = proxy->index(row,0).data().toString();

    QDateTime satEpoch;
    for(SatelliteForGUI &sat : satellites){
        if(name == QString::fromStdString(sat.getName())) {
            DateTime ti = sat.getTleData()->Epoch();
            satEpoch = QDateTime(QDate(ti.Year(),ti.Month(),ti.Day()), QTime(ti.Hour(),ti.Minute(),ti.Second()));
        }
    }

    if( selectedSatelliteModel->findItems(name,Qt::MatchExactly).isEmpty()){

        selectedSatelliteModel->insertRow(0);

        int nrow = allSatelliteModel->findItems(name).at(0)->row();
        for(int i=0; i<allSatelliteModel->columnCount(); ++i){
            selectedSatelliteModel->setItem(0, i, allSatelliteModel->item(nrow,i)->clone() );
        }

        selectedSatelliteModel->sort(0);

        int r = 0;
        for(int i = 0; i<allSatellitePlusGroupModel->rowCount(); ++i){
            QString txt = allSatellitePlusGroupModel->item(i)->text();
            if(groupSat->find(txt.toStdString()) != groupSat->end() || txt == "__all__") {
                ++r;
                continue;
            }
            if(txt>name) {
                break;
            } else {
                ++r;
            }
        }
        allSatellitePlusGroupModel->insertRow(r,new QStandardItem(QIcon(":/icons/icons/satellite.png"),name));
    }
    ui->lineEdit_availableFilter->setFocus();
    ui->lineEdit_availableFilter->selectAll();

    if(ui->checkBox_showTracks->isChecked()) {
        on_checkBox_showTracks_clicked(true);
    }
}


void SatelliteScheduling::on_treeView_selected_clicked(const QModelIndex &index)
{
    int row = index.row();
    QString Satname = QString::fromStdString(selectedSatelliteModel->item(row)->text().toStdString());
    auto series = worldmap->chart()->series();
    for(int i=0; i<series.count();++i)
    {
        QString name = series.at(i)->name();
        name.remove(0, 3);
        if(Satname == name) {
           worldmap->chart()->removeSeries(series.at(i));
        }
    }
    selectedSatelliteModel->removeRow(row);
}


void SatelliteScheduling::on_dateTimeEdit_showTime_dateTimeChanged(const QDateTime &dateTime)
{
    auto start = ui->dateTimeEdit_sessionStart->dateTime();
    auto dt = start.secsTo(dateTime);
    ui->horizontalSlider_adjustTime->setValue(dt);
}

void SatelliteScheduling::on_horizontalSlider_adjustTime_valueChanged(int value)
{
    auto start = ui->dateTimeEdit_sessionStart->dateTime();
    DateTime t = DateTime(sessionStart_.date().year(),sessionStart_.date().month(),sessionStart_.date().day(),sessionStart_.time().hour(),sessionStart_.time().minute(),sessionStart_.time().second());
    start = start.addSecs(value);
    ui->dateTimeEdit_showTime->setDateTime(start);
    int scanDur = sessionStart_.secsTo(ui->dateTimeEdit_showTime->dateTime());

    QScatterSeries *satelliteMarker = new QScatterSeries(worldmap->chart());
    QImage img(":/icons/icons/satellite.png");
    satelliteMarker->setBrush(QBrush(img));
    satelliteMarker->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    satelliteMarker->setMarkerSize(40);
    satelliteMarker->setPen(QColor(Qt::transparent));

    if(ui->checkBox_showTracks->isChecked())
    {
        auto series = worldmap->chart()->series();
        for(int i=0; i<series.count();++i)
        {
            QString name = series.at(i)->name();        
            QString leftSide = name.left(3);
            if((leftSide == "mrk"))
            {
               QScatterSeries *satelliteMarker = new QScatterSeries(worldmap->chart());
               satelliteMarker->clear();
               QImage img(":/icons/icons/satellite.png");
               satelliteMarker->setBrush(QBrush(img));
               satelliteMarker->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
               satelliteMarker->setMarkerSize(40);
               satelliteMarker->setPen(QColor(Qt::transparent));
               worldmap->chart()->addSeries(satelliteMarker);
               satelliteMarker->attachAxis(worldmap->chart()->axes(Qt::Horizontal).back());
               satelliteMarker->attachAxis(worldmap->chart()->axes(Qt::Vertical).back());
               worldmap->chart()->removeSeries(series.at(i));
               name =name.remove(0,3);
               for(SatelliteForGUI &sat : satellites) {
                   QString Satname = QString::fromStdString(sat.getName());
                   if(Satname == name) {
                      satelliteMarker->setName("mrk" + name);
                      CoordGeodetic pos = sat.getPosition(t.AddMinutes(scanDur/60));
                      satelliteMarker->append(pos.longitude*180/3.141592653589793,pos.latitude*180/3.141592653589793);
                      break;
                   }
               }
               connect(satelliteMarker,SIGNAL(hovered(QPointF,bool)),this,SLOT(sattrack_hovered(QPointF,bool)));
            }
        }
    }
}

void SatelliteScheduling::worldmap_hovered(QPointF point, bool state)
{
    if (state)
    {
        QString sta;
        for(size_t i = 0; i<satelliteScheduler.refNetwork().refStations().size();++i){
            double x = satelliteScheduler.refNetwork().refStations().at(i).getPosition()->getLon()*180/3.141592653589793;
            double y = satelliteScheduler.refNetwork().refStations().at(i).getPosition()->getLat()*180/3.141592653589793;
            QString name = QString::fromStdString(satelliteScheduler.refNetwork().refStations().at(i).getName());
            QString id = QString::fromStdString(satelliteScheduler.refNetwork().refStations().at(i).getAlternativeName());

            auto dx = x-point.x();
            auto dy = y-point.y();
            if(dx*dx+dy*dy < 1e-3) {
                if(sta.size()==0) {
                    sta.append(QString("%1 (%2)").arg(name).arg(id));
                } else {
                    sta.append(",").append(QString("%1 (%2)").arg(name).arg(id));
                }
            }
        }
        QString lon = QString().sprintf("lon: %.2f [deg]\n", point.x());
        QString lat = QString().sprintf("lat: %.2f [deg]", point.y());

        QString txt;
        txt.append(sta +"\n").append(lon).append(lat);
        worldMapCallout->setText(txt);
        worldMapCallout->setAnchor(point);
        worldMapCallout->setZValue(11);
        worldMapCallout->updateGeometry();
        worldMapCallout->show();
    }
    else {
        worldMapCallout->hide();
    }
}

void SatelliteScheduling::sattrack_hovered(QPointF point,bool state)
{
    if(state) {
        QObject *obj = sender();
        QScatterSeries *series = static_cast<QScatterSeries *>(obj);
        //QChart *chart = qobject_cast<QChart *>(obj->parent()->parent());
        QString name = series->name();
        name = name.remove(0, 3);
        QString lon = QString().sprintf("lon: %.2f [deg]\n", point.x());
        QString lat = QString().sprintf("lat: %.2f [deg]", point.y());
        QString txt;
        txt.append(name + "\n").append(lon).append(lat);
        worldMapCallout->setText(txt);
        worldMapCallout->setAnchor(point);
        worldMapCallout->setZValue(11);
        worldMapCallout->updateGeometry();
        worldMapCallout->show();
    }
    else {
       worldMapCallout->hide();
    }
}

void SatelliteScheduling::on_pushButton_process_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->label_numScans->setText(QString::number(scheduledScans.size()));
    ui->treeWidget_template->clear();
    std::vector<SatelliteForGUI> selectedSatellites;
    std::vector<std::string> selectedSatellitesNames;

    for (int i = 0; i < selectedSatelliteModel->rowCount(); ++i) {
        std::string name = selectedSatelliteModel->item(i,0)->text().toStdString();
        selectedSatellitesNames.push_back(name);
    }

    for (const auto &selectedSatellitesName : selectedSatellitesNames) {
        for (const auto &sat : satellites) {
            if (sat.hasName(selectedSatellitesName)){
                selectedSatellites.push_back(sat);
                break;
            }
        }
    }

    for (auto &any : satelliteScheduler.refNetwork().refStations()) {
        int preob = ui->spinBox_preob->value();
        int fieldSystem = ui->spinBox_fs->value();
        any.referencePARA().preob = preob;
        any.referencePARA().systemDelay = fieldSystem;
    }

    auto scans = satelliteScheduler.generateScanList(selectedSatellites);

    for (const auto &scan : scans) {
        QTreeWidgetItem *twi = new QTreeWidgetItem();
        QString srcname;
        for(const auto &sat : selectedSatellites){
            if(sat.getId() == scan.getSourceId()){
                srcname = QString::fromStdString(sat.getName());
                break;
            }
        }
        twi->setIcon(0,QIcon(":/icons/icons/satellite.png"));
        twi->setText(0,srcname);

        QDateTime scanStart = ui->dateTimeEdit_sessionStart->dateTime().addSecs(scan.getTimes().getScanTime(VieVS::Timestamp::start));
        QDateTime scanEnd = ui->dateTimeEdit_sessionStart->dateTime().addSecs(scan.getTimes().getScanTime(VieVS::Timestamp::end));
        int scanDur = scanStart.secsTo(scanEnd);

        twi->setText(1,scanStart.toString("dd.MM.yyyy hh:mm:ss"));
        twi->setText(2,scanEnd.toString("dd.MM.yyyy hh:mm:ss"));
        twi->setText(3,QString::number(scanDur));
        twi->setTextAlignment(3,Qt::AlignRight);

        std::string stations;
        for(int i = 0; i<scan.getNSta(); ++i){
            auto staid = scan.getPointingVector(i).getStaid();
            stations.append(satelliteScheduler.refNetwork().getStation(staid).getAlternativeName());
            stations.append(" ");

            QDateTime staStart = ui->dateTimeEdit_sessionStart->dateTime().addSecs(scan.getTimes().getObservingTime(i,VieVS::Timestamp::start));
            QDateTime staEnd = ui->dateTimeEdit_sessionStart->dateTime().addSecs(scan.getTimes().getObservingTime(i,VieVS::Timestamp::end));
            QString staName = QString::fromStdString(satelliteScheduler.refNetwork().getStation(staid).getName());
            int staDur = staStart.secsTo(staEnd);

            twi->addChild(new QTreeWidgetItem( QStringList() << "" << staStart.toString("dd.MM.yyyy hh:mm:ss") << staEnd.toString("dd.MM.yyyy hh:mm:ss") << QString::number(staDur) << staName ));
            twi->child(twi->childCount()-1)->setTextAlignment(3,Qt::AlignRight);
            twi->child(twi->childCount()-1)->setIcon(4,QIcon(":/icons/icons/station.png"));
        }
        twi->setText(4,QString::fromStdString(stations));
        twi->setExpanded(false);
        twi->setIcon(4,QIcon(":/icons/icons/station_group.png"));
        ui->treeWidget_template->addTopLevelItem(twi);
    }
}

void SatelliteScheduling::writeLists()
{
    ui->treeWidget_listOfSelectedScans->clear();
    ui->treeWidget_selectedScanPlots->clear();

    for (const auto &scan : scheduledScans) {
        QTreeWidgetItem *twi = new QTreeWidgetItem();
        QString srcname;
        for(const auto &sat : satellites){
            if(sat.getId() == scan.getSourceId()){
                srcname = QString::fromStdString(sat.getName());
                break;
            }
        }
        twi->setIcon(0,QIcon(":/icons/icons/satellite.png"));
        twi->setText(0,srcname);

        QDateTime scanStart = ui->dateTimeEdit_sessionStart->dateTime().addSecs(scan.getTimes().getScanTime(VieVS::Timestamp::start));
        QDateTime scanEnd = ui->dateTimeEdit_sessionStart->dateTime().addSecs(scan.getTimes().getScanTime(VieVS::Timestamp::end));
        int scanDur = scanStart.secsTo(scanEnd);

        twi->setText(1,scanStart.toString("dd.MM.yyyy hh:mm:ss"));
        twi->setText(2,scanEnd.toString("dd.MM.yyyy hh:mm:ss"));
        twi->setText(3,QString::number(scanDur));
        twi->setTextAlignment(3,Qt::AlignRight);

        std::string stations;
        for(int i = 0; i<scan.getNSta(); ++i) {
            auto staid = scan.getPointingVector(i).getStaid();
            stations.append(satelliteScheduler.refNetwork().getStation(staid).getAlternativeName());
            stations.append(" ");

            QDateTime staStart = ui->dateTimeEdit_sessionStart->dateTime().addSecs(scan.getTimes().getObservingTime(i,VieVS::Timestamp::start));
            QDateTime staEnd = ui->dateTimeEdit_sessionStart->dateTime().addSecs(scan.getTimes().getObservingTime(i,VieVS::Timestamp::end));
            QString staName = QString::fromStdString(satelliteScheduler.refNetwork().getStation(staid).getName());
            int staDur = staStart.secsTo(staEnd);

            twi->addChild(new QTreeWidgetItem( QStringList() << "" << staStart.toString("dd.MM.yyyy hh:mm:ss") << staEnd.toString("dd.MM.yyyy hh:mm:ss") << QString::number(staDur) << staName ));
            twi->child(twi->childCount()-1)->setTextAlignment(3,Qt::AlignRight);
            twi->child(twi->childCount()-1)->setIcon(4,QIcon(":/icons/icons/station.png"));
        }
        twi->setText(4,QString::fromStdString(stations));
        twi->setExpanded(false);
        twi->setIcon(4,QIcon(":/icons/icons/station_group.png"));
        QTreeWidgetItem * twiClone = twi->clone();
        ui->treeWidget_listOfSelectedScans->addTopLevelItem(twi);
        ui->treeWidget_selectedScanPlots->addTopLevelItem(twiClone);
    }
}

void SatelliteScheduling::on_actionInfo_triggered()
{
    writeLists();
    ui->stackedWidget->setCurrentIndex(2);
}

void SatelliteScheduling::on_pushButton_showConstant_clicked()
{
    setTimes *dialog = new setTimes(this, ui->spinBox_fs->value(), ui->spinBox_preob->value());
    int result = dialog->exec();

    if(result == QDialog::Accepted) {
        int fs = dialog->getValues().at(0);
        int preob = dialog->getValues().at(1);
        ui->spinBox_fs->setValue(fs);
        ui->spinBox_preob->setValue(preob);
    }
    delete(dialog);
}

void SatelliteScheduling::on_treeWidget_template_itemClicked(QTreeWidgetItem *item)//, int column)
{
    if(item->parent()) {
        item = item->parent();
    }

    QString sat = item->text(0);
    ui->label_adjustSatName->setText(sat);

    QTableWidget *t = ui->tableWidget_adjust;
    t->clear();
    t->setRowCount(0);
    t->setHorizontalHeaderItem(0,new QTableWidgetItem(""));
    t->setHorizontalHeaderItem(1,new QTableWidgetItem("station"));
    t->setHorizontalHeaderItem(2,new QTableWidgetItem("start"));
    t->setHorizontalHeaderItem(3,new QTableWidgetItem("end"));
    t->setHorizontalHeaderItem(4,new QTableWidgetItem("duration"));

    for (int i = 0; i < item->childCount(); ++i) {
        t->insertRow(i);
        const auto &child = item->child(i);

        QDateTime min = QDateTime::fromString(child->text(1),"dd.MM.yyyy hh:mm:ss");
        QDateTime max = QDateTime::fromString(child->text(2),"dd.MM.yyyy hh:mm:ss");

        QWidget *checkBoxWidget = new QWidget();
        QCheckBox *cb = new QCheckBox();
        QHBoxLayout *l = new QHBoxLayout(checkBoxWidget);
        l->addWidget(cb);
        l->setAlignment(Qt::AlignCenter);
        l->setContentsMargins(0,0,0,0);
        cb->setCheckState(Qt::Checked);
        t->setCellWidget(i,0,checkBoxWidget);

        QString stationName = child->text(4);
        QTableWidgetItem *staItem = new QTableWidgetItem(QIcon(":/icons/icons/station.png"),stationName);
        staItem->setFlags(staItem->flags() ^ Qt::ItemIsEditable);
        t->setItem(i,1,staItem);

        QDateTimeEdit *start = new QDateTimeEdit();
        QDateTimeEdit *end = new QDateTimeEdit();
        start->setDisplayFormat("dd.MM.yyyy hh:mm:ss");
        end->setDisplayFormat("dd.MM.yyyy hh:mm:ss");
        start->setDateTime(min);
        end->setDateTime(max);
        t->setCellWidget(i,2,start);
        t->setCellWidget(i,3,end);

        QSpinBox *sp = new QSpinBox();
        sp->setReadOnly(false);
        sp->setMaximum(999999);
        sp->setButtonSymbols(QAbstractSpinBox::NoButtons);
        int d = min.secsTo(max);
        sp->setValue(d);
        t->setCellWidget(i,4,sp);
        if (d >= 900 || d < 20) {
            QPalette pal = sp->palette();
            pal.setColor(sp->backgroundRole(), Qt::red);
            sp->setPalette(pal);
        } else if(d >= 600){
            QPalette pal = sp->palette();
            pal.setColor(sp->backgroundRole(), QColor(253,174,107));
            sp->setPalette(pal);
        } else {
            QPalette pal = sp->palette();
            pal.setColor(sp->backgroundRole(), Qt::white);
            sp->setPalette(pal);
        }

        auto eval_start= [start, end, sp, min, max](){
            if(start->dateTime() < min){
                start->setDateTime(min);
            }
            if(start->dateTime() > max){
                start->setDateTime(max);
            }
            if(start->dateTime() > end->dateTime()){
                end->setDateTime(start->dateTime());
            }
            int d = start->dateTime().secsTo(end->dateTime());
            if (d >= 900 || d < 20) {
                QPalette pal = sp->palette();
                pal.setColor(sp->backgroundRole(), Qt::red);
                sp->setPalette(pal);
            } else if(d >= 600){
                QPalette pal = sp->palette();
                pal.setColor(sp->backgroundRole(), QColor(253,174,107));
                sp->setPalette(pal);
            } else {
                QPalette pal = sp->palette();
                pal.setColor(sp->backgroundRole(), Qt::white);
                sp->setPalette(pal);
            }
            sp->setValue(d);
            return;
        };

        auto eval_end= [start, end, sp, min, max](){
            if(end->dateTime() < min){
                end->setDateTime(min);
            }
            if(end->dateTime() > max){
                end->setDateTime(max);
            }
            if(start->dateTime() > end->dateTime()){
                start->setDateTime(end->dateTime());
            }
            int d = start->dateTime().secsTo(end->dateTime());
            if (d >= 900 || d < 20) {
                QPalette pal = sp->palette();
                pal.setColor(sp->backgroundRole(), Qt::red);
                sp->setPalette(pal);
            } else if(d >= 600){
                QPalette pal = sp->palette();
                pal.setColor(sp->backgroundRole(), QColor(253,174,107));
                sp->setPalette(pal);
            } else {
                QPalette pal = sp->palette();
                pal.setColor(sp->backgroundRole(), Qt::white);
                sp->setPalette(pal);
            }
            sp->setValue(d);
            return;
        };

        auto eval_dur= [start, end, sp, min, max](){
            end->setDateTime(start->dateTime().addSecs(sp->value()));
            if(end->dateTime() < min){
                end->setDateTime(min);
            }
            if(end->dateTime() > max){
                end->setDateTime(max);
            }
            if(start->dateTime() > end->dateTime()){
                start->setDateTime(end->dateTime());
            }
            int d = sp->value();
            if (d >= 900 || d < 20) {
                QPalette pal = sp->palette();
                pal.setColor(sp->backgroundRole(), Qt::red);
                sp->setPalette(pal);
            } else if(d >= 600){
                QPalette pal = sp->palette();
                pal.setColor(sp->backgroundRole(), QColor(253,174,107));
                sp->setPalette(pal);
            } else {
                QPalette pal = sp->palette();
                pal.setColor(sp->backgroundRole(), Qt::white);
                sp->setPalette(pal);
            }
            end->setDateTime(start->dateTime().addSecs(d));
            return;
        };

        connect(start, &QDateTimeEdit::dateTimeChanged, eval_start);
        connect(end,   &QDateTimeEdit::dateTimeChanged, eval_end);
        connect(sp, QOverload<int>::of(&QSpinBox::valueChanged), eval_dur);
    }
    t->resizeColumnsToContents();
}

void SatelliteScheduling::on_pushButton_checkAndSave_clicked()
{
    QTableWidget *tw = ui->tableWidget_adjust;
    if(tw->rowCount() == 0){
        QMessageBox::information(this,"no selection","Please select and adjust scan first");
        return;
    }
    std::vector<unsigned long> selectedStationIds;
    std::vector<unsigned long> startTimes;
    std::vector<unsigned long> endTimes;
    SatelliteForGUI satellite;
    for(int i = 0; i<tw->rowCount(); ++i){
        QWidget *checkBoxWidget = tw->cellWidget(i,0);
        auto l = checkBoxWidget->layout();
        QCheckBox *cb = qobject_cast<QCheckBox *>(l->itemAt(0)->widget());
        if(cb->checkState() != Qt::Checked){
            continue;
        }
        QString station = tw->item(i,1)->text();
        std::string Vstation = station.toStdString();
        unsigned long staid = satelliteScheduler.refNetwork().getStation(Vstation).getId();

        QDateTime start = qobject_cast<QDateTimeEdit *>(tw->cellWidget(i,2))->dateTime();
        unsigned long Vstart = ui->dateTimeEdit_sessionStart->dateTime().secsTo(start);

        QDateTime end = qobject_cast<QDateTimeEdit *>(tw->cellWidget(i,3))->dateTime();
        unsigned long Vend = ui->dateTimeEdit_sessionStart->dateTime().secsTo(end);
        if(Vstart >= Vend){
            continue;
        }
        selectedStationIds.push_back(staid);
        startTimes.push_back(Vstart);
        endTimes.push_back(Vend);
        //int fieldSystem = ui->spinBox_fs->value();
        //int preob = ui->spinBox_preob->value();

        for (const auto &sat : satellites) {
            if (sat.hasName(ui->label_adjustSatName->text().toStdString())){
                satellite = sat;
                break;
            }
        }
    }
    DateTime start = DateTime(sessionStart_.date().year(),sessionStart_.date().month(),sessionStart_.date().day(),sessionStart_.time().hour(),sessionStart_.time().minute(),sessionStart_.time().second());
    std::vector<bool> isobs(selectedStationIds.size(),false);
    std::vector<SatelliteObs::TimePoint> timePoints;
    for(size_t i=0;i<selectedStationIds.size(); i++) {
        VieVS::Station station = satelliteScheduler.refNetwork().getStation(selectedStationIds.at(i));
        SatelliteObs::TimePoint tstart;
        tstart.ts = VieVS::Timestamp ::start;
        tstart.time = start.AddSeconds(startTimes.at(i));
        tstart.satelliteID = satellite.getId();
        tstart.stationID = i;
        timePoints.push_back(tstart);

        SatelliteObs::TimePoint tend;
        tend.ts = VieVS::Timestamp ::end;
        tend.time = start.AddSeconds(endTimes.at(i));
        tend.satelliteID = satellite.getId();
        tend.stationID = i;
        timePoints.push_back(tend);
    }
    std::sort(timePoints.begin(), timePoints.end(), SatelliteObs::compareTimePoint);
    if(timePoints.size() == 2) {
       QMessageBox::information(this,"no valid scan","There is only one station selected! No valid Scan!");
       return;
    }
    if(timePoints.at(0).time == timePoints.at(1).time) {
       unsigned long staID_0 = timePoints.at(0).stationID;
       isobs.at(staID_0) = true;
       unsigned long staID_1 = timePoints.at(1).stationID;
       isobs.at(staID_1) = true;
    }
    else {
        QMessageBox::information(this,"no valid scan","There is a station observing alone!");
        return;
    }
    for(size_t i =2; i<timePoints.size();i++) {
        unsigned long staID = timePoints.at(i).stationID;
        if(timePoints.at(i).ts == VieVS::Timestamp::start)
        {
            isobs.at(staID) = true;
        }
        if(timePoints.at(i).ts == VieVS::Timestamp::end)
        {
            isobs.at(staID) = false;
        }
        if(count(isobs.begin(), isobs.end(), true) <2)
        {
            if(i == timePoints.size()-2 && timePoints.at(i).time == timePoints.at(timePoints.size()-1).time) {
                break;
            }
            QMessageBox::information(this,"no valid scan","There is a station observing alone !");
            return;
        }
    }
    VieVS::Scan scan = satelliteScheduler.createAdjustedScan(satellite,selectedStationIds,startTimes,endTimes);
    if(checkScan(scan))
    {
       scheduledScans.push_back(scan);
       QMessageBox::information(this,"Scan added","The scan was successfully added to the schedule!");
    }
    else
    {
        QMessageBox::information(this,"overlap in observing time","There is an overlap with a scheduled Scan!");
        return;
    }
    ui->label_numScans->setText(QString::number(scheduledScans.size()));
}

bool SatelliteScheduling::checkScan(VieVS::Scan scan)
{
    bool ret = true;
    for(VieVS::Scan scheduledScan : scheduledScans)
    {
        unsigned long nSta = scheduledScan.getNSta();
        for(int i =0 ; i<nSta; i++)
        {
            VieVS::PointingVector pvSchedScanStart = scheduledScan.getPointingVector(i,VieVS::Timestamp::start);
            VieVS::PointingVector pvSchedScanEnd = scheduledScan.getPointingVector(i,VieVS::Timestamp::end);
            for(int k = 0; k<scan.getNSta();k++) {
                VieVS::PointingVector pvScanStart = scan.getPointingVector(k,VieVS::Timestamp::start);
                VieVS::PointingVector pvScanEnd = scan.getPointingVector(k,VieVS::Timestamp::end);
                if(pvSchedScanStart.getStaid() == pvScanStart.getStaid()) {
                    if(pvScanEnd.getTime() < pvSchedScanStart.getTime() || pvScanStart.getTime() > pvSchedScanEnd.getTime()) {
                       continue;
                    }
                    else {
                       ret = false;
                       return ret;
                    }
                }
                else {
                    continue;
                }
            }
        }
    }
    return ret;
}

void SatelliteScheduling::on_pushButton_removeScan_clicked()
{
    auto list = ui->treeWidget_listOfSelectedScans->selectedItems();
    const QModelIndex index = ui->treeWidget_listOfSelectedScans->selectionModel()->currentIndex();
        for(const auto& any:list){
            delete(any);
            scheduledScans.erase(scheduledScans.begin() + index.row());
        }
}

void SatelliteScheduling::on_checkBox_showTracks_clicked(bool checked)
{
    QChart *worldChart = worldmap->chart();
    auto series = worldChart->series();
    for(int i =0; i<series.size();i++) {
        QString name = series.at(i)->name();
        if(name.left(3)=="mrk" || name.left(3)=="sat") {
             worldChart->removeSeries(series.at(i));
        }
    }
    long sessionDur = sessionStart_.secsTo(sessionEnd_);
    if(checked)
    {
        for( int i=0; i< selectedSatelliteModel->rowCount(); i++){
            QString name = selectedSatelliteModel->item(i)->text();
            QScatterSeries *satelliteTrack = new QScatterSeries(worldChart);
            satelliteTrack->clear();
            satelliteTrack->setName("sat" + name);
            satelliteTrack->setBrush(QBrush(QColor(228,26,28),Qt::SolidPattern));
            satelliteTrack->setMarkerSize(2);
            satelliteTrack->setPen(QColor(228,26,28));
            satelliteTrack->setOpacity(0.5);

            QScatterSeries *satelliteMarker = new QScatterSeries(worldChart);
            satelliteMarker->clear();
            satelliteMarker->setName("mrk" + name);
            QImage img(":/icons/icons/satellite.png");
            satelliteMarker->setBrush(QBrush(img));
            satelliteMarker->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
            satelliteMarker->setMarkerSize(40);
            satelliteMarker->setPen(QColor(Qt::transparent));

            for(SatelliteForGUI &sat : satellites)
            {
                QString Satname = QString::fromStdString(sat.getName());
                if(Satname == name) {
                   int scanDur = sessionStart_.secsTo(ui->dateTimeEdit_showTime->dateTime());
                   DateTime t = DateTime(sessionStart_.date().year(),sessionStart_.date().month(),sessionStart_.date().day(),sessionStart_.time().hour(),sessionStart_.time().minute(),sessionStart_.time().second());
                   CoordGeodetic pos = sat.getPosition(t);
                   for(int i = 0;i<sessionDur/60; i++) //each Minute
                   {
                       pos = sat.getPosition(t.AddMinutes(i));
                       satelliteTrack->append(pos.longitude*180/pi,pos.latitude*180/pi);
                   }
                   pos = sat.getPosition(t.AddMinutes(scanDur/60));
                   satelliteMarker->append(pos.longitude*180/pi,pos.latitude*180/pi);

                   connect(satelliteMarker,SIGNAL(hovered(QPointF,bool)),this,SLOT(sattrack_hovered(QPointF,bool)));
                }
            }
            worldChart->addSeries(satelliteTrack);
            worldChart->addSeries(satelliteMarker);
            satelliteTrack->attachAxis(worldChart->axes(Qt::Horizontal).back());
            satelliteTrack->attachAxis(worldChart->axes(Qt::Vertical).back());
            satelliteMarker->attachAxis(worldChart->axes(Qt::Horizontal).back());
            satelliteMarker->attachAxis(worldChart->axes(Qt::Vertical).back());
        }
        worldmap->setChart(worldChart);
   }
   else
   {
        auto series = worldmap->chart()->series();
        for(int i=0; i<series.count();++i)  {
            QString name = series.at(i)->name();
            name.remove(0, 3);
            for(SatelliteForGUI &sat : satellites) {
                QString Satname = QString::fromStdString(sat.getName());
                if(Satname == name) {
                   series.at(i)->setVisible(false);
                }
            }
        }
   }
}

void SatelliteScheduling::on_checkBox_showStations_clicked(bool checked)
{
    if(checked) {
        selectedStations->setVisible(true);
    }
    else {
       selectedStations->setVisible(false);
    }
}

int SatelliteScheduling::getIndexTopLevelItem()
{
    QList<QTreeWidgetItem *> itms = ui->treeWidget_selectedScanPlots->selectedItems();
    QTreeWidgetItem *itm;
    int idxTopLevelItem;
    int childIndex;
    if(itms.at(0)->childCount() == 0) {
        itm = itms.at(0)->parent();
        childIndex = ui->treeWidget_selectedScanPlots->selectionModel()->currentIndex().row();
    }
    else {
        itm = itms.at(0);
        childIndex = NULL;
    }

    idxTopLevelItem = ui->treeWidget_selectedScanPlots->indexOfTopLevelItem(itm);
    return idxTopLevelItem;
}

void SatelliteScheduling::SetupPlotsScan()
{
    int idxTopLevelItem = getIndexTopLevelItem();
    createSkyPlotScan(idxTopLevelItem);
    ui->splitter_ScanPlots_horizontal->setStretchFactor(0,1);
    ui->splitter_ScanPlots_horizontal->setStretchFactor(1,4);
    ui->splitter_ScanPlots_horizontal->setSizes({400,1000});

    ui->splitter_ScanPlots_vertical->setStretchFactor(0,1);
    ui->splitter_ScanPlots_vertical->setStretchFactor(1,3);
    ui->splitter_ScanPlots_vertical->setSizes({1000,2000});
    createElevationPlotScan(idxTopLevelItem);
}

void SatelliteScheduling::createSkyPlotScan(int idxTopLevelItem)
{
    //remove Item
    QLayoutItem *qw_remove = ui->SkyPlot_Scan->itemAt(0);
    ui->SkyPlot_Scan->removeItem(qw_remove);
    QTreeWidgetItem *itm = ui->treeWidget_selectedScanPlots->topLevelItem(idxTopLevelItem);
    QString satName = itm->data(0,0).toString();
    const VieVS::Network &network = satelliteScheduler.refNetwork(); //.refStations() //.getNetwork();
    QVBoxLayout *layout = new QVBoxLayout();
    QComboBox *c1 = new QComboBox();
    c1->addItem(QIcon(":/icons/icons/station_group.png"),"all");
    for(size_t i=0; i<scheduledScans.at(idxTopLevelItem).getNSta();i++)
    {
        size_t idx = scheduledScans.at(idxTopLevelItem).getStationId(i);
        std::string name = network.getStation(idx).getName();
        c1->addItem(QIcon(":/icons/icons/station.png"),QString::fromStdString(name));
    }
    layout->addWidget(c1);
    QWidget *qw = new QWidget(this);
    QPolarChart *chart = new QPolarChart();
    chart->setAnimationOptions(QPolarChart::NoAnimation);
    QChartView *chartView = new QChartView(chart);
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundRoundness(0);
    chart->legend()->setAlignment(Qt::Alignment(Qt::AlignBottom));
    chart->setTitle(satName);
    chart->acceptHoverEvents();
    chartView->setMouseTracking(true);
    Callout *callout = new Callout(chart);
    callout->hide();

    QValueAxis *angularAxis = new QValueAxis();
    angularAxis->setTickCount(13); // First and last ticks are co-located on 0/360 angle.
    angularAxis->setLabelFormat("%.0f");
    angularAxis->setShadesVisible(true);
    angularAxis->setShadesBrush(QBrush(QColor(230, 238, 255)));
    angularAxis->setRange(0,360);

    QFont labelsFont;
    labelsFont.setPixelSize(15);
    chart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

    QValueAxis *radialAxis = new QValueAxis();
    radialAxis->setTickCount(10);
    radialAxis->setRange(0,90);
    radialAxis->setLabelFormat(" ");
    chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);
    chartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(chartView);
    qw->setLayout(layout);
    c1->setCurrentIndex(0);
    connect(c1,SIGNAL(currentIndexChanged(QString)), this, SLOT(updateSkyPlotScan(QString)));

    ui->SkyPlot_Scan->insertWidget(0,qw);
    updateSkyPlotScan(c1->currentText());
}


void SatelliteScheduling::updateSkyPlotScan(QString name)
{
    if(name.isEmpty()) {
        return;
    }
    QWidget *qw = qobject_cast<QWidget*>(ui->SkyPlot_Scan->itemAt(0)->widget());
    QChartView *chartView = qobject_cast<QChartView*>(qw->layout()->itemAt(1)->widget());
    QChart *chart = chartView->chart();
    chart->removeAllSeries();
    int idxTopLevelItem = getIndexTopLevelItem();
    QTreeWidgetItem *itm = ui->treeWidget_selectedScanPlots->topLevelItem(idxTopLevelItem);
    QString Satellitename = itm->data(0,0).toString();

    SGP4 *sgp4 = nullptr;
    for(SatelliteForGUI &sat : satellites) {
        QString Satname = QString::fromStdString(sat.getName());
        if(Satellitename == Satname) {
            sgp4 = sat.getSGP4Data();
        }
    }
    QDateTime start = ui->dateTimeEdit_sessionStart->dateTime();
    QDateTime end = ui->dateTimeEdit_sessionEnd->dateTime();
    const VieVS::Network &network = satelliteScheduler.refNetwork(); //.refStations() //.getNetwork();

    if(name != "all") // it is a single station, plot horizon mask
    {
        const VieVS::Station &thisSta = network.getStation(name.toStdString());
        std::pair<std::vector<double>, std::vector<double>> mask = thisSta.getHorizonMask();
        const std::vector<double> &az = mask.first;
        const std::vector<double> &el = mask.second;

        QLineSeries *hmaskUp = new QLineSeries();
        for(size_t i=0; i<az.size(); ++i){
            hmaskUp->append(az[i]*rad2deg,90-el[i]*rad2deg);
        }
        if(hmaskUp->count() == 0){
            for(int i=0; i<=360; ++i){
                hmaskUp->append(static_cast<double>(i),89);
            }
        }
        QLineSeries *hmaskDown = new QLineSeries();
        for(int i=0; i<=360; ++i){
            hmaskDown->append(static_cast<double>(i),90);
        }
        QAreaSeries *hmask = new QAreaSeries();
        hmask->setName("horizon mask");
        hmask->setUpperSeries(hmaskDown);
        hmask->setLowerSeries(hmaskUp);
        hmask->setBrush(Qt::gray);
        hmask->setOpacity(0.5);
        chart->addSeries(hmask);
        hmask->attachAxis(chart->axes(Qt::Horizontal).back());
        hmask->attachAxis(chart->axes(Qt::Vertical).back());
    }
    QList<QColor> c;
    c.append(QColor(228,26,28));
    c.append(QColor(55,126,184));
    c.append(QColor(77,175,74));
    c.append(QColor(152,78,163));
    c.append(QColor(255,127,0));
    c.append(QColor(255,255,51));
    c.append(QColor(166,86,40));
    c.append(QColor(247,129,191));
    c.append(QColor(153,153,153));

    for(int i =0; i<itm->childCount();i++)
    {
        QString station = itm->child(i)->data(4,0).toString();
        if(station == name || name == "all")
        {
            QScatterSeries *data = new QScatterSeries();
            data->setMarkerSize(2);
            int nc = i/9;
            int cc = i%9;
            if(nc == 0){
                data->setBrush(c.at(cc));
                data->setBorderColor(c.at(cc));
            } else if(nc == 1){
                data->setBrush(c.at(cc));
                data->setBorderColor(Qt::white);
            } else if(nc == 2){
                data->setBrush(c.at(cc));
                data->setBorderColor(Qt::black);
            } else if(nc == 3){
                data->setBrush(c.at(cc));
                data->setBorderColor(Qt::red);
            } else if(nc == 4){
                data->setBrush(c.at(cc));
                data->setBorderColor(Qt::blue);
            }

            QString strStart = itm->child(i)->data(1,0).toString();
            QString strEnd = itm->child(i)->data(2,0).toString();
            start = QDateTime::fromString(strStart, "dd.MM.yyyy hh:mm:ss");
            end = QDateTime::fromString(strEnd, "dd.MM.yyyy hh:mm:ss");
            int scanDur = start.secsTo(end);
            DateTime t = DateTime(start.date().year(),start.date().month(),start.date().day(),start.time().hour(),start.time().minute(),start.time().second());
            const VieVS::Station &thisSta = network.getStation(station.toStdString());
            CoordGeodetic stat = CoordGeodetic(thisSta.getPosition()->getLat(),thisSta.getPosition()->getLon(),thisSta.getPosition()->getAltitude()/1000,true);
            Observer obs( stat );
            int j= 0;
            while(j <scanDur) //scanDur in sec, every second a point
            {
               DateTime tp = t.AddSeconds(j);
               Eci eci = sgp4->FindPosition(tp);
               CoordTopocentric topo = obs.GetLookAngle(eci);
               data->append(topo.azimuth*rad2deg,90-(topo.elevation*rad2deg));
               j = j+10;
            }
            Eci eci = sgp4->FindPosition(t.AddSeconds(scanDur));
            CoordTopocentric topo = obs.GetLookAngle(eci);
            data->append(topo.azimuth*rad2deg,90-(topo.elevation*rad2deg));
            data->setName(station);
            chart->addSeries(data);
            data->attachAxis(chart->axes(Qt::Horizontal).back());
            data->attachAxis(chart->axes(Qt::Vertical).back());
        }
    }
}

void SatelliteScheduling::on_pushButton_adjustStart_clicked()
{
    QTableWidget *tw = ui->tableWidget_adjust;
    QVector<QDateTime> startTimes;
    if(tw->rowCount() == 0){
            QMessageBox::information(this,"no selection","Please select scan first");
            return;
    }
     for(int i = 0; i<tw->rowCount(); ++i) {
         QWidget *checkBoxWidget = tw->cellWidget(i,0);
         auto l = checkBoxWidget->layout();
         QCheckBox *cb = qobject_cast<QCheckBox *>(l->itemAt(0)->widget());
         if(cb->checkState() != Qt::Checked) {
              continue;
         }
         QDateTime start = qobject_cast<QDateTimeEdit *>(tw->cellWidget(i,2))->dateTime();
         startTimes.push_back(start);
     }
     QDateTime maxStart = *std::max_element(startTimes.begin(),startTimes.end());

     for(int i = 0; i<tw->rowCount(); ++i) {
         qobject_cast<QDateTimeEdit *>(tw->cellWidget(i,2))->setDateTime(maxStart);
     }
}

void SatelliteScheduling::createElevationPlotScan(int idxTopLevelItem)
{
    int count = ui->ElevationPlot_Scan->layout()->count();

    QFont labelsFont;
    labelsFont.setPixelSize(15);

    for(int i =0;i<count;i++) {
        QLayoutItem *qw = ui->ElevationPlot_Scan->itemAt(0);
        ui->ElevationPlot_Scan->removeItem(qw);
    }
    QChart *chart = new QChart();
    int sessionDur = sessionStart_.secsTo(sessionEnd_);
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("hh:mm");
    axisX->setRange(sessionStart_,sessionEnd_);
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0,90);
    axisY->setTitleText("elevation [°]");
    chart->addAxis(axisY, Qt::AlignLeft);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QList<QColor> c;
    c.append(QColor(228,26,28));
    c.append(QColor(55,126,184));
    c.append(QColor(77,175,74));
    c.append(QColor(152,78,163));
    c.append(QColor(255,127,0));
    c.append(QColor(255,255,51));
    c.append(QColor(166,86,40));
    c.append(QColor(247,129,191));
    c.append(QColor(153,153,153));

    QTreeWidgetItem *a = ui->treeWidget_selectedScanPlots->topLevelItem(idxTopLevelItem);
    QString Satellitename = a->data(0,0).toString();
    chartView->chart()->setTitle(Satellitename);
    SGP4 *sgp4 = nullptr;
    for(SatelliteForGUI &sat : satellites) {
        QString Satname = QString::fromStdString(sat.getName());
        if(Satellitename == Satname) {
            sgp4 = sat.getSGP4Data();
        }
    }
    const VieVS::Network &network = satelliteScheduler.refNetwork(); //.refStations() //.getNetwork();
    for(int i=0; i<a->childCount(); ++i)
    {
        QString name = a->child(i)->data(4,0).toString();
        QLineSeries *series = new QLineSeries();
        series->setName(name);
        chart->addSeries(series);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
        int nc = i/9;
        int cc = i%9;
        if(nc == 0) {
            series->setPen(QPen(QBrush(c.at(cc)),1.5,Qt::SolidLine));
        } else if(nc == 1) {
            series->setPen(QPen(QBrush(c.at(cc)),1.5,Qt::DashLine));
        } else if(nc == 2) {
            series->setPen(QPen(QBrush(c.at(cc)),1.5,Qt::DotLine));
        } else if(nc == 3) {
            series->setPen(QPen(QBrush(c.at(cc)),1.5,Qt::DashDotLine));
        } else if(nc == 4) {
            series->setPen(QPen(QBrush(c.at(cc)),1.5,Qt::DashDotDotLine));
        }

        QLineSeries *obsSeries = new QLineSeries();
        obsSeries->setName("obs" + name);
        chart->addSeries(obsSeries);
        obsSeries->attachAxis(axisX);
        obsSeries->attachAxis(axisY);
        obsSeries->setPen(QPen(QBrush(Qt::black),4,Qt::SolidLine));
    }
    auto stationSeries = chart->series();
    QLineSeries *serie;
    for(int i = 0; i<a->childCount();i++)
    {
        QString staName = a->child(i)->data(4,0).toString();
        const VieVS::Station &thisSta = network.getStation(staName.toStdString());
        CoordGeodetic stat = CoordGeodetic(thisSta.getPosition()->getLat(),thisSta.getPosition()->getLon(),thisSta.getPosition()->getAltitude()/1000,true);
        Observer obs( stat );

        for(const auto &any :stationSeries)
        {
            QString name = any->name();
            if(name.toStdString() == staName.toStdString())
            {
                serie = qobject_cast<QLineSeries *>(any);
                serie->clear();
                DateTime t = DateTime(sessionStart_.date().year(),sessionStart_.date().month(),sessionStart_.date().day(),sessionStart_.time().hour(),sessionStart_.time().minute(),sessionStart_.time().second());
                int counter = 0;
                while(counter<sessionDur)
                {
                    QDateTime ts = sessionStart_.addSecs(counter);
                    Eci eci = sgp4->FindPosition(t.AddSeconds(counter));
                    CoordTopocentric topo = obs.GetLookAngle(eci);
                    serie->append(ts.toMSecsSinceEpoch(),topo.elevation*rad2deg);
                    counter=counter+600;
                }
            }
            else if(name.remove(0,3).toStdString() == staName.toStdString())
            {
                //observed series
                serie = qobject_cast<QLineSeries *>(any);
                serie->clear();
                QString strStart = a->child(i)->data(1,0).toString();
                QString strEnd = a->child(i)->data(2,0).toString();
                QDateTime start = QDateTime::fromString(strStart, "dd.MM.yyyy hh:mm:ss");
                QDateTime end = QDateTime::fromString(strEnd, "dd.MM.yyyy hh:mm:ss");
                int scanDur = start.secsTo(end);
                DateTime t = DateTime(start.date().year(),start.date().month(),start.date().day(),start.time().hour(),start.time().minute(),start.time().second());
                int counter = 0;
                while(counter<scanDur)
                {
                    QDateTime ts = start.addSecs(counter);
                    Eci eci = sgp4->FindPosition(t.AddSeconds(counter));
                    CoordTopocentric topo = obs.GetLookAngle(eci);
                    serie->append(ts.toMSecsSinceEpoch(),topo.elevation*rad2deg);
                    counter=counter+10;
                }
                serie->setName("observed");
                chart->legend()->markers(serie).at(0)->setVisible(false);
            }
         }
    }
    chart->legend()->markers(serie).at(0)->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);
    chart->setAcceptHoverEvents(true);
    Callout *callout = new Callout(chart);
    callout->hide();
    ui->ElevationPlot_Scan->insertWidget(0,chartView,1);
}

void SatelliteScheduling::ElevationSetup()
{
    ui->splitter_satelliteElevation->setStretchFactor(0,1);
    ui->splitter_satelliteElevation->setStretchFactor(1,4);
    ui->splitter_satelliteElevation->setSizes({700,2000});

    QChart *chart = new QChart();

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("hh:mm");
    axisX->setRange(sessionStart_,sessionEnd_);
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0,90);
    axisY->setTitleText("elevation [°]");
    chart->addAxis(axisY, Qt::AlignLeft);

    QFont labelsFont;
    labelsFont.setPixelSize(15);

    QFont titleFont;
    titleFont.setPixelSize(15);
    titleFont.setBold(true);
    axisY->setLabelsFont(labelsFont);
    axisX->setLabelsFont(labelsFont);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QList<QColor> c;
    c.append(QColor(228,26,28));
    c.append(QColor(55,126,184));
    c.append(QColor(77,175,74));
    c.append(QColor(152,78,163));
    c.append(QColor(255,127,0));
    c.append(QColor(255,255,51));
    c.append(QColor(166,86,40));
    c.append(QColor(247,129,191));
    c.append(QColor(153,153,153));

    const VieVS::Network &network = satelliteScheduler.refNetwork();
    for(int i=0; i<network.getNSta(); ++i){
        QString name = QString::fromStdString(network.getStation(i).getName());
        QLineSeries *series = new QLineSeries();
        series->setName(name);
        chart->addSeries(series);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
        int nc = i/9;
        int cc = i%9;
        if(nc == 0) {
            series->setPen(QPen(QBrush(c.at(cc)),1.5,Qt::SolidLine));
        } else if(nc == 1) {
            series->setPen(QPen(QBrush(c.at(cc)),1.5,Qt::DashLine));
        } else if(nc == 2) {
            series->setPen(QPen(QBrush(c.at(cc)),1.5,Qt::DotLine));
        } else if(nc == 3) {
            series->setPen(QPen(QBrush(c.at(cc)),1.5,Qt::DashDotLine));
        } else if(nc == 4) {
            series->setPen(QPen(QBrush(c.at(cc)),1.5,Qt::DashDotDotLine));
        }
        //connect(series,SIGNAL(hovered(QPointF,bool)),this,SLOT(statisticsSourceHovered(QPointF, bool)));
    }
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);
    chart->setAcceptHoverEvents(true);
    Callout *callout = new Callout(chart);
    callout->hide();

    ui->ElevationPlot->insertWidget(0,chartView,1);
    connect(ui->treeView_satellites->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),SLOT(updateElevation()));
}

void SatelliteScheduling::updateElevation()
{
    QChartView *chartView = qobject_cast<QChartView *>(ui->ElevationPlot->itemAt(0)->widget());
    QChart *chart = chartView->chart();

    //get seleted item
    QModelIndexList sel = ui->treeView_satellites->selectionModel()->selectedRows();
    QString name = ui->treeView_satellites->model()->data(sel.at(0)).toString();

    int idx = allSatelliteModel->findItems(name).at(0)->row();

    chart->setTitle(name);
    SatelliteForGUI &sat = satellites[idx];
    SGP4 *sgp4 = sat.getSGP4Data();
    std::vector<VieVS::Station> stations = satelliteScheduler.refNetwork().getStations();

    int scanDur = sessionStart_.secsTo(sessionEnd_);
    auto series = chart->series();

    for(VieVS::Station &sta : stations) {
        QLineSeries *serie;
        for(const auto &any:series){
            if(any->name() == QString::fromStdString(sta.getName())){
                serie = qobject_cast<QLineSeries *>(any);
                break;
            }
        }
        serie->clear();

        CoordGeodetic stat = CoordGeodetic(sta.getPosition()->getLat(),sta.getPosition()->getLon(),sta.getPosition()->getAltitude()/1000,true);
        Observer obs( stat );
        DateTime t = DateTime(sessionStart_.date().year(),sessionStart_.date().month(),sessionStart_.date().day(),sessionStart_.time().hour(),sessionStart_.time().minute(),sessionStart_.time().second());
        int counter = 0;
        while(counter<scanDur) {
            QDateTime ts = sessionStart_.addSecs(counter);
            Eci eci = sgp4->FindPosition(t.AddSeconds(counter));
            CoordTopocentric topo = obs.GetLookAngle(eci);
            serie->append(ts.toMSecsSinceEpoch(),topo.elevation*rad2deg);
            counter=counter+600;
        }
    }
    chart->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);
}

void SatelliteScheduling::StackedBarPlotSetup()
{
    QStackedBarSeries *StackedBarSeries = new QStackedBarSeries();
    StackedBarSeries->clear();
    QChart *StackedBarChart = new QChart();
    StackedBarChart->legend()->setAlignment(Qt::AlignBottom);
    StackedBarChart->legend()->setMarkerShape(QLegend::MarkerShapeRectangle);
    StackedBarChart->legend()->setBorderColor(QColor(0,0,0,1));
    StackedBarChart->legend()->setAlignment(Qt::Alignment(Qt::AlignRight));
    StackedBarChart->setAnimationOptions(QChart::NoAnimation);
    StackedBarChart->addSeries(StackedBarSeries);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    QValueAxis *axisY = new QValueAxis;
    VieVS::Network &network = satelliteScheduler.refNetwork();
    for(int k =0; k<network.getNSta()+1;k++)
    {
        QBarSet *barSet = new QBarSet(QString("%0").arg(k) + " stations");
        StackedBarSeries->append(barSet);
    }
    axisY->setTitleText("% obs dur");

    QFont labelsFont;
    labelsFont.setPixelSize(15);
    StackedBarChart->setAxisY(axisY, StackedBarSeries);
    StackedBarChart->setAxisX(axisX, StackedBarSeries);
    QChartView *obsDurChartView = new QChartView(StackedBarChart,this);
    obsDurChartView->setRenderHint(QPainter::Antialiasing);
    ui->BarPlotVisibilityStacked->insertWidget(0,obsDurChartView,1);
}


void SatelliteScheduling::satelliteStatisticsSetup()
{
    ui->splitter_satelliteStatistics->setStretchFactor(0,1);
    ui->splitter_satelliteStatistics->setStretchFactor(1,4);
    ui->splitter_satelliteStatistics->setSizes({700,2000});

    QBarSeries *barSeries = new QBarSeries();
    QBarSet *barSet = new QBarSet("time [s]");
    barSeries->append(barSet);

    QChart *nStaChart = new QChart();
    nStaChart->legend()->hide();
    nStaChart->setAnimationOptions(QChart::NoAnimation);
    nStaChart->addSeries(barSeries);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    QValueAxis *axisY = new QValueAxis;
    axisX->setTitleText("number of stations");
    axisY->setTitleText("% obs dur");

    QFont labelsFont;
    labelsFont.setPixelSize(15);
    nStaChart->setAxisY(axisY, barSeries);
    nStaChart->setAxisX(axisX, barSeries);
    nStaChart->setTitle("number of stations");
    QChartView *obsDurChartView = new QChartView(nStaChart,this);
    obsDurChartView->setRenderHint(QPainter::Antialiasing);
    ui->BarPlotVisibilitySingle->insertWidget(0,obsDurChartView,1);

    QStringList labels;
    int nSta = satelliteScheduler.refNetwork().getNSta();
    for(int i =0; i<=nSta; i++) {
     labels << QString("%0").arg(i);
    }
    axisX->append(labels);
    connect(ui->treeView_satelliteListStatistics->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),this,SLOT(updateSatelliteStatistics()));
}

/*void SatelliteScheduling::createTableVisibility()
{
    VieVS::Network &network = satelliteScheduler.refNetwork();
    int nSta = network.getNSta();
    MultiColumnSortFilterProxyModel *satelliteProxyTable = new MultiColumnSortFilterProxyModel(this);
    QStandardItemModel *satelliteTable = new QStandardItemModel(0,3+nSta,this);
    satelliteTable->setHeaderData(0, Qt::Horizontal, QObject::tr("satellite"));
    satelliteTable->setHeaderData(1, Qt::Horizontal, QObject::tr("epoche"));

    for(int i =0; i<=nSta; i++) {
        QString s = QString("%1 stations").arg(i);
        satelliteTable->setHeaderData(i+2, Qt::Horizontal, s);
    }
    satelliteProxyTable->setSourceModel(satelliteTable);
    satelliteProxyTable->setFilterCaseSensitivity(Qt::CaseInsensitive);
    satelliteProxyTable->setFilterKeyColumns({0});
    satelliteProxyTable->sort(0);
    ui->treeView_satelliteListStatistics_table->setModel(satelliteProxyTable);

    for(size_t i =0; i<satellites.size();i++){
        SatelliteForGUI sat = satellites.at(i);
        QString name = QString::fromStdString(sat.getName());
        DateTime ti = sat.getTleData()->Epoch();
        QDateTime satEpoch = QDateTime(QDate(ti.Year(),ti.Month(),ti.Day()), QTime(ti.Hour(),ti.Minute(),ti.Second()));
        QStandardItem * nameItem = new QStandardItem(QIcon(":/icons/icons/satellite.png"),name);
        QStandardItem * satEpochItem = new QStandardItem(satEpoch.toString("dd.MM.yyyy hh:mm:ss"));
        QList<QStandardItem *> list;
        list.clear();
        list.append(nameItem);
        list.append(satEpochItem);

        DateTime start = DateTime(sessionStart_.date().year(),sessionStart_.date().month(),sessionStart_.date().day(),sessionStart_.time().hour(),sessionStart_.time().minute(),sessionStart_.time().second());
        DateTime end = DateTime(sessionEnd_.date().year(),sessionEnd_.date().month(),sessionEnd_.date().day(),sessionEnd_.time().hour(),sessionEnd_.time().minute(),sessionEnd_.time().second());
        double totalObsTime =(end-start).TotalSeconds();
        std::vector<std::vector<SatelliteForGUI::SatPass>> passList = sat.generatePassList(network, start,end,60);
        std::vector<SatelliteObs> overlaps = SatelliteObs::passList2Overlap( passList );
        QVector<double> timesPerNSta(network.getNSta()+1,0);
        double obsTime =0;
        for(size_t i =0; i<overlaps.size();i++) {
            DateTime ovStart = overlaps.at(i).getStart();
            DateTime ovEnd = overlaps.at(i).getEnd();
            TimeSpan ts = ovEnd - ovStart;
            double scanDur = ts.TotalSeconds();
            int nSta = overlaps.at(i).getNumberofStations();
            timesPerNSta[nSta] =  timesPerNSta[nSta] +scanDur;
            obsTime = obsTime +scanDur;
        }
        timesPerNSta[0]= totalObsTime - obsTime;

        for(int j = 0; j<=nSta; j++) {
           QString s = QString().sprintf("%.2f %", timesPerNSta.at(j)*100/totalObsTime);
           QStandardItem * item = new QStandardItem(s);
           item->setTextAlignment(Qt::AlignLeft);
           list.append(item);
        }
        for(int i=0; i< ui->treeView_satelliteListStatistics_table->model()->columnCount(); ++i) {
            ui->treeView_satelliteListStatistics_table->resizeColumnToContents(i);
            int width = ui->treeView_satelliteListStatistics_table->columnWidth(i);
            ui->treeView_satelliteListStatistics_table->setColumnWidth(i,width+10);
        }
        satelliteTable->appendRow(list);
   }
}*/


void SatelliteScheduling::createBarPlotStackedTableVisibility()
{
    //general
    VieVS::Network &network = satelliteScheduler.refNetwork();
    int nSta = network.getNSta();
    DateTime start = DateTime(sessionStart_.date().year(),sessionStart_.date().month(),sessionStart_.date().day(),sessionStart_.time().hour(),sessionStart_.time().minute(),sessionStart_.time().second());
    DateTime end = DateTime(sessionEnd_.date().year(),sessionEnd_.date().month(),sessionEnd_.date().day(),sessionEnd_.time().hour(),sessionEnd_.time().minute(),sessionEnd_.time().second());
    double totalObsTime =(end-start).TotalSeconds();

    //BarPlot
    QChartView *barChartView = qobject_cast<QChartView *>(ui->BarPlotVisibilityStacked->itemAt(0)->widget());
    QChart *barChart = barChartView->chart();
    QStackedBarSeries *barSeries= qobject_cast<QStackedBarSeries *>(barChart->series().at(0));
    QValueAxis *axisY = qobject_cast<QValueAxis *>(barChart->axisY());
    QBarCategoryAxis *axisX = qobject_cast<QBarCategoryAxis *>(barChart->axisX());
    QStringList labels;
    for(int i =0; i<satellites.size(); i++)
    {
        SatelliteForGUI sat = satellites.at(i);
        QString name = QString::fromStdString(sat.getName());
        labels << name;
   }
    axisX->setCategories(labels);
    axisX->setLabelsAngle(-45);
    QVector<QVector<double>> BarSets;
    BarSets.clear();


    //Table
    MultiColumnSortFilterProxyModel *satelliteProxyTable = new MultiColumnSortFilterProxyModel(this);
    QStandardItemModel *satelliteTable = new QStandardItemModel(0,3+nSta,this);
    satelliteTable->setHeaderData(0, Qt::Horizontal, QObject::tr("satellite"));
    satelliteTable->setHeaderData(1, Qt::Horizontal, QObject::tr("epoche"));

    for(int i =0; i<=nSta; i++) {
        QString s = QString("%1 stations").arg(i);
        satelliteTable->setHeaderData(i+2, Qt::Horizontal, s);
    }
    satelliteProxyTable->setSourceModel(satelliteTable);
    satelliteProxyTable->setFilterCaseSensitivity(Qt::CaseInsensitive);
    satelliteProxyTable->setFilterKeyColumns({0});
    satelliteProxyTable->sort(0);
    ui->treeView_satelliteListStatistics_table->setModel(satelliteProxyTable);

    for(size_t i =0; i<satellites.size();i++){
        SatelliteForGUI sat = satellites.at(i);
        QString name = QString::fromStdString(sat.getName());
        DateTime ti = sat.getTleData()->Epoch();
        QDateTime satEpoch = QDateTime(QDate(ti.Year(),ti.Month(),ti.Day()), QTime(ti.Hour(),ti.Minute(),ti.Second()));
        QStandardItem * nameItem = new QStandardItem(QIcon(":/icons/icons/satellite.png"),name);
        QStandardItem * satEpochItem = new QStandardItem(satEpoch.toString("dd.MM.yyyy hh:mm:ss"));
        QList<QStandardItem *> list;
        list.clear();
        list.append(nameItem);
        list.append(satEpochItem);
        std::vector<std::vector<SatelliteForGUI::SatPass>> passList = sat.generatePassList(network, start,end,60);
        std::vector<SatelliteObs> overlaps = SatelliteObs::passList2Overlap( passList );
        QVector<double> timesPerNSta(network.getNSta()+1,0);
        double obsTime =0;
        for(size_t i =0; i<overlaps.size();i++) {
            DateTime ovStart = overlaps.at(i).getStart();
            DateTime ovEnd = overlaps.at(i).getEnd();
            TimeSpan ts = ovEnd - ovStart;
            double scanDur = ts.TotalSeconds();
            int nSta = overlaps.at(i).getNumberofStations();
            timesPerNSta[nSta] =  timesPerNSta[nSta] +scanDur;
            obsTime = obsTime +scanDur;
        }
        timesPerNSta[0]= totalObsTime - obsTime;
        BarSets.append(timesPerNSta);

        for(int j = 0; j<=nSta; j++) {
           QString s = QString().sprintf("%.2f %", timesPerNSta.at(j)*100/totalObsTime);
           QStandardItem * item = new QStandardItem(s);
           item->setTextAlignment(Qt::AlignLeft);
           list.append(item);
        }
        for(int i=0; i< ui->treeView_satelliteListStatistics_table->model()->columnCount(); ++i) {
            ui->treeView_satelliteListStatistics_table->resizeColumnToContents(i);
            int width = ui->treeView_satelliteListStatistics_table->columnWidth(i);
            ui->treeView_satelliteListStatistics_table->setColumnWidth(i,width+10);
        }
        satelliteTable->appendRow(list);

    }
    float a = 1.5 ;
    for(int k=0; k<=network.getNSta();k++)
    {
        QBarSet *barSet = barSeries->barSets().at(k);
        barSet->remove(0,barSet->count());
        for(int i = 0; i<satellites.size();i++)
        {
            *barSet << BarSets.at(i).at(k)*100/totalObsTime;
            a = BarSets.at(i).at(k)*100/totalObsTime;
        }
    }
    axisY->setRange(0, 100);
    int show = 5;
    auto categories = axisX->categories();
    if(BarSets.size() !=0) {
        QString minlabel = categories.at(0);
        QString maxlabel;
        if(BarSets.size()>show) {
            maxlabel = categories.at(show-1);
        }
        else {
            maxlabel = categories.at(categories.size()-1);
        }
        axisX->setMax(minlabel);
        axisX->setMax(maxlabel);
    }
    if(BarSets.size()>show) {
        ui->horizontalScrollBar_stackedPlot->setRange(0,BarSets.size()-show);
        ui->horizontalScrollBar_stackedPlot->setSingleStep(1);
    }
    else {
        ui->horizontalScrollBar_stackedPlot->setRange(0,0);
        ui->horizontalScrollBar_stackedPlot->setSingleStep(1);
    }
}


/*void SatelliteScheduling::createBarPlotVisibilityStacked()
{
    QChartView *barChartView = qobject_cast<QChartView *>(ui->BarPlotVisibilityStacked->itemAt(0)->widget());
    QChart *barChart = barChartView->chart();
    QStackedBarSeries *barSeries= qobject_cast<QStackedBarSeries *>(barChart->series().at(0));

    QValueAxis *axisY = qobject_cast<QValueAxis *>(barChart->axisY());
    QBarCategoryAxis *axisX = qobject_cast<QBarCategoryAxis *>(barChart->axisX());

    QStringList labels;
    for(int i =0; i<satellites.size(); i++)
    {
        SatelliteForGUI sat = satellites.at(i);
        QString name = QString::fromStdString(sat.getName());
        labels << name;
   }
    axisX->setCategories(labels);
    axisX->setLabelsAngle(-45);

    VieVS::Network &network = satelliteScheduler.refNetwork();
    QVector<QVector<double>> BarSets;
    BarSets.clear();
    DateTime start = DateTime(sessionStart_.date().year(),sessionStart_.date().month(),sessionStart_.date().day(),sessionStart_.time().hour(),sessionStart_.time().minute(),sessionStart_.time().second());
    DateTime end = DateTime(sessionEnd_.date().year(),sessionEnd_.date().month(),sessionEnd_.date().day(),sessionEnd_.time().hour(),sessionEnd_.time().minute(),sessionEnd_.time().second());
    double totalObsTime =(end-start).TotalSeconds();
    for(int i =0;i<satellites.size();i++)
    {
        SatelliteForGUI &sat = satellites[i];

        std::vector<std::vector<SatelliteForGUI::SatPass>> passList = sat.generatePassList(network, start,end,60);
        std::vector<SatelliteObs> overlaps = SatelliteObs::passList2Overlap( passList );
        QVector<double> timesPerNSta(network.getNSta()+1,0);
        double obsTime =0;
        for(size_t i =0; i<overlaps.size();i++)
        {
           DateTime ovStart = overlaps.at(i).getStart();
           DateTime ovEnd = overlaps.at(i).getEnd();
           TimeSpan ts = ovEnd - ovStart;
           double scanDur = ts.TotalSeconds();
           int nSta = overlaps.at(i).getNumberofStations();
           timesPerNSta[nSta] =  timesPerNSta[nSta] +scanDur;
           obsTime = obsTime +scanDur;
        }
        timesPerNSta[0]= totalObsTime - obsTime;
        BarSets.append(timesPerNSta);
    }
    float a = 1.5 ;
    for(int k=0; k<=network.getNSta();k++)
    {
        QBarSet *barSet = barSeries->barSets().at(k);
        barSet->remove(0,barSet->count());
        for(int i = 0; i<satellites.size();i++)
        {
            *barSet << BarSets.at(i).at(k)*100/totalObsTime;
            a = BarSets.at(i).at(k)*100/totalObsTime;
        }
    }
    axisY->setRange(0, 100);
    int show = 5;
    auto categories = axisX->categories();
    if(BarSets.size() !=0) {
        QString minlabel = categories.at(0);
        QString maxlabel;
        if(BarSets.size()>show) {
            maxlabel = categories.at(show-1);
        }
        else {
            maxlabel = categories.at(categories.size()-1);
        }
        axisX->setMax(minlabel);
        axisX->setMax(maxlabel);
    }
    if(BarSets.size()>show) {
        ui->horizontalScrollBar_stackedPlot->setRange(0,BarSets.size()-show);
        ui->horizontalScrollBar_stackedPlot->setSingleStep(1);
    }
    else {
        ui->horizontalScrollBar_stackedPlot->setRange(0,0);
        ui->horizontalScrollBar_stackedPlot->setSingleStep(1);
    }
}*/

void SatelliteScheduling::updateSatelliteStatistics()
{
    QChartView *barChartView = qobject_cast<QChartView *>(ui->BarPlotVisibilitySingle->itemAt(0)->widget());
    QChart *barChart = barChartView->chart();
    QBarSeries *barSeries= qobject_cast<QBarSeries *>(barChart->series().at(0));
    QBarSet *barSet = barSeries->barSets().at(0);
    barSet->remove(0,barSet->count());

    QValueAxis *axisY = qobject_cast<QValueAxis *>(barChart->axes(Qt::Vertical).back());
    QModelIndexList sel = ui->treeView_satelliteListStatistics->selectionModel()->selectedRows();
    QString name = ui->treeView_satelliteListStatistics->model()->data(sel.at(0)).toString();
    int idx = allSatelliteModel->findItems(name).at(0)->row();

    SatelliteForGUI &sat = satellites[idx];
    VieVS::Network &network = satelliteScheduler.refNetwork();

    DateTime start = DateTime(sessionStart_.date().year(),sessionStart_.date().month(),sessionStart_.date().day(),sessionStart_.time().hour(),sessionStart_.time().minute(),sessionStart_.time().second());
    DateTime end = DateTime(sessionEnd_.date().year(),sessionEnd_.date().month(),sessionEnd_.date().day(),sessionEnd_.time().hour(),sessionEnd_.time().minute(),sessionEnd_.time().second());
    int totalObsTime =(end-start).TotalSeconds();
    std::vector<std::vector<SatelliteForGUI::SatPass>> passList = sat.generatePassList(network, start,end,60);
    std::vector<SatelliteObs> overlaps = SatelliteObs::passList2Overlap( passList );
    QVector<int> timesPerNSta(network.getNSta()+1,0);
    int obsTime =0;
    for(size_t i =0; i<overlaps.size();i++)
    {
        DateTime ovStart = overlaps.at(i).getStart();
        DateTime ovEnd = overlaps.at(i).getEnd();
        TimeSpan ts = ovEnd - ovStart;
        int scanDur = ts.TotalSeconds();
        int nSta = overlaps.at(i).getNumberofStations();
        timesPerNSta[nSta] =  timesPerNSta[nSta] + scanDur;
        obsTime = obsTime + scanDur;
    }
    timesPerNSta[0]= totalObsTime - obsTime;

    for(int any: timesPerNSta) {
        barSet->append(any*100/totalObsTime);
    }
    double max = *std::max_element(timesPerNSta.begin(), timesPerNSta.end());
    barSeries->append(barSet);
    barChart->setTitle(name);
    axisY->setRange(0, ceil((max*100/totalObsTime)/10)*10);
}

void SatelliteScheduling::on_actionStatistic_triggered()
{
    ui->stackedWidget->setCurrentIndex(6);
    ui->tabWidget->setCurrentIndex(1);
    QModelIndexList sel = ui->treeView_satelliteListStatistics->selectionModel()->selectedRows();
    if(sel.empty()) {
        ui->treeView_satelliteListStatistics->setCurrentIndex(ui->treeView_satelliteListStatistics->model()->index(0,0));
    }
    updateSatelliteStatistics();
    QElapsedTimer timer;
    //createTableVisibility();
    //createBarPlotVisibilityStacked();
    createBarPlotStackedTableVisibility();
}

void SatelliteScheduling::on_actionElevation_triggered()
{
    QModelIndexList sel = ui->treeView_satellites->selectionModel()->selectedRows();
    if(sel.empty()) {
        ui->treeView_satellites->setCurrentIndex(ui->treeView_satellites->model()->index(0,0));
    }
    ui->stackedWidget->setCurrentIndex(4);
    updateElevation();
}

void SatelliteScheduling::on_actionSkyPlots_triggered()
{
    writeLists();
    if(scheduledScans.empty()) {
        QMessageBox::information(this,"select scan first","Please select a Scan first!");
    }
    else {
        ui->treeWidget_selectedScanPlots->setCurrentIndex(ui->treeWidget_selectedScanPlots->model()->index(0,0));
        ui->stackedWidget->setCurrentIndex(3);
        connect(ui->treeWidget_selectedScanPlots,SIGNAL(clicked(QModelIndex)),this,SLOT(SetupPlotsScan()));
        SetupPlotsScan();
    }
}

void SatelliteScheduling::on_treeView_satelliteListStatistics_clicked(const QModelIndex &index)
{
    updateSatelliteStatistics();
}

void SatelliteScheduling::on_horizontalScrollBar_stackedPlot_valueChanged(int value)
{
    QChartView *barChartView = qobject_cast<QChartView *>(ui->BarPlotVisibilityStacked->itemAt(0)->widget());
    auto axisX = qobject_cast<QBarCategoryAxis*>(barChartView->chart()->axisX());
    auto categories = axisX->categories();
    int show = 5;
    if(!categories.isEmpty()) {
        QString min = categories.at(value);
        QString max = categories.at(value+show-1);
        axisX->setMin(min);
        axisX->setMax(max);
        axisX->setMin(min);
    }
}


void SatelliteScheduling::on_lineEdit_satelliteStatistics_textChanged(const QString &arg1)
{
    MultiColumnSortFilterProxyModel *proxy = qobject_cast<MultiColumnSortFilterProxyModel *>(ui->treeView_satelliteListStatistics->model());
    proxy->addFilterFixedString(arg1);
}

void SatelliteScheduling::on_lineEdit_satelliteElevation_textChanged(const QString &arg1)
{
    MultiColumnSortFilterProxyModel *proxy = qobject_cast<MultiColumnSortFilterProxyModel *>(ui->treeView_satellites->model());
    proxy->addFilterFixedString(arg1);
}

void SatelliteScheduling::on_lineEdit_satelliteStatistics_table_textChanged(const QString &arg1)
{
   MultiColumnSortFilterProxyModel *proxy = qobject_cast<MultiColumnSortFilterProxyModel *>(ui->treeView_satelliteListStatistics_table->model());
   proxy->addFilterFixedString(arg1);
}

//PLOTS
void SatelliteScheduling::on_pushButton_screenshot_SkyPlotScan_clicked()
{

    int idxTopLevelItem = getIndexTopLevelItem();
    QTreeWidgetItem *itm = ui->treeWidget_selectedScanPlots->topLevelItem(idxTopLevelItem);
    QString strStart = itm->data(1,0).toString();
    QString strEnd = itm->data(2,0).toString();
    QDateTime start = QDateTime::fromString(strStart, "dd.MM.yyyy hh:mm:ss");
    QDateTime end = QDateTime::fromString(strEnd, "dd.MM.yyyy hh:mm:ss");

    QWidget *qw = qobject_cast<QWidget*>(ui->SkyPlot_Scan->itemAt(0)->widget());
    QChartView *chartView = qobject_cast<QChartView*>(qw->layout()->itemAt(1)->widget());
    QComboBox *c1 = qobject_cast<QComboBox*>(qw->layout()->itemAt(0)->widget());
    QString name = c1->currentText();
    QSize backupsize = chartView->size();
    QFont backupFontX = chartView->chart()->axes(Qt::Horizontal).back()->labelsFont();
    QFont backupFontY = chartView->chart()->axes(Qt::Vertical).back()->labelsFont();
    QFont backupFontTitle = chartView->chart()->titleFont();
    QFont backupFontLegend = chartView->chart()->legend()->font();

    chartView->resize(1000,700);
    QString satname = chartView->chart()->title();

    QString fpath = "../out/SatellitePlots/";
    QString fpathcopy = fpath;
    QDir out(fpathcopy);
    if(!out.exists()){
        QDir().mkpath(fpathcopy);
    }

    QFont fontTitle;
    fontTitle.setPixelSize(15);
    fontTitle.setBold(true);
    chartView->chart()->setTitleFont(fontTitle);

    QFont fontLegend;
    fontLegend.setPixelSize(15);
    chartView->chart()->legend()->setFont(fontLegend);
    chartView->chart()->axes(Qt::Horizontal).back()->setLabelsFont(fontLegend);
    chartView->chart()->axes(Qt::Vertical).back()->setLabelsFont(fontLegend);
    chartView->chart()->legend()->setAlignment(Qt::AlignRight);
    chartView->chart()->legend()->setFont(fontLegend);

    chartView->setRenderHint(QPainter::Antialiasing);
    qApp->processEvents(QEventLoop::AllEvents);
    fpath.append(QString("skyplot_%1_%2_%3_%4.png").arg(satname).arg(name).arg(start.toString("yyyyMMddhhmmss")).arg(end.toString("yyyyMMddhhmmss")));
    chartView->grab().toImage().save(fpath);

    chartView->resize(backupsize);
    chartView->chart()->legend()->setFont(backupFontLegend);
    chartView->chart()->legend()->setAlignment(Qt::AlignBottom);
    chartView->chart()->axes(Qt::Horizontal).back()->setLabelsFont(backupFontX);
    chartView->chart()->axes(Qt::Vertical).back()->setLabelsFont(backupFontY);
    chartView->chart()->setTitleFont(backupFontTitle);
}

void SatelliteScheduling::on_pushButton_screenshot_ElevationPlotScan_clicked()
{
    int idxTopLevelItem = getIndexTopLevelItem();
    QTreeWidgetItem *itm = ui->treeWidget_selectedScanPlots->topLevelItem(idxTopLevelItem);
    QString strStart = itm->data(1,0).toString();
    QString strEnd = itm->data(2,0).toString();
    QDateTime start = QDateTime::fromString(strStart, "dd.MM.yyyy hh:mm:ss");
    QDateTime end = QDateTime::fromString(strEnd, "dd.MM.yyyy hh:mm:ss");

    QChartView *chartView = qobject_cast<QChartView *>(ui->ElevationPlot_Scan->itemAt(0)->widget());
    QSize backupsize = chartView->size();
    QFont backupFontX = chartView->chart()->axes(Qt::Horizontal).back()->labelsFont();
    QFont backupFontY = chartView->chart()->axes(Qt::Vertical).back()->labelsFont();
    QFont backupFontYtitle = chartView->chart()->axes(Qt::Vertical).back()->titleFont();
    QFont backupFontTitle = chartView->chart()->titleFont();
    QFont backupFontLegend = chartView->chart()->legend()->font();
    chartView->resize(1200,550);

    QFont fontTitle;
    fontTitle.setPixelSize(22);
    fontTitle.setBold(true);
    chartView->chart()->setTitleFont(fontTitle);

    QFont fontLegend;
    fontLegend.setPixelSize(17);
    chartView->chart()->legend()->setFont(fontLegend);
    chartView->chart()->axes(Qt::Horizontal).back()->setLabelsFont(fontLegend);
    chartView->chart()->axes(Qt::Vertical).back()->setLabelsFont(fontLegend);

    QFont fontAxis;
    fontAxis.setPixelSize(17);
    fontAxis.setBold(true);
    chartView->chart()->axes(Qt::Horizontal).back()->setTitleFont(fontAxis);
    chartView->chart()->axes(Qt::Vertical).back()->setTitleFont(fontAxis);
    chartView->chart()->legend()->setAlignment(Qt::Alignment(Qt::AlignRight));

    chartView->setRenderHint(QPainter::Antialiasing);
    qApp->processEvents(QEventLoop::AllEvents);
    QString fpath = "../out/SatellitePlots/";
    QString fpathcopy = fpath;
    QDir out(fpathcopy);
    if(!out.exists()){
        QDir().mkpath(fpathcopy);
    }

    QString name = chartView->chart()->title();
    fpath.append(QString("ScanElevation_%1_%2_%3.png").arg(name).arg(start.toString("yyyyMMddhhmmss")).arg(end.toString("yyyyMMddhhmmss")));
    chartView->grab().toImage().save(fpath);
    chartView->resize(backupsize);
    chartView->chart()->setTitleFont(backupFontTitle);
    chartView->chart()->legend()->setFont(backupFontLegend);
    chartView->chart()->axes(Qt::Horizontal).back()->setLabelsFont(backupFontX);
    chartView->chart()->axes(Qt::Vertical).back()->setTitleFont(backupFontYtitle);
    chartView->chart()->axes(Qt::Vertical).back()->setLabelsFont(backupFontY);
    chartView->chart()->legend()->setAlignment(Qt::Alignment(Qt::AlignBottom));
}

void SatelliteScheduling::on_pushButton_screenshot_BarPlotVisibilitySingle_clicked()
{
    RenderSetup render(this);
    render.timeseriesUncheckable();
    render.addList(allSatelliteModel);
    int duration = sessionStart_.secsTo(sessionEnd_);
    render.addTimes(duration);
    QString fpath = "../out/SatellitePlots/";
    QString fpathcopy = fpath;
    QDir out(fpathcopy);
    render.setOutDir(fpath);
    render.setDefaultFormat(1024,550);
    int result = render.exec();
    QVector<int> selected = render.selected();

    if(selected.isEmpty()){
        QMessageBox::warning(this,"No selection!","No items selected");
    }
    if(result == QDialog::Accepted && !selected.isEmpty()){
        if(!out.exists()){
            QDir().mkpath(fpathcopy);
        }
        QString outDir = render.outDir();
        if(outDir.back() != '/'){
            outDir.append('/');
        }
        QPair<int, int> res = render.resolution();
        QChartView *chartView = qobject_cast<QChartView *>(ui->BarPlotVisibilitySingle->itemAt(0)->widget());
        QFont backupFontX = chartView->chart()->axes(Qt::Horizontal).back()->labelsFont();
        QFont backupFontY = chartView->chart()->axes(Qt::Vertical).back()->labelsFont();
        QFont backupFontXtitle = chartView->chart()->axes(Qt::Horizontal).back()->titleFont();
        QFont backupFontYtitle = chartView->chart()->axes(Qt::Vertical).back()->titleFont();
        QFont backupFontTitle = chartView->chart()->titleFont();
        QFont backupFontLegend = chartView->chart()->legend()->font();
        QSize backupSize = chartView->size();
        chartView->resize(res.first,res.second);
        QModelIndexList backupSelection = ui->treeView_satelliteListStatistics->selectionModel()->selectedRows();

        QFont fontTitle;
        fontTitle.setPixelSize(22);
        fontTitle.setBold(true);
        chartView->chart()->setTitleFont(fontTitle);

        QFont fontLegend;
        fontLegend.setPixelSize(17);
        chartView->chart()->legend()->setFont(fontLegend);
        chartView->chart()->axes(Qt::Horizontal).back()->setLabelsFont(fontLegend);
        chartView->chart()->axes(Qt::Vertical).back()->setLabelsFont(fontLegend);
        QFont fontAxis;
        fontAxis.setPixelSize(17);
        fontAxis.setBold(true);
        chartView->chart()->axes(Qt::Horizontal).back()->setTitleFont(fontAxis);
        chartView->chart()->axes(Qt::Vertical).back()->setTitleFont(fontAxis);

        for(auto idx : selected)
        {
            QString name = allSatelliteModel->item(idx)->data(0).toString();
            QModelIndex index = allSatelliteModel->findItems(name).at(0)->index();
            ui->treeView_satelliteListStatistics->selectionModel()->select(index,QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

            QChartView *chartView = qobject_cast<QChartView *>(ui->BarPlotVisibilitySingle->itemAt(0)->widget());
            chartView->resize(res.first,res.second);
            //chartView->chart()->setTitle(name);
            chartView->setRenderHint(QPainter::Antialiasing);
            qApp->processEvents(QEventLoop::AllEvents);

            QString totalPath = fpath + QString("obsDuration_%1.png").arg(name);
            chartView->grab().toImage().save(totalPath);
        }
        chartView->resize(backupSize);
        chartView->chart()->setTitleFont(backupFontTitle);
        chartView->chart()->legend()->setFont(backupFontLegend);
        chartView->chart()->axes(Qt::Horizontal).back()->setLabelsFont(backupFontX);
        chartView->chart()->axes(Qt::Vertical).back()->setLabelsFont(backupFontY);
        chartView->chart()->axes(Qt::Horizontal).back()->setTitleFont(backupFontXtitle);
        chartView->chart()->axes(Qt::Vertical).back()->setTitleFont(backupFontYtitle);
        ui->treeView_satelliteListStatistics->selectionModel()->select(backupSelection.at(0),QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
     }
}

void SatelliteScheduling::on_pushButton_screenshot_BarPlotVisibilityStacked_clicked()
{
    QChartView *chartView = qobject_cast<QChartView *>(ui->BarPlotVisibilityStacked->itemAt(0)->widget());
    QSize backupsize = chartView->size();
    QFont backupFontX = chartView->chart()->axes(Qt::Horizontal).back()->labelsFont();
    QFont backupFontY = chartView->chart()->axes(Qt::Vertical).back()->labelsFont();
    QFont backupFontXtitle = chartView->chart()->axes(Qt::Horizontal).back()->titleFont();
    QFont backupFontYtitle = chartView->chart()->axes(Qt::Vertical).back()->titleFont();
    QFont backupFontTitle = chartView->chart()->titleFont();
    QFont backupFontLegend = chartView->chart()->legend()->font();
    chartView->resize(1200,800);
    QString fpath = "../out/SatellitePlots/";
    QString fpathcopy = fpath;
    QDir out(fpathcopy);
    if(!out.exists()){
        QDir().mkpath(fpathcopy);
    }
    QFont fontLegend;
    fontLegend.setPixelSize(15);

    chartView->chart()->legend()->setFont(fontLegend);
    chartView->chart()->legend()->setAlignment(Qt::Alignment(Qt::AlignRight));
    chartView->chart()->axes(Qt::Horizontal).back()->setLabelsFont(fontLegend);
    chartView->chart()->axes(Qt::Vertical).back()->setLabelsFont(fontLegend);

    QFont fontAxis;
    fontAxis.setPixelSize(15);
    fontAxis.setBold(true);
    chartView->chart()->axes(Qt::Vertical).back()->setTitleFont(fontAxis);

    QString name = chartView->chart()->title();
    chartView->setRenderHint(QPainter::Antialiasing);
    qApp->processEvents(QEventLoop::AllEvents);
    QString totalPath = fpath + QString("obsDurationStacked.png");
    chartView->grab().toImage().save(totalPath);

    chartView->resize(backupsize);
    chartView->chart()->setTitleFont(backupFontTitle);
    chartView->chart()->legend()->setFont(backupFontLegend);
    chartView->chart()->axes(Qt::Horizontal).back()->setLabelsFont(backupFontX);
    chartView->chart()->axes(Qt::Vertical).back()->setLabelsFont(backupFontY);
    chartView->chart()->axes(Qt::Horizontal).back()->setTitleFont(backupFontXtitle);
    chartView->chart()->axes(Qt::Vertical).back()->setTitleFont(backupFontYtitle);
}

void SatelliteScheduling::on_pushButton_screenshot_ElevationPlot_clicked()
{
    RenderSetup render(this);
    render.timeseriesUncheckable();
    render.addList(allSatelliteModel);
    int duration = sessionStart_.secsTo(sessionEnd_);
    render.addTimes(duration);
    QString fpath = "../out/SatellitePlots/";
    QString fpathcopy = fpath;
    QDir out(fpathcopy);
    render.setOutDir(fpath);
    render.setDefaultFormat(1200,550);
    int result = render.exec();
    QVector<int> selected = render.selected();

    if(selected.isEmpty()){
        QMessageBox::warning(this,"No selection!","No items selected");
    }
    if(result == QDialog::Accepted && !selected.isEmpty()){
        if(!out.exists()){
            QDir().mkpath(fpathcopy);
        }
        QString outDir = render.outDir();
        if(outDir.back() != '/'){
            outDir.append('/');
        }
        QPair<int, int> res = render.resolution();
        QChartView *chartView = qobject_cast<QChartView *>(ui->ElevationPlot->itemAt(0)->widget());
        QSize backupSize = chartView->size();
        chartView->resize(res.first,res.second);
        QFont backupFontX = chartView->chart()->axes(Qt::Horizontal).back()->labelsFont();
        QFont backupFontY = chartView->chart()->axes(Qt::Vertical).back()->labelsFont();
        QFont backupFontXtitle = chartView->chart()->axes(Qt::Horizontal).back()->titleFont();
        QFont backupFontYtitle = chartView->chart()->axes(Qt::Vertical).back()->titleFont();
        QFont backupFontTitle = chartView->chart()->titleFont();
        QFont backupFontLegend = chartView->chart()->legend()->font();
        QModelIndexList backupSelection = ui->treeView_satellites->selectionModel()->selectedRows();

        QFont fontTitle;
        fontTitle.setPixelSize(22);
        fontTitle.setBold(true);
        chartView->chart()->setTitleFont(fontTitle);

        QFont fontLegend;
        fontLegend.setPixelSize(17);
        QFont fontAxis;
        fontAxis.setPixelSize(17);
        fontAxis.setBold(true);
        chartView->chart()->legend()->setFont(fontLegend);
        chartView->chart()->axes(Qt::Horizontal).back()->setLabelsFont(fontLegend);
        chartView->chart()->axes(Qt::Vertical).back()->setLabelsFont(fontLegend);
        chartView->chart()->axes(Qt::Horizontal).back()->setTitleFont(fontAxis);
        chartView->chart()->axes(Qt::Vertical).back()->setTitleFont(fontAxis);
        chartView->chart()->legend()->setAlignment(Qt::Alignment(Qt::AlignRight));

        for(auto idx : selected)
        {
            QString name = allSatelliteModel->item(idx)->data(0).toString();
            QModelIndex index = allSatelliteModel->findItems(name).at(0)->index();
            ui->treeView_satellites->selectionModel()->select(index,QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
            QChartView *chartView = qobject_cast<QChartView *>(ui->ElevationPlot->itemAt(0)->widget());
            chartView->resize(res.first,res.second);
            chartView->setRenderHint(QPainter::Antialiasing);
            qApp->processEvents(QEventLoop::AllEvents);

            QString totalPath = fpath + QString("satelliteElevation_%1.png").arg(name);
            chartView->grab().toImage().save(totalPath);
        }
        chartView->resize(backupSize);
        chartView->chart()->setTitleFont(backupFontTitle);
        chartView->chart()->legend()->setFont(backupFontLegend);
        chartView->chart()->axes(Qt::Horizontal).back()->setLabelsFont(backupFontX);
        chartView->chart()->axes(Qt::Vertical).back()->setLabelsFont(backupFontY);
        chartView->chart()->axes(Qt::Horizontal).back()->setTitleFont(backupFontXtitle);
        chartView->chart()->axes(Qt::Vertical).back()->setTitleFont(backupFontYtitle);
        ui->treeView_satellites->selectionModel()->select(backupSelection.at(0),QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        chartView->chart()->legend()->setAlignment(Qt::Alignment(Qt::AlignBottom));
    }
}

void SatelliteScheduling::on_pushButton_screenshot_worldmap_clicked()
{
    RenderSetup render(this);
    render.timeseriesUncheckable();
    render.addList(allSatelliteModel);
    int duration = sessionStart_.secsTo(sessionEnd_);
    render.addTimes(duration);
    QString fpath = "../out/SatellitePlots/";
    QString fpathcopy = fpath;
    QDir out(fpathcopy);
    render.setOutDir(fpath);

    render.setDefaultFormat(1024,550);

    int result = render.exec();
    QVector<int> selected = render.selected();
    if(selected.isEmpty()){
        QMessageBox::warning(this,"No selection!","No items selected");
    }
    if(result == QDialog::Accepted && !selected.isEmpty()){
        if(!out.exists()){
            QDir().mkpath(fpathcopy);
        }
        QString outDir = render.outDir();
        if(outDir.back() != '/'){
            outDir.append('/');
        }
        QPair<int, int> res = render.resolution();

        QSize backupSize = worldmap->size();
        QFont backupFontX = worldmap->chart()->axes(Qt::Horizontal).back()->labelsFont();
        QFont backupFontY = worldmap->chart()->axes(Qt::Vertical).back()->labelsFont();
        worldmap->resize(res.first,res.second);

        QFont fontTitle;
        fontTitle.setPixelSize(22);
        fontTitle.setBold(true);
        worldmap->chart()->setTitleFont(fontTitle);

        QFont fontLegend;
        fontLegend.setPixelSize(17);
        QFont fontAxis;
        fontAxis.setPixelSize(17);
        fontAxis.setBold(true);
        worldmap->chart()->axes(Qt::Horizontal).back()->setLabelsFont(fontLegend);
        worldmap->chart()->axes(Qt::Vertical).back()->setLabelsFont(fontLegend);
        worldmap->chart()->axes(Qt::Horizontal).back()->setTitleFont(fontAxis);
        worldmap->chart()->axes(Qt::Vertical).back()->setTitleFont(fontAxis);

        for(auto idx : selected){
            QString name = allSatelliteModel->item(idx)->data(0).toString();
            QModelIndex index = allSatelliteModel->findItems(name).at(0)->index();
            on_treeView_available_clicked(index);
            QModelIndex index_sel = selectedSatelliteModel->findItems(name).at(0)->index();

            on_checkBox_showTracks_clicked(true);
            auto series = worldmap->chart()->series();
            QString satelliteName = allSatelliteModel->item(idx)->data(0).toString();
            worldmap->chart()->setTitle(satelliteName);
            for(int i=0; i<series.count();++i)
             {
                 QString name = series.at(i)->name();
                 if((name.left(3)=="sat" || name.left(3)=="mrk") && name.right(name.size()-3) != satelliteName)
                 {
                     series.at(i)->setVisible(false);
                 }
             }

            worldmap->chart()->axes(Qt::Horizontal).back()->setTitleText("longitude [°]");
            worldmap->chart()->axes(Qt::Vertical).back()->setTitleText("latitude [°]");
            worldmap->chart()->setTitleFont(fontTitle);

            qApp->processEvents(QEventLoop::AllEvents);
            worldmap->setRenderHint(QPainter::Antialiasing);

            QString totalPath = fpath + QString("sattrack_%1.png").arg(satelliteName);
            worldmap->grab().toImage().save(totalPath);  
            selectedSatelliteModel->removeRow(index_sel.row());
        }
        on_checkBox_showTracks_clicked(true);
        auto backupSeries = worldmap->chart()->series();
        for(int i=0; i<backupSeries.count();++i)
        {
            backupSeries.at(i)->setVisible(true);
        }
        worldmap->resize(backupSize);
        worldmap->chart()->setTitle("");
        worldmap->chart()->axes(Qt::Horizontal).back()->setLabelsFont(backupFontX);
        worldmap->chart()->axes(Qt::Vertical).back()->setLabelsFont(backupFontY);
        worldmap->chart()->axes(Qt::Horizontal).back()->setTitleText("");
        worldmap->chart()->axes(Qt::Vertical).back()->setTitleText("");
    }
    if(ui->checkBox_showTracks->isChecked()) {
        on_checkBox_showTracks_clicked(true);
    }
    else {
        on_checkBox_showTracks_clicked(false);
    }
}

void SatelliteScheduling::on_actionFinish_triggered()
{
    emit finished();
    //createXMLOutput(scheduledScans, satelliteScheduler.refNetwork(), satellites);
    //VieVS::Scan::readXMLOutput("../Test.xml", satelliteScheduler.refNetwork(), satellites);
}

void SatelliteScheduling::createXMLOutput(std::vector<VieVS::Scan> scanList,VieVS::Network network, std::vector<SatelliteForGUI> satellites)
{
    //Printing the Scans
    for ( unsigned long i = 0; i < scanList.size(); i++ ) {
        std::cout << ".------------------------------------------------------------------------------------------------"
                     "----------------------------------------------.\n";
        const auto &thisScan = scanList[i];
        thisScan.getSourceId();
        SatelliteForGUI satellite;

        for(SatelliteForGUI sat : satellites) {
            if(sat.getId() == thisScan.getSourceId()) {
                satellite = sat;
            }
        }
        //thisScan.outputSatScan( i, network, satellite );
        std::cout << std::endl;
    }

    boost::property_tree::ptree ptSatelliteScans;
    boost::property_tree::ptree ptScan;
    boost::property_tree::ptree ptStations;
    boost::property_tree::ptree ptWaitTimes;
    boost::property_tree::ptree ptStart;
    boost::property_tree::ptree ptEnd;
    boost::property_tree::ptree ptSatellite;
    for(const auto &scan : scanList) {
        unsigned int satelliteId = scan.getPointingVector(0,VieVS::Timestamp::start).getSrcid();
        std::string satelliteName;
        for(SatelliteForGUI sat:satellites) {
            if(sat.getId() == satelliteId) {
                satelliteName = sat.getName();
            }
        }
        ptScan.add("scan.<xmlattr>.name", satelliteName);
        ptScan.add("scan.<xmlattr>.id", satelliteId);
        for (int i = 0; i < scan.getNSta(); i++) {
            const VieVS::PointingVector pvStart = scan.getPointingVector(i, VieVS::Timestamp::start);
            const VieVS::PointingVector pvEnd = scan.getPointingVector(i, VieVS::Timestamp::end);
            const VieVS::Station &thisSta = network.getStation(pvStart.getStaid());

            ptStart.add("start.time", scan.getTimes().getObservingTime(i, VieVS::Timestamp::start));
            ptStart.add("start.az", pvStart.getAz());
            ptStart.add("start.el", pvStart.getEl());
            ptStart.add("start.lha", pvStart.getHa());
            ptStart.add("start.dc", pvStart.getDc());

            ptEnd.add("end.time", scan.getTimes().getObservingTime(i, VieVS::Timestamp::end));
            ptEnd.add("end.az",pvEnd.getAz());
            ptEnd.add("end.el",pvEnd.getEl());
            ptEnd.add("end.lha",pvEnd.getHa());
            ptEnd.add("end.dc", pvEnd.getDc());

            ptSatellite.add("satellite.name",satelliteName);
            ptSatellite.add("satellite.id",satelliteId);

            ptStations.add("station.<xmlattr>.name", thisSta.getName());
            ptWaitTimes.add("scanTimes.endOfLastScan",scan.getTimes().getFieldSystemTime( i, VieVS::Timestamp::start ));
            ptWaitTimes.add("scanTimes.fieldsystem",scan.getTimes().getFieldSystemDuration(i));
            ptWaitTimes.add("scanTimes.slew",scan.getTimes().getSlewDuration(i));
            ptWaitTimes.add("scanTimes.idle",scan.getTimes().getIdleDuration(i));
            ptWaitTimes.add("scanTimes.preob",scan.getTimes().getPreobDuration(i));
            ptWaitTimes.add("scanTimes.observing",scan.getTimes().getObservingDuration(i));

            ptStations.add_child("station.start",ptStart.get_child("start"));
            ptStations.add_child("station.end",ptEnd.get_child("end"));
            ptStations.add_child("station.scanTimes",ptWaitTimes.get_child("scanTimes"));
            ptScan.add_child("scan.station", ptStations.get_child("station"));

            ptStations.clear();
            ptWaitTimes.clear();
            ptStart.clear();
            ptEnd.clear();
            ptSatellite.clear();
        }
        ptSatelliteScans.add_child("satelliteScan.scan",ptScan.get_child("scan"));
        ptScan.clear();
    }
    std::ofstream of;
    of.open("../Test.xml");
    boost::property_tree::xml_parser::write_xml(of, ptSatelliteScans,
                                                boost::property_tree::xml_writer_make_settings<std::string>('\t', 1));
    of.close();
}

void SatelliteScheduling::on_pushButton_selectAll_clicked()
{
    MultiColumnSortFilterProxyModel *proxy = qobject_cast<MultiColumnSortFilterProxyModel *>(ui->treeView_available->model());
    for(int j=proxy->rowCount()-1; j>=0;j--) {
        selectedSatelliteModel->insertRow(0);
        for(int i=0; i<allSatelliteModel->columnCount(); ++i){
                selectedSatelliteModel->setItem(0, i, allSatelliteModel->item(j,i)->clone() );
            }
    }
}
