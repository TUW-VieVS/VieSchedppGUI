#ifndef MULITSCHEDULINGWIDGET_H
#define MULITSCHEDULINGWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QDateTime>
#include <QDoubleSpinBox>
#include <QGroupBox>

#include "secondaryGUIs/multischededitdialogint.h"
#include "secondaryGUIs/multischededitdialogdouble.h"
#include "secondaryGUIs/multischededitdialogdatetime.h"
#include "../VieSchedpp/Misc/MultiScheduling.h"
#include "../VieSchedpp/XML/ParameterSettings.h"

namespace Ui {
class MulitSchedulingWidget;
}

class MulitSchedulingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MulitSchedulingWidget(
            QStandardItemModel *allSourcePlusGroupModel,
            QStandardItemModel *allStationPlusGroupModel,
            QStandardItemModel *allBaselinePlusGroupModel,
            QCheckBox *checkBox_weightCoverage,
            QDoubleSpinBox *doubleSpinBox_weightSkyCoverage,
            QCheckBox *checkBox_weightNobs,
            QDoubleSpinBox *doubleSpinBox_weightNumberOfObservations,
            QCheckBox *checkBox_weightDuration,
            QDoubleSpinBox *doubleSpinBox_weightDuration,
            QCheckBox *checkBox_weightAverageSources,
            QDoubleSpinBox *doubleSpinBox_weightAverageSources,
            QCheckBox *checkBox_weightAverageStations,
            QDoubleSpinBox *doubleSpinBox_weightAverageStations,
            QCheckBox *checkBox_weightAverageBaselines,
            QDoubleSpinBox *doubleSpinBox_weightAverageBaselines,
            QCheckBox *checkBox_weightIdleTime,
            QDoubleSpinBox *doubleSpinBox_weightIdleTime,
            QCheckBox *checkBox_weightLowDeclination,
            QDoubleSpinBox *doubleSpinBox_weightLowDec,
            QCheckBox *checkBox_weightLowElevation,
            QDoubleSpinBox *doubleSpinBox_weightLowEl,
            QGroupBox *groupBox_multiScheduling,
            QGroupBox *groupBox_simulator,
            std::map<std::string, std::vector<std::string>> *groupSta,
            std::map<std::string, std::vector<std::string>> *groupSrc,
            std::map<std::string, std::vector<std::string>> *groupBl,
            QWidget *parent = nullptr);

    ~MulitSchedulingWidget();

    void toXML(VieVS::ParameterSettings &para);

    void fromXML(const boost::property_tree::ptree &xml);

    void resetStationParameters(bool &mssta, bool &msbl);

    void resetSourceParameters(bool &mssrc);

    void clear();

    void defaultIntensive();

    void setMultiprocessing(std::string threads, int nThreadsManual, std::string jobScheduler, int chunkSize);

    std::pair<QStringList, QStringList> getMultiCoreSupport();

    QPushButton *newStationGroup;
    QPushButton *newSourceGroup;
    QPushButton *newBaselineGroup;
    QPushButton *saveMultiCoreSetup;

private slots:
    void on_comboBox_nThreads_currentTextChanged(const QString &arg1);

    void on_comboBox_jobSchedule_currentTextChanged(const QString &arg1);


    void on_pushButton_25_clicked();

    void on_pushButton_ms_pick_random_toggled(bool checked);

    void on_pushButton_ms_pick_random_clicked();

    void on_comboBox_multiSched_maxNumber_currentIndexChanged(const QString &arg1);

    void on_comboBox_multiSched_seed_currentIndexChanged(const QString &arg1);

    void multi_sched_count_nsched();

    void on_pushButton_multiSchedAddSelected_clicked();

    void createMultiSchedTable();

    void on_groupBox_ms_gen_clicked(bool checked);

private:
    Ui::MulitSchedulingWidget *ui;
    QStandardItemModel *allSourcePlusGroupModel;
    QStandardItemModel *allStationPlusGroupModel;
    QStandardItemModel *allBaselinePlusGroupModel;

    QCheckBox *checkBox_weightCoverage;
    QDoubleSpinBox *doubleSpinBox_weightSkyCoverage;
    QCheckBox *checkBox_weightNobs;
    QDoubleSpinBox *doubleSpinBox_weightNumberOfObservations;
    QCheckBox *checkBox_weightDuration;
    QDoubleSpinBox *doubleSpinBox_weightDuration;
    QCheckBox *checkBox_weightAverageSources;
    QDoubleSpinBox *doubleSpinBox_weightAverageSources;
    QCheckBox *checkBox_weightAverageStations;
    QDoubleSpinBox *doubleSpinBox_weightAverageStations;
    QCheckBox *checkBox_weightAverageBaselines;
    QDoubleSpinBox *doubleSpinBox_weightAverageBaselines;
    QCheckBox *checkBox_weightIdleTime;
    QDoubleSpinBox *doubleSpinBox_weightIdleTime;
    QCheckBox *checkBox_weightLowDeclination;
    QDoubleSpinBox *doubleSpinBox_weightLowDec;
    QCheckBox *checkBox_weightLowElevation;
    QDoubleSpinBox *doubleSpinBox_weightLowEl;

    QGroupBox *groupBox_multiScheduling;
    QGroupBox *groupBox_simulator;

    std::map<std::string, std::vector<std::string>> *groupSta;
    std::map<std::string, std::vector<std::string>> *groupSrc;
    std::map<std::string, std::vector<std::string>> *groupBl;



};

#endif // MULITSCHEDULINGWIDGET_H
