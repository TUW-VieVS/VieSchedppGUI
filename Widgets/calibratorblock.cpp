#include "calibratorblock.h"
#include "ui_calibratorblock.h"

CalibratorBlock::CalibratorBlock(QStandardItemModel *source_model, QDoubleSpinBox *session_duration, QWidget *parent) :
    QWidget(parent),
    source_model_{source_model},
    session_duration{session_duration},
    ui(new Ui::CalibratorBlock)
{
    ui->setupUi(this);

    newSourceGroup = ui->pushButton_calibration_addSrcGroup;
    default_blocks = ui->pushButton_save_blocks;
    default_general_setup = ui->pushButton_save_general;
    default_advanced_setup = ui->pushButton_save_advanced;

    ui->comboBox_calibrationBlockSources->setModel(source_model);

    ui->tableWidget_calibrationBlock->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    on_spinBox_NCalibrationBlocks_valueChanged(ui->spinBox_NCalibrationBlocks->value());

}

QString CalibratorBlock::reloadSources()
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

CalibratorBlock::~CalibratorBlock()
{
    delete ui;
}

boost::property_tree::ptree CalibratorBlock::toXML()
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

    return rules;
}

void CalibratorBlock::fromXML(const boost::property_tree::ptree &ctree)
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
}

void CalibratorBlock::on_spinBox_NCalibrationBlocks_valueChanged(int row)
{
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
        t->setValue(v);
        t->setSuffix(" [hours]");

        QSpinBox *d = new QSpinBox();
        d->setMinimum(30);
        d->setMaximum(1200);
        d->setSingleStep(30);
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



void CalibratorBlock::on_pushButton_tryToIncludeAllStations_toggled(bool checked)
{
    QString txt;
    if (checked){
        txt = "max scans";
    }else{
        txt = "scans";
    }
    ui->tableWidget_calibrationBlock->setHorizontalHeaderItem(2, new QTableWidgetItem(txt));
}

void CalibratorBlock::update()
{
    on_spinBox_NCalibrationBlocks_valueChanged(ui->spinBox_NCalibrationBlocks->value());
}

void CalibratorBlock::on_pushButton_save_advanced_clicked()
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


void CalibratorBlock::on_pushButton_save_general_clicked()
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


void CalibratorBlock::on_pushButton_save_blocks_clicked()
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


void CalibratorBlock::on_checkBox_tryToIncludeAllStations_toggled(bool checked)
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

