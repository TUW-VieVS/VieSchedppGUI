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

#include "mainwindow.h"
#include "ui_mainwindow.h"


QString MainWindow::writeXML()
{
    para = VieVS::ParameterSettings();
    para.software(QApplication::applicationName().toStdString(), QApplication::applicationVersion().toStdString());

    std::string experimentName = ui->experimentNameLineEdit->text().toStdString();

    QString path = ui->lineEdit_outputPath->text();
    path = path.simplified();
    path.replace("\\\\","/");
    path.replace("\\","/");
    if(!path.isEmpty() && path.right(1) != "/"){
        path.append("/");
    }
    QString NGSpath;
    bool NGS = false;
    if(ui->checkBox_redirectNGS->isChecked()){
        NGSpath = ui->lineEdit_outputNGS->text().simplified();
        NGSpath.replace("\\\\","/");
        NGSpath.replace("\\","/");
        if(!NGSpath.isEmpty() && NGSpath.right(1) != "/"){
            NGSpath.append("/");
        }
        if(NGSpath != path){
            NGS = true;
        }
    }

    QDir mainDir(path);
    if(!path.isEmpty() && !mainDir.exists() ){
        QDir().mkpath(path);
    }
    if(NGS){
        QDir mainDirNGS(NGSpath);
        if( !mainDirNGS.exists() ){
            QDir().mkpath(NGSpath);
        }
    }

    QString ename = QString::fromStdString(experimentName).trimmed();
    ename.simplified();
    ename.replace(" ","_");
    if(ui->checkBox_outputAddTimestamp->isChecked()){
        QString dateTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
        path.append(dateTime).append("_").append(ename).append("/");
        if(NGS){
            NGSpath.append(dateTime).append("_").append(ename).append("/");
        }
    }else{
        path.append(ename).append("/");
        if(NGS){
            NGSpath.append(ename).append("/");
        }
    }
    QDir mydir(path);
    if(! mydir.exists() ){
        QDir().mkpath(path);
    }
    if(NGS){
        QDir mydir(NGSpath);
        if(! mydir.exists() ){
            QDir().mkpath(NGSpath);
        }
    }

    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    std::string name = ui->nameLineEdit->text().toStdString();
    if(name.empty()){
        name = "unknown";
    }
    std::string email = ui->emailLineEdit->text().toStdString();
    if(email.empty()){
        email = "unknown";
    }
    para.created(now, name, email);

    int startYear = ui->dateTimeEdit_sessionStart->date().year();
    int startMonth = ui->dateTimeEdit_sessionStart->date().month();
    int startDay = ui->dateTimeEdit_sessionStart->date().day();
    int startHour = ui->dateTimeEdit_sessionStart->time().hour();
    int startMinute = ui->dateTimeEdit_sessionStart->time().minute();
    int startSecond = ui->dateTimeEdit_sessionStart->time().second();
    boost::posix_time::ptime start(boost::gregorian::date(startYear, startMonth, startDay), boost::posix_time::time_duration(startHour, startMinute, startSecond));
    int duration = ui->doubleSpinBox_sessionDuration->value()*3600;
    boost::posix_time::ptime end = start + boost::posix_time::seconds(duration);
    std::vector<std::string> station_names;
    for(int i=0; i<selectedStationModel->rowCount(); ++i){
        station_names.push_back(selectedStationModel->item(i)->text().toStdString());
    }

    std::vector<std::string> srcNames;
    bool useSourcesFromParameter_otherwiseIgnore;
    for(int i=0; i<selectedSourceModel->rowCount(); ++i){
        srcNames.push_back(selectedSourceModel->item(i)->text().toStdString());
    }

    int selSrc = selectedSourceModel->rowCount();
    int allSrc = allSourceModel->rowCount();
    std::vector<std::string> ignoreSrcNames;
    if(allSrc-selSrc < allSrc/2){
        useSourcesFromParameter_otherwiseIgnore = false;
        for(int i=0; i<allSourceModel->rowCount(); ++i){
            std::string thisSrc = allSourceModel->item(i)->text().toStdString();
            if(std::find(srcNames.begin(),srcNames.end(),thisSrc) == srcNames.end() ){
                ignoreSrcNames.push_back(thisSrc);
            }
        }
    }else{
        useSourcesFromParameter_otherwiseIgnore = true;
    }
    bool fillinModeAPosteriori = ui->checkBox_fillinmode_aposteriori->isChecked();
    bool fillinModeDuringScan = ui->checkBox_fillinmode_duringscan->isChecked();
    bool fillinModeInfluence = ui->checkBox_fillinModeInfluence->isChecked();
    bool idleToObservingTime = ui->radioButton_idleToObservingTime_yes->isChecked();
    bool subnetting = ui->groupBox_subnetting->isChecked();
    double subnettingAngle = ui->doubleSpinBox_subnettingDistance->value();

    double subnettingMinimumPercent = ui->doubleSpinBox_subnettingMinStations->value();
    double subnettingAllBut = ui->spinBox_subnetting_min_sta->value();

    bool useSubnettingPercent_otherwiseAllBut = false;
    double subnettingNumber = subnettingAllBut;

    if(ui->radioButton_subnetting_percent->isChecked()){
        useSubnettingPercent_otherwiseAllBut = true;
        subnettingNumber = subnettingMinimumPercent;
    }
    std::string logConsole = ui->comboBox_log_console->currentText().toStdString();
    std::string logFile = ui->comboBox_log_file->currentText().toStdString();
    std::string scanAlignment = "start";
    if(ui->radioButton_alignEnd->isChecked()){
        scanAlignment = "end";
    }else if(ui->radioButton_alignIndividual->isChecked()){
        scanAlignment = "individual";
    }

    if(useSourcesFromParameter_otherwiseIgnore){
        para.general(experimentName, start, end, subnetting, subnettingAngle, useSubnettingPercent_otherwiseAllBut, subnettingNumber,
                     fillinModeInfluence, fillinModeDuringScan, fillinModeAPosteriori,
                     idleToObservingTime, station_names, useSourcesFromParameter_otherwiseIgnore,
                     srcNames, scanAlignment, logConsole, logFile);
    }else{
        para.general(experimentName, start, end, subnetting, subnettingAngle, useSubnettingPercent_otherwiseAllBut, subnettingNumber,
                     fillinModeInfluence, fillinModeDuringScan, fillinModeAPosteriori,
                     idleToObservingTime, station_names, useSourcesFromParameter_otherwiseIgnore,
                     ignoreSrcNames, scanAlignment, logConsole, logFile);
    }


    std::string experimentDescription = ui->lineEdit_experimentDescription->text().toStdString();
    std::string scheduler = ui->schedulerLineEdit->text().toStdString();
    std::string correlator = ui->correlatorLineEdit->text().toStdString();

    std::vector<VieVS::ParameterSettings::Contact> contacts;
    QTableWidget *tableWidgetContacts = ui->tableWidget_contact;
    for (int i=0; i<tableWidgetContacts->rowCount(); ++i){
        VieVS::ParameterSettings::Contact c;

        c.function = tableWidgetContacts->item(i,0)->text().toStdString();
        c.name = tableWidgetContacts->item(i,1)->text().toStdString();
        c.email = tableWidgetContacts->item(i,2)->text().toStdString();
        c.phone = tableWidgetContacts->item(i,3)->text().toStdString();
        c.affiliation = tableWidgetContacts->item(i,4)->text().toStdString();

        contacts.push_back(c);
    }


    std::string notes = ui->plainTextEdit_notes->toPlainText().replace("\n","\\n").toStdString();
    bool initializer = ui->checkBox_outputInitializer->isChecked();
    bool iteration = ui->checkBox_outputIteration->isChecked();
    bool statistics = ui->checkBox_outputStatisticsFile->isChecked();
    bool vex = ui->checkBox_outputVex->isChecked();
    bool snrTabel = ui->checkBox_outputSnrTable->isChecked();
    bool ngs = ui->checkBox_outputNGSFile->isChecked();
    std::string NGS_directory = "";
    if(ui->checkBox_redirectNGS->isChecked()){
        NGS_directory = NGSpath.toStdString();
    }

    bool skd = ui->checkBox_outputSkdFile->isChecked();
    bool srcGrp = ui->checkBox_outputSourceGroupStatFile->isChecked();
    bool skyCov = ui->checkBox_outputSkyCoverageFile->isChecked();
    bool operNotes = ui->checkBox_outputOperationsNotes->isChecked();
    std::vector<std::string> srcGroupsForStatistic;
    for(int i=0; i<ui->treeWidget_srcGroupForStatistics->topLevelItemCount(); ++i){
        if(ui->treeWidget_srcGroupForStatistics->topLevelItem(i)->checkState(0) == Qt::Checked){
            srcGroupsForStatistic.push_back(ui->treeWidget_srcGroupForStatistics->topLevelItem(i)->text(0).toStdString());
        }
    }
    para.output(experimentDescription, scheduler, correlator, notes, initializer, iteration, statistics, ngs, NGS_directory,
                skd, vex, snrTabel, operNotes, srcGrp, srcGroupsForStatistic, skyCov, contacts);

    std::string antenna = ui->lineEdit_pathAntenna->text().toStdString();
    std::string equip = ui->lineEdit_pathEquip->text().toStdString();
    std::string flux = ui->lineEdit_pathFlux->text().toStdString();
    std::string freq = ui->lineEdit_pathFreq->text().toStdString();
    std::string hdpos = ui->lineEdit_pathHdpos->text().toStdString();
    std::string loif = ui->lineEdit_pathLoif->text().toStdString();
    std::string mask = ui->lineEdit_pathMask->text().toStdString();
    std::string modes = ui->lineEdit_pathModes->text().toStdString();
    std::string position = ui->lineEdit_pathPosition->text().toStdString();
    std::string rec = ui->lineEdit_pathRec->text().toStdString();
    std::string rx = ui->lineEdit_pathRx->text().toStdString();
    std::string source;
    if(ui->radioButton_browseSource->isChecked()){
        source = ui->lineEdit_pathSource->text().toStdString();
    }else{
        source = ui->lineEdit_browseSource2->text().toStdString();
    }

    std::string tracks = ui->lineEdit_pathTracks->text().toStdString();
    para.catalogs(antenna, equip, flux, freq, hdpos, loif, mask, modes, position, rec, rx, source, tracks);


    para.setup(VieVS::ParameterSettings::Type::station,setupStationTree);
    para.setup(VieVS::ParameterSettings::Type::source,setupSourceTree);
    para.setup(VieVS::ParameterSettings::Type::baseline,setupBaselineTree);

    for(const auto&any:paraSta){
        para.parameters(any.first, any.second);
    }
    for(const auto&any:paraSrc){
        para.parameters(any.first, any.second);
    }
    for(const auto&any:paraBl){
        para.parameters(any.first, any.second);
    }
    for(const auto&any:groupSta){
        para.group(VieVS::ParameterSettings::Type::station,VieVS::ParameterGroup(any.first,any.second));
    }
    for(const auto&any:groupSrc){
        para.group(VieVS::ParameterSettings::Type::source,VieVS::ParameterGroup(any.first,any.second));
    }
    for(const auto&any:groupBl){
        para.group(VieVS::ParameterSettings::Type::baseline,VieVS::ParameterGroup(any.first,any.second));
    }


    for(int i=0; i<ui->treeWidget_setupStationWait->topLevelItemCount(); ++i){
        auto itm = ui->treeWidget_setupStationWait->topLevelItem(i);
        std::string name = itm->text(0).toStdString();
        int fieldSystem = QString(itm->text(1).left(itm->text(1).count()-6)).toInt();
        int preob = QString(itm->text(2).left(itm->text(2).count()-6)).toInt();
        int midob = QString(itm->text(3).left(itm->text(3).count()-6)).toInt();
        int postob = QString(itm->text(4).left(itm->text(4).count()-6)).toInt();
        para.stationWaitTimes(name,fieldSystem,preob,midob,postob);
    }

    for(int i=0; i<ui->treeWidget_setupStationAxis->topLevelItemCount(); ++i){
        auto itm = ui->treeWidget_setupStationAxis->topLevelItem(i);
        std::string name = itm->text(0).toStdString();
        double ax1low = QString(itm->text(1).left(itm->text(1).count()-6)).toDouble();
        double ax1up = QString(itm->text(2).left(itm->text(1).count()-6)).toDouble();
        double ax2low = QString(itm->text(3).left(itm->text(1).count()-6)).toDouble();
        double ax2up = QString(itm->text(4).left(itm->text(1).count()-6)).toDouble();
        para.stationCableWrapBuffer(name,ax1low,ax1up,ax2low,ax2up);
    }

    double influenceDistance = ui->influenceDistanceDoubleSpinBox->value();
    double influenceTime = ui->influenceTimeSpinBox->value();
    double maxDistanceTwin = ui->maxDistanceForCombiningAntennasDoubleSpinBox->value();
    std::string interpolationDistance = ui->comboBox_skyCoverageDistanceType->currentText().toStdString();
    std::string interpolationTime = ui->comboBox_skyCoverageTimeType->currentText().toStdString();
    para.skyCoverage(influenceDistance,influenceTime,maxDistanceTwin, interpolationDistance, interpolationTime);

    double weightSkyCoverage = 0;
    if(ui->checkBox_weightCoverage->isChecked()){
        weightSkyCoverage = ui->doubleSpinBox_weightSkyCoverage->value();
    }
    double weightNumberOfObservations = 0;
    if(ui->checkBox_weightNobs->isChecked()){
        weightNumberOfObservations = ui->doubleSpinBox_weightNumberOfObservations->value();
    }
    double weightDuration = 0;
    if(ui->checkBox_weightDuration->isChecked()){
        weightDuration = ui->doubleSpinBox_weightDuration->value();
    }
    double weightAverageSources = 0;
    if(ui->checkBox_weightAverageSources->isChecked()){
        weightAverageSources = ui->doubleSpinBox_weightAverageSources->value();
    }
    double weightAverageStations = 0;
    if(ui->checkBox_weightAverageStations->isChecked()){
        weightAverageStations = ui->doubleSpinBox_weightAverageStations->value();
    }
    double weightAverageBaselines = 0;
    if(ui->checkBox_weightAverageBaselines->isChecked()){
        weightAverageBaselines = ui->doubleSpinBox_weightAverageBaselines->value();
    }
    double weightIdleTime = 0;
    unsigned int intervalIdleTime = 0;
    if(ui->checkBox_weightIdleTime->isChecked()){
        weightIdleTime = ui->doubleSpinBox_weightIdleTime->value();
        intervalIdleTime = ui->spinBox_idleTimeInterval->value();
    }
    double weightDeclination = 0;
    double weightDeclinationSlopeStart = 0;
    double weightDeclinationSlopeEnd = 0;
    if(ui->checkBox_weightLowDeclination->isChecked()){
        weightDeclination = ui->doubleSpinBox_weightLowDec->value();
        weightDeclinationSlopeStart = ui->doubleSpinBox_weightLowDecStart->value();
        weightDeclinationSlopeEnd = ui->doubleSpinBox_weightLowDecEnd->value();
    }
    double weightElevation = 0;
    double weightElevationSlopeStart = 0;
    double weightElevationSlopeEnd = 0;
    if(ui->checkBox_weightLowElevation->isChecked()){
        weightElevation = ui->doubleSpinBox_weightLowEl->value();
        weightElevationSlopeStart = ui->doubleSpinBox_weightLowElStart->value();
        weightElevationSlopeEnd = ui->doubleSpinBox_weightLowElEnd->value();
    }
    para.weightFactor(weightSkyCoverage, weightNumberOfObservations, weightDuration, weightAverageSources,
                      weightAverageStations, weightAverageBaselines, weightIdleTime, intervalIdleTime, weightDeclination,
                      weightDeclinationSlopeStart, weightDeclinationSlopeEnd,
                      weightElevation, weightElevationSlopeStart, weightElevationSlopeEnd);

    if(ui->groupBox_34->isChecked()){
        int cadence = ui->spinBox_intensiveBlockCadence->value();
        para.ruleFocusCorners(cadence);
    }

    if(ui->groupBox_scanSequence->isChecked()){
        int cadence = ui->spinBox_scanSequenceCadence->value();
        std::vector<unsigned int> modulo;
        std::vector<std::string> member;
        for(int i = 0; i<cadence; ++i){
            QWidget * w = ui->tableWidget_scanSequence->cellWidget(i,0);
            QComboBox* cb = qobject_cast<QComboBox*>(w);
            modulo.push_back(i);
            member.push_back(cb->currentText().toStdString());
        }
        para.ruleScanSequence(cadence,modulo,member);
    }

    // TODO: change elevationAngles in NScanSelections and change their names
    if(ui->groupBox_CalibratorBlock->isChecked()){
        std::string member = ui->comboBox_calibratorBlock_calibratorSources->currentText().toStdString();
        double lowStart = ui->doubleSpinBox_calibratorLowElStart->value();
        double lowEnd = ui->doubleSpinBox_calibratorLowElEnd->value();
        double highStart = ui->doubleSpinBox_calibratorHighElStart->value();
        double highEnd = ui->doubleSpinBox_calibratorHighElEnd->value();
        std::vector<std::pair<double, double> > elevationAngles {{lowStart,lowEnd},{highStart,highEnd}};
        int nmaxScans = ui->spinBox_calibrator_maxScanSequence->value();
        int scanTime = -1;
        if(ui->radioButton->isChecked()){
            scanTime = ui->spinBox_calibratorFixedScanLength->value();
        }
        if(ui->radioButton_calibratorTime->isChecked()){
            int cadence = ui->spinBox_calibratorTime->value();
            para.ruleCalibratorBlockTime(cadence,member,elevationAngles,nmaxScans,scanTime);
        }else if(ui->radioButton_calibratorScanSequence->isChecked()){
            int cadence = ui->spinBox_calibratorScanSequence->value();
            para.ruleCalibratorBlockNScanSelections(cadence,member,elevationAngles,nmaxScans,scanTime);
        }
    }

    if(ui->treeWidget_conditions->topLevelItemCount()>0){
        std::vector<std::string> members;
        std::vector<int> minScans;
        std::vector<int> minBaselines;
        for(int i=0; i<ui->treeWidget_conditions->topLevelItemCount(); ++i){
            auto itm = ui->treeWidget_conditions->topLevelItem(i);
            members.push_back(itm->text(1).toStdString());
            minScans.push_back(itm->text(2).toInt());
            minBaselines.push_back(itm->text(3).toInt());
        }
        bool andForCombination;
        if(ui->comboBox_conditions_combinations->currentText() == "and"){
            andForCombination = true;
        }
        int minNumberOfReducedSources = ui->spinBox_minNumberOfReducedSources->value();
        int maxNumberOfIterations = ui->spinBox_maxNumberOfIterations->value();
        int numberOfGentleSourceReductions = ui->spinBox_gentleSourceReduction->value();
        double percentage = ui->doubleSpinBox_gentleSourceReductionPercentage->value();
        para.conditions(members, minScans, minBaselines, andForCombination, maxNumberOfIterations, numberOfGentleSourceReductions, minNumberOfReducedSources, percentage);
    }

    if (ui->groupBox_multiScheduling->isChecked() && ui->treeWidget_multiSchedSelected->topLevelItemCount()>0){

        std::unordered_map<std::string, std::vector<std::string>> gsta;
        std::unordered_map<std::string, std::vector<std::string>> gsrc;
        std::unordered_map<std::string, std::vector<std::string>> gbl;
        for(const auto &any: groupSta){
            gsta[any.first] = any.second;
        }
        for(const auto &any: groupSrc){
            gsrc[any.first] = any.second;
        }
        for(const auto &any: groupBl){
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
                                         "fillinmode during scan selection",
                                         "fillinmode influence on scan selection",
                                         "fillinmode a posteriori"};

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
                                          "sky coverage",
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
                                          "min sun distance"};


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
                }else if(parameter == "fillinmode during scan selection"){
                    ms.addParameters(std::string("general_").append(parameter.replace(' ','_').toStdString()));
                }else if(parameter == "fillinmode influence on scan selection"){
                    ms.addParameters(std::string("general_").append(parameter.replace(' ','_').toStdString()));
                }else if(parameter == "fillinmode a posteriori"){
                    ms.addParameters(std::string("general_").append(parameter.replace(' ','_').toStdString()));

                }else if(parameter == "subnetting min participating stations"){
                    ms.addParameters(std::string("general_").append(parameter.replace(' ','_').toStdString()), vecDouble);
                }else if(parameter == "subnetting min source angle"){
                    ms.addParameters(std::string("general_").append(parameter.replace(' ','_').toStdString()), vecDouble);
                }else if(parameter == "sky coverage"){
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
                    ms.addParameters(std::string("weight_factor_").append(parameter.replace(' ','_').toStdString()), vecDouble);
                }else if(parameter == "influence time"){
                    ms.addParameters(std::string("weight_factor_").append(parameter.replace(' ','_').toStdString()), vecDouble);
                }
            }
        }

        std::string countNr = ui->comboBox_multiSched_maxNumber->currentText().toStdString();
        int n = ui->spinBox_multiSched_maxNumber->value();
        std::string seedStr = ui->comboBox_multiSched_seed->currentText().toStdString();
        int seed = ui->spinBox_multiSched_seed->value();
        para.multisched(ms.createPropertyTree(),countNr,n,seedStr,seed);

        std::string threads = ui->comboBox_nThreads->currentText().toStdString();
        int nThreadsManual = ui->spinBox_nCores->value();
        std::string jobScheduler = ui->comboBox_jobSchedule->currentText().toStdString();
        int chunkSize = ui->spinBox_chunkSize->value();
        para.multiCore(threads,nThreadsManual,jobScheduler,chunkSize);
    }

    if(ui->groupBox_highImpactAzEl->isChecked() && ui->treeWidget_highImpactAzEl->topLevelItemCount()>0){
        QTreeWidget * tree = ui->treeWidget_highImpactAzEl;
        std::vector<std::string> members;
        std::vector<double> azs;
        std::vector<double> els;
        std::vector<double> margins;
        for(int i=0; i<tree->topLevelItemCount(); ++i){
            QTreeWidgetItem *itm = tree->topLevelItem(i);
            members.push_back(itm->text(0).toStdString());
            azs.push_back(itm->text(1).toDouble());
            els.push_back(itm->text(2).toDouble());
            margins.push_back(itm->text(3).toDouble());
        }
        int interval = ui->spinBox_highImpactInterval->value();
        int repeat = ui->spinBox_highImpactMinRepeat->value();

        para.highImpactAzEl(members,azs,els,margins,interval,repeat);

    }

    if(ui->groupBox_modeSked->isChecked()){
        std::string skdMode = ui->comboBox_skedObsModes->currentText().toStdString();
        para.mode(skdMode);
    }else if(ui->groupBox_modeCustom->isChecked()){
        double sampleRate = ui->sampleRateDoubleSpinBox->value();
        int bits = ui->sampleBitsSpinBox->value();
        para.mode(sampleRate, bits);

        for(int i = 0; i<ui->tableWidget_modeCustonBand->rowCount(); ++i){
            std::string name = ui->tableWidget_modeCustonBand->verticalHeaderItem(i)->text().toStdString();
            double wavelength = 1/(qobject_cast<QDoubleSpinBox*>(ui->tableWidget_modeCustonBand->cellWidget(i,0))->value()*1e9)*299792458.;
            int chanels = qobject_cast<QSpinBox*>(ui->tableWidget_modeCustonBand->cellWidget(i,1))->value();
            para.mode_band(name,wavelength,chanels);
        }
    }else if(ui->groupBox_modeAdvanced->isChecked()){
        if(advancedObservingMode_.is_initialized()){
            para.mode(advancedObservingMode_->toPropertytree());
        }else{
            QMessageBox::warning(this,"No observing mode!","You did not create a custom observing mode!");
        }
    }
    for(int i = 0; i<ui->tableWidget_ModesPolicy->rowCount(); ++i){
        std::string name = ui->tableWidget_ModesPolicy->verticalHeaderItem(i)->text().toStdString();

        double minSNR = qobject_cast<QDoubleSpinBox*>(ui->tableWidget_ModesPolicy->cellWidget(i,0))->value();

        VieVS::ParameterSettings::ObservationModeProperty policySta;
        QString polSta = qobject_cast<QComboBox*>(ui->tableWidget_ModesPolicy->cellWidget(i,1))->currentText();
        if(polSta == "required"){
            policySta = VieVS::ParameterSettings::ObservationModeProperty::required;
        }else if(polSta == "optional"){
            policySta = VieVS::ParameterSettings::ObservationModeProperty::optional;
        }

        VieVS::ParameterSettings::ObservationModeProperty policySrc;
        QString polSrc = qobject_cast<QComboBox*>(ui->tableWidget_ModesPolicy->cellWidget(i,4))->currentText();
        if(polSrc == "required"){
            policySrc = VieVS::ParameterSettings::ObservationModeProperty::required;
        }else if(polSrc == "optional"){
            policySrc = VieVS::ParameterSettings::ObservationModeProperty::optional;
        }

        VieVS::ParameterSettings::ObservationModeBackup backupSta;
        QString bacSta = qobject_cast<QComboBox*>(ui->tableWidget_ModesPolicy->cellWidget(i,2))->currentText();
        if(bacSta == "none"){
            backupSta = VieVS::ParameterSettings::ObservationModeBackup::none;
        }else if(bacSta == "value"){
            backupSta = VieVS::ParameterSettings::ObservationModeBackup::value;
        }else if(bacSta == "min value Times"){
            backupSta = VieVS::ParameterSettings::ObservationModeBackup::minValueTimes;
        }else if(bacSta == "max value Times"){
            backupSta = VieVS::ParameterSettings::ObservationModeBackup::maxValueTimes;
        }

        VieVS::ParameterSettings::ObservationModeBackup backupSrc;
        QString bacSrc = qobject_cast<QComboBox*>(ui->tableWidget_ModesPolicy->cellWidget(i,5))->currentText();
        if(bacSrc == "none"){
            backupSrc = VieVS::ParameterSettings::ObservationModeBackup::none;
        }else if(bacSrc == "value"){
            backupSrc = VieVS::ParameterSettings::ObservationModeBackup::value;
        }else if(bacSrc == "min value Times"){
            backupSrc = VieVS::ParameterSettings::ObservationModeBackup::minValueTimes;
        }else if(bacSrc == "max value Times"){
            backupSrc = VieVS::ParameterSettings::ObservationModeBackup::maxValueTimes;
        }

        double valSta = qobject_cast<QDoubleSpinBox*>(ui->tableWidget_ModesPolicy->cellWidget(i,3))->value();
        double valSrc = qobject_cast<QDoubleSpinBox*>(ui->tableWidget_ModesPolicy->cellWidget(i,6))->value();

        para.mode_bandPolicy(name,minSNR,policySta,backupSta,valSta,policySrc,backupSrc,valSrc);
    }




    path.append("VieSchedpp.xml");
    para.write(path.toStdString());
    QMessageBox mb;
    QMessageBox::StandardButton reply = mb.information(this,"parameter file created",QString("A new parameter file was created and saved at: \n").append(path),QMessageBox::Open,QMessageBox::Ok);
    if(reply == QMessageBox::Open){
        QDesktopServices::openUrl(QUrl(mydir.absolutePath()));
    }
    return path;
}

