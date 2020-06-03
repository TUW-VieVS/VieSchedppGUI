#include "mulitschedulingwidget.h"
#include "ui_mulitschedulingwidget.h"

MulitSchedulingWidget::MulitSchedulingWidget(QStandardItemModel *allSourcePlusGroupModel,
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
                                             std::map<std::string, std::vector<std::string>> *groupSta,
                                             std::map<std::string, std::vector<std::string>> *groupSrc,
                                             std::map<std::string, std::vector<std::string>> *groupBl,
                                             QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MulitSchedulingWidget),
    allSourcePlusGroupModel{allSourcePlusGroupModel},
    allStationPlusGroupModel{allStationPlusGroupModel},
    allBaselinePlusGroupModel{allBaselinePlusGroupModel},
    checkBox_weightCoverage{checkBox_weightCoverage},
    doubleSpinBox_weightSkyCoverage{doubleSpinBox_weightSkyCoverage},
    checkBox_weightNobs{checkBox_weightNobs},
    doubleSpinBox_weightNumberOfObservations{doubleSpinBox_weightNumberOfObservations},
    checkBox_weightDuration{checkBox_weightDuration},
    doubleSpinBox_weightDuration{doubleSpinBox_weightDuration},
    checkBox_weightAverageSources{checkBox_weightAverageSources},
    doubleSpinBox_weightAverageSources{doubleSpinBox_weightAverageSources},
    checkBox_weightAverageStations{checkBox_weightAverageStations},
    doubleSpinBox_weightAverageStations{doubleSpinBox_weightAverageStations},
    checkBox_weightAverageBaselines{checkBox_weightAverageBaselines},
    doubleSpinBox_weightAverageBaselines{doubleSpinBox_weightAverageBaselines},
    checkBox_weightIdleTime{checkBox_weightIdleTime},
    doubleSpinBox_weightIdleTime{doubleSpinBox_weightIdleTime},
    checkBox_weightLowDeclination{checkBox_weightLowDeclination},
    doubleSpinBox_weightLowDec{doubleSpinBox_weightLowDec},
    checkBox_weightLowElevation{checkBox_weightLowElevation},
    doubleSpinBox_weightLowEl{doubleSpinBox_weightLowEl},
    groupBox_multiScheduling{groupBox_multiScheduling},
    groupSta{groupSta},
    groupSrc{groupSrc},
    groupBl{groupBl}
{
    ui->setupUi(this);
    ui->splitter_5->setStretchFactor(1,3);

    newStationGroup = ui->pushButton_addGroupStationSetup_2;
    newSourceGroup = ui->pushButton_addGroupsourceSetup_2;
    newBaselineGroup = ui->pushButton_addGroupBaselineSetup_2;
    saveMultiCoreSetup = ui->pushButton_save_multiCore;

    createMultiSchedTable();

}

MulitSchedulingWidget::~MulitSchedulingWidget()
{
    delete ui;
}


void MulitSchedulingWidget::on_pushButton_ms_pick_random_toggled(bool checked)
{
    ui->treeWidget_multiSchedSelected->clear();
    for(int i=0; i<ui->treeWidget_multiSched->topLevelItemCount(); ++i){
        ui->treeWidget_multiSched->topLevelItem(i)->setDisabled(false);
        for (int j=0; j<ui->treeWidget_multiSched->topLevelItem(i)->childCount(); ++j){
            ui->treeWidget_multiSched->topLevelItem(i)->child(j)->setDisabled(false);
        }
    }
    if(checked){
        ui->comboBox_multiSched_maxNumber->setEnabled(false);
        ui->spinBox_multiSched_maxNumber->setEnabled(true);
        ui->spinBox_multiSched_maxNumber->setValue(32);
    }else{
        ui->comboBox_multiSched_maxNumber->setEnabled(true);
        if(ui->comboBox_multiSched_maxNumber->currentText() == "all"){
            ui->spinBox_multiSched_maxNumber->setEnabled(false);
        }else{
            ui->spinBox_multiSched_maxNumber->setEnabled(true);
        }
        ui->spinBox_multiSched_maxNumber->setValue(1);
    }

    ui->label_multiSchedulingNsched->setText(QString("total number of schedules: 1"));
    ui->label_multiSchedulingNsched->setStyleSheet("");

}

void MulitSchedulingWidget::on_pushButton_ms_pick_random_clicked()
{

}
// ########################################### MULTI SCHED ###########################################

void MulitSchedulingWidget::createMultiSchedTable()
{

    QTreeWidget *t = ui->treeWidget_multiSched;

    t->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    t->expandAll();
    ui->treeWidget_multiSchedSelected->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    for(int i=0; i<ui->treeWidget_multiSched->topLevelItemCount(); ++i){
        for(int j=0; j<ui->treeWidget_multiSched->topLevelItem(i)->childCount(); ++j){
            ui->treeWidget_multiSched->topLevelItem(i)->child(j)->setDisabled(false);
        }
    }

}

