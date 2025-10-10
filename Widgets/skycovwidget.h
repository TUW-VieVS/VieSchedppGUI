#ifndef SKYCOVWIDGET_H
#define SKYCOVWIDGET_H

#include <QWidget>
#include "Utility/chartview.h"
#include <QtMath>
#include <QTime>
#include "boost/property_tree/ptree.hpp"

#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPolarChart>
#include <QtCharts/QAreaSeries>
#include <QStandardItem>
#include <QTableWidgetItem>
#include <QSpinBox>

namespace Ui {
class SkyCovWidget;
}

class SkyCovWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SkyCovWidget(QStandardItemModel *stations, QWidget *parent = nullptr);
    ~SkyCovWidget();

    void setPlot(bool flag){
        plotSkyCoverageTemplate = flag;
    }

    void setTime(int time);
    void setDistance(int dist);
    void setTimeType(QString type);
    void setDistanceType(QString type);

    boost::property_tree::ptree toXML();
    void fromXML(const boost::property_tree::ptree &tree);

    void setBlock(bool flag){
        block = flag;
    }

public slots:
    void addStations(QStandardItem * = nullptr);

private slots:
    void setupSkyCoverageTemplatePlot();

    void skyCoverageTemplate();

    void on_pushButton_skyCoverageTemplateRandom_clicked();

    void on_influenceTimeSpinBox_valueChanged(int arg1);

    double interpolate( QVector<double> &xData, QVector<double> &yData, double x, bool extrapolate=false );

    void updateCounter();

    void updateAllTime();

    void updateAllDist();

    void updateAllTimeType();

    void updateAllDistType();

    void on_groupBox_3_toggled(bool arg1);

private:
    bool block = false;
    QChartView *skyCoverageTemplateView;
    bool plotSkyCoverageTemplate = true;
    QStandardItemModel *stations_;

    QVector<double> obsAz;
    QVector<double> obsEl;
    QVector<int> obsTime;

    Ui::SkyCovWidget *ui;

    void setup();

};

class AlphabetSpinnBox : public QSpinBox{


public:
    static int qstr2int(QString text){
        QChar c = text[0];
        if (c.isUpper()){
            return c.unicode() - QChar('A').unicode();
        }else if (c.isLower()){
            return 26+c.unicode() - QChar('a').unicode();
        }else{
            return text.toInt();
        }
    }
    static QString int2qstr(int val){
        if(val<26){
            return QString(QChar('A'+val));
        }else if(val < 2*26){
            return QString(QChar('a'+(val-26)));
        }else{
            return QString::number(val);
        }
    }

    QString textFromValue(int val) const override{
        return int2qstr(val);
    }

    int valueFromText(const QString &text) const override{
        return qstr2int(text);
    }
};

#endif // SKYCOVWIDGET_H
