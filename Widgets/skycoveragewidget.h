#ifndef SKYCOVERAGEWIDGET_H
#define SKYCOVERAGEWIDGET_H

#include <QWidget>
#include "Utility/chartview.h"
#include <QtMath>
#include <QTime>

#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPolarChart>
#include <QtCharts/QAreaSeries>

namespace Ui {
class SkyCoverageWidget;
}

class SkyCoverageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SkyCoverageWidget(QWidget *parent = nullptr);
    ~SkyCoverageWidget();

    void setPlot(bool flag){
        plotSkyCoverageTemplate = flag;
    }

    void setTime(int time){
        ui->influenceTimeSpinBox->setValue(time);
    }
    void setDistance(int dist){
        ui->influenceTimeSpinBox->setValue(dist);
    }
    void setTimeType(QString type){
        ui->comboBox_skyCoverageTimeType->setText(type);
    }
    void setDistanceType(QString type){
        ui->comboBox_skyCoverageDistanceType->setText(type);
    }


private:
    QChartView *skyCoverageTemplateView;
    bool plotSkyCoverageTemplate;

    QVector<double> obsAz;
    QVector<double> obsEl;
    QVector<int> obsTime;


    Ui::SkyCoverageWidget *ui;

    void setupSkyCoverageTemplatePlot();

    void skyCoverageTemplate();

    void on_pushButton_skyCoverageTemplateRandom_clicked();

    void on_influenceTimeSpinBox_valueChanged(int arg1);

    double interpolate( QVector<double> &xData, QVector<double> &yData, double x, bool extrapolate=false );

};

#endif // SKYCOVERAGEWIDGET_H
