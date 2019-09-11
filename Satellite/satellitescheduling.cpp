#include "satellitescheduling.h"
#include "ui_satellitescheduling.h"

SatelliteScheduling::SatelliteScheduling(const QString &pathAntenna, const QString &pathEquip,
                                         const QString &pathPosition, const QString &pathMask,
                                         QDateTime startTime, QDateTime endTime,
                                         const QStringList &stations, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SatelliteScheduling)
{

    boost::posix_time::ptime start(boost::gregorian::date( startTime.date().year(), startTime.date().month(), startTime.date().day()),
                                   boost::posix_time::time_duration( startTime.time().hour(), startTime.time().minute(), startTime.time().second()));
    boost::posix_time::ptime end(boost::gregorian::date( endTime.date().year(), endTime.date().month(), endTime.date().day()),
                                   boost::posix_time::time_duration( endTime.time().hour(), endTime.time().minute(), endTime.time().second()));

    std::vector<std::string> stationList;
    for(const auto &sta : stations){
        stationList.push_back(sta.toStdString());
    }

    std::string antenna = pathAntenna.toStdString();
    std::string equip = pathEquip.toStdString();
    std::string position = pathPosition.toStdString();
    std::string mask = pathMask.toStdString();

    satelliteScheduler.initialize(antenna,equip,position,mask,start,end,stationList);

    ui->setupUi(this);
}

SatelliteScheduling::~SatelliteScheduling()
{
    delete ui;
}
