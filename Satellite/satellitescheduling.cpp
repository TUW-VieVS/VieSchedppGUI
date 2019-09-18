#include "satellitescheduling.h"
#include "ui_satellitescheduling.h"

SatelliteScheduling::SatelliteScheduling(const QString &pathAntenna, const QString &pathEquip,
                                         const QString &pathPosition, const QString &pathMask,
                                         QDateTime startTime, QDateTime endTime,
                                         const QStringList &stations, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SatelliteScheduling)
{
    ui->setupUi(this);

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
    std::vector<VieVS::Satellite> satellites;
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
        model->appendRow(new QStandardItem(name));
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

    if( !ui->treeWidget_selected->findItems(name,Qt::MatchExactly).isEmpty()){
        ui->treeWidget_selected->addTopLevelItem(new QTreeWidgetItem(QStringList() << name));

    }
    ui->treeWidget_selected->sortItems(0, Qt::AscendingOrder);

    ui->lineEdit_availableFilter->setFocus();
    ui->lineEdit_availableFilter->selectAll();

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