void MulitSchedulingWidget::on_pushButton_multiSchedAddSelected_clicked()
{
    bool random = ui->pushButton_ms_pick_random->isChecked();

    auto tall = ui->treeWidget_multiSched;
    auto list = tall->selectedItems();

    for(const auto&any:list){

        if(any->parent()){
            QString name = any->text(0);
            QString parameterType = any->parent()->text(0);

            QStringList row2toggle{"subnetting",
                                   "fillin-mode during scan selection",
                                   "fillin-mode influence on scan selection",
                                   "fillin-mode a posteriori"};

            QStringList row2intDialog {"min slew time",
                                       "max slew time",
                                       "max wait time",
                                       "max scan time",
                                       "min scan time",
                                       "min number of stations",
                                       "min repeat time",
                                       "idle time interval",
                                       "influence time",
                                       "max number of scans",
                                       "focus corner switch cadence"};

            QStringList row2doubleDialog {"subnetting min source angle",
                                          "subnetting min participating stations",
                                          "sky-coverage",
                                          "number of observations",
                                          "duration",
                                          "average stations",
                                          "average sources",
                                          "average baselines",
                                          "idle time",
                                          "low declination",
                                          "low declination begin",
                                          "low declination full",
                                          "low elevation",
                                          "low elevation begin",
                                          "low elevation full",
                                          "influence distance",
                                          "weight",
                                          "min slew distance",
                                          "max slew distance",
                                          "min elevation",
                                          "min flux",
                                          "min sun distance"};

            std::map<QString, QVector<double>> defaultValues;
            defaultValues["min slew time"         ] = {0, 150};
            defaultValues["max slew time"         ] = {150, 300, 450};
            defaultValues["max wait time"         ] = {150, 300, 450};
            defaultValues["max scan time"         ] = {900, 600, 300};
            defaultValues["min scan time"         ] = {20, 30, 40};
            defaultValues["min number of stations"] = {2, 3, 4};
            defaultValues["min repeat time"       ] = {1200, 1800};
            defaultValues["idle time interval"    ] = {120, 180, 240, 300};
            defaultValues["influence time"        ] = {900, 1200, 1800, 3600};
            defaultValues["max number of scans"   ] = {10, 25, 999};
            defaultValues["focus corner switch cadence"] = {600, 750, 900};

            defaultValues["subnetting min source angle"          ] = {120, 150};
            defaultValues["subnetting min participating stations"] = {60, 80, 100};
            defaultValues["sky-coverage"                         ] = {0.00, 0.33, 0.67, 1.00};
            defaultValues["number of observations"               ] = {0.00, 0.33, 0.67, 1.00};
            defaultValues["duration"                             ] = {0.00, 0.33, 0.67, 1.00};
            defaultValues["average stations"                     ] = {0.00, 0.33, 0.67, 1.00};
            defaultValues["average sources"                      ] = {0.00, 0.33, 0.67, 1.00};
            defaultValues["average baselines"                    ] = {0.00, 0.33, 0.67, 1.00};
            defaultValues["idle time"                            ] = {0.00, 0.33, 0.67, 1.00};
            defaultValues["low declination"                      ] = {0.00, 0.33, 0.67, 1.00};
            defaultValues["low declination begin"                ] = {0, -22.5, -45};
            defaultValues["low declination full"                 ] = {-75, -90};
            defaultValues["low elevation"                        ] = {0.00, 0.33, 0.67, 1.00};
            defaultValues["low elevation begin"                  ] = {40, 30, 20};
            defaultValues["low elevation full"                   ] = {20, 10};
            defaultValues["influence distance"                   ] = {15, 30, 45};
            defaultValues["weight"                               ] = {1, 1.5, 2, 3};
            defaultValues["min slew distance"                    ] = {0, 20, 40};
            defaultValues["max slew distance"                    ] = {90, 60};
            defaultValues["min elevation"                        ] = {5, 10};
            defaultValues["min flux"                             ] = {0.01, 0.15, 0.25, 0.35};
            defaultValues["min sun distance"                     ] = {4, 10};

            QIcon ic;
            if(parameterType == "general"){
                ic = QIcon(":/icons/icons/applications-internet-2.png");
            }else if(parameterType == "weight factor"){
                ic = QIcon(":/icons/icons/weight.png");
            }else if(parameterType == "sky-coverage"){
                ic = QIcon(":/icons/icons/sky_coverage.png");
            }else if(parameterType == "station"){
                ic = QIcon(":/icons/icons/station.png");
            }else if(parameterType == "source"){
                ic = QIcon(":/icons/icons/source.png");
            }else if(parameterType == "baseline"){
                ic = QIcon(":/icons/icons/baseline.png");
            }

            auto t = ui->treeWidget_multiSchedSelected;

            QTreeWidgetItem *itm = new QTreeWidgetItem();

            if(row2toggle.indexOf(name) != -1){
                if(parameterType == "general" || parameterType == "weight factor" || parameterType == "sky-coverage"){
                    any->setDisabled(true);
                }
                QString valuesString = "True, False";

                itm->setText(0,name);
                itm->setIcon(0,ic);
                itm->setText(1,"global");
                itm->setIcon(1,QIcon(":/icons/icons/applications-internet-2.png"));
                itm->setText(2,"2");

                QComboBox *cb = new QComboBox(this);
                cb->addItem("True");
                cb->addItem("False");

                t->addTopLevelItem(itm);
                t->setItemWidget(itm,3,cb);

            }else if(row2intDialog.indexOf(name) != -1){
                multiSchedEditDialogInt *dialog = new multiSchedEditDialogInt(this);
                if(parameterType == "station"){
                    dialog->addMember(allStationPlusGroupModel);
                }else if(parameterType == "source"){
                    dialog->addMember(allSourcePlusGroupModel);
                }else if(parameterType == "baseline"){
                    dialog->addMember(allBaselinePlusGroupModel);
                }
                dialog->addDefaultValues(defaultValues[name], random);

                int result = dialog->exec();
                if(result == QDialog::Accepted){
                    if(parameterType == "general" || parameterType == "weight factor" || parameterType == "sky-coverage"){
                        any->setDisabled(true);
                    }
                    QVector<int> val = dialog->getValues();
                    int n = val.size();
                    if(parameterType == "station" || parameterType == "source" || parameterType == "baseline"){
                        QStandardItem* member = dialog->getMember();
                        itm->setText(1,member->text());
                        itm->setIcon(1,member->icon());
                    }else if(parameterType == "weight factor"){
                        itm->setText(1,"global");
                        itm->setIcon(1,QIcon(":/icons/icons/weight.png"));
                    }else if(parameterType == "sky-coverage"){
                        itm->setText(1,"global");
                        itm->setIcon(1,QIcon(":/icons/icons/sky_coverage.png"));
                    }else{
                        itm->setText(1,"global");
                        itm->setIcon(1,QIcon(":/icons/icons/applications-internet-2.png"));
                    }
                    QComboBox *cb = new QComboBox(this);
                    for(const auto& any:val){
                        cb->addItem(QString::number(any));
                    }

                    itm->setText(2,QString::number(n));
                    itm->setText(0,name);
                    itm->setIcon(0,ic);
                    t->addTopLevelItem(itm);
                    t->setItemWidget(itm,3,cb);

                }
                delete(dialog);

            }else if(row2doubleDialog.indexOf(name) != -1){
                multiSchedEditDialogDouble *dialog = new multiSchedEditDialogDouble(this);
                if(parameterType == "station"){
                    dialog->addMember(allStationPlusGroupModel);
                }else if(parameterType == "source"){
                    dialog->addMember(allSourcePlusGroupModel);
                }else if(parameterType == "baseline"){
                    dialog->addMember(allBaselinePlusGroupModel);
                }else if(parameterType == "weight factor"){
                    itm->setText(1,"global");
                    itm->setIcon(1,QIcon(":/icons/icons/weight.png"));
                }
                dialog->addDefaultValues(defaultValues[name], random, parameterType == "weight factor");

                int result = dialog->exec();
                if(result == QDialog::Accepted){
                    if(parameterType == "general" || parameterType == "weight factor" || parameterType == "sky-coverage"){
                        any->setDisabled(true);
                    }
                    QVector<double> val = dialog->getValues();
                    int n = val.size();

                    if(parameterType == "station" || parameterType == "source" || parameterType == "baseline"){
                        QStandardItem* member = dialog->getMember();
                        itm->setText(1,member->text());
                        itm->setIcon(1,member->icon());
                    }else if(parameterType == "sky-coverage"){
                        itm->setText(1,"global");
                        itm->setIcon(1,QIcon(":/icons/icons/sky_coverage.png"));
                    }else{
                        itm->setText(1,"global");
                        itm->setIcon(1,QIcon(":/icons/icons/applications-internet-2.png"));
                    }
                    QComboBox *cb = new QComboBox(this);
                    for(const auto& any:val){
                        cb->addItem(QString::number(any));
                    }

                    itm->setText(2,QString::number(n));
                    itm->setText(0,name);
                    itm->setIcon(0,ic);
                    t->addTopLevelItem(itm);
                    t->setItemWidget(itm,3,cb);
                }
                delete(dialog);
            }

            multi_sched_count_nsched();
        }
    }
}

