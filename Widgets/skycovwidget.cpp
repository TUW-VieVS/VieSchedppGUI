#include "skycovwidget.h"
#include "ui_skycovwidget.h"

SkyCovWidget::SkyCovWidget(QStandardItemModel *stations, QWidget *parent) :
    QWidget(parent),
    stations_{stations},
    ui(new Ui::SkyCovWidget)
{
    ui->setupUi(this);
    setupSkyCoverageTemplatePlot();
    ui->tableWidget_sky_cov->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    //ui->tableWidget_sky_cov->setVisible(false);
    ui->tableWidget_stations->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    //ui->tableWidget_stations->setVisible(false);
    //ui->groupBox_example->setVisible(false);
}

SkyCovWidget::~SkyCovWidget()
{
    delete ui;
}

void SkyCovWidget::setTime(int time)
{
    ui->influenceTimeSpinBox->setValue(time);
}

void SkyCovWidget::setDistance(int dist)
{
    ui->influenceTimeSpinBox->setValue(dist);
}

void SkyCovWidget::setTimeType(QString type)
{
    ui->comboBox_skyCoverageTimeType->setCurrentText(type);
}

void SkyCovWidget::setDistanceType(QString type)
{
    ui->comboBox_skyCoverageDistanceType->setCurrentText(type);
}

// ######################################################################

boost::property_tree::ptree SkyCovWidget::toXML()
{
    const auto *tsta = ui->tableWidget_stations;
    const auto *t = ui->tableWidget_sky_cov;
    bool allSame = ui->radioButton_distance->isChecked();

    boost::property_tree::ptree tree;
    if ( allSame ){
        tree.add("influenceDistance", qobject_cast<QDoubleSpinBox *>(t->cellWidget(0,2))->value());
        tree.add("influenceInterval", qobject_cast<QSpinBox *>(t->cellWidget(0,3))->value());
        tree.add("maxTwinTelecopeDistance", ui->spinBox_distance->value());
        tree.add("interpolationDistance", qobject_cast<QComboBox *>(t->cellWidget(0,4))->currentText().toStdString());
        tree.add("interpolationTime", qobject_cast<QComboBox *>(t->cellWidget(0,5))->currentText().toStdString());
    } else {
        boost::property_tree::ptree skytree;
        for(int i = 0; i<t->rowCount(); ++i){
            boost::property_tree::ptree sky;
            if ( qobject_cast<QSpinBox *>(t->cellWidget(i,1))->value() == 0){
                continue;
            }
            QString id = qobject_cast<QSpinBox *>(t->cellWidget(i,0))->text();
            sky.add("<xmlattr>.ID", id.toStdString());
            boost::property_tree::ptree statree;
            for(int i = 0; i<tsta->rowCount(); ++i){
                QString name = tsta->item(i,0)->text();
                QString txt = qobject_cast<QSpinBox *>(tsta->cellWidget(i,1))->text();
                if( txt == id ){
                    statree.add("station", name.toStdString());
                }
            }
            sky.add_child("stations", statree);

            sky.add("influence_distance", qobject_cast<QDoubleSpinBox *>(t->cellWidget(i,2))->value());
            sky.add("influence_time", qobject_cast<QSpinBox *>(t->cellWidget(i,3))->value());
            sky.add("distance_function", qobject_cast<QComboBox *>(t->cellWidget(i,4))->currentText().toStdString());
            sky.add("time_function", qobject_cast<QComboBox *>(t->cellWidget(i,5))->currentText().toStdString());
            skytree.add_child("skyCoverage", sky);
        }
        tree.add_child("individual", skytree);
    }
    return tree;
}

