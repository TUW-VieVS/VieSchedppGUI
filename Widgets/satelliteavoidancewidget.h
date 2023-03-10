#ifndef SATELLITEAVOIDANCEWIDGET_H
#define SATELLITEAVOIDANCEWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <boost/property_tree/ptree.hpp>

namespace Ui {
class SatelliteAvoidanceWidget;
}

class SatelliteAvoidanceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SatelliteAvoidanceWidget(QWidget *parent = nullptr);
    ~SatelliteAvoidanceWidget();
    QPushButton *save_para;

    boost::property_tree::ptree toXML();
    void fromXML(const boost::property_tree::ptree &tree);

private slots:
    void on_pushButton_save_clicked();


signals:
    void update_settings(QStringList path, QStringList value, QString name);

private:
    Ui::SatelliteAvoidanceWidget *ui;
};

#endif // SATELLITEAVOIDANCEWIDGET_H
