#include "calibratorblockwidget.h"
#include "ui_calibratorblockwidget.h"

CalibratorBlockWidget::CalibratorBlockWidget(QStandardItemModel *source_model,
                                 QStandardItemModel *station_model,
                                 QStandardItemModel *baseline_model,
                                 QDoubleSpinBox *session_duration,
                                 QWidget *parent) :
    QWidget(parent),
    source_model_{source_model},
    station_model_{station_model},
    baseline_model_{baseline_model},
    session_duration{session_duration},
    ui(new Ui::CalibratorBlockWidget)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    newSourceGroup = ui->pushButton_calibration_addSrcGroup;
    newSourceGroup2 = ui->pushButton_addSrcGroup_2;
    newSourceGroup3 = ui->pushButton_addSrcGroup_3;
    newBaselineGroup = ui->pushButton_addBaselineGroup;
    newStationGroup = ui->pushButton_add_station_group;

    default_blocks = ui->pushButton_save_blocks;
    default_general_setup = ui->pushButton_save_general;
    default_advanced_setup = ui->pushButton_save_advanced;
    save_para = ui->pushButton_save_para;
    save_dpara = ui->pushButton_save_dpara;


    ui->comboBox_calibrationBlockSources->setModel(source_model_);
    ui->comboBox_dpara_sources->setModel(source_model_);
    ui->comboBox_para_sources->setModel(source_model_);
    ui->comboBox_dpara_baseline->setModel(baseline_model_);
    ui->comboBox_para_station->setModel(station_model_);

    ui->tableWidget_calibrationBlock->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    on_spinBox_NCalibrationBlocks_valueChanged(ui->spinBox_NCalibrationBlocks->value());

}

QString CalibratorBlockWidget::reloadSources()
{
    QString warnings;
    auto *t = ui->tableWidget_calibrationBlock;
    for(int r = 0; r<t->rowCount(); ++r){
        QComboBox *tmp = qobject_cast<QComboBox *>(ui->tableWidget_calibrationBlock->cellWidget(r,3));
        if ( tmp->currentText() != "__all__" ){
            warnings = "Source selection within calibraton blocks changed!\n";
            tmp->setCurrentIndex(0);
        }
    }

    return warnings;
}

CalibratorBlockWidget::~CalibratorBlockWidget()
{
    delete ui;
}