void MainWindow::loadXML(QString path)
{
    std::ifstream fid(path.toStdString());
    boost::property_tree::ptree xml;
    boost::property_tree::read_xml(fid,xml,boost::property_tree::xml_parser::trim_whitespace);
    QString warning;
    clearSetup(true,true,true);

    // read catalogs
    {
        std::string antenna = xml.get("VieSchedpp.catalogs.antenna","");
        if(!antenna.empty()){
            ui->lineEdit_pathAntenna->setText(QString::fromStdString(antenna));
        }
        std::string equip = xml.get("VieSchedpp.catalogs.equip","");
        if(!equip.empty()){
            ui->lineEdit_pathEquip->setText(QString::fromStdString(equip));
        }
        std::string position = xml.get("VieSchedpp.catalogs.position","");
        if(!position.empty()){
            ui->lineEdit_pathPosition->setText(QString::fromStdString(position));
        }
        std::string mask = xml.get("VieSchedpp.catalogs.mask","");
        if(!mask.empty()){
            ui->lineEdit_pathMask->setText(QString::fromStdString(mask));
        }
        on_pushButton_stations_clicked();

        std::string source = xml.get("VieSchedpp.catalogs.source","");
        if(!source.empty()){
            ui->lineEdit_pathSource->setText(QString::fromStdString(source));
        }
        ui->radioButton_browseSource->setChecked(true);
        std::string flux = xml.get("VieSchedpp.catalogs.flux","");
        if(!flux.empty()){
            ui->lineEdit_pathFlux->setText(QString::fromStdString(flux));
        }
        on_pushButton_reloadsources_clicked();

        std::string freq = xml.get("VieSchedpp.catalogs.freq","");
        if(!freq.empty()){
            ui->lineEdit_pathFreq->setText(QString::fromStdString(freq));
        }
        std::string hdpos = xml.get("VieSchedpp.catalogs.hdpos","");
        if(!hdpos.empty()){
            ui->lineEdit_pathHdpos->setText(QString::fromStdString(hdpos));
        }
        std::string loif = xml.get("VieSchedpp.catalogs.loif","");
        if(!loif.empty()){
            ui->lineEdit_pathLoif->setText(QString::fromStdString(loif));
        }
        std::string modes = xml.get("VieSchedpp.catalogs.modes","");
        if(!modes.empty()){
            ui->lineEdit_pathModes->setText(QString::fromStdString(modes));
        }
        std::string rec = xml.get("VieSchedpp.catalogs.rec","");
        if(!rec.empty()){
            ui->lineEdit_pathRec->setText(QString::fromStdString(rec));
        }
        std::string rx = xml.get("VieSchedpp.catalogs.rx","");
        if(!rx.empty()){
            ui->lineEdit_pathRx->setText(QString::fromStdString(rx));
        }
        std::string tracks = xml.get("VieSchedpp.catalogs.tracks","");
        if(!tracks.empty()){
            ui->lineEdit_pathTracks->setText(QString::fromStdString(tracks));
        }
        on_pushButton_reloadcatalogs_clicked();
    }


    // general
    {
        ui->experimentNameLineEdit->setText(QString::fromStdString(xml.get("VieSchedpp.general.experimentName","dummy")));

        std::string startTimeStr = xml.get("VieSchedpp.general.startTime","2018.01.01 00:00:00");
        QDateTime startTime = QDateTime::fromString(QString::fromStdString(startTimeStr),"yyyy.MM.dd HH:mm:ss");
        ui->dateTimeEdit_sessionStart->setDateTime(startTime);

        std::string endTimeStr = xml.get("VieSchedpp.general.endTime","2018.01.02 00:00:00");
        QDateTime endTime   = QDateTime::fromString(QString::fromStdString(endTimeStr),"yyyy.MM.dd HH:mm:ss");
        double dur = startTime.secsTo(endTime)/3600.0;
        ui->doubleSpinBox_sessionDuration->setValue(dur);

        double subnettingMinAngle = xml.get("VieSchedpp.general.subnettingMinAngle",150.0);
        ui->doubleSpinBox_subnettingDistance->setValue(subnettingMinAngle);
        bool subnettingFlag = false;
        if(xml.get_optional<double>("VieSchedpp.general.subnettingMinNStaPercent").is_initialized()){
            subnettingFlag = true;
        }
        double subnettingMinNStaPercent = xml.get("VieSchedpp.general.subnettingMinNStaPercent",80);
        ui->doubleSpinBox_subnettingMinStations->setValue(subnettingMinNStaPercent);
        double subnettingMinNStaAllBut = xml.get("VieSchedpp.general.subnettingMinNStaAllBut",1);
        ui->spinBox_subnetting_min_sta->setValue(subnettingMinNStaAllBut);
        if(subnettingFlag){
            ui->radioButton_subnetting_percent->setChecked(true);
        }else{
            ui->radioButton_subnetting_allBut->setChecked(true);
        }
        bool subnetting = xml.get("VieSchedpp.general.subnetting",false);
        ui->groupBox_subnetting->setChecked(subnetting);

        bool fillinmodeAPosteriori = xml.get("VieSchedpp.general.fillinmodeAPosteriori",false);
        ui->checkBox_fillinmode_aposteriori->setChecked(fillinmodeAPosteriori);
        bool fillinmodeDuringScan = xml.get("VieSchedpp.general.fillinmodeDuringScan",false);
        ui->checkBox_fillinmode_duringscan->setChecked(fillinmodeDuringScan);
        bool fillinmodeInfluenceOnSchedule = xml.get("VieSchedpp.general.fillinmodeInfluenceOnSchedule",false);
        ui->checkBox_fillinmode_duringscan->setChecked(fillinmodeInfluenceOnSchedule);

        bool idleToObservingTime = xml.get("VieSchedpp.general.idleToObservingTime",false);
        if(idleToObservingTime){
            ui->radioButton_idleToObservingTime_yes->setChecked(true);
        }else{
            ui->radioButton_idleToObservingTime_no->setChecked(true);
        }


        std::vector<std::string> sel_stations;
        const auto &stations = xml.get_child_optional("VieSchedpp.general.stations");
        if(stations.is_initialized()){
            auto it = stations->begin();
            while (it != stations->end()) {
                auto item = it->second.data();
                sel_stations.push_back(item);
                ++it;
            }
            for(const auto &station : sel_stations){
                bool found = false;
                for(int i=0; i<allStationProxyModel->rowCount(); ++i){
                    if(allStationProxyModel->index(i,0).data().toString() == QString::fromStdString(station)){
                        on_treeView_allAvailabeStations_clicked(allStationProxyModel->index(i,0));
                        found = true;
                        break;
                    }
                }
                if(!found){
                    warning.append("Station "+QString::fromStdString(station)+" not found!");
                }
            }
        }


        std::vector<std::string> sel_sources;
        const auto &ptree_useSources = xml.get_child_optional("VieSchedpp.general.onlyUseListedSources");
        if(ptree_useSources.is_initialized()){
            auto it = ptree_useSources->begin();
            while (it != ptree_useSources->end()) {
                auto item = it->second.data();
                sel_sources.push_back(item);
                ++it;
            }
        }
        for(const auto &source : sel_sources){
            bool found = false;
            for(int i=0; i<allSourceProxyModel->rowCount(); ++i){
                if(allSourceProxyModel->index(i,0).data().toString() == QString::fromStdString(source)){
                    on_treeView_allAvailabeSources_clicked(allSourceProxyModel->index(i,0));
                    found = true;
                    break;
                }
            }
            if(!found){
                warning.append("Source "+QString::fromStdString(source)+" not found!");
            }
        }

        std::vector<std::string> ignore_sources;
        const auto &ptree_ignoreSources = xml.get_child_optional("VieSchedpp.general.ignoreListedSources");
        if(ptree_ignoreSources.is_initialized()){
            auto it = ptree_ignoreSources->begin();
            while (it != ptree_ignoreSources->end()) {
                auto item = it->second.data();
                ignore_sources.push_back(item);
                ++it;
            }
        }

        std::string scanAlignment = xml.get("VieSchedpp.general.scanAlignment","start");
        if(scanAlignment == "start"){
            ui->radioButton_alignStart->setChecked(true);
        }else if(scanAlignment == "end"){
            ui->radioButton_alignEnd->setChecked(true);
        }else if(scanAlignment == "individual"){
            ui->radioButton_alignIndividual->setChecked(true);
        }

        std::string logSeverityConsole = xml.get("VieSchedpp.general.logSeverityConsole","info");
        if(logSeverityConsole == "trace"){
            ui->comboBox_log_console->setCurrentIndex(0);
        }else if(logSeverityConsole == "debug"){
            ui->comboBox_log_console->setCurrentIndex(1);
        }else if(logSeverityConsole == "info"){
            ui->comboBox_log_console->setCurrentIndex(2);
        }else if(logSeverityConsole == "warning"){
            ui->comboBox_log_console->setCurrentIndex(3);
        }else if(logSeverityConsole == "error"){
            ui->comboBox_log_console->setCurrentIndex(4);
        }else if(logSeverityConsole == "fatal"){
            ui->comboBox_log_console->setCurrentIndex(5);
        }

        std::string logSeverityFile = xml.get("VieSchedpp.general.logSeverityFile","info");
        if(logSeverityFile == "trace"){
            ui->comboBox_log_file->setCurrentIndex(0);
        }else if(logSeverityFile == "debug"){
            ui->comboBox_log_file->setCurrentIndex(1);
        }else if(logSeverityFile == "info"){
            ui->comboBox_log_file->setCurrentIndex(2);
        }else if(logSeverityFile == "warning"){
            ui->comboBox_log_file->setCurrentIndex(3);
        }else if(logSeverityFile == "error"){
            ui->comboBox_log_file->setCurrentIndex(4);
        }else if(logSeverityFile == "fatal"){
            ui->comboBox_log_file->setCurrentIndex(5);
        }
    }

    // groups
    {
        groupSta.clear();
        auto groupTree = xml.get_child_optional("VieSchedpp.station.groups");
        if(groupTree.is_initialized()){
            for (auto &it: *groupTree) {
                std::string name = it.first;
                if (name == "group") {
                    std::string groupName = it.second.get_child("<xmlattr>.name").data();
                    std::vector<std::string> members;
                    for (auto &it2: it.second) {
                        if (it2.first == "member") {
                            members.push_back(it2.second.data());
                        }
                    }

                    int r = 0;
                    for(int i = 0; i<allStationPlusGroupModel->rowCount(); ++i){
                        QString txt = allStationPlusGroupModel->item(i)->text();
                        if(txt == "__all__"){
                            ++r;
                            continue;
                        }
                        if(groupSta.find(txt.toStdString()) == groupSta.end()){
                            break;
                        }
                        if(txt>QString::fromStdString(groupName)){
                            break;
                        }else{
                            ++r;
                        }
                    }

                    groupSta[groupName] = members;
                    allStationPlusGroupModel->insertRow(r,new QStandardItem(QIcon(":/icons/icons/station_group.png"),
                                                                            QString::fromStdString(groupName) ));
                }
            }
        }
    }
    {
        groupSrc.clear();
        ui->treeWidget_srcGroupForStatistics->clear();
        auto groupTree = xml.get_child_optional("VieSchedpp.source.groups");
        if(groupTree.is_initialized()){
            for (auto &it: *groupTree) {
                std::string name = it.first;
                if (name == "group") {
                    std::string groupName = it.second.get_child("<xmlattr>.name").data();
                    std::vector<std::string> members;
                    for (auto &it2: it.second) {
                        if (it2.first == "member") {
                            members.push_back(it2.second.data());
                        }
                    }

                    int r = 0;
                    for(int i = 0; i<allSourcePlusGroupModel->rowCount(); ++i){
                        QString txt = allSourcePlusGroupModel->item(i)->text();
                        if(txt == "__all__"){
                            ++r;
                            continue;
                        }
                        if(groupSrc.find(txt.toStdString()) == groupSrc.end()){
                            break;
                        }
                        if(txt>QString::fromStdString(groupName)){
                            break;
                        }else{
                            ++r;
                        }
                    }

                    groupSrc[groupName] = members;
                    allSourcePlusGroupModel->insertRow(r,new QStandardItem(QIcon(":/icons/icons/source_group.png"),
                                                                            QString::fromStdString(groupName) ));
                    QTreeWidgetItem *itm = new QTreeWidgetItem();
                    itm->setText(0,QString::fromStdString(groupName));
                    itm->setCheckState(0,Qt::Unchecked);
                    ui->treeWidget_srcGroupForStatistics->addTopLevelItem(itm);

                }
            }
        }
    }
    {
        groupBl.clear();
        auto groupTree = xml.get_child_optional("VieSchedpp.baseline.groups");
        if(groupTree.is_initialized()){
            for (auto &it: *groupTree) {
                std::string name = it.first;
                if (name == "group") {
                    std::string groupName = it.second.get_child("<xmlattr>.name").data();
                    std::vector<std::string> members;
                    for (auto &it2: it.second) {
                        if (it2.first == "member") {
                            members.push_back(it2.second.data());
                        }
                    }

                    int r = 0;
                    for(int i = 0; i<allBaselinePlusGroupModel->rowCount(); ++i){
                        QString txt = allBaselinePlusGroupModel->item(i)->text();
                        if(txt == "__all__"){
                            ++r;
                            continue;
                        }
                        if(groupBl.find(txt.toStdString()) == groupBl.end()){
                            break;
                        }
                        if(txt>QString::fromStdString(groupName)){
                            break;
                        }else{
                            ++r;
                        }
                    }

                    groupBl[groupName] = members;
                    allBaselinePlusGroupModel->insertRow(r,new QStandardItem(QIcon(":/icons/icons/baseline_group.png"),
                                                                            QString::fromStdString(groupName) ));
                }
            }
        }
    }

    //parameters
    {
        paraSta.clear();
        ui->ComboBox_parameterStation->clear();
        const auto &para_tree = xml.get_child("VieSchedpp.station.parameters");
        for (auto &it: para_tree) {
            std::string name = it.first;
            if (name == "parameter") {

                VieVS::ParameterSettings::ParametersStations PARA;
                auto PARA_ = VieVS::ParameterSettings::ptree2parameterStation(it.second);
                PARA = PARA_.second;
                paraSta[PARA_.first] = PARA;
                ui->ComboBox_parameterStation->addItem(QString::fromStdString(PARA_.first));

            }
        }
    }
    {
        paraSrc.clear();
        ui->ComboBox_parameterSource->clear();
        const auto &para_tree = xml.get_child("VieSchedpp.source.parameters");
        for (auto &it: para_tree) {
            std::string name = it.first;
            if (name == "parameter") {

                VieVS::ParameterSettings::ParametersSources PARA;
                auto PARA_ = VieVS::ParameterSettings::ptree2parameterSource(it.second);
                PARA = PARA_.second;
                paraSrc[PARA_.first] = PARA;
                ui->ComboBox_parameterSource->addItem(QString::fromStdString(PARA_.first));

            }
        }
    }
    {
        paraBl.clear();
        ui->ComboBox_parameterBaseline->clear();
        const auto &para_tree = xml.get_child("VieSchedpp.baseline.parameters");
        for (auto &it: para_tree) {
            std::string name = it.first;
            if (name == "parameter") {

                VieVS::ParameterSettings::ParametersBaselines PARA;
                auto PARA_ = VieVS::ParameterSettings::ptree2parameterBaseline(it.second);
                PARA = PARA_.second;
                paraBl[PARA_.first] = PARA;
                ui->ComboBox_parameterBaseline->addItem(QString::fromStdString(PARA_.first));

            }
        }
    }

    //setup
    {
        auto ctree = xml.get_child("VieSchedpp.station.setup");
        for(const auto &any: ctree){
            if(any.first == "setup"){
                ui->treeWidget_setupStation->topLevelItem(0)->child(0)->setSelected(true);
                addSetup(ui->treeWidget_setupStation, any.second, ui->comboBox_stationSettingMember,
                         ui->ComboBox_parameterStation, ui->DateTimeEdit_startParameterStation,
                         ui->DateTimeEdit_endParameterStation, ui->comboBox_parameterStationTransition,
                         ui->pushButton_3);
            }
        }
    }
    {
        auto ctree = xml.get_child("VieSchedpp.source.setup");
        for(const auto &any: ctree){
            if(any.first == "setup"){
                ui->treeWidget_setupSource->topLevelItem(0)->child(0)->setSelected(true);
                addSetup(ui->treeWidget_setupSource, any.second, ui->comboBox_sourceSettingMember,
                         ui->ComboBox_parameterSource, ui->DateTimeEdit_startParameterSource,
                         ui->DateTimeEdit_endParameterSource, ui->comboBox_parameterSourceTransition,
                         ui->pushButton_addSetupSource);
            }
        }
    }
    {
        auto ctree = xml.get_child("VieSchedpp.baseline.setup");
        for(const auto &any: ctree){
            if(any.first == "setup"){
                ui->treeWidget_setupBaseline->topLevelItem(0)->child(0)->setSelected(true);
                addSetup(ui->treeWidget_setupBaseline, any.second, ui->comboBox_baselineSettingMember,
                         ui->ComboBox_parameterBaseline, ui->DateTimeEdit_startParameterBaseline,
                         ui->DateTimeEdit_endParameterBaseline, ui->comboBox_parameterBaselineTransition,
                         ui->pushButton_addSetupBaseline);
            }
        }
    }

    //wait times
    {
        auto waitTime_tree = xml.get_child("VieSchedpp.station.waitTimes");
        ui->treeWidget_setupStationWait->clear();
        for (auto &it: waitTime_tree) {
            std::string name = it.first;
            if (name == "waitTime") {
                std::string memberName = it.second.get_child("<xmlattr>.member").data();

                int fieldSystem = it.second.get<int>("fieldSystem");
                int preob = it.second.get<int>("preob");
                int midob = it.second.get<int>("midob");
                int postob = it.second.get<int>("postob");

                ui->comboBox_stationSettingMember_wait->setCurrentText(QString::fromStdString(memberName));
                ui->SpinBox_fieldSystem->setValue(fieldSystem);
                ui->SpinBox_preob->setValue(preob);
                ui->SpinBox_midob->setValue(midob);
                ui->SpinBox_postob->setValue(postob);

                ui->pushButton_setupWaitAdd->click();
            }
        }
    }
    // cable wrap buffer
    {
        auto waitTime_tree = xml.get_child("VieSchedpp.station.cableWrapBuffers");
        ui->treeWidget_setupStationAxis->clear();
        for (auto &it: waitTime_tree) {
            std::string name = it.first;
            if (name == "cableWrapBuffer") {
                std::string memberName = it.second.get_child("<xmlattr>.member").data();

                auto axis1Low = it.second.get<double>("axis1LowOffset");
                auto axis1Up = it.second.get<double>("axis1UpOffset");
                auto axis2Low = it.second.get<double>("axis2LowOffset");
                auto axis2Up = it.second.get<double>("axis2UpOffset");

                ui->comboBox_stationSettingMember_axis->setCurrentText(QString::fromStdString(memberName));
                ui->DoubleSpinBox_axis1low->setValue(axis1Low);
                ui->DoubleSpinBox_axis1up->setValue(axis1Up);
                ui->DoubleSpinBox_axis2low->setValue(axis2Low);
                ui->DoubleSpinBox_axis2up->setValue(axis2Up);

                ui->pushButton_setupAxisAdd->click();
            }
        }
    }

    // sky coverage
    {
        double influenceDistance = xml.get("VieSchedpp.skyCoverage.influenceDistance",30.0);
        ui->influenceDistanceDoubleSpinBox->setValue(influenceDistance);
        int influenceInterval = xml.get("VieSchedpp.skyCoverage.influenceInterval",3600);
        ui->influenceTimeSpinBox->setValue(influenceInterval);
        double maxTwinTelecopeDistance = xml.get("VieSchedpp.skyCoverage.maxTwinTelecopeDistance",0.0);
        ui->maxDistanceForCombiningAntennasDoubleSpinBox->setValue(maxTwinTelecopeDistance);
        std::string interpolationDistance = xml.get("VieSchedpp.skyCoverage.interpolationDistance","cosine");
        if(interpolationDistance == "cosine"){
            ui->comboBox_skyCoverageDistanceType->setCurrentIndex(0);
        }else if(interpolationDistance == "linear"){
            ui->comboBox_skyCoverageDistanceType->setCurrentIndex(1);
        }else if(interpolationDistance == "constant"){
            ui->comboBox_skyCoverageDistanceType->setCurrentIndex(2);
        }
        std::string interpolationTime = xml.get("VieSchedpp.skyCoverage.interpolationTime","cosine");
        if(interpolationTime == "cosine"){
            ui->comboBox_skyCoverageTimeType->setCurrentIndex(0);
        }else if(interpolationTime == "linear"){
            ui->comboBox_skyCoverageTimeType->setCurrentIndex(1);
        }else if(interpolationTime == "constant"){
            ui->comboBox_skyCoverageTimeType->setCurrentIndex(2);
        }
    }

    //weight factors
    {
        double weightFactor_skyCoverage = xml.get("VieSchedpp.weightFactor.skyCoverage",0.0);
        if(weightFactor_skyCoverage == 0){
            ui->checkBox_weightCoverage->setChecked(false);
        }else{
            ui->checkBox_weightCoverage->setChecked(true);
            ui->doubleSpinBox_weightSkyCoverage->setValue(weightFactor_skyCoverage);
        }
        double weightFactor_numberOfObservations = xml.get("VieSchedpp.weightFactor.numberOfObservations",0.0);
        if(weightFactor_numberOfObservations == 0){
            ui->checkBox_weightNobs->setChecked(false);
        }else{
            ui->checkBox_weightNobs->setChecked(true);
            ui->doubleSpinBox_weightNumberOfObservations->setValue(weightFactor_numberOfObservations);
        }
        double weightFactor_duration = xml.get("VieSchedpp.weightFactor.duration",0.0);
        if(weightFactor_duration == 0){
            ui->checkBox_weightDuration->setChecked(false);
        }else{
            ui->checkBox_weightDuration->setChecked(true);
            ui->doubleSpinBox_weightDuration->setValue(weightFactor_duration);
        }
        double weightFactor_averageSources = xml.get("VieSchedpp.weightFactor.averageSources",0.0);
        if(weightFactor_averageSources == 0){
            ui->checkBox_weightAverageSources->setChecked(false);
        }else{
            ui->checkBox_weightAverageSources->setChecked(true);
            ui->doubleSpinBox_weightAverageSources->setValue(weightFactor_averageSources);
        }
        double weightFactor_averageStations = xml.get("VieSchedpp.weightFactor.averageStations",0.0);
        if(weightFactor_averageStations == 0){
            ui->checkBox_weightAverageStations->setChecked(false);
        }else{
            ui->checkBox_weightAverageStations->setChecked(true);
            ui->doubleSpinBox_weightAverageStations->setValue(weightFactor_averageStations);
        }
        double weightFactor_averageBaselines = xml.get("VieSchedpp.weightFactor.averageBaselines",0.0);
        if(weightFactor_averageBaselines == 0){
            ui->checkBox_weightAverageBaselines->setChecked(false);
        }else{
            ui->checkBox_weightAverageBaselines->setChecked(true);
            ui->doubleSpinBox_weightAverageBaselines->setValue(weightFactor_averageBaselines);
        }
        double weightFactor_idleTime = xml.get("VieSchedpp.weightFactor.idleTime",0.0);
        int weightFactor_idleTimeInterval = xml.get("VieSchedpp.weightFactor.idleTimeInterval",600);
        if(weightFactor_idleTime == 0){
            ui->checkBox_weightIdleTime->setChecked(false);
        }else{
            ui->checkBox_weightIdleTime->setChecked(true);
            ui->doubleSpinBox_weightIdleTime->setValue(weightFactor_idleTime);
            ui->spinBox_idleTimeInterval->setValue(weightFactor_idleTimeInterval);
        }
        double weightFactor_weightDeclination = xml.get("VieSchedpp.weightFactor.weightDeclination",0.0);
        double weightFactor_declinationStartWeight = xml.get("VieSchedpp.weightFactor.declinationStartWeight",0.0);
        double weightFactor_declinationFullWeight = xml.get("VieSchedpp.weightFactor.declinationFullWeight",0.0);
        if(weightFactor_weightDeclination == 0){
            ui->checkBox_weightLowDeclination->setChecked(false);
        }else{
            ui->checkBox_weightLowDeclination->setChecked(true);
            ui->doubleSpinBox_weightLowDec->setValue(weightFactor_weightDeclination);
            ui->doubleSpinBox_weightLowDecStart->setValue(weightFactor_declinationStartWeight);
            ui->doubleSpinBox_weightLowDecEnd->setValue(weightFactor_declinationFullWeight);
        }
        double weightFactor_weightLowElevation = xml.get("VieSchedpp.weightFactor.weightLowElevation",0.0);
        double weightFactor_lowElevationStartWeight = xml.get("VieSchedpp.weightFactor.lowElevationStartWeight",0.0);
        double weightFactor_lowElevationFullWeight = xml.get("VieSchedpp.weightFactor.lowElevationFullWeight",0.0);
        if(weightFactor_weightLowElevation == 0){
            ui->checkBox_weightLowElevation->setChecked(false);
        }else{
            ui->checkBox_weightLowElevation->setChecked(true);
            ui->doubleSpinBox_weightLowEl->setValue(weightFactor_weightLowElevation);
            ui->doubleSpinBox_weightLowElStart->setValue(weightFactor_lowElevationStartWeight);
            ui->doubleSpinBox_weightLowElEnd->setValue(weightFactor_lowElevationFullWeight);
        }
    }

    //conditions
    boost::optional<boost::property_tree::ptree &> ctree = xml.get_child_optional("VieSchedpp.optimization");
    if (ctree.is_initialized()) {

        boost::property_tree::ptree PARA_source = xml.get_child("VieSchedpp.source");
        ui->checkBox_gentleSourceReduction->setChecked(false);

        for(const auto &any: *ctree){
            if(any.first == "combination"){
                if(any.second.get_value<std::string>() == "and"){
                    ui->comboBox_conditions_combinations->setCurrentText("and");
                }else{
                    ui->comboBox_conditions_combinations->setCurrentText("or");
                }
            }else if(any.first == "maxNumberOfIterations"){
                ui->spinBox_maxNumberOfIterations->setValue(any.second.get_value<int>());
            }else if(any.first == "numberOfGentleSourceReductions"){
                ui->spinBox_gentleSourceReduction->setValue(any.second.get_value<unsigned int>());
                ui->checkBox_gentleSourceReduction->setChecked(true);
            }else if(any.first == "minNumberOfSourcesToReduce"){
                ui->spinBox_minNumberOfReducedSources->setValue(any.second.get_value<unsigned int>());
            }else if(any.first == "percentageGentleSourceReduction"){
                ui->spinBox_gentleSourceReduction->setValue(any.second.get_value<double>());
            }else if(any.first == "condition"){
                std::string member = any.second.get<std::string>("members");
                auto scans = any.second.get<unsigned int>("minScans");
                auto bls = any.second.get<unsigned int>("minBaselines");

                ui->comboBox_conditions_members->setCurrentText(QString::fromStdString(member));
                ui->spinBox_condtionsMinNumScans->setValue(scans);
                ui->spinBox_conditionsMinNumBaselines->setValue(bls);

                ui->pushButton_addCondition->click();
            }
        }
    }

    //mode
    {
        ui->groupBox_modeSked->setChecked(false);
        ui->groupBox_modeCustom->setChecked(false);

        while(ui->tableWidget_ModesPolicy->rowCount() >0){
            ui->tableWidget_ModesPolicy->removeRow(0);
        }
        ui->tableWidget_ModesPolicy->setRowCount(0);

        if(xml.get_optional<std::string>("VieSchedpp.mode.skdMode").is_initialized()){
            QString mode = QString::fromStdString(xml.get<std::string>("VieSchedpp.mode.skdMode"));
            ui->groupBox_modeSked->setChecked(true);
            ui->comboBox_skedObsModes->setCurrentText(mode);
            addModesCustomTable("X",8.590,10);
            addModesCustomTable("S",2.260,6);
        }
        if(xml.get_child_optional("VieSchedpp.mode.simple").is_initialized()){
            while(ui->tableWidget_modeCustonBand->rowCount() >0){
                ui->tableWidget_modeCustonBand->removeRow(0);
            }
            ui->tableWidget_modeCustonBand->setRowCount(0);
            ui->sampleRateDoubleSpinBox->setValue(xml.get<double>("VieSchedpp.mode.simple.sampleRate"));
            ui->sampleBitsSpinBox->setValue(xml.get<double>("VieSchedpp.mode.simple.bits"));
            ui->groupBox_modeCustom->setChecked(true);
            boost::property_tree::ptree & bands = xml.get_child("VieSchedpp.mode.simple.bands");
            for(const auto &band:bands){
                if(band.first == "band"){
                    double wavelength = band.second.get<double>("wavelength");
                    int channels = band.second.get<int>("channels");
                    QString name = QString::fromStdString(band.second.get<std::string>("<xmlattr>.name"));

                    double freq = 1/(wavelength*1e9/299792458.);

                    addModesCustomTable(name,freq,channels);
                }
            }
        }
        if(xml.get_child_optional("VieSchedpp.mode.custom").is_initialized()){
            ui->groupBox_modeAdvanced->setChecked(true);

            std::vector<std::string> station_names;
            for(int i=0; i<selectedStationModel->rowCount(); ++i){
                station_names.push_back(selectedStationModel->item(i)->text().toStdString());
            }

            advancedObservingMode_ = VieVS::ObservingMode(xml.get_child("VieSchedpp.mode.custom"), station_names);
            updateAdvancedObservingMode();

        }
    }

    //mode_bandPolicy
    {
        boost::optional<boost::property_tree::ptree &> ctree = xml.get_child_optional("VieSchedpp.mode.bandPolicies");
        if(ctree.is_initialized()){
            for(const auto & any:*ctree){
                if(any.first == "bandPolicy"){
                    std::string name = any.second.get<std::string>("<xmlattr>.name");
                    std::string staReq = any.second.get("station.tag","required");
                    std::string srcReq = any.second.get("source.tag","required");
                    double minSNR = any.second.get<double>("minSNR");
                    std::string staBackup;
                    std::string srcBackup;
                    double stationBackupValue;
                    double sourceBackupValue;
                    if(any.second.get_optional<double>("station.backup_maxValueTimes").is_initialized()){
                        staBackup = "max value Times";
                        stationBackupValue = any.second.get<double>("station.backup_maxValueTimes");
                    } else if(any.second.get_optional<double>("station.backup_minValueTimes").is_initialized()){
                        staBackup = "min value Times";
                        stationBackupValue = any.second.get<double>("station.backup_minValueTimes");
                    } else if(any.second.get_optional<double>("station.backup_value").is_initialized()){
                        staBackup = "value";
                        stationBackupValue = any.second.get<double>("station.backup_value");
                    } else {
                        staBackup = "none";
                        stationBackupValue = 0;
                    }
                    if(any.second.get_optional<double>("source.backup_maxValueTimes").is_initialized()){
                        srcBackup = "max value Times";
                        sourceBackupValue = any.second.get<double>("source.backup_maxValueTimes");
                    } else if(any.second.get_optional<double>("source.backup_minValueTimes").is_initialized()){
                        srcBackup = "min value Times";
                        sourceBackupValue = any.second.get<double>("source.backup_minValueTimes");
                    } else if(any.second.get_optional<double>("source.backup_value").is_initialized()){
                        srcBackup = "value";
                        sourceBackupValue = any.second.get<double>("source.backup_value");
                    } else {
                        srcBackup = "none";
                        sourceBackupValue = 0;
                    }


                    auto t = ui->tableWidget_ModesPolicy;
                    for(int i=0; i<t->rowCount(); ++i){

                        std::string tname = t->verticalHeaderItem(i)->text().toStdString();
                        if(tname != name){
                            continue;
                        }

                        qobject_cast<QDoubleSpinBox*>(t->cellWidget(i,0))->setValue(minSNR);
                        qobject_cast<QComboBox*>(t->cellWidget(i,1))->setCurrentText(QString::fromStdString(staReq));
                        qobject_cast<QComboBox*>(t->cellWidget(i,4))->setCurrentText(QString::fromStdString(srcReq));
                        qobject_cast<QComboBox*>(t->cellWidget(i,2))->setCurrentText(QString::fromStdString(staBackup));
                        qobject_cast<QComboBox*>(t->cellWidget(i,5))->setCurrentText(QString::fromStdString(srcBackup));
                        qobject_cast<QDoubleSpinBox*>(t->cellWidget(i,3))->setValue(stationBackupValue);
                        qobject_cast<QDoubleSpinBox*>(t->cellWidget(i,6))->setValue(sourceBackupValue);
                        break;
                    }

                }
            }
        }

    }

    //multisched
    {
        auto twmss = ui->treeWidget_multiSchedSelected;
        auto twms = ui->treeWidget_multiSched;
        ui->groupBox_multiScheduling->setChecked(false);
        twmss->clear();
        for(int i=0; i<3; ++i){
            for(int j=0; j<twms->topLevelItem(i)->childCount(); ++j){
                twms->topLevelItem(i)->child(j)->setDisabled(false);
            }
        }


        boost::optional<boost::property_tree::ptree &> ctree_o = xml.get_child_optional("VieSchedpp.multisched");
        if(ctree_o.is_initialized()){
            const boost::property_tree::ptree &ctree = ctree_o.get();
            ui->groupBox_multiScheduling->setChecked(true);
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


            for(const auto &any: ctree){
                QString name = QString::fromStdString(any.first);
                if(name == "maxNumber" || name == "seed"){
                    continue;
                }
                QString parameterName;
                bool hasMember = false;
                QString member = "global";

                if(name.left(8) == "station_"){
                    name = name.mid(8);
                    hasMember = true;
                    parameterName = "Station";
                }else if(name.left(7) == "source_"){
                    name = name.mid(7);
                    hasMember = true;
                    parameterName = "Source";
                }else if(name.left(9) == "baseline_"){
                    name = name.mid(9);
                    hasMember = true;
                    parameterName = "Baseline";
                }else if(name.left(14) == "weight_factor_"){
                    name = name.mid(14);
                    hasMember = false;
                    parameterName = "Weight factor";
                }else if(name.left(8) == "general_"){
                    name = name.mid(8);
                    hasMember = false;
                    parameterName = "General";
                }else if(name.left(13) == "Sky_Coverage_"){
                    name = name.mid(13);
                    hasMember = false;
                    parameterName = "Sky Coverage";
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
                if(name != "general subnetting" && name != "general fillinmode during scan selection" &&
                        name != "general fillinmode influence on scan selection" && name != "general fillinmode a posteriori" ){
                    for(const auto &any2 : any.second){
                        if(any2.first == "value"){
                            values.push_back(any2.second.get_value<double>());
                        }
                    }
                }

                if(parameterName == "General"){
                    for(int i=0; i<twms->topLevelItem(0)->childCount(); ++i){
                        if(name == twms->topLevelItem(0)->child(i)->text(0)){
                            twms->topLevelItem(0)->child(i)->setDisabled(true);
                            break;
                        }
                    }
                }else if(parameterName == "Weight factor"){
                    for(int i=0; i<twms->topLevelItem(1)->childCount(); ++i){
                        if(name == twms->topLevelItem(1)->child(i)->text(0)){
                            twms->topLevelItem(1)->child(i)->setDisabled(true);
                            break;
                        }
                    }
                }else if(parameterName == "Sky Coverage"){
                    for(int i=0; i<twms->topLevelItem(2)->childCount(); ++i){
                        if(name == twms->topLevelItem(2)->child(i)->text(0)){
                            twms->topLevelItem(2)->child(i)->setDisabled(true);
                            break;
                        }
                    }
                }

                QIcon ic1;
                QIcon ic2;
                if(parameterName == "General"){
                    ic1 = QIcon(":/icons/icons/applications-internet-2.png");
                    ic2 = QIcon(":/icons/icons/applications-internet-2.png");
                }else if(parameterName == "Weight factor"){
                    ic1 = QIcon(":/icons/icons/weight.png");
                    ic2 = QIcon(":/icons/icons/applications-internet-2.png");
                }else if(parameterName == "Sky Coverage"){
                    ic1 = QIcon(":/icons/icons/sky_coverage.png");
                    ic2 = QIcon(":/icons/icons/sky_coverage.png");
                }else if(parameterName == "Station"){
                    ic1 = QIcon(":/icons/icons/station.png");
                    if(member == "__all__" || groupSta.find(member.toStdString()) != groupSta.end()){
                        ic2 = QIcon(":/icons/icons/station_group.png");
                    }else{
                        ic2 = QIcon(":/icons/icons/station.png");
                    }
                }else if(parameterName == "Source"){
                    ic1 = QIcon(":/icons/icons/source.png");
                    if(member == "__all__" || groupSrc.find(member.toStdString()) == groupSrc.end()){
                        ic2 = QIcon(":/icons/icons/source_group.png");
                    }else{
                        ic2 = QIcon(":/icons/icons/source.png");
                    }

                }else if(parameterName == "Baseline"){
                    ic1 = QIcon(":/icons/icons/baseline.png");
                    if(member == "__all__" || groupBl.find(member.toStdString()) == groupBl.end()){
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
    }

    //output
    {
        ui->lineEdit_experimentDescription->setText(QString::fromStdString(xml.get("VieSchedpp.output.experimentDescription","dummy")));

        ui->schedulerLineEdit->setText(QString::fromStdString(xml.get("VieSchedpp.output.scheduler","unknown")));
        ui->correlatorLineEdit->setText(QString::fromStdString(xml.get("VieSchedpp.output.correlator","unknown")));

        ui->lineEdit_PIName->setText(QString::fromStdString(xml.get("VieSchedpp.output.pi.name","")));
        ui->lineEdit_PIEmail->setText(QString::fromStdString(xml.get("VieSchedpp.output.pi.email","")));
        ui->lineEdit_pi_phone->setText(QString::fromStdString(xml.get("VieSchedpp.output.pi.phone","")));
        ui->lineEdit_pi_affiliation->setText(QString::fromStdString(xml.get("VieSchedpp.output.pi.affiliation","")));


        QTableWidget *tw = ui->tableWidget_contact;

        const auto &t = xml.get_child_optional("VieSchedpp.output.contacts");
        if(t.is_initialized()){
            for (const auto &any : *t){
                std::string function = any.second.get("function","");
                std::string name = any.second.get("name","");
                std::string email = any.second.get("email","");
                std::string phone = any.second.get("phone","");
                std::string affiliation = any.second.get("affiliation","");

                tw->insertRow (tw->rowCount());
                tw->setItem   (tw->rowCount()-1, 0, new QTableWidgetItem(QString::fromStdString(function)));
                tw->setItem   (tw->rowCount()-1, 1, new QTableWidgetItem(QString::fromStdString(name)));
                tw->setItem   (tw->rowCount()-1, 2, new QTableWidgetItem(QString::fromStdString(email)));
                tw->setItem   (tw->rowCount()-1, 3, new QTableWidgetItem(QString::fromStdString(phone)));
                tw->setItem   (tw->rowCount()-1, 4, new QTableWidgetItem(QString::fromStdString(affiliation)));
            }
        }

        ui->plainTextEdit_notes->setPlainText(QString::fromStdString(xml.get("VieSchedpp.output.notes","")));

        if(xml.get("VieSchedpp.output.initializer_log",false)){
            ui->checkBox_outputInitializer->setChecked(true);
        }else{
            ui->checkBox_outputInitializer->setChecked(false);
        }
        if(xml.get("VieSchedpp.output.iteration_log",false)){
            ui->checkBox_outputIteration->setChecked(true);
        }else{
            ui->checkBox_outputIteration->setChecked(false);
        }
        if(xml.get("VieSchedpp.output.createSummary",false)){
            ui->checkBox_outputStatisticsFile->setChecked(true);
        }else{
            ui->checkBox_outputStatisticsFile->setChecked(false);
        }
        if(xml.get("VieSchedpp.output.createNGS",false)){
            ui->checkBox_outputNGSFile->setChecked(true);
        }else{
            ui->checkBox_outputNGSFile->setChecked(false);
        }

        if(xml.get("VieSchedpp.output.redirectNGS",false)){
            ui->checkBox_redirectNGS->setChecked(true);
        }else{
            ui->checkBox_redirectNGS->setChecked(false);
        }
        std::string ngs_direcotry = xml.get("VieSchedpp.output.NGS_directory","");
        if(ngs_direcotry.empty()){
            ui->lineEdit_outputNGS->setText("");
        }else{
            ui->lineEdit_outputNGS->setText(QString::fromStdString(ngs_direcotry));
        }

        if(xml.get("VieSchedpp.output.createSKD",false)){
            ui->checkBox_outputSkdFile->setChecked(true);
        }else{
            ui->checkBox_outputSkdFile->setChecked(false);
        }
        if(xml.get("VieSchedpp.output.createVEX",false)){
            ui->checkBox_outputVex->setChecked(true);
        }else{
            ui->checkBox_outputVex->setChecked(false);
        }
        if(xml.get("VieSchedpp.output.createSnrTable",false)){
            ui->checkBox_outputSnrTable->setChecked(true);
        }else{
            ui->checkBox_outputSnrTable->setChecked(false);
        }
        if(xml.get("VieSchedpp.output.createSourceGroupStatistics",false)){
            ui->checkBox_outputSourceGroupStatFile->setChecked(true);
            // TODO check statistics
        }else{
            ui->checkBox_outputSourceGroupStatFile->setChecked(false);
        }
    }

    //ruleFocusCorners
    {
        ui->groupBox_34->setChecked(false);
        boost::optional<boost::property_tree::ptree &> ctree = xml.get_child_optional("VieSchedpp.focusCorners");
        if (ctree.is_initialized()) {
            ui->groupBox_34->setChecked(true);
            for(const auto &any: *ctree){
                if(any.first == "cadence"){
                    ui->spinBox_intensiveBlockCadence->setValue(xml.get("VieSchedpp.focusCorners.cadence",900));
                }
            }
        }
    }

    //ruleScanSequence
    {
        ui->groupBox_scanSequence->setChecked(false);
        boost::optional<boost::property_tree::ptree &> ctree = xml.get_child_optional("VieSchedpp.rules.sourceSequence");
        if (ctree.is_initialized()) {
            ui->groupBox_scanSequence->setChecked(true);
            ui->spinBox_scanSequenceCadence->setValue(xml.get<int>("VieSchedpp.rules.sourceSequence.cadence"));
            for(const auto &any: *ctree){
                if(any.first == "sequence"){
                    int modulo = any.second.get<int>("modulo");
                    QString member = QString::fromStdString(any.second.get<std::string>("member"));
                    QComboBox* cb = qobject_cast<QComboBox*>(ui->tableWidget_scanSequence->cellWidget(modulo,0));
                    cb->setCurrentText(member);
                }
            }
        }
    }

    //ruleCalibratorBlock
    {
        ui->groupBox_CalibratorBlock->setChecked(false);
        boost::optional<boost::property_tree::ptree &> ctree = xml.get_child_optional("VieSchedpp.rules.calibratorBlock");
        if (ctree.is_initialized()) {
            ui->groupBox_CalibratorBlock->setChecked(true);
            if(xml.get("VieSchedpp.rules.calibratorBlock.cadence_nScanSelections", -1) != -1){
                ui->radioButton_calibratorScanSequence->setChecked(true);
                ui->spinBox_calibratorScanSequence->setValue(xml.get("VieSchedpp.rules.calibratorBlock.cadence_nScanSelections",5));
            }
            if(xml.get("VieSchedpp.rules.calibratorBlock.cadence_seconds", -1) != -1){
                ui->radioButton_calibratorTime->setChecked(true);
                ui->spinBox_calibratorTime->setValue(xml.get("VieSchedpp.rules.calibratorBlock.cadence_seconds",3600));
            }
            std::string members = xml.get("VieSchedpp.rules.calibratorBlock.member","__all__");
            ui->comboBox_calibratorBlock_calibratorSources->setCurrentText(QString::fromStdString(members));
            ui->spinBox_calibrator_maxScanSequence->setValue(xml.get("VieSchedpp.rules.calibratorBlock.nMaxScans",4));
            ui->spinBox_calibratorFixedScanLength->setValue(xml.get("VieSchedpp.rules.calibratorBlock.fixedScanTime",120));
            ui->radioButton->setChecked(true);

            ui->doubleSpinBox_calibratorHighElEnd->setValue(xml.get("VieSchedpp.rules.calibratorBlock.highElevation.fullWeight",70));
            ui->doubleSpinBox_calibratorHighElStart->setValue(xml.get("VieSchedpp.rules.calibratorBlock.highElevation.startWeight",50));
            ui->doubleSpinBox_calibratorLowElEnd->setValue(xml.get("VieSchedpp.rules.calibratorBlock.lowElevation.fullWeight",20));
            ui->doubleSpinBox_calibratorLowElStart->setValue(xml.get("VieSchedpp.rules.calibratorBlock.lowElevation.startWeight",40));
        }
    }

    //highImpactAzEl
    {
        ui->groupBox_highImpactAzEl->setChecked(false);
        boost::optional<boost::property_tree::ptree &> ctree = xml.get_child_optional("VieSchedpp.highImpact");
        if (ctree.is_initialized()) {
            ui->groupBox_highImpactAzEl->setChecked(true);

            ui->spinBox_highImpactInterval->setValue(xml.get("VieSchedpp.highImpact.interval",60));
            ui->spinBox_highImpactMinRepeat->setValue(xml.get("VieSchedpp.highImpact.repeat",300));

            for(const auto &any: *ctree){
                if(any.first == "targetAzEl"){
                    std::string member = any.second.get<std::string>("member");
                    ui->comboBox_highImpactStation->setCurrentText(QString::fromStdString(member));
                    ui->doubleSpinBox_highImpactAzimuth->setValue(any.second.get<double>("az"));
                    ui->doubleSpinBox_highImpactElevation->setValue(any.second.get<double>("el"));
                    ui->doubleSpinBox_highImpactMargin->setValue(any.second.get<double>("margin"));

                    ui->pushButton_addHighImpactAzEl->click();
                }
            }
        }
    }
}

// ################################################ default settings ##############################################################

void MainWindow::readSettings()
{
    std::string name = settings_.get<std::string>("settings.general.name","");
    ui->nameLineEdit->setText(QString::fromStdString(name));
    std::string email = settings_.get<std::string>("settings.general.email","");
    ui->emailLineEdit->setText(QString::fromStdString(email));
    std::string pathToScheduler = settings_.get<std::string>("settings.general.pathToScheduler","");
    ui->pathToSchedulerLineEdit->setText(QString::fromStdString(pathToScheduler));


    int fontSize = settings_.get<int>("settings.font.size",0);
    if(fontSize != 0){
        ui->spinBox_fontSize->setValue(fontSize);
    }
    std::string fontFamily = settings_.get<std::string>("settings.font.family","");
    if(!fontFamily.empty()){
        int idx = ui->fontComboBox_font->findText(QString::fromStdString(fontFamily));
        if(idx != -1){
            ui->fontComboBox_font->setCurrentIndex(idx);
        }
    }

    int iconSize = settings_.get<int>("settings.icon.size",0);
    if(iconSize != 0){
        ui->iconSizeSpinBox->setValue(iconSize);
    }



    std::string cAntenna = settings_.get<std::string>("settings.catalog_path.antenna","../CATALOGS/antenna.cat");
    ui->lineEdit_pathAntenna->setText(QString::fromStdString(cAntenna));
    std::string cEquip = settings_.get<std::string>("settings.catalog_path.equip","../CATALOGS/equip.cat");
    ui->lineEdit_pathEquip->setText(QString::fromStdString(cEquip));
    std::string cPosition = settings_.get<std::string>("settings.catalog_path.position","../CATALOGS/position.cat");
    ui->lineEdit_pathPosition->setText(QString::fromStdString(cPosition));
    std::string cMask = settings_.get<std::string>("settings.catalog_path.mask","../CATALOGS/mask.cat");
    ui->lineEdit_pathMask->setText(QString::fromStdString(cMask));
    std::string cSource = settings_.get<std::string>("settings.catalog_path.source","../CATALOGS/source.cat.geodetic.good");
    ui->lineEdit_pathSource->setText(QString::fromStdString(cSource));
    std::string cSource2 = settings_.get<std::string>("settings.catalog_path.source2","../CATALOGS/source.cat");
    ui->lineEdit_browseSource2->setText(QString::fromStdString(cSource2));
    std::string cFlux = settings_.get<std::string>("settings.catalog_path.flux","../CATALOGS/flux.cat");
    ui->lineEdit_pathFlux->setText(QString::fromStdString(cFlux));
    std::string cModes = settings_.get<std::string>("settings.catalog_path.modes","../CATALOGS/modes.cat");
    ui->lineEdit_pathModes->setText(QString::fromStdString(cModes));
    std::string cFreq = settings_.get<std::string>("settings.catalog_path.freq","../CATALOGS/freq.cat");
    ui->lineEdit_pathFreq->setText(QString::fromStdString(cFreq));
    std::string cTracks = settings_.get<std::string>("settings.catalog_path.tracks","../CATALOGS/tracks.cat");
    ui->lineEdit_pathTracks->setText(QString::fromStdString(cTracks));
    std::string cLoif = settings_.get<std::string>("settings.catalog_path.loif","../CATALOGS/loif.cat");
    ui->lineEdit_pathLoif->setText(QString::fromStdString(cLoif));
    std::string cRec = settings_.get<std::string>("settings.catalog_path.rec","../CATALOGS/rec.cat");
    ui->lineEdit_pathRec->setText(QString::fromStdString(cRec));
    std::string cRx = settings_.get<std::string>("settings.catalog_path.rx","../CATALOGS/rx.cat");
    ui->lineEdit_pathRx->setText(QString::fromStdString(cRx));
    std::string cHdpos = settings_.get<std::string>("settings.catalog_path.hdpos","../CATALOGS/hdpos.cat");
    ui->lineEdit_pathHdpos->setText(QString::fromStdString(cHdpos));

    std::string outputDirectory = settings_.get<std::string>("settings.output.directory","../out/");
    ui->lineEdit_outputPath->setText(QString::fromStdString(outputDirectory));
    std::string outputScheduler = settings_.get<std::string>("settings.output.scheduler","");
    ui->schedulerLineEdit->setText(QString::fromStdString(outputScheduler));
    std::string outputCorrelator = settings_.get<std::string>("settings.output.correlator","");
    ui->correlatorLineEdit->setText(QString::fromStdString(outputCorrelator));

    ui->lineEdit_PIName->setText(QString::fromStdString(settings_.get("settings.output.pi.name","")));
    ui->lineEdit_PIEmail->setText(QString::fromStdString(settings_.get("settings.output.pi.email","")));
    ui->lineEdit_pi_phone->setText(QString::fromStdString(settings_.get("settings.output.pi.phone","")));
    ui->lineEdit_pi_affiliation->setText(QString::fromStdString(settings_.get("settings.output.pi.affiliation","")));

//    ui->lineEdit_contactName->setText(QString::fromStdString(settings_.get("settings.output.contact1.name","")));
//    ui->lineEdit_contactEmail->setText(QString::fromStdString(settings_.get("settings.output.contact1.email","")));
//    ui->lineEdit_contact_phone->setText(QString::fromStdString(settings_.get("settings.output.contact1.phone","")));
//    ui->lineEdit_contact_affiliation->setText(QString::fromStdString(settings_.get("settings.output.contact1.affiliation","")));

//    ui->lineEdit_contactName_2->setText(QString::fromStdString(settings_.get("settings.output.contact2.name","")));
//    ui->lineEdit_contactEmail_2->setText(QString::fromStdString(settings_.get("settings.output.contact2.email","")));
//    ui->lineEdit_contact_phone_2->setText(QString::fromStdString(settings_.get("settings.output.contact2.phone","")));
//    ui->lineEdit_contact_affiliation_2->setText(QString::fromStdString(settings_.get("settings.output.contact2.affiliation","")));

//    ui->lineEdit_contactName_3->setText(QString::fromStdString(settings_.get("settings.output.contact3.name","")));
//    ui->lineEdit_contactEmail_3->setText(QString::fromStdString(settings_.get("settings.output.contact3.email","")));
//    ui->lineEdit_contact_phone_3->setText(QString::fromStdString(settings_.get("settings.output.contact3.phone","")));
//    ui->lineEdit_contact_affiliation_3->setText(QString::fromStdString(settings_.get("settings.output.contact3.affiliation","")));

    std::string notes = settings_.get<std::string>("settings.output.notes","");
    if(!notes.empty()){
        ui->plainTextEdit_notes->setPlainText(QString::fromStdString(notes).replace("\\n","\n"));
    }

    std::string threads = settings_.get<std::string>("settings.multiCore.threads","auto");
    ui->comboBox_nThreads->setCurrentText(QString::fromStdString(threads));
    int nThreadsManual = settings_.get<int>("settings.multiCore.nThreads",1);
    ui->spinBox_nCores->setValue(nThreadsManual);
    std::string jobScheduler = settings_.get<std::string>("settings.multiCore.jobScheduling","auto");
    ui->comboBox_jobSchedule->setCurrentText(QString::fromStdString(jobScheduler));
    int chunkSize = settings_.get<int>("settings.multiCore.chunkSize",0);
    ui->spinBox_chunkSize->setValue(chunkSize);

}

void MainWindow::createDefaultParameterSettings()
{
    VieVS::ParameterSettings::ParametersStations sta;
    sta.maxScan = 600;
    sta.minScan = 30;
    sta.minSlewtime = 0;
    sta.maxSlewtime = 600;
    sta.maxSlewDistance = 175;
    sta.minSlewDistance = 0;
    sta.maxWait = 600;
    sta.maxNumberOfScans = 9999;
    sta.weight = 1;
    sta.minElevation = 5;
    settings_.add_child("settings.station.parameters.parameter",VieVS::ParameterSettings::parameterStation2ptree("default",sta).get_child("parameters"));

    VieVS::ParameterSettings::ParametersSources src;
    src.minRepeat = 1800;
    src.minScan = 0;
    src.maxScan = 9999;
    src.weight = 1;
    src.minFlux = 0.05;
    src.maxNumberOfScans = 999;
    src.minNumberOfStations = 2;
    src.minElevation = 0;
    src.minSunDistance = 4;
    settings_.add_child("settings.source.parameters.parameter",VieVS::ParameterSettings::parameterSource2ptree("default",src).get_child("parameters"));

    VieVS::ParameterSettings::ParametersBaselines bl;
    bl.maxScan = 9999;
    bl.minScan = 0;
    bl.weight = 1;
    settings_.add_child("settings.baseline.parameters.parameter",VieVS::ParameterSettings::parameterBaseline2ptree("default",bl).get_child("parameters"));
    settings_.add("settings.station.waitTimes.setup",0);
    settings_.add("settings.station.waitTimes.source",5);
    settings_.add("settings.station.waitTimes.tape",1);
    settings_.add("settings.station.waitTimes.calibration",10);
    settings_.add("settings.station.waitTimes.corsynch",3);

    settings_.add("settings.station.cableWrapBuffers.axis1LowOffset", 5);
    settings_.add("settings.station.cableWrapBuffers.axis1UpOffset", 5);
    settings_.add("settings.station.cableWrapBuffers.axis2LowOffset", 0);
    settings_.add("settings.station.cableWrapBuffers.axis2UpOffset", 0);

    settings_.add("settings.output.directory", "../out/");
    settings_.add("settings.output.NGS_directory", "../out/");

    std::ofstream os;
    os.open("settings.xml");
    boost::property_tree::xml_parser::write_xml(os, settings_,
                                                boost::property_tree::xml_writer_make_settings<std::string>('\t', 1));
    os.close();

}

// ########################################### SAVE DEFAULT PARAMETERS ###########################################

void MainWindow::changeDefaultSettings(QStringList path, QStringList value, QString name)
{
    for(int i=0; i<path.count(); ++i){
        settings_.put(path.at(i).toStdString(),value.at(i).toStdString());
    }
    std::ofstream os;
    os.open("settings.xml");
    boost::property_tree::xml_parser::write_xml(os, settings_,
                                                boost::property_tree::xml_writer_make_settings<std::string>('\t', 1));
    os.close();
    QMessageBox::information(this,"Default settings changed",name);
}

void MainWindow::on_pushButton_5_clicked()
{
    QStringList path {"settings.general.name"};
    QStringList value {ui->nameLineEdit->text()};
    QString name = "Default user name changed!";
    changeDefaultSettings(path,value,name);
}


void MainWindow::on_pushButton_20_clicked()
{
    QStringList path;
    QStringList value;
    QString name = "Default settings changed!";

    path << "settings.font.size"
         << "settings.font.family"
         << "settings.icon.size";
    value << QString::number(ui->spinBox_fontSize->value())
          << ui->fontComboBox_font->currentText()
          << QString::number(ui->iconSizeSpinBox->value());

    changeDefaultSettings(path,value,name);
}


void MainWindow::on_pushButton_6_clicked()
{
    QStringList path {"settings.general.email"};
    QStringList value {ui->emailLineEdit->text()};
    QString name = "Default user email address changed!";
    changeDefaultSettings(path,value,name);
}

void MainWindow::on_pushButton_17_clicked()
{
    QStringList path {"settings.general.pathToScheduler"};
    QStringList value {ui->pathToSchedulerLineEdit->text()};
    QString name = "Default path to scheduler executable changed!";
    changeDefaultSettings(path,value,name);
}

void MainWindow::on_pushButton_saveCatalogPathes_clicked()
{
    settings_.put("settings.catalog_path.antenna",ui->lineEdit_pathAntenna->text().toStdString());
    settings_.put("settings.catalog_path.equip",ui->lineEdit_pathEquip->text().toStdString());
    settings_.put("settings.catalog_path.position",ui->lineEdit_pathPosition->text().toStdString());
    settings_.put("settings.catalog_path.mask",ui->lineEdit_pathMask->text().toStdString());
    settings_.put("settings.catalog_path.source",ui->lineEdit_pathSource->text().toStdString());
    settings_.put("settings.catalog_path.source2",ui->lineEdit_browseSource2->text().toStdString());
    settings_.put("settings.catalog_path.flux",ui->lineEdit_pathFlux->text().toStdString());
    settings_.put("settings.catalog_path.modes",ui->lineEdit_pathModes->text().toStdString());
    settings_.put("settings.catalog_path.freq",ui->lineEdit_pathFreq->text().toStdString());
    settings_.put("settings.catalog_path.tracks",ui->lineEdit_pathTracks->text().toStdString());
    settings_.put("settings.catalog_path.loif",ui->lineEdit_pathLoif->text().toStdString());
    settings_.put("settings.catalog_path.rec",ui->lineEdit_pathRec->text().toStdString());
    settings_.put("settings.catalog_path.rx",ui->lineEdit_pathRx->text().toStdString());
    settings_.put("settings.catalog_path.hdpos",ui->lineEdit_pathHdpos->text().toStdString());
    std::ofstream os;
    os.open("settings.xml");
    boost::property_tree::xml_parser::write_xml(os, settings_,
                                                boost::property_tree::xml_writer_make_settings<std::string>('\t', 1));
    os.close();
    QString txt = "Your default catalog pathes have been changed!";
    QMessageBox::information(this,"Default settings changed",txt);
}

void MainWindow::on_pushButton_26_clicked()
{
    QStringList path;
    QStringList value;
    QString name = "Default output selection changed!";

    path << "settings.output.directory"
         << "settings.output.initializer_log"
         << "settings.output.iteration_log"
         << "settings.output.createSummary"
         << "settings.output.createNGS"
         << "settings.output.redirectNGS"
         << "settings.output.NGS_directory"
         << "settings.output.createSKD"
         << "settings.output.createVEX"
         << "settings.output.createSnrTable"
         << "settings.output.createOperationsNotes"
         << "settings.output.createSourceGroupStatistics"
         << "settings.output.addTimestamps";

    value << ui->lineEdit_outputPath->text();
    ui->checkBox_outputInitializer->isChecked() ? value << "true" : value << "false";
    ui->checkBox_outputIteration->isChecked() ? value << "true" : value << "false";
    ui->checkBox_outputStatisticsFile->isChecked() ? value << "true" : value << "false";
    ui->checkBox_outputNGSFile->isChecked() ? value << "true" : value << "false";
    ui->checkBox_redirectNGS->isChecked() ? value << "true" : value << "false";
    value << ui->lineEdit_outputNGS->text();
    ui->checkBox_outputSkdFile->isChecked() ? value << "true" : value << "false";
    ui->checkBox_outputVex->isChecked() ? value << "true" : value << "false";
    ui->checkBox_outputSnrTable->isChecked() ? value << "true" : value << "false";
    ui->checkBox_outputOperationsNotes->isChecked() ? value << "true" : value << "false";
    ui->checkBox_outputSourceGroupStatFile->isChecked() ? value << "true" : value << "false";
    ui->checkBox_outputAddTimestamp->isChecked() ? value << "true" : value << "false";

    changeDefaultSettings(path,value,name);
}

void MainWindow::on_pushButton_23_clicked()
{
    QStringList path {"settings.output.scheduler"};
    QStringList value {ui->schedulerLineEdit->text()};
    QString name = "Default scheduler changed!";
    changeDefaultSettings(path,value,name);
}

void MainWindow::on_pushButton_22_clicked()
{
    QStringList path {"settings.output.correlator"};
    QStringList value {ui->correlatorLineEdit->text()};
    QString name = "Default correlator changed!";
    changeDefaultSettings(path,value,name);
}

void MainWindow::on_pushButton_7_clicked()
{
    QStringList path {"settings.mode.skdMode"};
    QStringList value {ui->comboBox_skedObsModes->currentText()};
    QString name = "Default skd observing mode changed";
    changeDefaultSettings(path,value,name);

}

void MainWindow::on_pushButton_8_clicked()
{
    QStringList path {"settings.general.subnetting"};
    QStringList value;
    if(ui->groupBox_subnetting->isChecked()){
        value << "true";
    }else{
        value << "false";
    }

    path << "settings.general.subnettingMinAngle";
    value << QString::number(ui->doubleSpinBox_subnettingDistance->value());

    path << "settings.general.subnettingMinNSta";
    value << QString::number(ui->doubleSpinBox_subnettingMinStations->value());
    path << "settings.general.subnettingMinNStaPercent";
    value << QString::number(ui->doubleSpinBox_subnettingMinStations->value());
    path << "settings.general.subnettingMinNStaAllBut";
    value << QString::number(ui->spinBox_subnetting_min_sta->value());
    path << "settings.general.subnettingMinNstaPercent_otherwiseAllBut";
    if(ui->radioButton_subnetting_percent->isChecked()){
        value << "true";
    }else{
        value << "false";
    }

    path << "settings.general.fillinmodeInfluenceOnSchedule";
    if(ui->checkBox_fillinModeInfluence->isChecked()){
        value << "true";
    }else{
        value << "false";
    }

    path << "settings.general.fillinmodeAPosteriori";
    if(ui->checkBox_fillinmode_aposteriori->isChecked()){
        value << "true";
    }else{
        value << "false";
    }

    path << "settings.general.fillinmodeDuringScanSelection";
    if(ui->checkBox_fillinmode_duringscan->isChecked()){
        value << "true";
    }else{
        value << "false";
    }

    path << "settings.general.idleToObservingTime";
    if(ui->radioButton_idleToObservingTime_yes->isChecked()){
        value << "true";
    }else{
        value << "false";
    }

    path << "settings.general.alignObservingTime";
    if(ui->radioButton_alignStart->isChecked()){
        value << "start";
    }else if(ui->radioButton_alignEnd->isChecked()){
        value << "end";
    }else{
        value << "individual";
    }

    QString name = "Default general parameters changed!";
    changeDefaultSettings(path,value,name);

}

void MainWindow::on_pushButton_9_clicked()
{
    QStringList path;
    QStringList value;

    path << "settings.weightFactor.skyCoverageChecked";
    if(ui->checkBox_weightCoverage->isChecked()){
        value << "true";
    }else{
        value << "false";
    }
    path << "settings.weightFactor.skyCoverage";
    value << QString("%1").arg(ui->doubleSpinBox_weightSkyCoverage->value());

    path << "settings.weightFactor.numberOfObservationsChecked";
    if(ui->checkBox_weightNobs->isChecked()){
        value << "true";
    }else{
        value << "false";
    }
    path << "settings.weightFactor.numberOfObservations";
    value << QString("%1").arg(ui->doubleSpinBox_weightNumberOfObservations->value());

    path << "settings.weightFactor.durationChecked";
    if(ui->checkBox_weightDuration->isChecked()){
        value << "true";
    }else{
        value << "false";
    }
    path << "settings.weightFactor.duration";
    value << QString("%1").arg(ui->doubleSpinBox_weightDuration->value());

    path << "settings.weightFactor.averageSourcesChecked";
    if(ui->checkBox_weightAverageSources->isChecked()){
        value << "true";
    }else{
        value << "false";
    }
    path << "settings.weightFactor.averageSources";
    value << QString("%1").arg(ui->doubleSpinBox_weightAverageSources->value());

    path << "settings.weightFactor.averageStationsChecked";
    if(ui->checkBox_weightAverageStations->isChecked()){
        value << "true";
    }else{
        value << "false";
    }
    path << "settings.weightFactor.averageStations";
    value << QString("%1").arg(ui->doubleSpinBox_weightAverageStations->value());

    path << "settings.weightFactor.idleTimeChecked";
    if(ui->checkBox_weightIdleTime->isChecked()){
        value << "true";
    }else{
        value << "false";
    }
    path << "settings.weightFactor.weightIdleTime";
    value << QString("%1").arg(ui->doubleSpinBox_weightIdleTime->value());
    path << "settings.weightFactor.idleTimeInterval";
    value << QString("%1").arg(ui->spinBox_idleTimeInterval->value());


    path << "settings.weightFactor.weightDeclinationChecked";
    if(ui->checkBox_weightLowDeclination->isChecked()){
        value << "true";
    }else{
        value << "false";
    }
    path << "settings.weightFactor.weightDeclination";
    value << QString("%1").arg(ui->doubleSpinBox_weightLowDec->value());
    path << "settings.weightFactor.declinationStartWeight";
    value << QString("%1").arg(ui->doubleSpinBox_weightLowDecStart->value());
    path << "settings.weightFactor.declinationFullWeight";
    value << QString("%1").arg(ui->doubleSpinBox_weightLowDecEnd->value());

    path << "settings.weightFactor.weightLowElevationChecked";
    if(ui->checkBox_weightLowElevation->isChecked()){
        value << "true";
    }else{
        value << "false";
    }
    path << "settings.weightFactor.weightLowElevation";
    value << QString("%1").arg(ui->doubleSpinBox_weightLowEl->value());
    path << "settings.weightFactor.lowElevationStartWeight";
    value << QString("%1").arg(ui->doubleSpinBox_weightLowElStart->value());
    path << "settings.weightFactor.lowElevationFullWeight";
    value << QString("%1").arg(ui->doubleSpinBox_weightLowElEnd->value());

    QString name = "Default weight factors changed!";
    changeDefaultSettings(path,value,name);

}

void MainWindow::on_pushButton_10_clicked()
{
    QStringList path;
    QStringList value;

    path << "settings.skyCoverage.influenceDistance";
    value << QString("%1").arg(ui->influenceDistanceDoubleSpinBox->value());
    path << "settings.skyCoverage.influenceInterval";
    value << QString("%1").arg(ui->influenceTimeSpinBox->value());
    path << "settings.skyCoverage.distanceType";
    value << ui->comboBox_skyCoverageDistanceType->currentText();
    path << "settings.skyCoverage.timeType";
    value << ui->comboBox_skyCoverageTimeType->currentText();
    path << "settings.skyCoverage.maxTwinTelecopeDistance";
    value << QString("%1").arg(ui->maxDistanceForCombiningAntennasDoubleSpinBox->value());

    QString name = "Default sky coverage parametrization changed!";
    changeDefaultSettings(path,value,name);

}

void MainWindow::on_pushButton_11_clicked()
{
    QStringList path;
    QStringList value;

    path << "settings.station.waitTimes.fieldSystem";
    value << QString("%1").arg(ui->SpinBox_fieldSystem->value());
    path << "settings.station.waitTimes.preob";
    value << QString("%1").arg(ui->SpinBox_preob->value());
    path << "settings.station.waitTimes.midob";
    value << QString("%1").arg(ui->SpinBox_midob->value());
    path << "settings.station.waitTimes.postob";
    value << QString("%1").arg(ui->SpinBox_postob->value());

    QString name = "Default wait times changed!";
    changeDefaultSettings(path,value,name);
}

void MainWindow::on_pushButton_12_clicked()
{
    QStringList path;
    QStringList value;

    path << "settings.station.cableWrapBuffers.axis1LowOffset";
    value << QString("%1").arg(ui->DoubleSpinBox_axis1low->value());
    path << "settings.station.cableWrapBuffers.axis1UpOffset";
    value << QString("%1").arg(ui->DoubleSpinBox_axis1up->value());
    path << "settings.station.cableWrapBuffers.axis2LowOffset";
    value << QString("%1").arg(ui->DoubleSpinBox_axis2low->value());
    path << "settings.station.cableWrapBuffers.axis2UpOffset";
    value << QString("%1").arg(ui->DoubleSpinBox_axis2up->value());

    QString name = "Default cable wrap buffers changed!";
    changeDefaultSettings(path,value,name);
}


void MainWindow::on_pushButton_41_clicked()
{
    QStringList path {"settings.output.notes"};
    QStringList value {ui->plainTextEdit_notes->toPlainText().replace("\n","\\n")};
    QString name = "Default notes changed!";
    changeDefaultSettings(path,value,name);
}


void MainWindow::on_pushButton_save_multiCore_clicked()
{
    QString threads = ui->comboBox_nThreads->currentText();
    QString nThreadsManual = QString::number(ui->spinBox_nCores->value());
    QString jobScheduler = ui->comboBox_jobSchedule->currentText();
    QString chunkSize = QString::number(ui->spinBox_chunkSize->value());

    QStringList path {"settings.multiCore.threads", "settings.multiCore.nThreads", "settings.multiCore.jobScheduling", "settings.multiCore.chunkSize"};
    QStringList value {threads, nThreadsManual, jobScheduler, chunkSize};
    QString name = "Default multi core settings changed!";
    changeDefaultSettings(path,value,name);

}


void MainWindow::on_pushButton_contact_save_clicked()
{
    QString function = ui->lineEdit_PITask->text();
    QString name = ui->lineEdit_PIName->text();
    QString email = ui->lineEdit_PIEmail->text();
    QString phone = ui->lineEdit_pi_phone->text();
    QString affiliation = ui->lineEdit_pi_affiliation ->text();

    boost::property_tree::ptree c;
    c.add("contact.function", function.toStdString());
    c.add("contact.name", name.toStdString());
    c.add("contact.email", email.toStdString());
    c.add("contact.phone", phone.toStdString());
    c.add("contact.affiliation", affiliation.toStdString());

    settings_.add_child( "settings.contacts.contact", c.get_child( "contact" ) );

    std::ofstream os;
    os.open("settings.xml");
    boost::property_tree::xml_parser::write_xml(os, settings_, boost::property_tree::xml_writer_make_settings<std::string>('\t', 1));
    os.close();
    QMessageBox::information(this,"Default parameters changed","New contact added!");

}


void MainWindow::on_pushButton_contactlist_save_clicked()
{
    QTableWidget *tableWidget = ui->tableWidget_contact;
    std::vector<VieVS::ParameterSettings::Contact> contacts;


    for (int i=0; i<tableWidget->rowCount(); ++i){
        VieVS::ParameterSettings::Contact c;

        c.function = tableWidget->item(i,0)->text().toStdString();
        c.name = tableWidget->item(i,1)->text().toStdString();
        c.email = tableWidget->item(i,2)->text().toStdString();
        c.phone = tableWidget->item(i,3)->text().toStdString();
        c.affiliation = tableWidget->item(i,4)->text().toStdString();

        contacts.push_back(c);
    }

    if (settings_.get_child_optional("settings.output.contacts").is_initialized()){
        settings_.get_child("settings.output").erase("contacts");
    }

    for(const auto &any : contacts){
        boost::property_tree::ptree c;
        c.add("contact.function", any.function);
        c.add("contact.name", any.name);
        c.add("contact.email", any.email);
        c.add("contact.phone", any.phone);
        c.add("contact.affiliation", any.affiliation);

        settings_.add_child( "settings.output.contacts.contact", c.get_child( "contact" ) );
    }

    std::ofstream os;
    os.open("settings.xml");
    boost::property_tree::xml_parser::write_xml(os, settings_, boost::property_tree::xml_writer_make_settings<std::string>('\t', 1));
    os.close();
    QMessageBox::information(this,"Default contacts change","Default contacts changed!");



}





