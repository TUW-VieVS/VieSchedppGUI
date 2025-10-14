#include "statistics.h"

Statistics::Statistics(
        QTreeWidget *itemlist_,
        QTreeWidget *hovered_,
        QLabel *hoveredTitle_,
        QVBoxLayout *layout_,
        QScrollBar *scrollBar_,
        QSpinBox *show_,
        QListWidget *statisticsCSV_,
        QCheckBox *removeMinimum_,
        QRadioButton *relative_,
        QRadioButton *absolute_,
        QLineEdit *outputPath_)
{
    itemlist = itemlist_;
    hovered = hovered_;
    hoveredTitle = hoveredTitle_;
    layout = layout_;
    scrollBar = scrollBar_;
    show = show_;
    statisticsCSV = statisticsCSV_;
    removeMinimum = removeMinimum_;
    relative = relative_;
    absolute = absolute_;
    outputPath = outputPath_;
}

void Statistics::setupStatisticView()
{
    auto hv1 = itemlist->header();
    hv1->setSectionResizeMode(QHeaderView::ResizeToContents);
    auto hv2 = hovered->header();
    hv2->setSectionResizeMode(QHeaderView::ResizeToContents);

    statisticsView = new QChartView(this);
    statisticsView->setToolTip("version comparison");
    statisticsView->setStatusTip("version comparison");
    statisticsView->setChart(new QChart());
    layout->insertWidget(0,statisticsView,1);
    scrollBar->setRange(0,0);
    scrollBar->setSingleStep(1);

    for(int i=0; i<itemlist->topLevelItemCount(); ++i){
        auto db = new QDoubleSpinBox(itemlist);
        db->setMinimum(-99);
        itemlist->setItemWidget(itemlist->topLevelItem(i),2,db);
        connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
    }


    connect(absolute,SIGNAL(toggled(bool)),this,SLOT(plotStatistics()));
    connect(removeMinimum,SIGNAL(toggled(bool)),this,SLOT(plotStatistics()));

    hoveredTitle->setStyleSheet("font-weight: bold");

    general << "#scans" << "#single source scans" << "#subnetting scans" << "#fillin-mode scans" << "#calibrator_scans" << "#observations" << "#stations" << "#sources";
    weightFactors << "weight_factor_sky-coverage"
                  << "weight_factor_number_of_observations"
                  << "weight_factor_duration"
                  << "weight_factor_average_sources"
                  << "weight_factor_average_stations"
                  << "weight_factor_average_baselines"
                  << "weight_factor_idle_time"
                  << "weight_factor_idle_time_interval"
                  << "weight_factor_closures"
                  << "weight_factor_max_closures"
                  << "weight_factor_low_declination"
                  << "weight_factor_low_declination_start_weight"
                  << "weight_factor_low_declination_full_weight"
                  << "weight_factor_low_elevation"
                  << "weight_factor_low_elevation_start_weight"
                  << "weight_factor_low_elevation_full_weight";
}

void Statistics::on_pushButton_addStatistic_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Browse to statistics.csv file", outputPath->text(),"*.csv");
    if( !path.isEmpty() ){
        if(statisticsCSV->findItems(path, Qt::MatchExactly).size()>0){
            QMessageBox::warning(this,"already visible","There is already one statistics file from this folder visible!");
            return;
        }
        statisticsCSV->insertItem(statisticsCSV->count(),path);

        reload();
    }
}


void Statistics::on_pushButton_removeStatistic_clicked()
{
    if(statisticsCSV->selectedItems().size()==1){
        QString txt = statisticsCSV->selectedItems().at(0)->text();
        int row = statisticsCSV->selectionModel()->selectedRows(0).at(0).row();
        statistics.remove(txt);
        delete statisticsCSV->item(row);

        reload();
    }
}