boost::property_tree::ptree CalibratorBlockWidget::toXML()
{
    std::vector<VieVS::CalibratorBlock> blocks;

    auto *t = ui->tableWidget_calibrationBlock;
    for(int r = 0; r<t->rowCount(); ++r){
        double hour = qobject_cast<QDoubleSpinBox *>(ui->tableWidget_calibrationBlock->cellWidget(r,0))->value();
        unsigned int t = hour * 3600;
        unsigned int dur = qobject_cast<QSpinBox *>(ui->tableWidget_calibrationBlock->cellWidget(r,1))->value();
        unsigned int scans = qobject_cast<QSpinBox *>(ui->tableWidget_calibrationBlock->cellWidget(r,2))->value();
        std::string srcGroup = qobject_cast<QComboBox *>(ui->tableWidget_calibrationBlock->cellWidget(r,3))->currentText().toStdString();

        VieVS::CalibratorBlock tmp(t, scans, dur, srcGroup);
        blocks.push_back(tmp);
    }

    boost::property_tree::ptree rules;
    for ( const auto &any : blocks ) {
        boost::property_tree::ptree tmp;
        tmp.add( "block.startTime", any.getStartTime() );
        tmp.add( "block.scans", any.getNScans() );
        tmp.add( "block.duration", any.getDuration() );
        tmp.add( "block.sources", any.getAllowedSourceGroup() );
        rules.add_child( "calibration.block", tmp.get_child( "block" ) );
    }
    if ( ui->comboBox_calibrationIntent->currentText() != "NONE" ) {
        rules.add( "calibration.intent", ui->comboBox_calibrationIntent->currentText().toStdString() );
    }
    if ( !ui->checkBox_subnetting->isChecked()){
        rules.add( "calibration.subnetting", true);
    }
    if ( ui->checkBox_tryToIncludeAllStations->isChecked()){
        rules.add( "calibration.tryToIncludeAllStations", true);
        rules.add( "calibration.tryToIncludeAllStations_factor", ui->doubleSpinBox_stationFactor->value());
    }
    rules.add( "calibration.numberOfObservations_factor", ui->doubleSpinBox_nobs_f->value());
    rules.add( "calibration.numberOfObservations_offset", ui->doubleSpinBox_nobs_o->value());

    rules.add( "calibration.averageStations_factor", ui->doubleSpinBox_asta_f->value());
    rules.add( "calibration.averageStations_offset", ui->doubleSpinBox_asta_o->value());

    rules.add( "calibration.averageBaseline_factor", ui->doubleSpinBox_abl_f->value());
    rules.add( "calibration.averageBaseline_offset", ui->doubleSpinBox_abl_o->value());

    rules.add( "calibration.duration_factor", ui->doubleSpinBox_dur_f->value());
    rules.add( "calibration.duration_offset", ui->doubleSpinBox_dur_o->value());

    if ( ui->spinBox_dpara_scans->value() > 0){
        rules.add( "calibration.diffParallacticAngle.nscans", ui->spinBox_dpara_scans->value());
        boost::property_tree::ptree angles;
        for (int i = 0; i<ui->tableWidget_dParList->rowCount(); ++i ) {
            boost::property_tree::ptree tmp;
            double v =  qobject_cast<QDoubleSpinBox *>(ui->tableWidget_dParList->cellWidget(i,0))->value();
            tmp.add("angle",v);
            angles.add_child("angles.angle", tmp.get_child( "angle" ));
        }
        rules.add_child("calibration.diffParallacticAngle.angles", angles.get_child( "angles" ));

        rules.add( "calibration.diffParallacticAngle.duration", ui->spinBox_dpara_duration->value());
        rules.add( "calibration.diffParallacticAngle.distanceScaling", ui->doubleSpinBox_dpara_distanceScaling->value());
        rules.add( "calibration.diffParallacticAngle.sources", ui->comboBox_dpara_sources->currentText().toStdString());
        rules.add( "calibration.diffParallacticAngle.baselines", ui->comboBox_dpara_baseline->currentText().toStdString());
        rules.add( "calibration.diffParallacticAngle.investigationCadence", ui->spinBox_dpara_cadence->value());
        rules.add( "calibration.diffParallacticAngle.intent", ui->comboBox_dpara_intent->currentText().toStdString());
    }
    if ( ui->spinBox_para_nscans->value() > 0){
        rules.add( "calibration.parallacticAngleChange.nscans", ui->spinBox_para_nscans->value());
        rules.add( "calibration.parallacticAngleChange.duration", ui->spinBox_para_duration->value());
        rules.add( "calibration.parallacticAngleChange.distanceScaling", ui->doubleSpinBox_para_distanceScaling->value());
        rules.add( "calibration.parallacticAngleChange.sources", ui->comboBox_para_sources->currentText().toStdString());
        rules.add( "calibration.parallacticAngleChange.stations", ui->comboBox_para_station->currentText().toStdString());
        rules.add( "calibration.parallacticAngleChange.investigationCadence", ui->spinBox_para_cadence->value());
        rules.add( "calibration.parallacticAngleChange.intent", ui->comboBox_para_intent->currentText().toStdString());
    }

    return rules;
}