void MulitSchedulingWidget::on_pushButton_25_clicked()
{
    auto list = ui->treeWidget_multiSchedSelected->selectedItems();{
        for(const auto& any:list){
            if(any->text(0) == "session start"){
//                ui->treeWidget_multiSched->topLevelItem(0)->child(0)->setDisabled(false);
            }else if(any->text(0) == "subnetting"){
                ui->treeWidget_multiSched->topLevelItem(0)->child(0)->setDisabled(false);
            }else if(any->text(0) == "subnetting min source angle"){
                ui->treeWidget_multiSched->topLevelItem(0)->child(1)->setDisabled(false);
            }else if(any->text(0) == "subnetting min participating stations"){
                ui->treeWidget_multiSched->topLevelItem(0)->child(2)->setDisabled(false);
            }else if(any->text(0) == "fillin-mode during scan selection"){
                ui->treeWidget_multiSched->topLevelItem(0)->child(3)->setDisabled(false);
            }else if(any->text(0) == "fillin-mode influence on scan selection"){
                ui->treeWidget_multiSched->topLevelItem(0)->child(4)->setDisabled(false);
            }else if(any->text(0) == "fillin-mode a posteriori"){
                ui->treeWidget_multiSched->topLevelItem(0)->child(5)->setDisabled(false);
            }else if(any->text(0) == "focus corner switch cadence"){
                ui->treeWidget_multiSched->topLevelItem(0)->child(6)->setDisabled(false);

            }else if(any->text(0) == "sky-coverage"){
                ui->treeWidget_multiSched->topLevelItem(1)->child(0)->setDisabled(false);
            }else if(any->text(0) == "number of observations"){
                ui->treeWidget_multiSched->topLevelItem(1)->child(1)->setDisabled(false);
            }else if(any->text(0) == "duration"){
                ui->treeWidget_multiSched->topLevelItem(1)->child(2)->setDisabled(false);
            }else if(any->text(0) == "average stations"){
                ui->treeWidget_multiSched->topLevelItem(1)->child(3)->setDisabled(false);
            }else if(any->text(0) == "average sources"){
                ui->treeWidget_multiSched->topLevelItem(1)->child(4)->setDisabled(false);
            }else if(any->text(0) == "average baselines"){
                ui->treeWidget_multiSched->topLevelItem(1)->child(5)->setDisabled(false);
            }else if(any->text(0) == "idle time"){
                ui->treeWidget_multiSched->topLevelItem(1)->child(6)->setDisabled(false);
            }else if(any->text(0) == "idle time interval"){
                ui->treeWidget_multiSched->topLevelItem(1)->child(7)->setDisabled(false);
            }else if(any->text(0) == "low declination"){
                ui->treeWidget_multiSched->topLevelItem(1)->child(8)->setDisabled(false);
            }else if(any->text(0) == "low declination begin"){
                ui->treeWidget_multiSched->topLevelItem(1)->child(9)->setDisabled(false);
            }else if(any->text(0) == "low declination full"){
                ui->treeWidget_multiSched->topLevelItem(1)->child(10)->setDisabled(false);
            }else if(any->text(0) == "low elevation"){
                ui->treeWidget_multiSched->topLevelItem(1)->child(11)->setDisabled(false);
            }else if(any->text(0) == "low elevation begin"){
                ui->treeWidget_multiSched->topLevelItem(1)->child(12)->setDisabled(false);
            }else if(any->text(0) == "low elevation full"){
                ui->treeWidget_multiSched->topLevelItem(1)->child(13)->setDisabled(false);

            }else if(any->text(0) == "influence distance"){
                ui->treeWidget_multiSched->topLevelItem(2)->child(0)->setDisabled(false);
            }else if(any->text(0) == "influence time"){
                ui->treeWidget_multiSched->topLevelItem(2)->child(1)->setDisabled(false);
            }
            delete(any);
        }
    }

    multi_sched_count_nsched();

}

void MulitSchedulingWidget::on_comboBox_nThreads_currentTextChanged(const QString &arg1)
{
    if(arg1 == "manual"){
        ui->label_nCores->setEnabled(true);
        ui->spinBox_nCores->setEnabled(true);
    }else{
        ui->label_nCores->setEnabled(false);
        ui->spinBox_nCores->setEnabled(false);
    }
}

void MulitSchedulingWidget::on_comboBox_jobSchedule_currentTextChanged(const QString &arg1)
{
    if(arg1 == "auto"){
        ui->label_chunkSize->setEnabled(false);
        ui->spinBox_chunkSize->setEnabled(false);
    }else{
        ui->label_chunkSize->setEnabled(true);
        ui->spinBox_chunkSize->setEnabled(true);
    }
}

void MulitSchedulingWidget::on_comboBox_multiSched_maxNumber_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "all"){
        ui->spinBox_multiSched_maxNumber->setEnabled(false);
        ui->comboBox_multiSched_seed->setEnabled(false);
        ui->label_multiSched_seed->setEnabled(false);
        ui->spinBox_multiSched_seed->setEnabled(false);
    } else {
        ui->spinBox_multiSched_maxNumber->setEnabled(true);
        ui->comboBox_multiSched_seed->setEnabled(true);
        ui->label_multiSched_seed->setEnabled(true);
        on_comboBox_multiSched_seed_currentIndexChanged(ui->comboBox_multiSched_seed->currentText());
    }
}

void MulitSchedulingWidget::on_comboBox_multiSched_seed_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "random"){
        ui->spinBox_multiSched_seed->setEnabled(false);
    } else {
        ui->spinBox_multiSched_seed->setEnabled(true);
    }
}

