#include "skycoveragewidget.h"
#include "ui_skycoveragewidget.h"

SkyCoverageWidget::SkyCoverageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SkyCoverageWidget)
{
    ui->setupUi(this);
}

SkyCoverageWidget::~SkyCoverageWidget()
{
    delete ui;
}

// ########################################### SKY COVERAGE ###########################################

void SkyCoverageWidget::setupSkyCoverageTemplatePlot()
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
    ui->verticalLayout_54->insertWidget(0,skyCoverageTemplateView,2);

    connect(ui->spinBox_skyCoverageTemplateRandomObservations,SIGNAL(valueChanged(int)),this,SLOT(on_pushButton_skyCoverageTemplateRandom_clicked()));

    connect(ui->influenceTimeSpinBox,SIGNAL(valueChanged(double)),this,SLOT(skyCoverageTemplate()));
    connect(ui->influenceDistanceDoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(skyCoverageTemplate()));
    connect(ui->horizontalSlider_skyCoverageMarkerSize,SIGNAL(valueChanged(int)),this,SLOT(skyCoverageTemplate()));
    connect(ui->horizontalSlider_skyCoverageMarkerDistance,SIGNAL(valueChanged(int)),this,SLOT(skyCoverageTemplate()));
    connect(ui->horizontalSlider_skyCoverageColorResultion,SIGNAL(valueChanged(int)),this,SLOT(skyCoverageTemplate()));

    connect(ui->comboBox_skyCoverageDistanceType,SIGNAL(currentIndexChanged(int)),this,SLOT(skyCoverageTemplate()));
    connect(ui->comboBox_skyCoverageTimeType,SIGNAL(currentIndexChanged(int)),this,SLOT(skyCoverageTemplate()));
    on_pushButton_skyCoverageTemplateRandom_clicked();

}

void SkyCoverageWidget::skyCoverageTemplate()
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

void SkyCoverageWidget::on_pushButton_skyCoverageTemplateRandom_clicked()
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

void SkyCoverageWidget::on_influenceTimeSpinBox_valueChanged(int arg1)
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

double SkyCoverageWidget::interpolate( QVector<double> &xData, QVector<double> &yData, double x, bool extrapolate )
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
