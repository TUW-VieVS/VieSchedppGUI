#include "satellitescheduling.h"
#include "ui_satellitescheduling.h"

SatelliteScheduling::SatelliteScheduling(const QString &pathAntenna, const QString &pathEquip,
                                         const QString &pathPosition, const QString &pathMask,
                                         QDateTime startTime, QDateTime endTime,
                                         const QStringList &stations,
                                         boost::property_tree::ptree *settings,
                                         QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SatelliteScheduling)
{
    ui->setupUi(this);
    settings_ = settings;
    std::string pathToTleFile = settings_->get("settings.satellite_scheduling.defaultInputPath","");
    ui->lineEdit_inputPath->setText(QString::fromStdString(pathToTleFile));

    ui->stackedWidget->setCurrentIndex(0);
    ui->dateTimeEdit_sessionStart->setDateTime(startTime);
    ui->dateTimeEdit_sessionEnd->setDateTime(endTime);

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


    QStandardItemModel *satelliteModel = new QStandardItemModel(this);
    MultiColumnSortFilterProxyModel *satelliteProxyModel = new MultiColumnSortFilterProxyModel(this);
    satelliteProxyModel->setSourceModel(satelliteModel);
    satelliteProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    satelliteProxyModel->setFilterKeyColumns({0});
    ui->treeView_available->setModel(satelliteProxyModel);


    // worldmap
    worldmap = new ChartView(this);
    qtUtil::worldMap(worldmap);
    Callout *worldMapCallout = new Callout(worldmap->chart());
    worldMapCallout->hide();
    ui->horizontalLayout_map->insertWidget(0,worldmap,10);


    QChart *worldChart = worldmap->chart();


    QScatterSeries *stationMarker = new QScatterSeries(worldChart);
    QImage img(":/icons/icons/station_white.png");
    img = img.scaled(35,35);
    stationMarker->setBrush(QBrush(img));
    stationMarker->setMarkerSize(35);
    stationMarker->setPen(QColor(Qt::transparent));


    stationMarker->setName("selectedStations");

    worldChart->addSeries(stationMarker);

    connect(stationMarker,SIGNAL(hovered(QPointF,bool)),this,SLOT(worldmap_hovered(QPointF,bool)));


    for(const auto &any : satelliteScheduler.refNetwork().getStations()){
        double lat = any.getPosition().getLat()*180/3.141592653589793;
        double lon = any.getPosition().getLon()*180/3.141592653589793;
        stationMarker->append(lon,lat);
    }

    stationMarker->attachAxis(worldChart->axisX());
    stationMarker->attachAxis(worldChart->axisY());

    ui->dateTimeEdit_showTime->setDateTime(startTime);
    auto dt = startTime.secsTo(endTime);
    ui->horizontalSlider_adjustTime->setMinimum(0);
    ui->horizontalSlider_adjustTime->setMaximum(dt);


    if(!pathToTleFile.empty()){
        on_pushButton_inputReload_clicked();
    }

}

SatelliteScheduling::~SatelliteScheduling()
{
    delete ui;
}

void SatelliteScheduling::on_actionSatellite_triggered()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void SatelliteScheduling::on_actionScan_triggered()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void SatelliteScheduling::on_actionInfo_triggered()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void SatelliteScheduling::on_pushButton_inputBrowse_clicked()
{
    QString startPath = ui->lineEdit_inputPath->text();
    QString path = QFileDialog::getOpenFileName(this, "Browse to xml file", startPath, tr("xml files (*.xml)"));
    if( !path.isEmpty() ){
        on_pushButton_inputReload_clicked();
    }
}

void SatelliteScheduling::on_pushButton_inputReload_clicked()
{
    QString path = ui->lineEdit_inputPath->text();
    satellites.clear();
    try {
        satellites = satelliteScheduler.readSatelliteFile(path.toStdString());
    } catch (...) {
        QMessageBox::warning(this,"Error while reading file!","There was an error while reading the TLE file!");
        return;
    }

    if(satellites.empty()){
        QMessageBox::warning(this,"No satellites found!","There was no satellite information provided within the selected file!");
        return;
    }

    MultiColumnSortFilterProxyModel *proxy = qobject_cast<MultiColumnSortFilterProxyModel *>(ui->treeView_available->model());
    auto sourceModel = proxy->sourceModel();
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(sourceModel);

    model->clear();

    for(const auto &sat : satellites){
        QString name = QString::fromStdString(sat.getName());
        model->appendRow(new QStandardItem(QIcon(":/icons/icons/satellite.png"),name));
    }
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

    if( ui->treeWidget_selected->findItems(name,Qt::MatchExactly).isEmpty()){
        ui->treeWidget_selected->addTopLevelItem(new QTreeWidgetItem(QStringList() << name));
        ui->treeWidget_selected->topLevelItem(ui->treeWidget_selected->topLevelItemCount()-1)->setIcon(0,QIcon(":/icons/icons/satellite.png"));
    }

    ui->treeWidget_selected->sortItems(0, Qt::AscendingOrder);

    ui->lineEdit_availableFilter->setFocus();
    ui->lineEdit_availableFilter->selectAll();

    for( int i=0; i< ui->treeWidget_selected->columnCount(); ++i){
        ui->treeWidget_selected->resizeColumnToContents(0);
    }

}

