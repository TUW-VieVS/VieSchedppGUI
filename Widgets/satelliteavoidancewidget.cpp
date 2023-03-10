#include "satelliteavoidancewidget.h"
#include "ui_satelliteavoidancewidget.h"

SatelliteAvoidanceWidget::SatelliteAvoidanceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SatelliteAvoidanceWidget)
{
    ui->setupUi(this);
    save_para = ui->pushButton_save;
}

SatelliteAvoidanceWidget::~SatelliteAvoidanceWidget()
{
    delete ui;
}

boost::property_tree::ptree SatelliteAvoidanceWidget::toXML()
{
    double extraMargin = ui->doubleSpinBox_errorMargin->value();
    double orbitError = ui->spinBox_orbitError->value();
    double orbitErrorPerDay = ui->spinBox_orbitErrorPerDay->value();
    double checkFrequency =ui->spinBox_checkFrequency->value();
    double minElevation =ui->doubleSpinBox_minElevation->value();

    boost::property_tree::ptree rules;
    rules.add("satelliteAvoidance.extraMargin", extraMargin);
    rules.add("satelliteAvoidance.orbitError", orbitError);
    rules.add("satelliteAvoidance.orbitErrorPerDay", orbitErrorPerDay);
    rules.add("satelliteAvoidance.checkFrequency", checkFrequency);
    rules.add("satelliteAvoidance.minElevation", minElevation);

    return rules;
}

void SatelliteAvoidanceWidget::fromXML(const boost::property_tree::ptree &tree)
{
    double extraMargin = tree.get( "extraMargin", 0.2 );
    int orbitError = tree.get( "orbitError", 2000 );
    int orbitErrorPerDay = tree.get( "orbitErrorPerDay", 2000 );
    int checkFrequency = tree.get( "checkFrequency", 30 );
    double minElevation = tree.get( "minElevation", 20 );

    ui->doubleSpinBox_errorMargin->setValue(extraMargin);
    ui->spinBox_orbitError->setValue(orbitError);
    ui->spinBox_orbitErrorPerDay->setValue(orbitErrorPerDay);
    ui->spinBox_checkFrequency->setValue(checkFrequency);
    ui->doubleSpinBox_minElevation->setValue(minElevation);
}

void SatelliteAvoidanceWidget::on_pushButton_save_clicked()
{
    QStringList path;
    QStringList value;

    path << "settings.satelliteAvoidance.extraMargin"
         << "settings.satelliteAvoidance.orbitError"
         << "settings.satelliteAvoidance.orbitErrorPerDay"
         << "settings.satelliteAvoidance.checkFrequency"
         << "settings.satelliteAvoidance.minElevation"
            ;

    value << QString::number(ui->doubleSpinBox_errorMargin->value())
          << QString::number(ui->spinBox_orbitError->value())
          << QString::number(ui->spinBox_orbitErrorPerDay->value())
          << QString::number(ui->spinBox_checkFrequency->value())
          << QString::number(ui->doubleSpinBox_minElevation->value())
             ;

    QString name = "satellite avoidance settings";
    emit update_settings(path, value, name);

}