void CalibratorBlockWidget::fromXML(const boost::property_tree::ptree &ctree)
{

    ui->comboBox_calibrationIntent->setCurrentText(QString::fromStdString(ctree.get("intent","NONE")));
    ui->checkBox_subnetting->setChecked(!ctree.get("subnetting", false));
    ui->checkBox_tryToIncludeAllStations->setChecked(ctree.get("tryToIncludeAllStations", false));
    ui->doubleSpinBox_stationFactor->setValue(ctree.get("tryToIncludeAllStations_factor", 3.0));

    ui->doubleSpinBox_nobs_f->setValue(ctree.get("numberOfObservations_factor", 5.0));
    ui->doubleSpinBox_nobs_o->setValue(ctree.get("numberOfObservations_offset", 0.0));

    ui->doubleSpinBox_asta_f->setValue(ctree.get("averageStations_factor", 100.0));
    ui->doubleSpinBox_asta_o->setValue(ctree.get("averageStations_offset", 1.0));

    ui->doubleSpinBox_abl_f->setValue(ctree.get("averageBaseline_factor", 0.0));
    ui->doubleSpinBox_abl_o->setValue(ctree.get("averageBaseline_offset", 1.0));

    ui->doubleSpinBox_dur_f->setValue(ctree.get("duration_factor", 2.0));
    ui->doubleSpinBox_dur_o->setValue(ctree.get("duration_offset", 1.0));

    auto numberOfBlocks = ctree.get_optional<int>("numberOfBlocks");
    if(numberOfBlocks.is_initialized()){
        ui->spinBox_NCalibrationBlocks->setValue(ctree.get<int>("numberOfBlocks"));
    }
    auto duration = ctree.get_optional<int>("duration");
    if(duration.is_initialized()){
        ui->spinBox_calibrationBlockDuration->setValue(ctree.get<int>("duration"));
    }
    auto scans = ctree.get_optional<int>("scans");
    if(scans.is_initialized()){
        ui->spinBox_calibrationBlockScans->setValue(ctree.get<int>("scans"));
    }


    int i=0;
    for(const auto &any : ctree) {
        if(any.first == "block"){
            ++i;
        }
    }
    if ( i > 0){
        ui->spinBox_NCalibrationBlocks->setValue(i);
    }

    i = 0;
    for(const auto &any : ctree) {
        if(any.first == "block"){
            unsigned int time = any.second.get( "startTime", 0 );
            double t = static_cast<double>(time)/3600.0;
            unsigned int scans = any.second.get( "scans", 2 );
            unsigned int duration = any.second.get( "duration", 300 );
            std::string sourceGroup = any.second.get( "sources", "__all__" );

            qobject_cast<QDoubleSpinBox *>(ui->tableWidget_calibrationBlock->cellWidget(i,0))->setValue(t);
            qobject_cast<QSpinBox *>(ui->tableWidget_calibrationBlock->cellWidget(i,1))->setValue(duration);
            qobject_cast<QSpinBox *>(ui->tableWidget_calibrationBlock->cellWidget(i,2))->setValue(scans);
            qobject_cast<QComboBox *>(ui->tableWidget_calibrationBlock->cellWidget(i,3))->setCurrentText(QString::fromStdString(sourceGroup));
            ++i;
        }
    }

    if ( ctree.get_child_optional("diffParallacticAngle").is_initialized()){
        ui->spinBox_dpara_scans->setValue(ctree.get("diffParallacticAngle.nscans",0));
        int counter = 0;
        auto ptree = ctree.get_child_optional("diffParallacticAngle.angles");
        if ( ptree.is_initialized()){
            for(const auto &any : *ptree) {
                if(any.first == "angle"){
                    double v = any.second.get_value<double>();
                    QDoubleSpinBox *dsp =  qobject_cast<QDoubleSpinBox *>(ui->tableWidget_dParList->cellWidget(counter,0));
                    dsp->setValue(v);
                    ++counter;
                }
            }
        }
        ui->spinBox_dpara_duration->setValue(ctree.get("diffParallacticAngle.duration",300));
        ui->doubleSpinBox_dpara_distanceScaling->setValue(ctree.get("diffParallacticAngle.distanceScaling",2.0));
        ui->comboBox_dpara_sources->setCurrentText(QString::fromStdString(ctree.get("diffParallacticAngle.sources","__all__")));
        ui->comboBox_dpara_baseline->setCurrentText(QString::fromStdString(ctree.get("diffParallacticAngle.baselines","__all__")));
        ui->spinBox_dpara_cadence->setValue(ctree.get("diffParallacticAngle.investigationCadence",300));
        ui->comboBox_dpara_intent->setCurrentText(QString::fromStdString(ctree.get("diffParallacticAngle.intent","CALIBRATE_BANDPASS")));
    }
    if ( ctree.get_child_optional("parallacticAngleChange").is_initialized()){
        ui->spinBox_para_nscans->setValue(ctree.get("parallacticAngleChange.nscans",0));
        ui->spinBox_para_duration->setValue(ctree.get("parallacticAngleChange.duration",300));
        ui->doubleSpinBox_para_distanceScaling->setValue(ctree.get("parallacticAngleChange.distanceScaling",10.0));
        ui->comboBox_para_sources->setCurrentText(QString::fromStdString(ctree.get("parallacticAngleChange.sources","__all__")));
        ui->comboBox_para_station->setCurrentText(QString::fromStdString(ctree.get("parallacticAngleChange.stations","__all__")));
        ui->spinBox_para_cadence->setValue(ctree.get("parallacticAngleChange.investigationCadence",300));
        ui->comboBox_para_intent->setCurrentText(QString::fromStdString(ctree.get("parallacticAngleChange.intent","CALIBRATE_BANDPASS")));
    }
}