void SkyCovWidget::fromXML(const boost::property_tree::ptree &tree)
{
    QTableWidget *tsta = ui->tableWidget_stations;
    QTableWidget *t = ui->tableWidget_sky_cov;

    if ( tree.get_child_optional("individual").is_initialized() ){
        ui->radioButton_manually->setChecked(true);
        const auto &indtree = tree.get_child("individual");
        for( const auto &any : indtree){
            QString name = QString::fromStdString(any.second.get("<xmlattr>.ID",""));
            int id_ = AlphabetSpinnBox::qstr2int(name);

            for (int i = 0; i<tsta->rowCount(); ++i){
                QString rowName = AlphabetSpinnBox::int2qstr(qobject_cast<QSpinBox *>(t->cellWidget(i,0))->value());
                if (rowName == name){
                    double dist = any.second.get<double>("influence_distance");
                    int time = any.second.get<int>("influence_time");;
                    QString distType = QString::fromStdString(any.second.get("distance_function",""));
                    QString timeType = QString::fromStdString(any.second.get("time_function",""));

                    qobject_cast<QDoubleSpinBox *>(t->cellWidget(i,2))->setValue(dist);
                    qobject_cast<QSpinBox *>(t->cellWidget(i,3))->setValue(time);
                    qobject_cast<QComboBox *>(t->cellWidget(i,4))->setCurrentText(distType);
                    qobject_cast<QComboBox *>(t->cellWidget(i,5))->setCurrentText(timeType);
                    break;
                }
            }
            for ( const auto & stas : any.second.get_child("stations")){
                QString name = QString::fromStdString(stas.second.data());
                for ( int i =0; i<tsta->rowCount();++i){
                    QString sta = tsta->item(i,0)->text();
                    if ( name == sta){
                        qobject_cast<QSpinBox *>(tsta->cellWidget(i,1))->setValue(id_);
                        break;
                    }
                }
            }
        }
    }else{
        ui->radioButton_distance->setChecked(true);
        ui->influenceDistanceDoubleSpinBox->setValue(tree.get("influenceDistance",30.0));
        ui->influenceTimeSpinBox->setValue(tree.get("influenceInterval",3600));
        ui->spinBox_distance->setValue(tree.get("maxTwinTelecopeDistance",0));
        ui->comboBox_skyCoverageDistanceType->setCurrentText(QString::fromStdString(tree.get("interpolationDistance","cosine")));
        ui->comboBox_skyCoverageTimeType->setCurrentText(QString::fromStdString(tree.get("interpolationTime","cosine")));
    }
}

void SkyCovWidget::addStations(QStandardItem *)
{
    setup();
}

// ########################################### SKY COVERAGE ###########################################

void SkyCovWidget::setupSkyCoverageTemplatePlot()
{
    plotSkyCoverageTemplate = true;
    skyCoverageTemplateView = new QChartView(this);
    skyCoverageTemplateView->setStatusTip("sky-coverage example");
    skyCoverageTemplateView->setToolTip("sky-coverage example");

    QPolarChart *chart = new QPolarChart();

    QValueAxis *angularAxis = new QValueAxis();
    angularAxis->setTickCount(13);
    angularAxis->setLabelFormat("%d");
    angularAxis->setShadesVisible(true);
    angularAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
    chart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);
    angularAxis->setRange(0, 361);


    QValueAxis *radialAxis = new QValueAxis();
    radialAxis->setTickCount(10);
    radialAxis->setLabelFormat(" ");
    chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);
    radialAxis->setRange(0, 90);


    chart->legend()->setVisible(false);
    chart->setTitle("possible sky-coverage example");
    skyCoverageTemplateView->setChart(chart);
    skyCoverageTemplateView->setRenderHint(QPainter::Antialiasing);
    ui->verticalLayout_54->insertWidget(0,skyCoverageTemplateView,1);

    connect(ui->spinBox_skyCoverageTemplateRandomObservations,SIGNAL(valueChanged(int)),this,SLOT(on_pushButton_skyCoverageTemplateRandom_clicked()));

    connect(ui->influenceTimeSpinBox,SIGNAL(valueChanged(int)),this,SLOT(skyCoverageTemplate()));
    connect(ui->influenceTimeSpinBox,SIGNAL(valueChanged(int)),this,SLOT(updateAllTime()));
    connect(ui->influenceDistanceDoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(skyCoverageTemplate()));
    connect(ui->influenceDistanceDoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(updateAllDist()));

    connect(ui->horizontalSlider_skyCoverageMarkerSize,SIGNAL(valueChanged(int)),this,SLOT(skyCoverageTemplate()));
    connect(ui->horizontalSlider_skyCoverageMarkerDistance,SIGNAL(valueChanged(int)),this,SLOT(skyCoverageTemplate()));
    connect(ui->horizontalSlider_skyCoverageColorResultion,SIGNAL(valueChanged(int)),this,SLOT(skyCoverageTemplate()));

    connect(ui->comboBox_skyCoverageDistanceType,SIGNAL(currentIndexChanged(int)),this,SLOT(skyCoverageTemplate()));
    connect(ui->comboBox_skyCoverageDistanceType,SIGNAL(currentIndexChanged(int)),this,SLOT(updateAllDistType()));
    connect(ui->comboBox_skyCoverageTimeType,SIGNAL(currentIndexChanged(int)),this,SLOT(skyCoverageTemplate()));
    connect(ui->comboBox_skyCoverageTimeType,SIGNAL(currentIndexChanged(int)),this,SLOT(updateAllTimeType()));
    on_pushButton_skyCoverageTemplateRandom_clicked();

}