void Statistics::reload()
{
    // ################# read header line #################
    stations.clear();
    stations_scans.clear();
    baselines.clear();
    sources.clear();
    times.clear();
    multiScheduling.clear();
    lookupTable.clear();

    for(int i=0; i<statisticsCSV->count(); ++i){
        QString path = statisticsCSV->item(i)->text();
        QStringList tmp = path.split("/");
        QString folder = tmp.at(tmp.size()-2);
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this,"could not open file!","Error while opening:\n"+path,QMessageBox::Ok);
            return;
        }

        QTextStream in(&file);
        QString line = in.readLine();
        QStringList names = line.split(",",Qt::SkipEmptyParts);
        bool lookForMultiSchedulingParameteres = false;
        for(const auto &name : names){
            lookupTable.append(name);

            if( name.left(10) == "n_sta_obs_" ){
                QString thisName = name.mid(10);
                if(stations.indexOf(thisName) == -1){
                    stations.append(thisName);
                }
            }else if( name.left(9) == "n_bl_obs_" ){
                QString thisName = name.mid(9);
                if(baselines.indexOf(thisName) == -1){
                    baselines.append(thisName);
                }
            }else if( name.left(15) == "n_src_closures_" ){
                QString thisName = name.mid(15);
                if(sources.indexOf(thisName) == -1){
                    sources.append(thisName);
                }
                lookForMultiSchedulingParameteres = true;
            }else if( name.left(4) == "sim_") {
                continue;
            }else if( name.right(14) == "-station_scans") {
                QString tmp = name;
                tmp.replace('_',' ');
                stations_scans.append(name);
            }else if(lookForMultiSchedulingParameteres){
                if(multiScheduling.indexOf(name) == -1){
                    multiScheduling.append(name);
                }
            }
        }
    }
    //stations.sort();
    //baselines.sort();

    // ################# read data #################
    statistics.clear();
    QVector<int> counter(lookupTable.size(),0);
    for(int i=0; i<statisticsCSV->count(); ++i){
        QString path = statisticsCSV->item(i)->text();
        QStringList tmp = path.split("/");
        QString folder = tmp.at(tmp.size()-2);

        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this,"could not open file!","Error while opening:\n"+path,QMessageBox::Ok);
            return;
        }

        QTextStream in(&file);
        QString line = in.readLine();
        QStringList header = line.split(",",Qt::SkipEmptyParts);

        while (!in.atEnd()){
            QString line = in.readLine();
            QStringList split = line.split(",",Qt::SkipEmptyParts);

            int version = split.at(0).toInt();
            statistics[folder][version] = QVector<double>(lookupTable.size(), 0.0);

            for(int i = 1; i<split.count();++i){
                const QString &name = header.at(i);
                double value = split.at(i).toDouble();
                int idx = lookupTable.indexOf(name);

                if ( name.startsWith("sim_") && value == 9999){
                    value = 0;
                }

                if(idx == -1){
                    continue;
                }
                if(value != 0){
                    ++counter[idx];
                }
                statistics[folder][version][idx] = value;
            }
        }
    }
    int offset = 1 + general.size() + 5+5*stations.size() +6+6*stations.size() + 2*stations.size() + baselines.size() + 16;
    QList<char> remove;
    for (int i=offset; i<offset+sources.size(); ++i) {
        if(counter[i] > 0){
            remove.append(false);
        }else{
            remove.append(true);
        }
    }
    QList<int> removeIdx;
    int nSrc = sources.size();

    for(int i=remove.size()-1; i>=0; --i){
        if(remove[i]){
            sources.removeAt(i);
        }
    }

    int offset_sim_rep = lookupTable.indexOf("sim_repeatability_average_2d_source_coord._[mas]");
    if ( offset_sim_rep > 0 ){
        for(int i=remove.size()-1; i>=0; --i){
            if(remove[i]){
                lookupTable.removeAt(i+offset_sim_rep + 1 );
                removeIdx.append(i+offset_sim_rep + 1);
            }
        }
    }

    int offset_sim_mfe = lookupTable.indexOf("sim_mean_formal_error_average_2d_source_coord._[mas]");
    if ( offset_sim_mfe > 0 ){
        for(int i=remove.size()-1; i>=0; --i){
            if(remove[i]){
                lookupTable.removeAt(i+offset_sim_mfe + 1);
                removeIdx.append(i+offset_sim_mfe + 1);
            }
        }
    }

    for(int i=remove.size()-1; i>=0; --i){
        if(remove[i]){
            int idx = i+3*nSrc+offset;
            lookupTable.removeAt(idx);
            removeIdx.append(idx);
        }
    }
    for(int i=remove.size()-1; i>=0; --i){
        if(remove[i]){
            int idx = i+2*nSrc+offset;
            lookupTable.removeAt(idx);
            removeIdx.append(idx);
        }
    }
    for(int i=remove.size()-1; i>=0; --i){
        if(remove[i]){
            int idx = i+nSrc+offset;
            lookupTable.removeAt(idx);
            removeIdx.append(idx);
        }
    }
    for(int i=remove.size()-1; i>=0; --i){
        if(remove[i]){
            int idx = i+offset;
            lookupTable.removeAt(idx);
            removeIdx.append(idx);
        }
    }

    for(auto & any: statistics){
        for(auto & any2: any){
            for(int i : removeIdx){
                any2.removeAt(i);
            }
        }
    }
    //sources.sort();

    // ################# add items to itemlist #################
    itemlist->blockSignals(true);
    itemlist->clear();
    itemlist->addTopLevelItem(new QTreeWidgetItem(QStringList() << "general"));
    const auto &gen = itemlist->topLevelItem(0);
    gen->setCheckState(0,Qt::PartiallyChecked);
    for(const auto &any : general){

        gen->addChild(new QTreeWidgetItem(QStringList() << any));
        if(any == "#scans" || any == "#observations" || any == "#sources"){
            gen->child(gen->childCount()-1)->setCheckState(0,Qt::Checked);
        }else{
            gen->child(gen->childCount()-1)->setCheckState(0,Qt::Unchecked);
        }


        auto db = new QDoubleSpinBox(itemlist);
        db->setMinimum(-99);
        itemlist->setItemWidget(gen->child(gen->childCount()-1),2,db);
        connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
    }
    gen->setExpanded(true);

    itemlist->addTopLevelItem(new QTreeWidgetItem(QStringList() << "station"));
    const auto &sta = itemlist->topLevelItem(1);
    sta->setCheckState(0,Qt::Unchecked);
    sta->addChild(new QTreeWidgetItem(QStringList() << "scans"));
    sta->addChild(new QTreeWidgetItem(QStringList() << "obs"));
    const auto &staScans = sta->child(0);
    const auto &staObs = sta->child(1);
    staScans->setCheckState(0,Qt::Unchecked);
    staObs->setCheckState(0,Qt::Unchecked);
    for(const auto &any : stations){

        staScans->addChild(new QTreeWidgetItem(QStringList() << any));
        staScans->child(staScans->childCount()-1)->setCheckState(0,Qt::Unchecked);

        auto db1 = new QDoubleSpinBox(itemlist);
        db1->setMinimum(-99);
        itemlist->setItemWidget(staScans->child(staScans->childCount()-1),2,db1);
        connect(db1,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));

        staObs->addChild(new QTreeWidgetItem(QStringList() << any));
        staObs->child(staObs->childCount()-1)->setCheckState(0,Qt::Unchecked);

        auto db2 = new QDoubleSpinBox(itemlist);
        db2->setMinimum(-99);
        itemlist->setItemWidget(staObs->child(staObs->childCount()-1),2,db2);
        connect(db2,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
    }

    itemlist->addTopLevelItem(new QTreeWidgetItem(QStringList() << "baseline"));
    const auto &bl = itemlist->topLevelItem(2);
    bl->addChild(new QTreeWidgetItem(QStringList() << "obs"));
    bl->setCheckState(0,Qt::Unchecked);
    const auto &blObs = bl->child(0);
    blObs->setCheckState(0,Qt::Unchecked);
    for(const auto &any : baselines){
        blObs->addChild(new QTreeWidgetItem(QStringList() << any));
        blObs->child(blObs->childCount()-1)->setCheckState(0,Qt::Unchecked);

        auto db2 = new QDoubleSpinBox(itemlist);
        db2->setMinimum(-99);
        itemlist->setItemWidget(blObs->child(blObs->childCount()-1),2,db2);
        connect(db2,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
    }


    itemlist->addTopLevelItem(new QTreeWidgetItem(QStringList() << "source"));
    const auto &src = itemlist->topLevelItem(3);
    src->setCheckState(0,Qt::Unchecked);
    src->addChild(new QTreeWidgetItem(QStringList() << "scans"));
    src->addChild(new QTreeWidgetItem(QStringList() << "obs"));
    src->addChild(new QTreeWidgetItem(QStringList() << "closures"));
    src->addChild(new QTreeWidgetItem(QStringList() << "closure phases"));
    const auto &srcScans = src->child(0);
    const auto &srcObs = src->child(1);
    const auto &srcClosures = src->child(2);
    const auto &srcClosurePhases = src->child(3);
    srcScans->setCheckState(0,Qt::Unchecked);
    srcObs->setCheckState(0,Qt::Unchecked);
    srcClosures->setCheckState(0,Qt::Unchecked);
    srcClosurePhases->setCheckState(0,Qt::Unchecked);

    for(const auto &any : sources){
        srcScans->addChild(new QTreeWidgetItem(QStringList() << any));
        srcScans->child(srcScans->childCount()-1)->setCheckState(0,Qt::Unchecked);
        auto db1 = new QDoubleSpinBox(itemlist);
        db1->setMinimum(-99);
        itemlist->setItemWidget(srcScans->child(srcScans->childCount()-1),2,db1);
        connect(db1,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));


        srcObs->addChild(new QTreeWidgetItem(QStringList() << any));
        srcObs->child(srcObs->childCount()-1)->setCheckState(0,Qt::Unchecked);
        auto db2 = new QDoubleSpinBox(itemlist);
        db2->setMinimum(-99);
        itemlist->setItemWidget(srcObs->child(srcObs->childCount()-1),2,db2);
        connect(db2,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));


        srcClosures->addChild(new QTreeWidgetItem(QStringList() << any));
        srcClosures->child(srcClosures->childCount()-1)->setCheckState(0,Qt::Unchecked);
        auto db3 = new QDoubleSpinBox(itemlist);
        db3->setMinimum(-99);
        itemlist->setItemWidget(srcClosures->child(srcClosures->childCount()-1),2,db3);
        connect(db3,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));


        srcClosurePhases->addChild(new QTreeWidgetItem(QStringList() << any));
        srcClosurePhases->child(srcClosurePhases->childCount()-1)->setCheckState(0,Qt::Unchecked);
        auto db4 = new QDoubleSpinBox(itemlist);
        db4->setMinimum(-99);
        itemlist->setItemWidget(srcClosurePhases->child(srcClosurePhases->childCount()-1),2,db4);
        connect(db4,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
    }

    itemlist->addTopLevelItem(new QTreeWidgetItem(QStringList() << "weight factors"));
    const auto &wf = itemlist->topLevelItem(4);
    wf->setCheckState(0,Qt::Unchecked);
    for(const auto &any : weightFactors){

        wf->addChild(new QTreeWidgetItem(QStringList() << any.mid(14).replace("_"," ")));
        wf->child(wf->childCount()-1)->setCheckState(0,Qt::Unchecked);

        auto db = new QDoubleSpinBox(itemlist);
        db->setMinimum(-99);
        itemlist->setItemWidget(wf->child(wf->childCount()-1),2,db);
        connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
    }


    itemlist->addTopLevelItem(new QTreeWidgetItem(QStringList() << "multi scheduling"));
    const auto &ms = itemlist->topLevelItem(5);
    ms->setCheckState(0,Qt::Unchecked);
    for(QString any : multiScheduling){

        ms->addChild(new QTreeWidgetItem(QStringList() << any.replace("_"," ")));
        ms->child(ms->childCount()-1)->setCheckState(0,Qt::Unchecked);

        auto db = new QDoubleSpinBox(itemlist);
        db->setMinimum(-99);
        itemlist->setItemWidget(ms->child(ms->childCount()-1),2,db);
        connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
    }

    itemlist->addTopLevelItem(new QTreeWidgetItem(QStringList() << "stations/scan"));
    const auto &sta_scan = itemlist->topLevelItem(6);
    sta_scan->setCheckState(0,Qt::Unchecked);
    for(const auto &any : stations_scans){

        sta_scan->addChild(new QTreeWidgetItem(QStringList() << any));
        sta_scan->child(sta_scan->childCount()-1)->setCheckState(0,Qt::Unchecked);


        auto db = new QDoubleSpinBox(itemlist);
        db->setMinimum(-99);
        itemlist->setItemWidget(sta_scan->child(sta_scan->childCount()-1),2,db);
        connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
    }
    sta_scan->setExpanded(false);

    // time spend
    {
        QStringList list;
        list << "average [%]" << "observation [%]" << "preob [%]" << "slew [%]" << "idle [%]" << "field system [%]";

        itemlist->addTopLevelItem(new QTreeWidgetItem(QStringList() << "time spend"));
        const auto &t = itemlist->topLevelItem(7);
        t->setCheckState(0,Qt::Unchecked);
        for(const auto &any : list){
            t->addChild(new QTreeWidgetItem(QStringList() << any));
            t->child(t->childCount()-1)->setCheckState(0,Qt::Unchecked);
        }

        QStringList llist;
        llist << "observation" << "preob" << "slew" << "idle" << "field system";
        const auto &tavg = t->child(0);
        for(const auto &any : llist){
            tavg->addChild(new QTreeWidgetItem(QStringList() << any));
            tavg->child(tavg->childCount()-1)->setCheckState(0,Qt::Unchecked);

            auto db = new QDoubleSpinBox(itemlist);
            db->setMinimum(-99);
            itemlist->setItemWidget(tavg->child(tavg->childCount()-1),2,db);
            connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
        }


        const auto &tobs = t->child(1);
        for(const auto &any : stations){

            tobs->addChild(new QTreeWidgetItem(QStringList() << any));
            tobs->child(tobs->childCount()-1)->setCheckState(0,Qt::Unchecked);

            auto db = new QDoubleSpinBox(itemlist);
            db->setMinimum(-99);
            itemlist->setItemWidget(tobs->child(tobs->childCount()-1),2,db);
            connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
        }

        const auto &tpre = t->child(2);
        for(const auto &any : stations){

            tpre->addChild(new QTreeWidgetItem(QStringList() << any));
            tpre->child(tpre->childCount()-1)->setCheckState(0,Qt::Unchecked);

            auto db = new QDoubleSpinBox(itemlist);
            db->setMinimum(-99);
            itemlist->setItemWidget(tpre->child(tpre->childCount()-1),2,db);
            connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
        }

        const auto &tslew = t->child(3);
        for(const auto &any : stations){

            tslew->addChild(new QTreeWidgetItem(QStringList() << any));
            tslew->child(tslew->childCount()-1)->setCheckState(0,Qt::Unchecked);

            auto db = new QDoubleSpinBox(itemlist);
            db->setMinimum(-99);
            itemlist->setItemWidget(tslew->child(tslew->childCount()-1),2,db);
            connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
        }

        const auto &tidle = t->child(4);
        for(const auto &any : stations){

            tidle->addChild(new QTreeWidgetItem(QStringList() << any));
            tidle->child(tidle->childCount()-1)->setCheckState(0,Qt::Unchecked);

            auto db = new QDoubleSpinBox(itemlist);
            db->setMinimum(-99);
            itemlist->setItemWidget(tidle->child(tidle->childCount()-1),2,db);
            connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
        }

        const auto &tfield = t->child(5);
        for(const auto &any : stations){

            tfield->addChild(new QTreeWidgetItem(QStringList() << any));
            tfield->child(tfield->childCount()-1)->setCheckState(0,Qt::Unchecked);

            auto db = new QDoubleSpinBox(itemlist);
            db->setMinimum(-99);
            itemlist->setItemWidget(tfield->child(tfield->childCount()-1),2,db);
            connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
        }
    }


    // sky Coverage
    {
        QStringList skyList;
        skyList << "average" << "13 areas 30 min" << "25 areas 30 min" << "37 areas 30 min" << "13 areas 60 min" << "25 areas 60 min" << "37 areas 60 min";

        itemlist->addTopLevelItem(new QTreeWidgetItem(QStringList() << "sky-coverage score"));
        const auto &st = itemlist->topLevelItem(8);
        st->setCheckState(0,Qt::Unchecked);
        for(const auto &any : skyList){
            st->addChild(new QTreeWidgetItem(QStringList() << any));
            st->child(st->childCount()-1)->setCheckState(0,Qt::Unchecked);
        }

        QStringList skyLlist;
        skyLlist << "13 areas 30 min" << "25 areas 30 min" << "37 areas 30 min" << "13 areas 60 min" << "25 areas 60 min" << "37 areas 60 min";
        const auto &stavg = st->child(0);
        for(const auto &any : skyLlist){
            stavg->addChild(new QTreeWidgetItem(QStringList() << any));
            stavg->child(stavg->childCount()-1)->setCheckState(0,Qt::Unchecked);

            auto db = new QDoubleSpinBox(itemlist);
            db->setMinimum(-99);
            itemlist->setItemWidget(stavg->child(stavg->childCount()-1),2,db);
            connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
        }


        const auto &ta13m30 = st->child(1);
        for(const auto &any : stations){

            ta13m30->addChild(new QTreeWidgetItem(QStringList() << any));
            ta13m30->child(ta13m30->childCount()-1)->setCheckState(0,Qt::Unchecked);

            auto db = new QDoubleSpinBox(itemlist);
            db->setMinimum(-99);
            itemlist->setItemWidget(ta13m30->child(ta13m30->childCount()-1),2,db);
            connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
        }
        const auto &ta25m30 = st->child(2);
        for(const auto &any : stations){

            ta25m30->addChild(new QTreeWidgetItem(QStringList() << any));
            ta25m30->child(ta25m30->childCount()-1)->setCheckState(0,Qt::Unchecked);

            auto db = new QDoubleSpinBox(itemlist);
            db->setMinimum(-99);
            itemlist->setItemWidget(ta25m30->child(ta25m30->childCount()-1),2,db);
            connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
        }
        const auto &ta37m30 = st->child(3);
        for(const auto &any : stations){

            ta37m30->addChild(new QTreeWidgetItem(QStringList() << any));
            ta37m30->child(ta37m30->childCount()-1)->setCheckState(0,Qt::Unchecked);

            auto db = new QDoubleSpinBox(itemlist);
            db->setMinimum(-99);
            itemlist->setItemWidget(ta37m30->child(ta37m30->childCount()-1),2,db);
            connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
        }
        const auto &ta13m60 = st->child(4);
        for(const auto &any : stations){

            ta13m60->addChild(new QTreeWidgetItem(QStringList() << any));
            ta13m60->child(ta13m60->childCount()-1)->setCheckState(0,Qt::Unchecked);

            auto db = new QDoubleSpinBox(itemlist);
            db->setMinimum(-99);
            itemlist->setItemWidget(ta13m60->child(ta13m60->childCount()-1),2,db);
            connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
        }
        const auto &ta25m60 = st->child(5);
        for(const auto &any : stations){

            ta25m60->addChild(new QTreeWidgetItem(QStringList() << any));
            ta25m60->child(ta25m60->childCount()-1)->setCheckState(0,Qt::Unchecked);

            auto db = new QDoubleSpinBox(itemlist);
            db->setMinimum(-99);
            itemlist->setItemWidget(ta25m60->child(ta25m60->childCount()-1),2,db);
            connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
        }
        const auto &ta37m60 = st->child(6);
        for(const auto &any : stations){

            ta37m60->addChild(new QTreeWidgetItem(QStringList() << any));
            ta37m60->child(ta37m60->childCount()-1)->setCheckState(0,Qt::Unchecked);

            auto db = new QDoubleSpinBox(itemlist);
            db->setMinimum(-99);
            itemlist->setItemWidget(ta37m60->child(ta37m60->childCount()-1),2,db);
            connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
        }
    }

    // simulations
    {
        itemlist->addTopLevelItem(new QTreeWidgetItem(QStringList() << "simulations"));
        const auto &sim = itemlist->topLevelItem(9);
        sim->setCheckState(0,Qt::Unchecked);


        QStringList simList;
        simList << "mean formal error" << "repeatability";
        for(const auto &any : simList){
            sim->addChild(new QTreeWidgetItem(QStringList() << any));
            sim->child(sim->childCount()-1)->setCheckState(0,Qt::Unchecked);
        }

        QStringList simPara;
        simPara << "#sim" << "dUT1 [mus]" << "x pol [muas]" << "y pol [muas]" << "x nut [muas]" << "y nut [muas]" << "scale [ppb]";

        for(int i=0; i<2; ++i){
            const auto &itm = sim->child(i);

            for(const auto &any : simPara){
                itm->addChild(new QTreeWidgetItem(QStringList() << any));
                itm->child(itm->childCount()-1)->setCheckState(0,Qt::Unchecked);

                auto db = new QDoubleSpinBox(itemlist);
                db->setMinimum(-99);
                itemlist->setItemWidget(itm->child(itm->childCount()-1),2,db);
                connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
            }
            itm->addChild(new QTreeWidgetItem(QStringList() << "average 3d station coord. [mm]"));
            itm->child(itm->childCount()-1)->setCheckState(0,Qt::Unchecked);
            auto db = new QDoubleSpinBox(itemlist);
            db->setMinimum(-99);
            itemlist->setItemWidget(itm->child(itm->childCount()-1),2,db);
            connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));

            itm->addChild(new QTreeWidgetItem(QStringList() << "3d station coord. [mm]"));
            itm->child(itm->childCount()-1)->setCheckState(0,Qt::Unchecked);
            const auto &iitm = itm->child(itm->childCount()-1);
            for(const auto &sta : stations){
                iitm->addChild(new QTreeWidgetItem(QStringList() << sta));
                iitm->child(iitm->childCount()-1)->setCheckState(0,Qt::Unchecked);

                auto db = new QDoubleSpinBox(itemlist);
                db->setMinimum(-99);
                itemlist->setItemWidget(iitm->child(iitm->childCount()-1),2,db);
                connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
            }

            itm->addChild(new QTreeWidgetItem(QStringList() << "average 2d source coord. [mas]"));
            itm->child(itm->childCount()-1)->setCheckState(0,Qt::Unchecked);
            auto db2 = new QDoubleSpinBox(itemlist);
            db2->setMinimum(-99);
            itemlist->setItemWidget(itm->child(itm->childCount()-1),2,db2);
            connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));

            itm->addChild(new QTreeWidgetItem(QStringList() << "2d source coord. [mas]"));
            itm->child(itm->childCount()-1)->setCheckState(0,Qt::Unchecked);
            const auto &iiitm = itm->child(itm->childCount()-1);
            for(const auto &src : sources){
                iiitm->addChild(new QTreeWidgetItem(QStringList() << src));
                iiitm->child(iiitm->childCount()-1)->setCheckState(0,Qt::Unchecked);

                auto db = new QDoubleSpinBox(itemlist);
                db->setMinimum(-99);
                itemlist->setItemWidget(iiitm->child(iiitm->childCount()-1),2,db);
                connect(db,SIGNAL(valueChanged(double)),this,SLOT(plotStatistics()));
            }
        }
    }

    // ################# create plot #################

    plotStatistics(true);

}

