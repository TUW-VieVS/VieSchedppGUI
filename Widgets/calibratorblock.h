#ifndef CALIBRATORBLOCK_H
#define CALIBRATORBLOCK_H

#include <QWidget>
#include <QStandardItemModel>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <boost/property_tree/ptree.hpp>
#include "../VieSchedpp/Misc/CalibratorBlock.h"

namespace Ui {
class CalibratorBlock;
}

class CalibratorBlock : public QWidget
{
    Q_OBJECT

private slots:

    void on_spinBox_NCalibrationBlocks_valueChanged(int arg1);

    void on_pushButton_tryToIncludeAllStations_toggled(bool checked);

    void on_pushButton_save_advanced_clicked();

    void on_pushButton_save_general_clicked();

    void on_pushButton_save_blocks_clicked();

    void on_checkBox_tryToIncludeAllStations_toggled(bool checked);

public slots:

    void update();

public:
    explicit CalibratorBlock(QStandardItemModel *source_model, QDoubleSpinBox *session_duration, QWidget *parent = nullptr);

    QString reloadSources();

    ~CalibratorBlock();

    QPushButton *newSourceGroup;
    QPushButton *default_blocks;
    QPushButton *default_general_setup;
    QPushButton *default_advanced_setup;

    boost::property_tree::ptree toXML();
    void fromXML(const boost::property_tree::ptree &rules);

signals:
    void update_settings(QStringList path, QStringList value, QString name);


private:
    Ui::CalibratorBlock *ui;
    QStandardItemModel *source_model_;
    QDoubleSpinBox *session_duration;
};

#endif // CALIBRATORBLOCK_H