void CalibratorBlockWidget::on_spinBox_NCalibrationBlocks_valueChanged(int row)
{
    if (row > 0){
        ui->tabWidget->setTabIcon(0, QIcon(":/icons/icons/dialog-ok-2.png"));
    }else{
        ui->tabWidget->setTabIcon(0, QIcon(":/icons/icons/edit-delete-6.png"));
    }
    double session_dur = session_duration->value();
    double delta = session_dur/(row-1);
    auto *tab = ui->tableWidget_calibrationBlock;
    tab->setRowCount(row);
    for(int i_row = 0; i_row<row; ++i_row){
        QDoubleSpinBox *t = new QDoubleSpinBox();
        t->setMaximum(session_dur);
        t->setSingleStep(.5);
        double v = i_row * delta;
        if(v==0){
            if (session_dur <= 1){
                v=0.25;
            }else{
                v=.5;
            }
        }else if(abs(v-session_dur)<.05){
            if (session_dur <= 1){
                v = session_dur-.25;
            }else{
                v = session_dur-.5;
            }
        }
        t->setValue(v);        t->setSuffix(" [hours]");

        QSpinBox *d = new QSpinBox();
        d->setMinimum(10);
        d->setMaximum(1200);
        d->setSingleStep(10);
        d->setValue(ui->spinBox_calibrationBlockDuration->value());
        d->setSuffix(" [s]");
        connect(ui->spinBox_calibrationBlockDuration,SIGNAL(valueChanged(int)), d,SLOT(setValue(int)));

        QSpinBox *s = new QSpinBox();
        s->setMinimum(1);
        s->setMaximum(10);
        s->setValue(ui->spinBox_calibrationBlockScans->value());
        connect(ui->spinBox_calibrationBlockScans,SIGNAL(valueChanged(int)), s,SLOT(setValue(int)));

        QComboBox *c = new QComboBox();
        c->setModel(source_model_);
        c->setCurrentText(ui->comboBox_calibrationBlockSources->currentText());
        connect(ui->comboBox_calibrationBlockSources,SIGNAL(currentIndexChanged(int)), c,SLOT(setCurrentIndex(int)));

        tab->setCellWidget(i_row,0,t);
        tab->setCellWidget(i_row,1,d);
        tab->setCellWidget(i_row,2,s);
        tab->setCellWidget(i_row,3,c);
    }
}



void CalibratorBlockWidget::on_pushButton_tryToIncludeAllStations_toggled(bool checked)
{
    QString txt;
    if (checked){
        txt = "max scans";
    }else{
        txt = "scans";
    }
    ui->tableWidget_calibrationBlock->setHorizontalHeaderItem(2, new QTableWidgetItem(txt));
}

void CalibratorBlockWidget::update()
{
    on_spinBox_NCalibrationBlocks_valueChanged(ui->spinBox_NCalibrationBlocks->value());
}

void CalibratorBlockWidget::on_pushButton_save_advanced_clicked()
{
    QStringList path;
    QStringList value;

    path << "settings.rules.calibration.numberOfObservations_factor"
         << "settings.rules.calibration.numberOfObservations_offset"
         << "settings.rules.calibration.averageStations_factor"
         << "settings.rules.calibration.averageStations_offset"
         << "settings.rules.calibration.duration_factor"
         << "settings.rules.calibration.duration_offset"
         << "settings.rules.calibration.averageBaseline_factor"
         << "settings.rules.calibration.averageBaseline_offset";

    value << QString::number(ui->doubleSpinBox_nobs_f->value())
          << QString::number(ui->doubleSpinBox_nobs_o->value())
          << QString::number(ui->doubleSpinBox_asta_f->value())
          << QString::number(ui->doubleSpinBox_asta_o->value())
          << QString::number(ui->doubleSpinBox_dur_f->value())
          << QString::number(ui->doubleSpinBox_dur_o->value())
          << QString::number(ui->doubleSpinBox_abl_f->value())
          << QString::number(ui->doubleSpinBox_abl_o->value());

    QString name = "advanced settings";
    emit update_settings(path, value, name);
}


void CalibratorBlockWidget::on_pushButton_save_general_clicked()
{
    QStringList path;
    QStringList value;

    path << "settings.rules.calibration.intent"
         << "settings.rules.calibration.subnetting"
         << "settings.rules.calibration.tryToIncludeAllStations"
         << "settings.rules.calibration.tryToIncludeAllStations_factor";

    value << ui->comboBox_calibrationIntent->currentText();
    ui->checkBox_subnetting->isChecked() ? value << "false" : value << "true"; // WARNING: REVERSED!
    ui->checkBox_tryToIncludeAllStations->isChecked() ? value << "true" : value << "false";
    value << QString::number(ui->doubleSpinBox_stationFactor->value());


    QString name = "general calibration settings changed!";
    emit update_settings(path, value, name);
}