void MulitSchedulingWidget::multi_sched_count_nsched()
{

    auto t = ui->treeWidget_multiSchedSelected;

    int nsched = 1;
    if(!ui->spinBox_multiSched_maxNumber->isEnabled()) {
        double wsky_ = 0;
        if(checkBox_weightCoverage->isChecked()){
            wsky_ = doubleSpinBox_weightSkyCoverage->value();
        }
        double wobs_ = 0;
        if(checkBox_weightNobs->isChecked()){
            wobs_ = doubleSpinBox_weightNumberOfObservations->value();
        }
        double wdur_ = 0;
        if(checkBox_weightDuration->isChecked()){
            wdur_ = doubleSpinBox_weightDuration->value();
        }
        double wasrc_ = 0;
        if(checkBox_weightAverageSources->isChecked()){
            wasrc_ = doubleSpinBox_weightAverageSources->value();
        }
        double wasta_ = 0;
        if(checkBox_weightAverageStations->isChecked()){
            wasta_ = doubleSpinBox_weightAverageStations->value();
        }
        double wabls_ = 0;
        if(checkBox_weightAverageBaselines->isChecked()){
            wabls_ = doubleSpinBox_weightAverageBaselines->value();
        }
        double widle_ = 0;
        if(checkBox_weightIdleTime->isChecked()){
            widle_ = doubleSpinBox_weightIdleTime->value();
        }
        double wdec_ = 0;
        if(checkBox_weightLowDeclination->isChecked()){
            wdec_ = doubleSpinBox_weightLowDec->value();
        }
        double wel_ = 0;
        if(checkBox_weightLowElevation->isChecked()){
            wel_ = doubleSpinBox_weightLowEl->value();
        }


        std::map<std::string,std::vector<double>> weightFactors = {{"weight_factor_sky_coverage",std::vector<double>{wsky_}},
                                                        {"weight_factor_number_of_observations",std::vector<double>{wobs_}},
                                                        {"weight_factor_duration",std::vector<double>{wdur_}},
                                                        {"weight_factor_average_sources",std::vector<double>{wasrc_}},
                                                        {"weight_factor_average_stations",std::vector<double>{wasta_}},
                                                        {"weight_factor_average_baselines",std::vector<double>{wabls_}},
                                                        {"weight_factor_idle_time",std::vector<double>{widle_}},
                                                        {"weight_factor_low_declination",std::vector<double>{wdec_}},
                                                        {"weight_factor_low_elevation",std::vector<double>{wel_}}};

        bool weigthFactorFound = false;
        for(int i = 0; i<t->topLevelItemCount(); ++i){
            if(t->topLevelItem(i)->text(0) == "sky-coverage"){
                QComboBox *list = qobject_cast<QComboBox*>(t->itemWidget(t->topLevelItem(i),3));
                std::vector<double> values;
                for(int ilist = 0; ilist<list->count(); ++ilist){
                    values.push_back( QString(list->itemText(ilist)).toDouble());
                }
                weightFactors["weight_factor_sky_coverage"] = values;
                weigthFactorFound = true;
            }else if(t->topLevelItem(i)->text(0) == "number of observations"){
                QComboBox *list = qobject_cast<QComboBox*>(t->itemWidget(t->topLevelItem(i),3));
                std::vector<double> values;
                for(int ilist = 0; ilist<list->count(); ++ilist){
                    values.push_back( QString(list->itemText(ilist)).toDouble());
                }
                weightFactors["weight_factor_number_of_observations"] = values;
                weigthFactorFound = true;
            }else if(t->topLevelItem(i)->text(0) == "duration"){
                QComboBox *list = qobject_cast<QComboBox*>(t->itemWidget(t->topLevelItem(i),3));
                std::vector<double> values;
                for(int ilist = 0; ilist<list->count(); ++ilist){
                    values.push_back( QString(list->itemText(ilist)).toDouble());
                }
                weightFactors["weight_factor_duration"] = values;
                weigthFactorFound = true;
            }else if(t->topLevelItem(i)->text(0) == "average stations"){
                QComboBox *list = qobject_cast<QComboBox*>(t->itemWidget(t->topLevelItem(i),3));
                std::vector<double> values;
                for(int ilist = 0; ilist<list->count(); ++ilist){
                    values.push_back( QString(list->itemText(ilist)).toDouble());
                }
                weightFactors["weight_factor_average_stations"] = values;
                weigthFactorFound = true;
            }else if(t->topLevelItem(i)->text(0) == "average baselines"){
                QComboBox *list = qobject_cast<QComboBox*>(t->itemWidget(t->topLevelItem(i),3));
                std::vector<double> values;
                for(int ilist = 0; ilist<list->count(); ++ilist){
                    values.push_back( QString(list->itemText(ilist)).toDouble());
                }
                weightFactors["weight_factor_average_baselines"] = values;
                weigthFactorFound = true;
            }else if(t->topLevelItem(i)->text(0) == "average sources"){
                QComboBox *list = qobject_cast<QComboBox*>(t->itemWidget(t->topLevelItem(i),3));
                std::vector<double> values;
                for(int ilist = 0; ilist<list->count(); ++ilist){
                    values.push_back( QString(list->itemText(ilist)).toDouble());
                }
                weightFactors["weight_factor_average_sources"] = values;
                weigthFactorFound = true;
            }else if(t->topLevelItem(i)->text(0) == "idle time"){
                QComboBox *list = qobject_cast<QComboBox*>(t->itemWidget(t->topLevelItem(i),3));
                std::vector<double> values;
                for(int ilist = 0; ilist<list->count(); ++ilist){
                    values.push_back( QString(list->itemText(ilist)).toDouble());
                }
                weightFactors["weight_factor_idle_time"] = values;
                weigthFactorFound = true;
            }else if(t->topLevelItem(i)->text(0) == "low declination"){
                QComboBox *list = qobject_cast<QComboBox*>(t->itemWidget(t->topLevelItem(i),3));
                std::vector<double> values;
                for(int ilist = 0; ilist<list->count(); ++ilist){
                    values.push_back( QString(list->itemText(ilist)).toDouble());
                }
                weightFactors["weight_factor_low_declination"] = values;
                weigthFactorFound = true;
            }else if(t->topLevelItem(i)->text(0) == "low elevation"){
                QComboBox *list = qobject_cast<QComboBox*>(t->itemWidget(t->topLevelItem(i),3));
                std::vector<double> values;
                for(int ilist = 0; ilist<list->count(); ++ilist){
                    values.push_back( QString(list->itemText(ilist)).toDouble());
                }
                weightFactors["weight_factor_low_elevation"] = values;
                weigthFactorFound = true;
            }
        }

        std::vector<std::vector<double> > weightFactorValues;
        if(weigthFactorFound){
            for (double wsky: weightFactors["weight_factor_sky_coverage"]) {
                for (double wobs: weightFactors["weight_factor_number_of_observations"]) {
                    for (double wdur: weightFactors["weight_factor_duration"]) {
                        for (double wasrc: weightFactors["weight_factor_average_sources"]) {
                            for (double wasta: weightFactors["weight_factor_average_stations"]) {
                                for (double wabls: weightFactors["weight_factor_average_baselines"]) {
                                    for (double widle: weightFactors["weight_factor_idle_time"]) {
                                        for (double wdec: weightFactors["weight_factor_low_declination"]) {
                                            for (double wel: weightFactors["weight_factor_low_elevation"]) {

                                                double sum = wsky + wobs + wdur + wasrc + wasta + wabls + widle + wdec + wel;

                                                if (sum == 0) {
                                                    continue;
                                                }

                                                std::vector<double> wf{wsky/sum, wobs/sum, wdur/sum, wasrc/sum, wasta/sum,
                                                                       wabls/sum, widle/sum, wdec/sum, wel/sum};
                                                weightFactorValues.push_back(std::move(wf));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // remove duplicated weight factors
        int i1 = 0;
        while (i1 < weightFactorValues.size()) {
            const std::vector<double> &v1 = weightFactorValues[i1];
            int i2 = i1 + 1;

            while (i2 < weightFactorValues.size()) {
                const std::vector<double> &v2 = weightFactorValues[i2];
                int equal = 0;
                for (int i3 = 0; i3 < v1.size(); ++i3) {
                    if (fabs(v1[i3] - v2[i3]) < 1e-10) {
                        ++equal;
                    }
                }
                if (equal == v1.size()) {
                    weightFactorValues.erase(next(weightFactorValues.begin(), i2));
                } else {
                    ++i2;
                }
            }
            ++i1;
        }

        if (!weightFactorValues.empty()) {
            nsched = weightFactorValues.size();
        }

        QStringList weightFactorsStr {"sky-coverage",
                                      "number of observations",
                                      "duration",
                                      "average stations",
                                      "average sources",
                                      "average baselines",
                                      "idle time",
                                      "low declination",
                                      "low elevation"};

        for(int i = 0; i<t->topLevelItemCount(); ++i){
            if(weightFactorsStr.indexOf(t->topLevelItem(i)->text(0)) != -1){
                continue;
            }
            nsched *= t->topLevelItem(i)->text(2).toInt();
        }
    }else{
        nsched = ui->spinBox_multiSched_maxNumber->value();
    }

    if(ui->groupBox_ms_gen->isChecked()){
        nsched += ui->spinBox_ms_gen_popsize->value() * (ui->spinBox_ms_gen_iterations->value() - 1);
    }

    if(nsched > 300){
        ui->label_multiSchedulingNsched->setText(QString("total number of schedules: %1 (this might take some time - consider setting a maximum number of schedules)").arg(nsched));
        ui->label_multiSchedulingNsched->setStyleSheet("color : red; font-weight : bold");
    }else if(nsched > 100){
        ui->label_multiSchedulingNsched->setText(QString("total number of schedules: %1 (this might take some time - consider setting a maximum number of schedules)").arg(nsched));
        ui->label_multiSchedulingNsched->setStyleSheet("color : orange; font-weight : bold");
    }else if(nsched > 50){
        ui->label_multiSchedulingNsched->setText(QString("total number of schedules: %1").arg(nsched));
        ui->label_multiSchedulingNsched->setStyleSheet("font-weight : bold");
    }else{
        ui->label_multiSchedulingNsched->setText(QString("total number of schedules: %1").arg(nsched));
        ui->label_multiSchedulingNsched->setStyleSheet("");
    }

    if(!ui->pushButton_ms_pick_random->isChecked()){
        if(nsched>9999){
            ui->spinBox_multiSched_maxNumber->setValue(9999);
            ui->comboBox_multiSched_maxNumber->setCurrentIndex(1);
            ui->comboBox_multiSched_maxNumber->setEnabled(false);
        }else{
            ui->spinBox_multiSched_maxNumber->setValue(nsched);
            ui->comboBox_multiSched_maxNumber->setEnabled(true);
        }
    }else{
        if(nsched >9999){
            QMessageBox::warning(this,"ignoring multi scheduling","Too many possible multi scheduling parameters!\nMulti scheduling will be ignored");
        }
    }

}


void MulitSchedulingWidget::toXML(VieVS::ParameterSettings &para){
     std::unordered_map<std::string, std::vector<std::string>> gsta;
     std::unordered_map<std::string, std::vector<std::string>> gsrc;
     std::unordered_map<std::string, std::vector<std::string>> gbl;
     for(const auto &any: *groupSta){
         gsta[any.first] = any.second;
     }
     for(const auto &any: *groupSrc){
         gsrc[any.first] = any.second;
     }
     for(const auto &any: *groupBl){
         gbl[any.first] = any.second;
     }

     VieVS::MultiScheduling ms(gsta, gsrc, gbl);
     QIcon icSta = QIcon(":/icons/icons/station.png");
     QIcon icSrc = QIcon(":/icons/icons/source.png");
     QIcon icBl = QIcon(":/icons/icons/baseline.png");
     QIcon icStaGrp = QIcon(":/icons/icons/station_group.png");
     QIcon icSrcGrp = QIcon(":/icons/icons/source_group.png");
     QIcon icBlGrp = QIcon(":/icons/icons/baseline_group.png");

     for(int i = 0; i<ui->treeWidget_multiSchedSelected->topLevelItemCount(); ++i){
         auto itm = ui->treeWidget_multiSchedSelected->topLevelItem(i);
         QString parameter = itm->text(0);
         QIcon parameterIcon = itm->icon(0);
         std::string member = itm->text(1).toStdString();
         QIcon memberIcon = itm->icon(0);
         QComboBox *list = qobject_cast<QComboBox*>(ui->treeWidget_multiSchedSelected->itemWidget(itm,3));

         QStringList parameter2dateTime {"session start"};

         QStringList parameter2toggle{"subnetting",
                                      "subnetting min participating stations",
                                      "subnetting min source angle",
                                      "fillin-mode during scan selection",
                                      "fillin-mode influence on scan selection",
                                      "fillin-mode a posteriori"};

         QStringList parameter2double {"min slew time",
                                       "max slew time",
                                       "max wait time",
                                       "max scan time",
                                       "min scan time",
                                       "min number of stations",
                                       "min repeat time",
                                       "idle time interval",
                                       "max number of scans",
                                       "subnetting min source angle",
                                       "subnetting min participating stations",
                                       "sky-coverage",
                                       "number of observations",
                                       "duration",
                                       "average stations",
                                       "average baselines",
                                       "average sources",
                                       "idle time",
                                       "low declination",
                                       "low declination begin",
                                       "low declination full",
                                       "low elevation",
                                       "low elevation begin",
                                       "low elevation full",
                                       "influence distance",
                                       "influence time",
                                       "weight",
                                       "min slew distance",
                                       "max slew distance",
                                       "min elevation",
                                       "min flux",
                                       "min sun distance",
                                       "focus corner switch cadence"};


         std::vector<double> vecDouble;
         std::vector<unsigned int> vecInt;
         if(parameter2double.indexOf(parameter) != -1 ){
             for(int j = 0; j<list->count(); ++j){
                 vecDouble.push_back( QString(list->itemText(j)).toDouble());
             }
    //            }else if(parameter2int.indexOf(parameter) != -1){
    //                for(int j = 0; j<list->count(); ++j){
    //                    vecInt.push_back( QString(list->itemText(j)).toInt());
    //                }
         }

         if(parameterIcon.pixmap(16,16).toImage() == icSta.pixmap(16,16).toImage() || parameterIcon.pixmap(16,16).toImage() == icStaGrp.pixmap(16,16).toImage()){
             if(parameter == "weight"){
                 ms.addParameters(std::string("station_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }else if(parameter == "min slew time"){
                 ms.addParameters(std::string("station_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }else if(parameter == "max slew time"){
                 ms.addParameters(std::string("station_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }else if(parameter == "min slew distance"){
                 ms.addParameters(std::string("station_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }else if(parameter == "max slew distance"){
                 ms.addParameters(std::string("station_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }else if(parameter == "max wait time"){
                 ms.addParameters(std::string("station_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }else if(parameter == "min elevation"){
                 ms.addParameters(std::string("station_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }else if(parameter == "max number of scans"){
                 ms.addParameters(std::string("station_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }else if(parameter == "max scan time"){
                 ms.addParameters(std::string("station_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }else if(parameter == "min scan time"){
                 ms.addParameters(std::string("station_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }

         }else if(parameterIcon.pixmap(16,16).toImage() == icSrc.pixmap(16,16).toImage() || parameterIcon.pixmap(16,16).toImage() == icSrcGrp.pixmap(16,16).toImage()){
             if(parameter == "weight"){
                 ms.addParameters(std::string("source_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }else if(parameter == "min number of stations"){
                 ms.addParameters(std::string("source_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }else if(parameter == "min flux"){
                 ms.addParameters(std::string("source_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }else if(parameter == "max number of scans"){
                 ms.addParameters(std::string("source_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }else if(parameter == "min elevation"){
                 ms.addParameters(std::string("source_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }else if(parameter == "min sun distance"){
                 ms.addParameters(std::string("source_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }else if(parameter == "max scan time"){
                 ms.addParameters(std::string("source_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }else if(parameter == "min scan time"){
                 ms.addParameters(std::string("source_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }else if(parameter == "min repeat time"){
                 ms.addParameters(std::string("source_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }

         }else if(parameterIcon.pixmap(16,16).toImage() == icBl.pixmap(16,16).toImage() || parameterIcon.pixmap(16,16).toImage() == icBlGrp.pixmap(16,16).toImage()){
             if(parameter == "weight"){
                 ms.addParameters(std::string("baseline_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }else if(parameter == "max scan time"){
                 ms.addParameters(std::string("baseline_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }else if(parameter == "min scan time"){
                 ms.addParameters(std::string("baseline_").append(parameter.replace(' ','_').toStdString()), member, vecDouble);
             }

         }else{
             if(parameter == "session start"){
                 std::vector<boost::posix_time::ptime> times;
                 for(int j = 0; j<list->count(); ++j){
                     QString txt = list->itemText(j);
                     int year = QString(txt.mid(6,4)).toInt();
                     int month = QString(txt.mid(3,2)).toInt();
                     int day = QString(txt.mid(0,2)).toInt();
                     int hour = QString(txt.mid(11,2)).toInt();
                     int min = QString(txt.mid(14,2)).toInt();
                     int sec = 0;
                     boost::posix_time::ptime t(boost::gregorian::date(year,month,day),boost::posix_time::time_duration(hour,min,sec));
                     times.push_back(t);
                 }
                 ms.setStart(times);
             }else if(parameter == "subnetting"){
                 ms.addParameters(std::string("general_").append(parameter.replace(' ','_').toStdString()));
             }else if(parameter == "fillin-mode during scan selection"){
                 ms.addParameters(std::string("general_").append(parameter.replace(' ','_').toStdString()));
             }else if(parameter == "fillin-mode influence on scan selection"){
                 ms.addParameters(std::string("general_").append(parameter.replace(' ','_').toStdString()));
             }else if(parameter == "fillin-mode a posteriori"){
                 ms.addParameters(std::string("general_").append(parameter.replace(' ','_').toStdString()));

             }else if(parameter == "focus corner switch cadence"){
                 ms.addParameters(std::string("general_").append(parameter.replace(' ','_').toStdString()), vecDouble);
             }else if(parameter == "subnetting min participating stations"){
                 ms.addParameters(std::string("general_").append(parameter.replace(' ','_').toStdString()), vecDouble);
             }else if(parameter == "subnetting min source angle"){
                 ms.addParameters(std::string("general_").append(parameter.replace(' ','_').toStdString()), vecDouble);
             }else if(parameter == "sky-coverage"){
                 ms.addParameters(std::string("weight_factor_").append(parameter.replace(' ','_').toStdString()), vecDouble);
             }else if(parameter == "number of observations"){
                 ms.addParameters(std::string("weight_factor_").append(parameter.replace(' ','_').toStdString()), vecDouble);
             }else if(parameter == "duration"){
                 ms.addParameters(std::string("weight_factor_").append(parameter.replace(' ','_').toStdString()), vecDouble);
             }else if(parameter == "average stations"){
                 ms.addParameters(std::string("weight_factor_").append(parameter.replace(' ','_').toStdString()), vecDouble);
             }else if(parameter == "average baselines"){
                 ms.addParameters(std::string("weight_factor_").append(parameter.replace(' ','_').toStdString()), vecDouble);
             }else if(parameter == "average sources"){
                 ms.addParameters(std::string("weight_factor_").append(parameter.replace(' ','_').toStdString()), vecDouble);
             }else if(parameter == "idle time"){
                 ms.addParameters(std::string("weight_factor_").append(parameter.replace(' ','_').toStdString()), vecDouble);
             }else if(parameter == "idle time interval"){
                 ms.addParameters(std::string("weight_factor_").append(parameter.replace(' ','_').toStdString()), vecDouble);
             }else if(parameter == "low declination"){
                 ms.addParameters(std::string("weight_factor_").append(parameter.replace(' ','_').toStdString()), vecDouble);
             }else if(parameter == "low declination begin"){
                 ms.addParameters(std::string("weight_factor_").append(parameter.replace(' ','_').toStdString()), vecDouble);
             }else if(parameter == "low declination full"){
                 ms.addParameters(std::string("weight_factor_").append(parameter.replace(' ','_').toStdString()), vecDouble);
             }else if(parameter == "low elevation"){
                 ms.addParameters(std::string("weight_factor_").append(parameter.replace(' ','_').toStdString()), vecDouble);
             }else if(parameter == "low elevation begin"){
                 ms.addParameters(std::string("weight_factor_").append(parameter.replace(' ','_').toStdString()), vecDouble);
             }else if(parameter == "low elevation full"){
                 ms.addParameters(std::string("weight_factor_").append(parameter.replace(' ','_').toStdString()), vecDouble);
             }else if(parameter == "influence distance"){
                 ms.addParameters(std::string("sky-coverage_").append(parameter.replace(' ','_').toStdString()), vecDouble);
             }else if(parameter == "influence time"){
                 ms.addParameters(std::string("sky-coverage_").append(parameter.replace(' ','_').toStdString()), vecDouble);
             }
         }
     }

     std::string countNr = ui->comboBox_multiSched_maxNumber->currentText().toStdString();
     int n = ui->spinBox_multiSched_maxNumber->value();
     std::string seedStr = ui->comboBox_multiSched_seed->currentText().toStdString();
     int seed = ui->spinBox_multiSched_seed->value();
     bool pickRandom = ui->pushButton_ms_pick_random->isChecked();
     para.multisched(ms.createPropertyTree(),countNr,n,seedStr,seed, pickRandom);

     std::string threads = ui->comboBox_nThreads->currentText().toStdString();
     int nThreadsManual = ui->spinBox_nCores->value();
     std::string jobScheduler = ui->comboBox_jobSchedule->currentText().toStdString();
     int chunkSize = ui->spinBox_chunkSize->value();
     para.multiCore(threads,nThreadsManual,jobScheduler,chunkSize);


     if(ui->groupBox_ms_gen->isChecked()){
      int iterations = ui->spinBox_ms_gen_iterations->value();
      int parents = ui->spinBox_ms_gen_parents->value();
      int popsize = ui->spinBox_ms_gen_popsize->value();
      double selectBest =ui->doubleSpinBox_ms_gen_selBest->value();
      double selectRandom = ui->doubleSpinBox_ms_gen_selRandom->value();
      double mutation = ui->doubleSpinBox_ms_gen_mutation->value();
      double minmutation = ui->doubleSpinBox_ms_gen_minMutation->value();
      para.mulitsched_genetic(iterations,popsize,selectBest,selectRandom,mutation,minmutation,parents);
     }

}



void MulitSchedulingWidget::fromXML(const boost::property_tree::ptree &xml){
    auto twmss = ui->treeWidget_multiSchedSelected;
    auto twms = ui->treeWidget_multiSched;
    twmss->clear();
    for(int i=0; i<3; ++i){
        for(int j=0; j<twms->topLevelItem(i)->childCount(); ++j){
            twms->topLevelItem(i)->child(j)->setDisabled(false);
        }
    }


    boost::optional<const boost::property_tree::ptree &> ctree_o = xml.get_child_optional("VieSchedpp.multisched");
    if(ctree_o.is_initialized()){
        groupBox_multiScheduling->setChecked(true);
        const boost::property_tree::ptree &ctree = ctree_o.get();
        ui->comboBox_multiSched_maxNumber->setCurrentText("all");
        ui->comboBox_multiSched_seed->setCurrentText("random");

        if(ctree.get_optional<int>("maxNumber").is_initialized()){
            ui->comboBox_multiSched_maxNumber->setCurrentText("select");
            ui->spinBox_multiSched_maxNumber->setValue(ctree.get<int>("maxNumber"));
        }
        if(ctree.get_optional<int>("seed").is_initialized()){
            ui->comboBox_multiSched_seed->setCurrentText("select");
            ui->spinBox_multiSched_seed->setValue(ctree.get<int>("seed"));
        }
        if(ctree.get_optional<bool>("pick_random").is_initialized()){
            ui->pushButton_ms_pick_random->setChecked(ctree.get<bool>("pick_random"));
        }

        for(const auto &any: ctree){
            QString name = QString::fromStdString(any.first);
            if(name == "maxNumber" || name == "seed" || name == "genetic"){
                continue;
            }
            QString parameterName;
            bool hasMember = false;
            QString member = "global";

            if(name.left(8) == "station_"){
                name = name.mid(8);
                hasMember = true;
                parameterName = "station";
            }else if(name.left(7) == "source_"){
                name = name.mid(7);
                hasMember = true;
                parameterName = "source";
            }else if(name.left(9) == "baseline_"){
                name = name.mid(9);
                hasMember = true;
                parameterName = "baseline";
            }else if(name.left(14) == "weight_factor_"){
                name = name.mid(14);
                hasMember = false;
                parameterName = "weight factor";
            }else if(name.left(8) == "general_"){
                name = name.mid(8);
                hasMember = false;
                parameterName = "general";
            }else if(name.left(13) == "sky-coverage_"){
                name = name.mid(13);
                hasMember = false;
                parameterName = "sky-coverage";
            }
            name.replace("_"," ");
            for(int i=0; i<3; ++i){
                for(int j=0; j<twms->topLevelItem(i)->childCount(); ++j){
                    if(twms->topLevelItem(i)->child(j)->text(0) == name){
                        parameterName = twms->topLevelItem(i)->text(0);
                        break;
                    }
                }
            }



            if(hasMember){
                member = QString::fromStdString(any.second.get<std::string>("<xmlattr>.member"));
            }
            QVector<double> values;
            if(name != "general subnetting" && name != "general fillin-mode during scan selection" &&
                    name != "general fillin-mode influence on scan selection" && name != "general fillin-mode a posteriori" ){
                for(const auto &any2 : any.second){
                    if(any2.first == "value"){
                        values.push_back(any2.second.get_value<double>());
                    }
                }
            }

            if(parameterName == "general"){
                for(int i=0; i<twms->topLevelItem(0)->childCount(); ++i){
                    if(name == twms->topLevelItem(0)->child(i)->text(0)){
                        twms->topLevelItem(0)->child(i)->setDisabled(true);
                        break;
                    }
                }
            }else if(parameterName == "weight factor"){
                for(int i=0; i<twms->topLevelItem(1)->childCount(); ++i){
                    if(name == twms->topLevelItem(1)->child(i)->text(0)){
                        twms->topLevelItem(1)->child(i)->setDisabled(true);
                        break;
                    }
                }
            }else if(parameterName == "sky-coverage"){
                for(int i=0; i<twms->topLevelItem(2)->childCount(); ++i){
                    if(name == twms->topLevelItem(2)->child(i)->text(0)){
                        twms->topLevelItem(2)->child(i)->setDisabled(true);
                        break;
                    }
                }
            }

            QIcon ic1;
            QIcon ic2;
            if(parameterName == "general"){
                ic1 = QIcon(":/icons/icons/applications-internet-2.png");
                ic2 = QIcon(":/icons/icons/applications-internet-2.png");
            }else if(parameterName == "weight factor"){
                ic1 = QIcon(":/icons/icons/weight.png");
                ic2 = QIcon(":/icons/icons/applications-internet-2.png");
            }else if(parameterName == "sky-coverage"){
                ic1 = QIcon(":/icons/icons/sky_coverage.png");
                ic2 = QIcon(":/icons/icons/sky_coverage.png");
            }else if(parameterName == "station"){
                ic1 = QIcon(":/icons/icons/station.png");
                if(member == "__all__" || groupSta->find(member.toStdString()) != groupSta->end()){
                    ic2 = QIcon(":/icons/icons/station_group.png");
                }else{
                    ic2 = QIcon(":/icons/icons/station.png");
                }
            }else if(parameterName == "source"){
                ic1 = QIcon(":/icons/icons/source.png");
                if(member == "__all__" || groupSrc->find(member.toStdString()) == groupSrc->end()){
                    ic2 = QIcon(":/icons/icons/source_group.png");
                }else{
                    ic2 = QIcon(":/icons/icons/source.png");
                }

            }else if(parameterName == "baseline"){
                ic1 = QIcon(":/icons/icons/baseline.png");
                if(member == "__all__" || groupBl->find(member.toStdString()) == groupBl->end()){
                    ic2 = QIcon(":/icons/icons/baseline.png");
                }else{
                    ic2 = QIcon(":/icons/icons/baseline_group.png");
                }

            }

            QTreeWidgetItem *itm = new QTreeWidgetItem();
            itm->setText(0,name);
            itm->setText(1,member);
            itm->setIcon(0,ic1);
            itm->setIcon(1,ic2);
            QComboBox *cb = new QComboBox(this);
            if(!values.empty()){
                itm->setText(2,QString::number(values.count()));
                for(const auto& any:values){
                    cb->addItem(QString::number(any));
                }
            }else{
                itm->setText(2,QString::number(2));
                cb->addItem("True");
                cb->addItem("False");
            }

            twmss->addTopLevelItem(itm);
            twmss->setItemWidget(itm,3,cb);
        }
    }
    multi_sched_count_nsched();

    ui->groupBox_ms_gen->setChecked(false);
    boost::optional<const boost::property_tree::ptree &> ctree = xml.get_child_optional("VieSchedpp.multisched.genetic");
    if (ctree.is_initialized()) {
        ui->groupBox_ms_gen->setChecked(true);
        ui->spinBox_ms_gen_iterations->setValue(xml.get("VieSchedpp.multisched.genetic.evolutions",2));
        ui->spinBox_ms_gen_parents->setValue(xml.get("VieSchedpp.multisched.genetic.parents_for_crossover",2));
        ui->spinBox_ms_gen_popsize->setValue(xml.get("VieSchedpp.multisched.genetic.population_size",128));
        ui->doubleSpinBox_ms_gen_selBest->setValue(xml.get("VieSchedpp.multisched.genetic.select_best_percent",10.0));
        ui->doubleSpinBox_ms_gen_selRandom->setValue(xml.get("VieSchedpp.multisched.genetic.select_random_percent",2.5));
        ui->doubleSpinBox_ms_gen_mutation->setValue(xml.get("VieSchedpp.multisched.genetic.mutation_acceleration",0.5));
        ui->doubleSpinBox_ms_gen_minMutation->setValue(xml.get("VieSchedpp.multisched.genetic.min_mutation_percent",10.0));
    }

}

void MulitSchedulingWidget::resetStationParameters(bool &mssta, bool &msbl)
{
    QIcon icSta = QIcon(":/icons/icons/station.png");
    QIcon icBl = QIcon(":/icons/icons/baseline.png");
    QIcon icStaGrp = QIcon(":/icons/icons/station_group.png");
    QIcon icBlGrp = QIcon(":/icons/icons/baseline_group.png");
    int i = 0;
    while(i < ui->treeWidget_multiSchedSelected->topLevelItemCount()){
        QIcon parameterIcon = ui->treeWidget_multiSchedSelected->topLevelItem(i)->icon(0);
        if(parameterIcon.pixmap(16,16).toImage() == icSta.pixmap(16,16).toImage() || parameterIcon.pixmap(16,16).toImage() == icStaGrp.pixmap(16,16).toImage()){
            auto itm = ui->treeWidget_multiSchedSelected->takeTopLevelItem(i);
            delete(itm);
            mssta = true;
            continue;
        }else if(parameterIcon.pixmap(16,16).toImage() == icBl.pixmap(16,16).toImage() || parameterIcon.pixmap(16,16).toImage() == icBlGrp.pixmap(16,16).toImage()){
            auto itm = ui->treeWidget_multiSchedSelected->takeTopLevelItem(i);
            delete(itm);
            msbl = true;
            continue;
        }else{
            ++i;
        }
    }
}

void MulitSchedulingWidget::resetSourceParameters(bool &mssrc)
{
    QIcon icSrc = QIcon(":/icons/icons/source.png");
    QIcon icSrcGrp = QIcon(":/icons/icons/source_group.png");
    int i=0;
    while(i < ui->treeWidget_multiSchedSelected->topLevelItemCount()){
        QIcon parameterIcon = ui->treeWidget_multiSchedSelected->topLevelItem(i)->icon(0);
        if(parameterIcon.pixmap(16,16).toImage() == icSrc.pixmap(16,16).toImage() || parameterIcon.pixmap(16,16).toImage() == icSrcGrp.pixmap(16,16).toImage()){
            auto itm = ui->treeWidget_multiSchedSelected->takeTopLevelItem(i);
            delete(itm);
            mssrc = true;
            continue;
        }else{
            ++i;
        }
    }
}

void MulitSchedulingWidget::clear()
{
    ui->treeWidget_multiSchedSelected->clear();
    for(int i=0; i<ui->treeWidget_multiSched->topLevelItemCount(); ++i){
        ui->treeWidget_multiSched->topLevelItem(i)->setDisabled(false);
        for (int j=0; j<ui->treeWidget_multiSched->topLevelItem(i)->childCount(); ++j){
            ui->treeWidget_multiSched->topLevelItem(i)->child(j)->setDisabled(false);
        }
    }
}

void MulitSchedulingWidget::defaultIntensive()
{
    groupBox_multiScheduling->setChecked(Qt::Checked);
    auto t = ui->treeWidget_multiSchedSelected;
    t->clear();

    QTreeWidgetItem *itm = new QTreeWidgetItem();
    QVector<double> val = {0, 0.01, 0.02, 0.03, 0.04, 0.06, 0.08, 0.1};
    int n = val.size();

    itm->setText(1,"global");
    itm->setIcon(1,QIcon(":/icons/icons/applications-internet-2.png"));

    QComboBox *cb = new QComboBox(this);
    for(const auto& any:val){
        cb->addItem(QString::number(any));
    }

    itm->setText(2,QString::number(n));
    itm->setText(0,"sky-coverage");
    itm->setIcon(0,QIcon(":/icons/icons/weight.png"));
    t->addTopLevelItem(itm);
    t->setItemWidget(itm,3,cb);
    ui->treeWidget_multiSched->topLevelItem(1)->child(0)->setDisabled(true);
}

void MulitSchedulingWidget::setMultiprocessing(std::string threads, int nThreadsManual, std::string jobScheduler, int chunkSize)
{
    ui->comboBox_nThreads->setCurrentText(QString::fromStdString(threads));
    ui->spinBox_nCores->setValue(nThreadsManual);
    ui->comboBox_jobSchedule->setCurrentText(QString::fromStdString(jobScheduler));
    ui->spinBox_chunkSize->setValue(chunkSize);
}

std::pair<QStringList, QStringList> MulitSchedulingWidget::getMultiCoreSupport()
{
    QString threads = ui->comboBox_nThreads->currentText();
    QString nThreadsManual = QString::number(ui->spinBox_nCores->value());
    QString jobScheduler = ui->comboBox_jobSchedule->currentText();
    QString chunkSize = QString::number(ui->spinBox_chunkSize->value());

    QStringList path {"settings.multiCore.threads", "settings.multiCore.nThreads", "settings.multiCore.jobScheduling", "settings.multiCore.chunkSize"};
    QStringList value {threads, nThreadsManual, jobScheduler, chunkSize};
    return {path,value};
}


