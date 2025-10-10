#ifndef SITEWIDGET_H
#define SITEWIDGET_H

#include <QWidget>
#include <QStandardItem>
#include <QTableWidgetItem>
#include <QSpinBox>
#include "boost/property_tree/ptree.hpp"
#include <QtMath>

namespace Ui {
class SiteWidget;
}

class SiteWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SiteWidget(QStandardItemModel *stations, QWidget *parent = nullptr);
    ~SiteWidget();
    boost::property_tree::ptree toXML();
    void fromXML(const boost::property_tree::ptree &tree);
    void setBlock(bool flag){
        block = flag;
    }

public slots:
    void addStations(QStandardItem * = nullptr);
    void updateCounter();

private slots:
    void on_pushButton_clicked();

private:
    bool block = false;
    QStandardItemModel *stations_;

    Ui::SiteWidget *ui;
    void setup();
};

class AlphabetSpinnBoxSites : public QSpinBox{
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
            return QString(QChar('A' + val));
        } else if(val < 2*26){
            return QString(QChar('a' + (val - 26)));
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

#endif // SITEWIDGET_H