void SkyCovWidget::skyCoverageTemplate()
{
    if(plotSkyCoverageTemplate){
        auto chart = skyCoverageTemplateView->chart();
        chart->removeAllSeries();

        QLineSeries *upper = new QLineSeries();
        QLineSeries *lower = new QLineSeries();
        double minElevation = 5;
        for(int az=0; az<=365; az+=5){
            upper->append(az,90);
            lower->append(az,90-minElevation);
        }

        QAreaSeries *area = new QAreaSeries();
        area->setName("minimum elevation");
        area->setUpperSeries(upper);
        area->setLowerSeries(lower);
        area->setBrush(QBrush(Qt::gray));
        area->setOpacity(0.7);

        chart->addSeries(area);

        area->attachAxis(chart->axisX());
        area->attachAxis(chart->axisY());

        QVector<double> V{0.00,0.05,0.10,0.15,0.20,0.25,0.30,0.35,0.40,0.45,0.50,0.55,0.60,0.65,0.70,0.75,0.80,0.85,0.90,0.95,1.00};
        QVector<double> R{62,68,71,72,67,52,45,37,28,4,18,48,72,113,159,200,234,254,250,245,249};
        QVector<double> G{38,51,67,85,103,122,140,156,170,182,190,197,203,205,201,193,186,193,212,232,251};
        QVector<double> B{168,204,231,246,253,253,243,231,223,206,185,162,134,100,66,41,48,58,46,37,21};
        int nColor = ui->horizontalSlider_skyCoverageColorResultion->value();
        double dist = 1.0/(nColor-1);

        QVector<double> Rq;
        QVector<double> Gq;
        QVector<double> Bq;
        for(int i=0; i<nColor; ++i){
            double vq = i*dist;
            Rq.append(interpolate(V,R,vq,false));
            Gq.append(interpolate(V,G,vq,false));
            Bq.append(interpolate(V,B,vq,false));
        }


        QVector<QScatterSeries*> ss;
        for(int i=0;i<nColor;++i){
            QScatterSeries *tss = new QScatterSeries();
            tss->setMarkerShape(QScatterSeries::MarkerShapeCircle);
            tss->setMarkerSize(ui->horizontalSlider_skyCoverageMarkerSize->value());
            tss->setBrush(QBrush(QColor(Rq.at(i),Gq.at(i),Bq.at(i))));
            tss->setBorderColor(QColor(Rq.at(i),Gq.at(i),Bq.at(i)));
            ss.append(tss);
        }

        double d = (double)ui->horizontalSlider_skyCoverageMarkerDistance->value()/10;
        for (double el = 0; el <= 90; el+=d) {
            if(el<= minElevation){
                continue;
            }
            double zd = 90-el;

            double deltaAz;
            if(el!=90){
                deltaAz = d/qCos(qDegreesToRadians(el));
            }else{
                deltaAz = 360;
            }

            for (double az = 0; az < 360; az+=deltaAz) {

                double score = 1;
                for(int i=0; i<obsTime.count(); ++i){
                    int deltaTime = obsTime.at(0)-obsTime.at(i);

                    double el1 = qDegreesToRadians(el);
                    double el2 = qDegreesToRadians(obsEl.at(i));
                    double az1 = qDegreesToRadians(az);
                    double az2 = qDegreesToRadians(obsAz.at(i));

                    double tmp = (qSin(el1) * qSin(el2) + qCos(el1) * qCos(el2) * qCos(az2-az1));
                    double deltaDistance = qRadiansToDegrees(qAcos(tmp));
                    double scoreDistance;
                    double scoreTime;

                    if(deltaDistance >= ui->influenceDistanceDoubleSpinBox->value()){
                        scoreDistance = 0;
                    }else if(ui->comboBox_skyCoverageDistanceType->currentText() == "cosine"){
                        scoreDistance = .5+.5*qCos(deltaDistance * M_PI / ui->influenceDistanceDoubleSpinBox->value());
                    }else if(ui->comboBox_skyCoverageDistanceType->currentText() == "linear"){
                        scoreDistance = 1-deltaDistance/ui->influenceDistanceDoubleSpinBox->value();
                    }else{
                        scoreDistance = 1;
                    }

                    if(deltaTime >= ui->influenceTimeSpinBox->value()){
                        scoreTime = 0;
                    }else if(ui->comboBox_skyCoverageTimeType->currentText() == "cosine"){
                        scoreTime = .5+.5*qCos(deltaTime * M_PI / ui->influenceTimeSpinBox->value());
                    }else if(ui->comboBox_skyCoverageTimeType->currentText() == "linear"){
                        scoreTime = 1-(double)deltaTime/(double)ui->influenceTimeSpinBox->value();
                    }else{
                        scoreTime = 1;
                    }

                    double thisScore = 1-(scoreDistance*scoreTime);
                    if(thisScore<score){
                        score=thisScore;
                    }
                }

                int idx = score*(nColor-1);
                ss.at(idx)->append(az,zd);
            }
        }

        for(int i=nColor-1; i>=0; --i){
            chart->addSeries(ss.at(i));
            ss.at(i)->attachAxis(chart->axisX());
            ss.at(i)->attachAxis(chart->axisY());
        }

        QScatterSeries *obs = new QScatterSeries();
        obs->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        obs->setMarkerSize(12);
        obs->setBrush(QBrush(Qt::red));
        obs->setBorderColor(Qt::red);
        for(int i=0;i<obsTime.count();++i){
            obs->append(obsAz.at(i),90-obsEl.at(i));
        }

        chart->addSeries(obs);
        obs->attachAxis(chart->axisX());
        obs->attachAxis(chart->axisY());
    }
}

