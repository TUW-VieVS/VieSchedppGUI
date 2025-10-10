#ifndef STATISTICS_H
#define STATISTICS_H

#include <QTreeWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QHeaderView>
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QTextStream>
#include <QtCharts>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

class Statistics : public QWidget
{
    Q_OBJECT
public:
    Statistics(
            QTreeWidget *itemlist,
            QTreeWidget *hovered,
            QLabel *hoveredTitle,
            QVBoxLayout *layout,
            QScrollBar *scrollBar,
            QSpinBox *show,
            QListWidget *statisticsCSV,
            QCheckBox *removeMinimum,
            QRadioButton *relative,
            QRadioButton *absolute,
            QLineEdit *outputPath);

public slots:
    void setupStatisticView();

    void on_pushButton_addStatistic_clicked();

    void on_pushButton_removeStatistic_clicked();

    void on_treeWidget_statisticGeneral_itemChanged(QTreeWidgetItem *item, int column);

    void on_horizontalScrollBar_statistics_valueChanged(int value);

    void on_spinBox_statistics_show_valueChanged(int arg1);

private:

    QTreeWidget *itemlist;
    QTreeWidget *hovered;
    QLabel *hoveredTitle;
    QVBoxLayout *layout;
    QScrollBar *scrollBar;
    QSpinBox *show;
    QListWidget *statisticsCSV;
    QCheckBox *removeMinimum;
    QRadioButton *relative;
    QRadioButton *absolute;
    QLineEdit *outputPath;
    QChartView *statisticsView;

    QStringList general;
    QStringList stations_scans;
    QStringList weightFactors;
    QStringList stations;
    QStringList baselines;
    QStringList sources;
    QStringList times;
    QStringList multiScheduling;
    QStringList lookupTable;
    QMap<QString, QMap<int, QVector< double >>> statistics;

    void addEmptyStatistic(int idx);

    QBarSet* statisticsBarSet(int idx, QString name="");

    void reload();

private slots:
    void plotStatistics(bool animation = false);

    void statisticsHovered(bool status, int index, QBarSet* barset);

};

#endif // STATISTICS_H
