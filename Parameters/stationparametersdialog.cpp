/* 
 *  VieSched++ Very Long Baseline Interferometry (VLBI) Scheduling Software
 *  Copyright (C) 2018  Matthias Schartner
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stationparametersdialog.h"
#include "ui_stationparametersdialog.h"

stationParametersDialog::stationParametersDialog(boost::property_tree::ptree &settings_, QWidget *parent) :
    QDialog(parent), settings{settings_},
    ui(new Ui::stationParametersDialog)
{
    ui->setupUi(this);
    QApplication::setWindowIcon(QIcon(":/icons/icons/VieSchedppGUI_logo.png"));
    this->setWindowTitle("VieSched++");
    sources = new QStandardItemModel(this);
    sources_proxy = new QSortFilterProxyModel(this);
    sources_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    sources_proxy->setSourceModel(sources);
    ui->listView_ignoreSources->setModel(sources_proxy);
    new QShortcut(QKeySequence(Qt::Key_F1), this, SLOT(on_pushButton_clicked()));
}

stationParametersDialog::~stationParametersDialog()
{
    delete ui;
}

void stationParametersDialog::addBandNames(QStringList bands)
{
    ui->tableWidget_SNR->clear();
    ui->tableWidget_SNR->setRowCount(0);
    ui->tableWidget_SNR->setColumnCount(1);
    ui->tableWidget_SNR->setHorizontalHeaderItem(0,new QTableWidgetItem("min SNR"));

    for(int i = 0; i<bands.length(); ++i){
        int nextrow = ui->tableWidget_SNR->rowCount();
        ui->tableWidget_SNR->insertRow(nextrow);
        QString text = bands.at(i);
        ui->tableWidget_SNR->setVerticalHeaderItem(nextrow,new QTableWidgetItem(text));
        QDoubleSpinBox *spin = new QDoubleSpinBox(this);
        spin->setSuffix(" [Jy]");
        spin->setMaximum(10000.0);
        ui->tableWidget_SNR->setCellWidget(nextrow,0,spin);
    }
}

void stationParametersDialog::addDefaultParameters(VieVS::ParameterSettings::ParametersStations d)
{
    dp = d;
    if(d.weight.is_initialized()){
        ui->doubleSpinBox_weight->setValue(*d.weight);
    }
    if(d.minElevation.is_initialized()){
        ui->doubleSpinBox_minElevation->setValue(*d.minElevation);
    }
    if(d.minScan.is_initialized()){
        ui->spinBox_minScanTime->setValue(*d.minScan);
    }
    if(d.maxScan.is_initialized()){
        ui->spinBox_maxScanTime->setValue(*d.maxScan);
    }
    if(d.maxWait.is_initialized()){
        ui->spinBox_maxWaitTime->setValue(*d.maxWait);
    }
    if(d.minSlewtime.is_initialized()){
        ui->spinBox_minSlewTime->setValue(*d.minSlewtime);
    }
    if(d.maxSlewtime.is_initialized()){
        ui->spinBox_maxSlewTime->setValue(*d.maxSlewtime);
    }
    if(d.minSlewDistance.is_initialized()){
        ui->doubleSpinBox_minSlewDistance->setValue(*d.minSlewDistance);
    }
    if(d.maxSlewDistance.is_initialized()){
        ui->doubleSpinBox_maxSlewDistance->setValue(*d.maxSlewDistance);
    }
    if(d.maxNumberOfScans.is_initialized()){
        ui->spinBox_maxNumberOfScans->setValue(*d.maxNumberOfScans);
    }
    if(d.maxNumberOfScansDist.is_initialized()){
        ui->spinBox_maxNumberOfScansDist->setValue(*d.maxNumberOfScansDist);
    }
    if(d.maxTotalObsTime.is_initialized()){
        ui->spinBox_maxTotalObsTime->setValue(*d.maxTotalObsTime);
    }
    if(d.midob.is_initialized()){
        ui->spinBox_midob->setValue(*d.midob);
    }
    if(d.preob.is_initialized()){
        ui->spinBox_preob->setValue(*d.preob);
    }
    if(d.systemDelay.is_initialized()){
        ui->spinBox_systemDelay->setValue(*d.systemDelay);
    }
}

void stationParametersDialog::addSelectedParameters(VieVS::ParameterSettings::ParametersStations para, QString paraName)
{
    changeParameters(para);
    ui->lineEdit->setText(paraName);
    if(paraName == "default"){
        ui->groupBox_available->setCheckable(false);
        ui->groupBox_availableForFillinmode->setCheckable(false);

        ui->groupBox_tagalong->setCheckable(false);
        ui->groupBox_tagalong->setEnabled(false);

        ui->checkBox_weight->setChecked(true);
        connect(ui->checkBox_weight, &QCheckBox::toggled, this, &stationParametersDialog::force_checked);
        ui->checkBox_minElevation->setChecked(true);
        connect(ui->checkBox_minElevation, &QCheckBox::toggled, this, &stationParametersDialog::force_checked);
        ui->checkBox_maxWaitTime->setChecked(true);
        connect(ui->checkBox_maxWaitTime, &QCheckBox::toggled, this, &stationParametersDialog::force_checked);
        ui->checkBox_minSlewTime->setChecked(true);
        connect(ui->checkBox_minSlewTime, &QCheckBox::toggled, this, &stationParametersDialog::force_checked);
        ui->checkBox_maxSlewTime->setChecked(true);
        connect(ui->checkBox_maxSlewTime, &QCheckBox::toggled, this, &stationParametersDialog::force_checked);
        ui->checkBox_maxSlewDistance->setChecked(true);
        connect(ui->checkBox_maxSlewDistance, &QCheckBox::toggled, this, &stationParametersDialog::force_checked);
        ui->checkBox_minSlewDistance->setChecked(true);
        connect(ui->checkBox_minSlewDistance, &QCheckBox::toggled, this, &stationParametersDialog::force_checked);
        ui->checkBox_maxNumberOfScans->setChecked(true);
        connect(ui->checkBox_maxNumberOfScans, &QCheckBox::toggled, this, &stationParametersDialog::force_checked);
        ui->checkBox_maxNumberOfScansDist->setChecked(true);
        connect(ui->checkBox_maxNumberOfScansDist, &QCheckBox::toggled, this, &stationParametersDialog::force_checked);
        // ui->checkBox_dataWriteSpeed->setEnabled(false);
        ui->checkBox_maxTotalObsTime->setChecked(true);
        connect(ui->checkBox_maxTotalObsTime, &QCheckBox::toggled, this, &stationParametersDialog::force_checked);

        ui->groupBox_scanTime->setCheckable(false);
        ui->pushButton_load->setEnabled(false);
        ui->pushButton_save->setEnabled(false);

        ui->checkBox_midob->setChecked(true);
        connect(ui->checkBox_midob, &QCheckBox::toggled, this, &stationParametersDialog::force_checked);
        ui->checkBox_preob->setChecked(true);
        connect(ui->checkBox_preob, &QCheckBox::toggled, this, &stationParametersDialog::force_checked);
        ui->checkBox_systemDelay->setChecked(true);
        connect(ui->checkBox_systemDelay, &QCheckBox::toggled, this, &stationParametersDialog::force_checked);

    }
    ui->lineEdit->setEnabled(false);
}

void stationParametersDialog::changeParameters(VieVS::ParameterSettings::ParametersStations sp)
{

    QString warningTxt;
    if(sp.available.is_initialized()){
        if(*sp.available){
            ui->radioButton_available_yes->setChecked(true);
        }else{
            ui->radioButton_available_no->setChecked(true);
        }
        ui->groupBox_available->setChecked(true);
    }else{
        ui->groupBox_available->setChecked(false);
    }

    if(sp.availableForFillinmode.is_initialized()){
        if(*sp.availableForFillinmode){
            ui->radioButton_availableFillin_yes->setChecked(true);
        }else{
            ui->radioButton_availableFillin_no->setChecked(true);
        }
        ui->groupBox_availableForFillinmode->setChecked(true);
    }else{
        ui->groupBox_availableForFillinmode->setChecked(false);
    }

    if(sp.tagalong.is_initialized()){
        if(*sp.tagalong){
            ui->radioButton_tagalong_yes->setChecked(true);
        }else{
            ui->radioButton_tagalong_no->setChecked(true);
        }
        ui->groupBox_tagalong->setChecked(true);
    }else{
        ui->groupBox_tagalong->setChecked(false);
    }

    if(sp.minSlewtime.is_initialized()){
        ui->spinBox_minSlewTime->setValue(*sp.minSlewtime);
        ui->checkBox_minSlewTime->setChecked(true);
    }else{
        ui->spinBox_minSlewTime->setValue(*dp.minSlewtime);
        ui->checkBox_minSlewTime->setChecked(false);
    }
    if(sp.maxSlewtime.is_initialized()){
        ui->spinBox_maxSlewTime->setValue(*sp.maxSlewtime);
        ui->checkBox_maxSlewTime->setChecked(true);
    }else{
        ui->spinBox_maxSlewTime->setValue(*dp.maxSlewtime);
        ui->checkBox_maxSlewTime->setChecked(false);
    }
    if(sp.maxSlewDistance.is_initialized()){
        ui->doubleSpinBox_maxSlewDistance->setValue(*sp.maxSlewDistance);
        ui->checkBox_maxSlewDistance->setChecked(true);
    }else{
        ui->doubleSpinBox_maxSlewDistance->setValue(*dp.maxSlewDistance);
        ui->checkBox_maxSlewDistance->setChecked(false);
    }
    if(sp.minSlewDistance.is_initialized()){
        ui->doubleSpinBox_minSlewDistance->setValue(*sp.minSlewDistance);
        ui->checkBox_minSlewDistance->setChecked(true);
    }else{
        ui->doubleSpinBox_minSlewDistance->setValue(*dp.minSlewDistance);
        ui->checkBox_minSlewDistance->setChecked(false);
    }

    if(sp.maxWait.is_initialized()){
        ui->spinBox_maxWaitTime->setValue(*sp.maxWait);
        ui->checkBox_maxWaitTime->setChecked(true);
    }else{
        ui->spinBox_maxWaitTime->setValue(*dp.maxWait);
        ui->checkBox_maxWaitTime->setChecked(false);
    }

    if(sp.maxNumberOfScans.is_initialized()){
        ui->spinBox_maxNumberOfScans->setValue(*sp.maxNumberOfScans);
        ui->checkBox_maxNumberOfScans->setChecked(true);
    }else{
        ui->spinBox_maxNumberOfScans->setValue(*dp.maxNumberOfScans);
        ui->checkBox_maxNumberOfScans->setChecked(false);
    }
    if(sp.maxNumberOfScansDist.is_initialized()){
        ui->spinBox_maxNumberOfScansDist->setValue(*sp.maxNumberOfScansDist);
        ui->checkBox_maxNumberOfScansDist->setChecked(true);
    }else{
        ui->spinBox_maxNumberOfScansDist->setValue(*dp.maxNumberOfScansDist);
        ui->checkBox_maxNumberOfScansDist->setChecked(false);
    }
    if(sp.maxTotalObsTime.is_initialized()){
        ui->spinBox_maxTotalObsTime->setValue(*sp.maxTotalObsTime);
        ui->checkBox_maxTotalObsTime->setChecked(true);
    }else{
        ui->spinBox_maxTotalObsTime->setValue(*dp.maxTotalObsTime);
        ui->checkBox_maxTotalObsTime->setChecked(false);
    }


    if(sp.minElevation.is_initialized()){
        ui->doubleSpinBox_minElevation->setValue(*sp.minElevation);
        ui->checkBox_minElevation->setChecked(true);
    }else{
        ui->doubleSpinBox_minElevation->setValue(*dp.minElevation);
        ui->checkBox_minElevation->setChecked(false);
    }

    if(sp.weight.is_initialized()){
        ui->doubleSpinBox_weight->setValue(*sp.weight);
        ui->checkBox_weight->setChecked(true);
    }else{
        ui->doubleSpinBox_weight->setValue(*dp.weight);
        ui->checkBox_weight->setChecked(false);
    }

    ui->groupBox_scanTime->setChecked(false);
    if(sp.maxScan.is_initialized()){
        ui->spinBox_maxScanTime->setValue(*sp.maxScan);
        ui->groupBox_scanTime->setChecked(true);
    }else{
        ui->spinBox_maxScanTime->setValue(*dp.maxScan);
    }

    if(sp.minScan.is_initialized()){
        ui->spinBox_minScanTime->setValue(*sp.minScan);
        ui->groupBox_scanTime->setChecked(true);
    }else{
        ui->spinBox_minScanTime->setValue(*dp.minScan);
    }

    if(sp.dataWriteRate.is_initialized()){
        ui->doubleSpinBox_dataWriteSpeed->setValue(*sp.dataWriteRate);
        ui->checkBox_dataWriteSpeed->setChecked(true);
    }else{
        ui->doubleSpinBox_dataWriteSpeed->setValue(4096.0);
    }

    QVector<QString> bands;
    for(int i=0; i<ui->tableWidget_SNR->rowCount(); ++i){
        qobject_cast<QDoubleSpinBox*>(ui->tableWidget_SNR->cellWidget(i,0))->setValue(0);
        bands.push_back(ui->tableWidget_SNR->verticalHeaderItem(i)->text());
    }
    for(const auto &any:sp.minSNR){
        QString name = QString::fromStdString(any.first);
        double val = any.second;
        int idx = bands.indexOf(name);
        if(idx != -1){
            qobject_cast<QDoubleSpinBox*>(ui->tableWidget_SNR->cellWidget(idx,0))->setValue(val);
            ui->groupBox_scanTime->setChecked(true);
        }else{
            warningTxt.append("    unknown band name: ").append(name).append(" for minimum SNR!\n");
        }
    }

    ui->listWidget_selectedIgnoreSources->clear();
    ui->groupBox_ignoreSources->setChecked(false);
    for(const auto &any:sp.ignoreSourcesString){
        QString name = QString::fromStdString(any);
        auto list = sources->findItems(name);
        if(list.size()==1){
            ui->listWidget_selectedIgnoreSources->insertItem(ui->listWidget_selectedIgnoreSources->count(),name);
            ui->groupBox_ignoreSources->setChecked(true);
        }else{
            warningTxt.append("    unknown source: ").append(name).append(" which should be ignored!\n");
        }
    }
    ui->listWidget_selectedIgnoreSources->sortItems();

    if(!warningTxt.isEmpty()){
        QString txt = "The following errors occurred while loading the parameters:\n";
        txt.append(warningTxt).append("These parameters were ignored!\nPlease double check parameters again!");
        QMessageBox::warning(this,"Unknown parameters!",txt);
    }


    if(sp.preob.is_initialized()){
        ui->spinBox_preob->setValue(*sp.preob);
        ui->checkBox_preob->setChecked(true);
    }else{
        ui->spinBox_preob->setValue(*dp.preob);
        ui->checkBox_preob->setChecked(false);
    }
    if(sp.midob.is_initialized()){
        ui->spinBox_midob->setValue(*sp.midob);
        ui->checkBox_midob->setChecked(true);
    }else{
        ui->spinBox_midob->setValue(*dp.midob);
        ui->checkBox_midob->setChecked(false);
    }
    if(sp.systemDelay.is_initialized()){
        ui->spinBox_systemDelay->setValue(*sp.systemDelay);
        ui->checkBox_systemDelay->setChecked(true);
    }else{
        ui->spinBox_systemDelay->setValue(*dp.systemDelay);
        ui->checkBox_systemDelay->setChecked(false);
    }


}

void stationParametersDialog::addSourceNames(QStandardItemModel *otherSources)
{
    for(int i=0; i<otherSources->rowCount();++i){
        sources->appendRow(otherSources->item(i)->clone());
    }
}



void stationParametersDialog::on_listView_ignoreSources_clicked(const QModelIndex &index)
{
    QString itm = index.data().toString();

    bool found = false;
    for(int i = 0; i<ui->listWidget_selectedIgnoreSources->count(); ++i){
        if(itm == ui->listWidget_selectedIgnoreSources->item(i)->text()){
            found = true;
            break;
        }
    }

    if(!found){
        ui->listWidget_selectedIgnoreSources->insertItem(0,itm);
        ui->listWidget_selectedIgnoreSources->sortItems();
    }
}

void stationParametersDialog::on_lineEdit_filter_textChanged(const QString &arg1)
{
    sources_proxy->setFilterRegExp(arg1);
}

void stationParametersDialog::on_listWidget_selectedIgnoreSources_clicked(const QModelIndex &index)
{
    QListWidgetItem *itm = ui->listWidget_selectedIgnoreSources->takeItem(index.row());
    delete(itm);
}

void stationParametersDialog::on_buttonBox_accepted()
{
    if(ui->lineEdit->text().isEmpty()){
        QMessageBox mb;
        mb.warning(this,"missing parameter name","Please add a parameter name");
    }else{
        this->accept();
    }
}

std::pair<std::string, VieVS::ParameterSettings::ParametersStations> stationParametersDialog::getParameters()
{
    VieVS::ParameterSettings::ParametersStations para;

    QString txt = ui->lineEdit->text();
    txt = txt.trimmed();
    txt = txt.simplified();
    txt.replace(" ","_");

    std::string name = txt.toStdString();

    if(ui->groupBox_available->isChecked() || !ui->groupBox_available->isCheckable()){
        if(ui->radioButton_available_yes->isChecked()){
            para.available = true;
        }else{
            para.available = false;
        }
    }
    if(ui->groupBox_availableForFillinmode->isChecked() || !ui->groupBox_availableForFillinmode->isCheckable()){
        if(ui->radioButton_availableFillin_yes->isChecked()){
            para.availableForFillinmode = true;
        }else{
            para.availableForFillinmode = false;
        }
    }
    if(ui->groupBox_tagalong->isChecked()){
        if(ui->radioButton_tagalong_yes->isChecked()){
            para.tagalong = true;
        }else{
            para.tagalong = false;
        }

    }

    if(ui->spinBox_minSlewTime->isEnabled()){
        para.minSlewtime = ui->spinBox_minSlewTime->value();
    }
    if(ui->spinBox_maxSlewTime->isEnabled()){
        para.maxSlewtime = ui->spinBox_maxSlewTime->value();
    }
    if(ui->doubleSpinBox_maxSlewDistance->isEnabled()){
        para.maxSlewDistance = ui->doubleSpinBox_maxSlewDistance->value();
    }
    if(ui->doubleSpinBox_minSlewDistance->isEnabled()){
        para.minSlewDistance = ui->doubleSpinBox_minSlewDistance->value();
    }
    if(ui->spinBox_maxWaitTime->isEnabled() ){
        para.maxWait = ui->spinBox_maxWaitTime->value();
    }
    if(ui->spinBox_maxNumberOfScans->isEnabled() ){
        para.maxNumberOfScans = ui->spinBox_maxNumberOfScans->value();
    }
    if(ui->spinBox_maxNumberOfScansDist->isEnabled() ){
        para.maxNumberOfScansDist = ui->spinBox_maxNumberOfScansDist->value();
    }
    if(ui->doubleSpinBox_minElevation->isEnabled()){
        para.minElevation = ui->doubleSpinBox_minElevation->value();
    }
    if(ui->doubleSpinBox_weight->isEnabled()){
        para.weight = ui->doubleSpinBox_weight->value();
    }
    if(ui->doubleSpinBox_dataWriteSpeed->isEnabled()){
        para.dataWriteRate = ui->doubleSpinBox_dataWriteSpeed->value();
    }
    if(ui->spinBox_maxTotalObsTime->isEnabled() ){
        para.maxTotalObsTime = ui->spinBox_maxTotalObsTime->value();
    }

    if(ui->groupBox_scanTime->isChecked() || !ui->groupBox_scanTime->isCheckable()){
        para.minScan = ui->spinBox_minScanTime->value();
        para.maxScan = ui->spinBox_maxScanTime->value();
        for(int i = 0; i<ui->tableWidget_SNR->rowCount(); ++i){
            QDoubleSpinBox *w = qobject_cast<QDoubleSpinBox*> (ui->tableWidget_SNR->cellWidget(i,0));
            if(w->value()!=0){
                para.minSNR[ui->tableWidget_SNR->verticalHeaderItem(i)->text().toStdString()] = w->value();
            }
        }
    }

    if(ui->groupBox_ignoreSources->isChecked()){
        for(int i = 0; i<ui->listWidget_selectedIgnoreSources->count(); ++i){
            para.ignoreSourcesString.push_back(ui->listWidget_selectedIgnoreSources->item(i)->text().toStdString());
        }
    }

    if(ui->spinBox_preob->isEnabled()){
        para.preob = ui->spinBox_preob->value();
    }
    if(ui->spinBox_midob->isEnabled()){
        para.midob = ui->spinBox_midob->value();
    }
    if(ui->spinBox_systemDelay->isEnabled()){
        para.systemDelay = ui->spinBox_systemDelay->value();
    }



    return std::make_pair(name,para);
}

void stationParametersDialog::on_pushButton_save_clicked()
{
    if(ui->lineEdit->text().isEmpty()){
        QMessageBox::warning(this,"No parameter Name!","Please add parameter name first!");
    }else{
        if(QMessageBox::Yes == QMessageBox::question(this,"Save?","Are you sure you want to save this settings?\nThis will save the settings to settings.xml file for further use.")){
            std::pair<std::string, VieVS::ParameterSettings::ParametersStations> para = getParameters();

            settings.add_child("settings.station.parameters.parameter",VieVS::ParameterSettings::parameterStation2ptree(para.first,para.second).get_child("parameters"));
            std::ofstream os;
            os.open("settings.xml");
            boost::property_tree::xml_parser::write_xml(os, settings,
                                                        boost::property_tree::xml_writer_make_settings<std::string>('\t', 1));
            os.close();
        }
    }
}

void stationParametersDialog::on_pushButton_load_clicked()
{
    boost::property_tree::ptree parameters = settings.get_child("settings.station.parameters");
    QVector<QString> names;
    QVector<VieVS::ParameterSettings::ParametersStations> paras;

    for(const auto &it:parameters){
        auto tmp = VieVS::ParameterSettings::ptree2parameterStation(it.second);
        std::string name = tmp.first;
        names.push_back(QString::fromStdString(name));
        VieVS::ParameterSettings::ParametersStations para = tmp.second;
        paras.push_back(para);
    }
    settingsLoadWindow *dial = new settingsLoadWindow(this);
    dial->setStationParameters(names,paras);
    int result = dial->exec();
    if(result == QDialog::Accepted){

        QString itm = dial->selectedItem();
        int idx = dial->selectedIdx();
        VieVS::ParameterSettings::ParametersStations sp = paras.at(idx);

        changeParameters(sp);

        ui->lineEdit->setText(itm);
    }
}

void stationParametersDialog::on_pushButton_clicked()
{
    QWhatsThis::enterWhatsThisMode();
}

void stationParametersDialog::force_checked()
{
    auto *cb = qobject_cast<QCheckBox *>( sender() );
    cb->setChecked(true);
}