void SatelliteScheduling::on_treeWidget_selected_itemClicked(QTreeWidgetItem *item, int column)
{
    delete item;
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
    start = start.addSecs(value);
    ui->dateTimeEdit_showTime->setDateTime(start);
}


void SatelliteScheduling::worldmap_hovered(QPointF point, bool state)
{

}


void SatelliteScheduling::on_pushButton_process_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    std::vector<VieVS::Satellite> selectedSatellites;
    std::vector<std::string> selectedSatellitesNames;

    for (int i = 0; i < ui->treeWidget_selected->topLevelItemCount(); ++i) {
        const auto &tli = ui->treeWidget_selected->topLevelItem(i);
        selectedSatellitesNames.push_back(tli->text(0).toStdString());
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
        VieVS::Station::WaitTimes wt;
        wt.preob = ui->spinBox_preob->value();
        wt.fieldSystem = ui->spinBox_fs->value();
        any.setWaitTimes(wt);
    }

    auto scans = satelliteScheduler.generateScanList(selectedSatellites);

    for (const auto &scan : scans){

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
    for (int i=0; i< ui->treeWidget_template->columnCount(); ++i){
        ui->treeWidget_template->resizeColumnToContents(i);
    }

}

void SatelliteScheduling::on_pushButton_showAll_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void SatelliteScheduling::on_pushButton_showConstant_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}


void SatelliteScheduling::on_treeWidget_template_itemClicked(QTreeWidgetItem *item, int column)
{

    if(item->parent()){
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


//        QCheckBox *cb = new QCheckBox();
//        cb->setCheckState(Qt::Checked);
//        t->setCellWidget(i,0,cb);
        QWidget *checkBoxWidget = new QWidget();
        QCheckBox *cb = new QCheckBox();
        QHBoxLayout *l = new QHBoxLayout(checkBoxWidget);
        l->addWidget(cb);
        l->setAlignment(Qt::AlignCenter);
        l->setContentsMargins(0,0,0,0);
        cb->setCheckState(Qt::Checked);
        t->setCellWidget(i,0,checkBoxWidget);

        QString stationName = child->text(4);
        t->setItem(i,1,new QTableWidgetItem(QIcon(":/icons/icons/station.png"),stationName));

        QDateTimeEdit *start = new QDateTimeEdit();
        QDateTimeEdit *end = new QDateTimeEdit();
        start->setDisplayFormat("dd.MM.yyyy hh:mm:ss");
        end->setDisplayFormat("dd.MM.yyyy hh:mm:ss");
        start->setDateTime(min);
//        start->setMinimumDateTime(min);
//        start->setMaximumDateTime(max);
        end->setDateTime(max);
//        end->setMinimumDateTime(min);
//        end->setMaximumDateTime(max);
        t->setCellWidget(i,2,start);
        t->setCellWidget(i,3,end);

        QSpinBox *sp = new QSpinBox();
        sp->setReadOnly(true);
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

        connect(start, &QDateTimeEdit::dateTimeChanged, eval_start);
        connect(end,   &QDateTimeEdit::dateTimeChanged, eval_end);

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

    for(int i = 1; i<tw->rowCount(); ++i){

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
        unsigned int Vstart = ui->dateTimeEdit_sessionStart->dateTime().secsTo(start);

        QDateTime end = qobject_cast<QDateTimeEdit *>(tw->cellWidget(i,3))->dateTime();
        unsigned int Vend = ui->dateTimeEdit_sessionStart->dateTime().secsTo(start);
        if(Vstart >= Vend){
            continue;
        }

        int fieldSystem = ui->spinBox_fs->value();
        int preob = ui->spinBox_preob->value();

//        scheduledScans.push_back();
    }
}


void SatelliteScheduling::on_pushButton_savePath_clicked()
{
    QString inputPath = ui->lineEdit_inputPath->text();

    settings_->add( "settings.satellite_scheduling.defaultInputPath", inputPath.toStdString() );

    std::ofstream os;
    os.open("settings.xml");
    boost::property_tree::xml_parser::write_xml(os, *settings_, boost::property_tree::xml_writer_make_settings<std::string>('\t', 1));
    os.close();
    QMessageBox::information(this,"Default parameters changed","New default input path for satellite TLE file added!");

}
