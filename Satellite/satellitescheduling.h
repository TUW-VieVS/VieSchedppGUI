#ifndef SATELLITESCHEDULING_H
#define SATELLITESCHEDULING_H

#include <QMainWindow>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QtCharts/QScatterSeries>
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
                        const QStringList &stations, QWidget *parent = nullptr);

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

private:
    Ui::SatelliteScheduling *ui;
    ChartView *worldmap;

    VieVS::SatelliteMain satelliteScheduler;
};

#endif // SATELLITESCHEDULING_H
