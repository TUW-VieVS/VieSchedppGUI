#ifndef SATELLITESCHEDULING_H
#define SATELLITESCHEDULING_H

#include <QMainWindow>
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
                        const QStringList &stations, QWidget *parent = nullptr);

    ~SatelliteScheduling();

private:
    Ui::SatelliteScheduling *ui;

    VieVS::SatelliteMain satelliteScheduler;
};

#endif // SATELLITESCHEDULING_H