void SkyCovWidget::on_pushButton_skyCoverageTemplateRandom_clicked()
{
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
    obsAz.clear();
    obsEl.clear();
    obsTime.clear();
    double minElevation = 5;
    int nobs = ui->spinBox_skyCoverageTemplateRandomObservations->value() * (double)ui->influenceTimeSpinBox->value()/3600.;

    for(int i=0; i<nobs; ++i){
        obsAz.append(qrand() % ((360 + 1) - 0) + 0);

        double thisEl;
        double rn = (double)(qrand() % ((100 + 1) - 0) + 0);

        if(rn<58){
            thisEl = qrand() % ((40 + 1) - (int)minElevation) + (int)minElevation;
        }else{
            double u = (double)(qrand() % ((1000 + 1) - 0) + 0);
            u = u/1000;
            thisEl = 90-qSqrt((1-u)*(90-40)*(90-40));
        }
        obsEl.append(thisEl);
        obsTime.append((nobs-i)*ui->influenceTimeSpinBox->value()/nobs);
    }
    skyCoverageTemplate();
}

void SkyCovWidget::on_influenceTimeSpinBox_valueChanged(int arg1)
{
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
    obsTime.clear();
    double minElevation = 5;
    int nobs = ui->spinBox_skyCoverageTemplateRandomObservations->value() * (double)ui->influenceTimeSpinBox->value()/3600.;

    if(nobs>obsAz.count()){
        for(int i=obsAz.count(); i<nobs; ++i){
            obsAz.append(qrand() % ((360 + 1) - 0) + 0);
            obsEl.append(qrand() % ((90 + 1) - (int)minElevation) + (int)minElevation);
        }
    }else{
        for(int i=nobs; i<obsAz.count(); ++i){
            obsAz.remove(obsAz.count()-1);
            obsEl.remove(obsEl.count()-1);
        }
    }

    for(int i=0; i<nobs; ++i){
        obsTime.append((nobs-i)*ui->influenceTimeSpinBox->value()/nobs);
    }
    skyCoverageTemplate();

}

