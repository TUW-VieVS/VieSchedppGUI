#ifndef CALIBRATORBLOCKWIDGET_H
#define CALIBRATORBLOCKWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <boost/property_tree/ptree.hpp>
#include "../VieSchedpp/Misc/CalibratorBlock.h"

namespace Ui {
class CalibratorBlockWidget;
}

class CalibratorBlockWidget : public QWidget
{
    Q_OBJECT

private slots:

    void on_spinBox_NCalibrationBlocks_valueChanged(int arg1);

    void on_pushButton_save_advanced_clicked();

    void on_pushButton_save_general_clicked();

    void on_pushButton_save_blocks_clicked();

    void on_checkBox_tryToIncludeAllStations_toggled(bool checked);

    void on_pushButton_save_dpara_clicked();

    void on_pushButton_save_para_clicked();

    void on_spinBox_dpara_scans_valueChanged(int arg1);

    void on_spinBox_para_nscans_valueChanged(int arg1);

public slots:

    void update();

public:
    explicit CalibratorBlockWidget(QStandardItemModel *source_model,
                             QStandardItemModel *station_model,
                             QStandardItemModel *baseline_model,
                             QDoubleSpinBox *session_duration,
                             QWidget *parent = nullptr);

    QString reloadSources();

    ~CalibratorBlockWidget();

    QPushButton *newSourceGroup;
    QPushButton *newSourceGroup2;
    QPushButton *newSourceGroup3;
    QPushButton *newStationGroup;
    QPushButton *newBaselineGroup;

    QPushButton *default_blocks;
    QPushButton *default_general_setup;
    QPushButton *default_advanced_setup;
    QPushButton *save_para;
    QPushButton *save_dpara;

    boost::property_tree::ptree toXML();
    void fromXML(const boost::property_tree::ptree &rules);

signals:
    void update_settings(QStringList path, QStringList value, QString name);


private:
    Ui::CalibratorBlockWidget *ui;
    QStandardItemModel *source_model_;
    QStandardItemModel *station_model_;
    QStandardItemModel *baseline_model_;
    QDoubleSpinBox *session_duration;
};

#endif // CALIBRATORBLOCKWIDGET_H