void CalibratorBlockWidget::on_pushButton_save_blocks_clicked()
{
    QStringList path;
    QStringList value;

    path << "settings.rules.calibration.numberOfBlocks"
         << "settings.rules.calibration.duration"
         << "settings.rules.calibration.scans";

    value << QString::number(ui->spinBox_NCalibrationBlocks->value())
          << QString::number(ui->spinBox_calibrationBlockDuration->value())
          << QString::number(ui->spinBox_calibrationBlockScans->value());

    QString name = "general calibration settings changed";
    emit update_settings(path, value, name);
}



void CalibratorBlockWidget::on_pushButton_save_dpara_clicked()
{
    QStringList path;
    QStringList value;

    path << "settings.rules.calibration.diffParallacticAngle.nscans"
         << "settings.rules.calibration.diffParallacticAngle.duration"
         << "settings.rules.calibration.diffParallacticAngle.distanceScaling"
         << "settings.rules.calibration.diffParallacticAngle.sources"
         << "settings.rules.calibration.diffParallacticAngle.baselines"
         << "settings.rules.calibration.diffParallacticAngle.investigationCadence"
         << "settings.rules.calibration.diffParallacticAngle.intent"
            ;

    value << 0 //QString::number(ui->spinBox_dpara_scans->value())
          << QString::number(ui->spinBox_dpara_duration->value())
          << QString::number(ui->doubleSpinBox_dpara_distanceScaling->value())
          <<  ui->comboBox_dpara_sources->currentText()
          <<  ui->comboBox_dpara_baseline->currentText()
          <<  QString::number(ui->spinBox_dpara_cadence->value())
          <<  ui->comboBox_dpara_intent->currentText();

    QString name = "differential parallactic angle calibration settings changed";
    emit update_settings(path, value, name);
}


void CalibratorBlockWidget::on_pushButton_save_para_clicked()
{
    QStringList path;
    QStringList value;

    path << "settings.rules.calibration.parallacticAngleChange.nscans"
         << "settings.rules.calibration.parallacticAngleChange.duration"
         << "settings.rules.calibration.parallacticAngleChange.distanceScaling"
         << "settings.rules.calibration.parallacticAngleChange.sources"
         << "settings.rules.calibration.parallacticAngleChange.stations"
         << "settings.rules.calibration.parallacticAngleChange.investigationCadence"
         << "settings.rules.calibration.parallacticAngleChange.intent"
            ;

    value << 0 // QString::number(ui->spinBox_para_nscans->value())
          << QString::number(ui->spinBox_para_duration->value())
          << QString::number(ui->doubleSpinBox_para_distanceScaling->value())
          <<  ui->comboBox_para_sources->currentText()
          <<  ui->comboBox_para_station->currentText()
          <<  QString::number(ui->spinBox_para_cadence->value())
          <<  ui->comboBox_para_intent->currentText();

    QString name = "parallactic angle calibration settings changed";
    emit update_settings(path, value, name);
}


void CalibratorBlockWidget::on_checkBox_tryToIncludeAllStations_toggled(bool checked)
{
    QString txt;
    if ( checked ){
        txt = "max scans";
    }else{
        txt = "scans";
    }

    ui->tableWidget_calibrationBlock->horizontalHeaderItem(2)->setText(txt);
    ui->label_127->setText(txt + " per block");
}



void CalibratorBlockWidget::on_spinBox_dpara_scans_valueChanged(int arg1)
{
    if (arg1 > 0){
        ui->tabWidget->setTabIcon(1, QIcon(":/icons/icons/dialog-ok-2.png"));
    }else{
        ui->tabWidget->setTabIcon(1, QIcon(":/icons/icons/edit-delete-6.png"));
    }

    auto *tab = ui->tableWidget_dParList;
    tab->setRowCount(arg1);
    int left = (arg1-1)/2;
    int right = arg1 - left - 1;
    for(int i_row = 0; i_row<arg1; ++i_row){
        QDoubleSpinBox *t = new QDoubleSpinBox();
        t->setMaximum(90);
        t->setDecimals(2);
        t->setSingleStep(5);
        if ( i_row < left){
            double delta = 45.0/(left+1);
            t->setValue((i_row+1) * delta);
        } else if (i_row == left){
            t->setValue(45);
        } else if (i_row > left){
            double delta = 45.0/(right+1);
            t->setValue(45+(i_row-left) * delta);
        }

        t->setSuffix(" [deg]");
        tab->setCellWidget(i_row,0,t);
    }
}


void CalibratorBlockWidget::on_spinBox_para_nscans_valueChanged(int arg1)
{
    if (arg1 > 0){
        ui->tabWidget->setTabIcon(2, QIcon(":/icons/icons/dialog-ok-2.png"));
    }else{
        ui->tabWidget->setTabIcon(2, QIcon(":/icons/icons/edit-delete-6.png"));
    }

}