double SkyCovWidget::interpolate( QVector<double> &xData, QVector<double> &yData, double x, bool extrapolate )
{
   int size = xData.size();

   int i = 0;
   if ( x >= xData[size - 2] )
   {
      i = size - 2;
   }
   else
   {
      while ( x > xData[i+1] ) i++;
   }
   double xL = xData[i], yL = yData[i], xR = xData[i+1], yR = yData[i+1];
   if ( !extrapolate )
   {
      if ( x < xL ) yR = yL;
      if ( x > xR ) yL = yR;
   }

   double dydx = ( yR - yL ) / ( xR - xL );
   return yL + dydx * ( x - xL );
}

void SkyCovWidget::updateCounter()
{
    int n = stations_->rowCount();
    QMap<int, QString> id2station;
    for (int i =0; i< 0; ++i) {
        id2station[i] = "";
    }

    QVector<int>counter(n, 0);
    for( int i = 0; i<n; ++i){
        QSpinBox *sp = qobject_cast<QSpinBox *>(ui->tableWidget_stations->cellWidget(i,1));
        int id = sp->value();
        id2station[id].append(ui->tableWidget_stations->item(i,0)->text()).append(" ");
        ++counter[id];
    }
    for( int i = 0; i<n; ++i){
        QSpinBox *sp = qobject_cast<QSpinBox *>(ui->tableWidget_stations->cellWidget(i,1));
        int id = sp->value();
        int val = counter[id];

        QPalette pal = sp->palette();
        if( val > 1 ){
            pal.setColor(sp->backgroundRole(), Qt::green);
        }else{
            pal.setColor(sp->backgroundRole(), Qt::white);
        }
        sp->setPalette(pal);
    }


    for( int i = 0; i<n; ++i){
        int val = counter[i];
        QSpinBox *sp = qobject_cast<QSpinBox *>(ui->tableWidget_sky_cov->cellWidget(i,1));
        sp->setValue(val);

        QSpinBox *id = qobject_cast<QSpinBox *>(ui->tableWidget_sky_cov->cellWidget(i,0));
        QPalette pal = id->palette();

        if ( val > 1){
           pal.setColor(id->backgroundRole(), Qt::green);
        } else if (val == 1) {
            pal.setColor(id->backgroundRole(), Qt::white);
        } else{
            pal.setColor(id->backgroundRole(), Qt::red);
        }
        id->setPalette(pal);

        ui->tableWidget_sky_cov->item(i,6)->setText(id2station[i]);
    }
}

void SkyCovWidget::updateAllTime()
{
    int n = stations_->rowCount();
    for( int i = 0; i<n; ++i){
        qobject_cast<QSpinBox *>(ui->tableWidget_sky_cov->cellWidget(i,3))->setValue(ui->influenceTimeSpinBox->value());
    }
}

void SkyCovWidget::updateAllDist()
{
    int n = stations_->rowCount();
    for( int i = 0; i<n; ++i){
        qobject_cast<QDoubleSpinBox *>(ui->tableWidget_sky_cov->cellWidget(i,2))->setValue(ui->influenceDistanceDoubleSpinBox->value());
    }
}