void Statistics::addEmptyStatistic(int idx)
{
    for(const auto &key1: statistics.keys()){
        for(const auto &key2: statistics[key1].keys()){
            statistics[key1][key2].insert(idx,0);
        }
    }
}

void Statistics::plotStatistics(bool animation)
{

    try{

    itemlist->blockSignals(true);

    // ################# prepare brushes #################

    QVector<QColor> colors{
                QColor(31,120,180),
                QColor(51,160,44),
                QColor(227,26,28),
                QColor(255,127,0),
                QColor(106,61,154),
                QColor(177,89,40),
                QColor(166,206,227),
                QColor(178,223,138),
                QColor(251,154,153),
                QColor(253,191,111),
                QColor(202,178,214),
                QColor(255,255,153),
    };

    QVector<QBrush> brushes;
    for(int i=0; i<colors.size(); ++i){
        brushes.push_back(QBrush(colors.at(i),Qt::SolidPattern));
    }
    for(int i=0; i<colors.size(); ++i){
        brushes.push_back(QBrush(colors.at(i),Qt::BDiagPattern));
    }
    for(int i=0; i<colors.size(); ++i){
        brushes.push_back(QBrush(colors.at(i),Qt::FDiagPattern));
    }
    for(int i=0; i<colors.size(); ++i){
        brushes.push_back(QBrush(colors.at(i),Qt::DiagCrossPattern));
    }
    for(int i=0; i<colors.size(); ++i){
        brushes.push_back(QBrush(colors.at(i),Qt::HorPattern));
    }
    for(int i=0; i<colors.size(); ++i){
        brushes.push_back(QBrush(colors.at(i),Qt::VerPattern));
    }
    for(int i=0; i<colors.size(); ++i){
        brushes.push_back(QBrush(colors.at(i),Qt::CrossPattern));
    }

    // ################# create bar sets #################

    QVector<QBarSet*> barSets;
    int counter = 0;


    const auto &gen = itemlist->topLevelItem(0);

    const auto &staScans = itemlist->topLevelItem(1)->child(0);
    const auto &staObs = itemlist->topLevelItem(1)->child(1);

    const auto &blObs = itemlist->topLevelItem(2)->child(0);

    const auto &srcScans = itemlist->topLevelItem(3)->child(0);
    const auto &srcObs = itemlist->topLevelItem(3)->child(1);
    const auto &srcClosures = itemlist->topLevelItem(3)->child(2);
    const auto &srcClosurePhases = itemlist->topLevelItem(3)->child(3);

    const auto &wf = itemlist->topLevelItem(4);

    const auto &ms = itemlist->topLevelItem(5);

    const auto &sta_scans = itemlist->topLevelItem(6);

    const auto &time_avg = itemlist->topLevelItem(7)->child(0);
    const auto &time_obs = itemlist->topLevelItem(7)->child(1);
    const auto &time_preob = itemlist->topLevelItem(7)->child(2);
    const auto &time_slew = itemlist->topLevelItem(7)->child(3);
    const auto &time_idle = itemlist->topLevelItem(7)->child(4);
    const auto &time_field = itemlist->topLevelItem(7)->child(5);

    const auto &skyCov_avg = itemlist->topLevelItem(8)->child(0);
    const auto &skyCov_a13m30 = itemlist->topLevelItem(8)->child(1);
    const auto &skyCov_a25m30 = itemlist->topLevelItem(8)->child(2);
    const auto &skyCov_a37m30 = itemlist->topLevelItem(8)->child(3);
    const auto &skyCov_a13m60 = itemlist->topLevelItem(8)->child(4);
    const auto &skyCov_a25m60 = itemlist->topLevelItem(8)->child(5);
    const auto &skyCov_a37m60 = itemlist->topLevelItem(8)->child(6);

    const auto &simMeanFormalError = itemlist->topLevelItem(9)->child(0);
    const auto &simMeanFormalError_station = simMeanFormalError->child(8);
    const auto &simMeanFormalError_source = simMeanFormalError->child(10);
    const auto &simRepeatability = itemlist->topLevelItem(9)->child(1);
    const auto &simRepeatability_station = simRepeatability->child(8);
    const auto &simRepeatability_source = simRepeatability->child(10);


    int offset = 1;
    for(int i=0; i<gen->childCount(); ++i){
        const auto &child = gen->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("n_").append(child->text(0).mid(1).replace(" ","_"));
            barSets.push_back(statisticsBarSet(offset,name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }

    // time average
    for(int i=0; i<time_avg->childCount(); ++i){
        const auto &child = time_avg->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("time_average_").append(child->text(0).replace(" ","_"));
            barSets.push_back(statisticsBarSet(offset,name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    // sky coverage average
    for(int i=0; i<skyCov_avg->childCount(); ++i){
        const auto &child = skyCov_avg->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("sky-coverage_average_").append(child->text(0).replace(" ","_"));
            barSets.push_back(statisticsBarSet(offset,name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }

    // weight factors
    for(int i=0; i<wf->childCount(); ++i){
        const auto &child = wf->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("weight_factor_").append(child->text(0).replace(" ","_"));
            barSets.push_back(statisticsBarSet(offset,name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }


    // time per station
    for(int i=0; i<time_obs->childCount(); ++i){
        const auto &child = time_obs->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("time_").append(child->text(0)).append("_observation");
            barSets.push_back(statisticsBarSet(offset,name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    for(int i=0; i<time_preob->childCount(); ++i){
        const auto &child = time_preob->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("time_").append(child->text(0)).append("_preob");
            barSets.push_back(statisticsBarSet(offset,name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    for(int i=0; i<time_slew->childCount(); ++i){
        const auto &child = time_slew->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("time_").append(child->text(0)).append("_slew");
            barSets.push_back(statisticsBarSet(offset,name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    for(int i=0; i<time_idle->childCount(); ++i){
        const auto &child = time_idle->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("time_").append(child->text(0)).append("_idle");
            barSets.push_back(statisticsBarSet(offset,name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    for(int i=0; i<time_field->childCount(); ++i){
        const auto &child = time_field->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("time_").append(child->text(0)).append("_field_system");
            barSets.push_back(statisticsBarSet(offset,name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }

    // sky coverage per station
    for(int i=0; i<skyCov_a13m30->childCount(); ++i){
        const auto &child = skyCov_a13m30->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("sky-coverage_").append(child->text(0)).append("_13_areas_30_min");
            barSets.push_back(statisticsBarSet(offset,name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    for(int i=0; i<skyCov_a25m30->childCount(); ++i){
        const auto &child = skyCov_a25m30->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("sky-coverage_").append(child->text(0)).append("_25_areas_30_min");
            barSets.push_back(statisticsBarSet(offset,name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    for(int i=0; i<skyCov_a37m30->childCount(); ++i){
        const auto &child = skyCov_a37m30->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("sky-coverage_").append(child->text(0)).append("_37_areas_30_min");
            barSets.push_back(statisticsBarSet(offset,name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    for(int i=0; i<skyCov_a13m60->childCount(); ++i){
        const auto &child = skyCov_a13m60->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("sky-coverage_").append(child->text(0)).append("_13_areas_60_min");
            barSets.push_back(statisticsBarSet(offset,name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    for(int i=0; i<skyCov_a25m60->childCount(); ++i){
        const auto &child = skyCov_a25m60->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("sky-coverage_").append(child->text(0)).append("_25_areas_60_min");
            barSets.push_back(statisticsBarSet(offset,name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    for(int i=0; i<skyCov_a37m60->childCount(); ++i){
        const auto &child = skyCov_a37m60->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("sky-coverage_").append(child->text(0)).append("_37_areas_60_min");
            barSets.push_back(statisticsBarSet(offset,name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }


    for(int i=0; i<staScans->childCount(); ++i){
        const auto &child = staScans->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("n_sta_scans_").append(child->text(0));
            barSets.push_back(statisticsBarSet(offset, name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    for(int i=0; i<staObs->childCount(); ++i){
        const auto &child = staObs->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("n_sta_obs_").append(child->text(0));
            barSets.push_back(statisticsBarSet(offset, name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }

    for(int i=0; i<blObs->childCount(); ++i){
        const auto &child = blObs->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("n_bl_obs_").append(child->text(0));
            barSets.push_back(statisticsBarSet(offset, name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }

    for(int i=0; i<srcScans->childCount(); ++i){
        const auto &child = srcScans->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("n_src_scans_").append(child->text(0));
            barSets.push_back(statisticsBarSet(offset, name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    for(int i=0; i<srcObs->childCount(); ++i){
        const auto &child = srcObs->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("n_src_obs_").append(child->text(0));
            barSets.push_back(statisticsBarSet(offset, name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    for(int i=0; i<srcClosures->childCount(); ++i){
        const auto &child = srcClosures->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("n_src_closures_").append(child->text(0));
            barSets.push_back(statisticsBarSet(offset, name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    for(int i=0; i<srcClosurePhases->childCount(); ++i){
        const auto &child = srcClosurePhases->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("n_src_closure_phases_").append(child->text(0));
            barSets.push_back(statisticsBarSet(offset, name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }

    for(int i=0; i<ms->childCount(); ++i){
        const auto &child = ms->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = child->text(0);
            barSets.push_back(statisticsBarSet(offset, name.replace(" ","_")));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }

    // stations/scan
    for(int i=0; i<sta_scans->childCount(); ++i){
        const auto &child = sta_scans->child(i);
        if(child->checkState(0) == Qt::Checked){

            //QString name = child->text(0).replace(" ","_");
            QString name = child->text(0);
            barSets.push_back(statisticsBarSet(offset,name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }

    for(int i=0; i<8; ++i){
        const auto &child = simMeanFormalError->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("sim_mean_formal_error_").append(child->text(0));
            name = name.replace("#","n_");
            name = name.replace(" ","_");
            barSets.push_back(statisticsBarSet(offset, name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    for(int i=0; i<simMeanFormalError_station->childCount(); ++i){
        const auto &child = simMeanFormalError_station->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("sim_mean_formal_error_").append(child->text(0));
            barSets.push_back(statisticsBarSet(offset, name.replace(" ","_")));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    for(int i=9; i<10; ++i){
        const auto &child = simMeanFormalError->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("sim_mean_formal_error_").append(child->text(0));
            name = name.replace("#","n_");
            name = name.replace(" ","_");
            barSets.push_back(statisticsBarSet(offset, name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    for(int i=0; i<simMeanFormalError_source->childCount(); ++i){
        const auto &child = simMeanFormalError_source->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("sim_mean_formal_error_").append(child->text(0));
            barSets.push_back(statisticsBarSet(offset, name.replace(" ","_")));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }


    for(int i=0; i<8; ++i){
        const auto &child = simRepeatability->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("sim_repeatability_").append(child->text(0));
            name = name.replace("#","n_");
            name = name.replace(" ","_");
            barSets.push_back(statisticsBarSet(offset, name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    for(int i=0; i<simRepeatability_station->childCount(); ++i){
        const auto &child = simRepeatability_station->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("sim_repeatability_").append(child->text(0));
            barSets.push_back(statisticsBarSet(offset, name.replace(" ","_")));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    for(int i=9; i<10; ++i){
        const auto &child = simRepeatability->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("sim_repeatability_").append(child->text(0));
            name = name.replace("#","n_");
            name = name.replace(" ","_");
            barSets.push_back(statisticsBarSet(offset, name));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }
    for(int i=0; i<simRepeatability_source->childCount(); ++i){
        const auto &child = simRepeatability_source->child(i);
        if(child->checkState(0) == Qt::Checked){
            QString name = QString("sim_repeatability_").append(child->text(0));
            barSets.push_back(statisticsBarSet(offset, name.replace(" ","_")));
            child->setBackground(1,brushes.at(counter));

            barSets.at(barSets.count()-1)->setBrush(brushes.at(counter));
            ++counter;
            counter = counter%brushes.count();
        }else{
            child->setBackground(1,Qt::white);
        }
        ++offset;
    }



    // ################# create categories #################

    QStringList categories;
    for(const auto &key1: statistics.keys()){
        for(const auto &key2: statistics[key1].keys()){
            categories.push_back("v"+QString("%1 ").arg(key2)+key1);
        }
    }

    // ################# calculate score #################

    QVector<double>score(categories.size(),0);

    offset = 1;
    for(int i=0; i<gen->childCount(); ++i){
        const auto &child = gen->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<time_avg->childCount(); ++i){
        const auto &child = time_avg->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<skyCov_avg->childCount(); ++i){
        const auto &child = skyCov_avg->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }

    for(int i=0; i<wf->childCount(); ++i){
        const auto &child = wf->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<time_obs->childCount(); ++i){
        const auto &child = time_obs->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<time_preob->childCount(); ++i){
        const auto &child = time_preob->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<time_slew->childCount(); ++i){
        const auto &child = time_slew->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<time_idle->childCount(); ++i){
        const auto &child = time_idle->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<time_field->childCount(); ++i){
        const auto &child = time_field->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }

    // sky coverage
    for(int i=0; i<skyCov_a13m30->childCount(); ++i){
        const auto &child = skyCov_a13m30->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<skyCov_a25m30->childCount(); ++i){
        const auto &child = skyCov_a25m30->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<skyCov_a37m30->childCount(); ++i){
        const auto &child = skyCov_a37m30->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<skyCov_a13m60->childCount(); ++i){
        const auto &child = skyCov_a13m60->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<skyCov_a25m60->childCount(); ++i){
        const auto &child = skyCov_a25m60->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<skyCov_a37m60->childCount(); ++i){
        const auto &child = skyCov_a37m60->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }

    for(int i=0; i<staScans->childCount(); ++i){
        const auto &child = staScans->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<staObs->childCount(); ++i){
        const auto &child = staObs->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }

    for(int i=0; i<blObs->childCount(); ++i){
        const auto &child = blObs->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }

    for(int i=0; i<srcScans->childCount(); ++i){
        const auto &child = srcScans->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<srcObs->childCount(); ++i){
        const auto &child = srcObs->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<srcClosures->childCount(); ++i){
        const auto &child = srcClosures->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<srcClosurePhases->childCount(); ++i){
        const auto &child = srcClosurePhases->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }

    for(int i=0; i<ms->childCount(); ++i){
        const auto &child = ms->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }

    for(int i=0; i<sta_scans->childCount(); ++i){
        const auto &child = sta_scans->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }

    for(int i=0; i<8; ++i){
        const auto &child = simMeanFormalError->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<simMeanFormalError_station->childCount(); ++i){
        const auto &child = simMeanFormalError_station->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=9; i<10; ++i){
        const auto &child = simMeanFormalError->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<simMeanFormalError_source->childCount(); ++i){
        const auto &child = simMeanFormalError_source->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }

    for(int i=0; i<8; ++i){
        const auto &child = simRepeatability->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<simRepeatability_station->childCount(); ++i){
        const auto &child = simRepeatability_station->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=9; i<10; ++i){
        const auto &child = simRepeatability->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }
    for(int i=0; i<simRepeatability_source->childCount(); ++i){
        const auto &child = simRepeatability_source->child(i);
        double val = qobject_cast<QDoubleSpinBox*>(itemlist->itemWidget(child,2))->value();
        if(val!=0){
            auto data = statisticsBarSet(offset);
            for(int id = 0; id<data->count(); ++id){
                score[id] += data->at(id)*val;
            }
        }
        ++offset;
    }

    // ################# sort bars #################

    QVector<int> idx(score.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::stable_sort(idx.begin(), idx.end(),[&score](int i1, int i2) {return score[i1] > score[i2];});

    QStringList sortedCategories;
    for(int i=0; i<idx.count(); ++i){
        sortedCategories << categories.at(idx.at(i));
    }
    QBarSeries* sortedSeries = new QBarSeries();
    for(int i=0; i<barSets.count(); ++i){
        auto thisBarSet = barSets.at(i);
        QBarSet *sortedBarSet = new QBarSet(thisBarSet->label());
        sortedBarSet->setBrush(thisBarSet->brush());
        for(int j = 0; j<thisBarSet->count(); ++j){
            auto thisValue = thisBarSet->at(idx.at(j));
            *sortedBarSet << thisValue;
        }
        sortedSeries->append(sortedBarSet);
        delete(thisBarSet);
    }

    // ################# plot #################


    QChart *chart = statisticsView->chart();
    chart->removeAllSeries();
    chart->addSeries(sortedSeries);
    chart->setTitle("statistics");
    if(animation){
        chart->setAnimationOptions(QChart::SeriesAnimations);
    }else{
        chart->setAnimationOptions(QChart::NoAnimation);
    }

    chart->legend()->setVisible(false);

    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(sortedCategories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, sortedSeries);
    sortedSeries->attachAxis(chart->axisY());
    static_cast<QValueAxis *>(chart->axisY())->applyNiceNumbers();

    int showN = show->value();

    if(!sortedCategories.isEmpty()){
        QString minax = sortedCategories.at(0);
        QString maxax;
        if(sortedCategories.count()>showN){
            maxax = sortedCategories.at(showN-1);
        }else{
            maxax = sortedCategories.at(sortedCategories.size()-1);
        }
        axis->setMin(minax);
        axis->setMax(maxax);
    }


    statisticsView->setRenderHint(QPainter::Antialiasing);

    if(categories.count()>showN){
        scrollBar->setRange(0,categories.size()-showN);
        scrollBar->setSingleStep(1);
    }else{
        scrollBar->setRange(0,0);
        scrollBar->setSingleStep(1);
    }

    connect(sortedSeries,SIGNAL(hovered(bool,int,QBarSet*)),this,SLOT(statisticsHovered(bool,int,QBarSet*)));
    itemlist->blockSignals(false);

    }catch(...){
        QMessageBox::warning(this,"keep it slow!","A Error occured! Maybe because you canged too many values too fast!");
        itemlist->blockSignals(false);
    }

}

void Statistics::statisticsHovered(bool status, int index, QBarSet *barset)
{
    if (status) {

        auto axis = qobject_cast<QBarCategoryAxis*>(statisticsView->chart()->axisX());
        auto categories = axis->categories();
        QString catName = categories.at(index);
        QStringList splitCatName = catName.split(" ");
        int version = splitCatName.at(0).right(splitCatName.at(0).count()-1).toInt();
        QString name = splitCatName.at(1);
        double value = 0;

        QString fullLabel = barset->label();
        int idx = lookupTable.indexOf(fullLabel);
        QString label = fullLabel;
        if(fullLabel.left(2) == "n_"){
            label = QString("#").append(fullLabel.mid(2));
        }
        label = label.replace("sta_scans_","scans ");
        label = label.replace("sta_obs_","observations ");
        label = label.replace("bl_obs_","observations ");
        label = label.replace("src_scans_","scans ");
        label = label.replace("src_obs_","observations ");
        label = label.replace("_"," ");
        if(label.left(4) == "time"){
            label.append(" [%]");
        }
        value = statistics[name][version][idx];

        hoveredTitle->setText(label);
        hovered->clear();
        hovered->addTopLevelItem(new QTreeWidgetItem(QStringList() << "session" << name));
        hovered->addTopLevelItem(new QTreeWidgetItem(QStringList() << "version" << QString("%1").arg(version)));
        hovered->addTopLevelItem(new QTreeWidgetItem(QStringList() << "value" << QString("%1").arg(value)));
    } else {
        hoveredTitle->setText("hovered item");
        hovered->clear();
    }
}

QBarSet *Statistics::statisticsBarSet(int idx, QString name)
{

    QVector<double> v;
    for(const auto &key1: statistics.keys()){
        for(const auto &key2: statistics[key1].keys()){
            if(idx >= statistics[key1][key2].count()){
                v << 0;
            }else{
                v << statistics[key1][key2][idx];
            }
        }
    }
    if (!v.empty()){
        if(removeMinimum->isChecked()){
            double min = *std::min_element(v.begin(), v.end());
            for(int i=0; i<v.count(); ++i){
                v[i] -= min;
            }
        }

        if(relative->isChecked()){
            double max = *std::max_element(v.begin(), v.end());
            for(int i=0; i<v.count(); ++i){
                v[i] /= max;
            }
        }
    }
    QBarSet *set = new QBarSet(name);
    for(int i=0; i<v.count(); ++i){
        *set << v.at(i);
    }

    return set;
}


void Statistics::on_treeWidget_statisticGeneral_itemChanged(QTreeWidgetItem *item, int column)
{
    itemlist->blockSignals(true);

    if(item->checkState(0) == Qt::PartiallyChecked){
        return;
    }
    if(item->checkState(0) != Qt::Unchecked){
        item->setExpanded(true);
    }else{
        item->setExpanded(false);
    }
    for(int i=0; i<item->childCount(); ++i){
        item->child(i)->setCheckState(0,item->checkState(0));
        if(item->checkState(0) != Qt::Unchecked){
            item->child(i)->setExpanded(true);
        }else{
            item->child(i)->setExpanded(false);
        }
        for(int j=0; j<item->child(i)->childCount(); ++j){
            item->child(i)->child(j)->setCheckState(0,item->checkState(0));
        }
    }

    auto parent = item->parent();
    bool parchecked = false;
    bool parunchecked = false;
    if(parent){
        for(int i=0; i<parent->childCount();++i){
            if(parent->child(i)->checkState(0) == Qt::Checked){
                parchecked = true;
            }else{
                parunchecked = true;
            }
        }

        if(parchecked && parunchecked){
            parent->setCheckState(0,Qt::PartiallyChecked);
        }else if(parchecked){
            parent->setCheckState(0,Qt::Checked);
        }else if(parunchecked){
            parent->setCheckState(0,Qt::Unchecked);
        }

        auto grandfather = parent->parent();
        if(grandfather){
            if(parchecked && parunchecked){
                grandfather->setCheckState(0,Qt::PartiallyChecked);
            }else{
                bool grandfatherChecked = false;
                bool grandfatherUnchecked = false;

                for(int i=0; i<grandfather->childCount();++i){
                    if(grandfather->child(i)->checkState(0) == Qt::PartiallyChecked){
                        grandfatherChecked = true;
                        grandfatherUnchecked = true;
                        break;
                    }else if(grandfather->child(i)->checkState(0) == Qt::Checked){
                        grandfatherChecked = true;
                    }else{
                        grandfatherUnchecked = true;
                    }
                }

                if(grandfatherChecked && grandfatherUnchecked){
                    grandfather->setCheckState(0,Qt::PartiallyChecked);
                }else if(grandfatherChecked){
                    grandfather->setCheckState(0,Qt::Checked);
                }else if(grandfatherUnchecked){
                    grandfather->setCheckState(0,Qt::Unchecked);
                }
            }
        }
    }


    itemlist->blockSignals(false);

    plotStatistics(false);
}


void Statistics::on_horizontalScrollBar_statistics_valueChanged(int value)
{

    statisticsView->chart()->setAnimationOptions(QChart::NoAnimation);
    hoveredTitle->setText("hovered item");
    hovered->clear();

    auto axis = qobject_cast<QBarCategoryAxis*>(statisticsView->chart()->axisX());
    auto categories = axis->categories();
    if(!categories.isEmpty()){
        QString min = categories.at(value);
        QString max = categories.at(value+show->value()-1);
        axis->setMin(min);
        axis->setMax(max);
        axis->setMin(min);
    }
}

void Statistics::on_spinBox_statistics_show_valueChanged(int arg1)
{
    hoveredTitle->setText("hovered item");
    hovered->clear();

    plotStatistics(false);
}