void SkyCovWidget::updateAllTimeType()
{
    int n = stations_->rowCount();
    for( int i = 0; i<n; ++i){
        qobject_cast<QComboBox *>(ui->tableWidget_sky_cov->cellWidget(i,5))->setCurrentText(ui->comboBox_skyCoverageTimeType->currentText());
    }
}

void SkyCovWidget::updateAllDistType()
{
    int n = stations_->rowCount();
    for( int i = 0; i<n; ++i){
        qobject_cast<QComboBox *>(ui->tableWidget_sky_cov->cellWidget(i,4))->setCurrentText(ui->comboBox_skyCoverageDistanceType->currentText());
    }
}

void SkyCovWidget::setup()
{
    int n = stations_->rowCount();
    ui->tableWidget_stations->setRowCount(n);
    ui->tableWidget_sky_cov->setRowCount(n);

    for( int i = 0; i<n; ++i){
        QString name = stations_->item(i,0)->text();
        QTableWidgetItem *itm = new QTableWidgetItem(QIcon(":/icons/icons/station.png"), name);
        ui->tableWidget_stations->setItem(i,0,itm);

        AlphabetSpinnBox *sp = new AlphabetSpinnBox();
        sp->setRange(0,n-1);
        sp->setValue(i);
        connect(sp, SIGNAL(valueChanged(int)), this, SLOT(updateCounter()));
        ui->tableWidget_stations->setCellWidget(i,1,sp);
    }

    for ( int i = 0; i<n; ++i){
        AlphabetSpinnBox *sp = new AlphabetSpinnBox();
        sp->setRange(0,n-1);
        sp->setValue(i);
        sp->setReadOnly(true);
        sp->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
        ui->tableWidget_sky_cov->setCellWidget(i,0,sp);

        QSpinBox *counter = new QSpinBox();
        counter->setReadOnly(true);
        counter->setValue(1);
        counter->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
        ui->tableWidget_sky_cov->setCellWidget(i,1,counter);

        QDoubleSpinBox *dist = new QDoubleSpinBox();
        dist->setSuffix(" [deg]");
        dist->setRange(ui->influenceDistanceDoubleSpinBox->minimum(), ui->influenceDistanceDoubleSpinBox->maximum());
        dist->setValue(ui->influenceDistanceDoubleSpinBox->value());
        dist->setSingleStep(ui->influenceDistanceDoubleSpinBox->singleStep());
        ui->tableWidget_sky_cov->setCellWidget(i,2,dist);

        QSpinBox *time = new QSpinBox();
        time->setSuffix(" [s]");
        time->setRange(ui->influenceTimeSpinBox->minimum(), ui->influenceTimeSpinBox->maximum());
        time->setValue(ui->influenceTimeSpinBox->value());
        time->setSingleStep(ui->influenceTimeSpinBox->singleStep());
        ui->tableWidget_sky_cov->setCellWidget(i,3,time);

        QComboBox *cdist = new QComboBox();
        for( int j = 0; j < ui->comboBox_skyCoverageDistanceType->count(); ++j){
            cdist->addItem(ui->comboBox_skyCoverageDistanceType->itemText(j));
        }
        cdist->setCurrentText(ui->comboBox_skyCoverageDistanceType->currentText());
        ui->tableWidget_sky_cov->setCellWidget(i,4,cdist);

        QComboBox *ctime = new QComboBox();
        for( int j = 0; j < ui->comboBox_skyCoverageTimeType->count(); ++j){
            ctime->addItem(ui->comboBox_skyCoverageTimeType->itemText(j));
        }
        ctime->setCurrentText(ui->comboBox_skyCoverageTimeType->currentText());
        ui->tableWidget_sky_cov->setCellWidget(i,5,ctime);

        ui->tableWidget_sky_cov->setItem(i,6,new QTableWidgetItem(""));
    }
}

void SkyCovWidget::on_groupBox_3_toggled(bool arg1)
{
    ui->groupBox_example->setVisible(arg1);
}
