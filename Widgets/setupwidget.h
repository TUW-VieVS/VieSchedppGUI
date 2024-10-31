#ifndef SETUPWIDGET_H
#define SETUPWIDGET_H

#include <QWidget>
#include <QIcon>
#include <QtCharts>

#include "secondaryGUIs/parsedowntimes.h"
#include "Utility/callout.h"
#include "Utility/qtutil.h"
#include "../Parameters/stationparametersdialog.h"
#include "../Parameters/sourceparametersdialog.h"
#include "../Parameters/baselineparametersdialog.h"

#include "../VieSchedpp/XML/ParameterSettings.h"

namespace Ui {
class setupWidget;
}

class setupWidget : public QWidget
{
    Q_OBJECT

public:
    enum class Type{
        station,
        baseline,
        source,
        satellite,
        spacecraft
    };


    explicit setupWidget(Type type,
                         boost::property_tree::ptree &settings,
                         QTableWidget *tableWidget_ModesPolicy,
                         QStandardItemModel *allStationModel,
                         QStandardItemModel *allSourceModel,
                         QStandardItemModel *allSatelliteModel,
                         QStandardItemModel *allSpacecraftModel,
                         QStandardItemModel *selectedStationModel,
                         QStandardItemModel *selectedSourceModel,
                         QStandardItemModel *selectedBaselineModel,
                         QStandardItemModel *selectedSatelliteModel,
                         QStandardItemModel *selectedSpacecraftModel,
                         QStandardItemModel *allSourcePlusGroupModel,
                         QStandardItemModel *allStationPlusGroupModel,
                         QStandardItemModel *allBaselinePlusGroupModel,
                         QStandardItemModel *allSatellitePlusGroupModel,
                         QStandardItemModel *allSpacecraftPlusGroupModel,
                         std::map<std::string, std::vector<std::string>> *groupSta,
                         std::map<std::string, std::vector<std::string>> *groupSrc,
                         std::map<std::string, std::vector<std::string>> *groupBl,
                         std::map<std::string, std::vector<std::string>> *groupSat,
                         std::map<std::string, std::vector<std::string>> *groupSpace,
                         QSpinBox *int_downtime,
                         QWidget *parent = nullptr);
    ~setupWidget();

    void setDateTimeLimits(QDateTime start, QDateTime end);

    QPushButton *addGroupButton() const;
    
    QComboBox *memberComboBox() const;

    QComboBox *setupComboBox() const;

    QSplitter *horizontal_splitter() const;

    QSplitter *vertical_splitter() const;

    void clearParameters();

    void addParameter(const std::string &name, const VieVS::ParameterSettings::ParametersStations &para);

    void addParameter(const std::string &name, const VieVS::ParameterSettings::ParametersSources &para);

    void addParameter(const std::string &name, const VieVS::ParameterSettings::ParametersBaselines &para);

    void addSetup(const boost::property_tree::ptree &setup);

    void addDownTime(std::string station, unsigned int downStart, unsigned int downEnd);

    bool reset();

    void blockSignal(bool flag);

    const std::map<std::string, VieVS::ParameterSettings::ParametersStations> &getStationParameters(){
        return paraSta;
    }

    const std::map<std::string, VieVS::ParameterSettings::ParametersSources> &getSourceParameters(){
        return paraSrc;
    }

    const std::map<std::string, VieVS::ParameterSettings::ParametersBaselines> &getBaselineParameters(){
        return paraBl;
    }

    VieVS::ParameterSettings::ParametersStations &refStationParameter(const std::string &name){
        return paraSta[name];
    }

    VieVS::ParameterSettings::ParametersSources &refSourceParameter(const std::string &name){
        return paraSrc[name];
    }

    VieVS::ParameterSettings::ParametersBaselines &refBaselineParameter(const std::string &name){
        return paraBl[name];
    }

    const VieVS::ParameterSetup &getSetup(){
        return setupTree;
    }

    void setBlock(bool flag){
        block = flag;
    }
    bool getBlock(){
        return block;
    }

public slots:
    void on_pushButton_IvsDownTime_clicked();

private slots:
    void on_pushButton_editParameter_clicked();

    void on_pushButton_addParameter_clicked();

    void on_pushButton_addSetup_clicked();

    void displaySetupParameterFromPlot(QPointF point, bool flag);


    void on_pushButton_removeSetup_clicked();

    void on_pushButton_parseDownTime_clicked();

    void on_treeWidget_setup_itemEntered(QTreeWidgetItem *item, int column);

    void on_DateTimeEdit_start_dateTimeChanged(const QDateTime &dateTime);

    void on_DateTimeEdit_end_dateTimeChanged(const QDateTime &dateTime);

    void on_comboBox_setup_currentTextChanged(const QString &arg1);

    void on_comboBox_members_currentTextChanged(const QString &arg1);

    void on_ComboBox_parameters_currentTextChanged(const QString &arg1);

private:
    Type type;
    Ui::setupWidget *ui;
    bool block = false;

    QIcon icon_single;
    QIcon icon_group;
    QIcon icon_add_group;

    QStandardItemModel* allModel;
    QStandardItemModel* selectedModel;
    QStandardItemModel* allPlusGroupModel;

    std::map<std::string, std::vector<std::string>> *groups = new std::map<std::string, std::vector<std::string>>();

    VieVS::ParameterSetup setupTree;

    Callout *callout;

    QDateTime minDateTime;
    QDateTime maxDateTime;

    std::map<std::string, VieVS::ParameterSettings::ParametersStations> paraSta;
    std::map<std::string, VieVS::ParameterSettings::ParametersSources> paraSrc;
    std::map<std::string, VieVS::ParameterSettings::ParametersBaselines> paraBl;

    QSpinBox *int_downtime;

    // pointers to MainWindow content

    boost::property_tree::ptree &settings;
    QTableWidget *tableWidget_ModesPolicy;

    QStandardItemModel *allStationModel;
    QStandardItemModel *allSourceModel;
    QStandardItemModel *allSatelliteModel;
    QStandardItemModel *allSpacecraftModel;

    QStandardItemModel *selectedStationModel;
    QStandardItemModel *selectedSourceModel;
    QStandardItemModel *selectedBaselineModel;
    QStandardItemModel *selectedSatelliteModel;
    QStandardItemModel *selectedSpacecraftModel;

    QStandardItemModel *allSourcePlusGroupModel;
    QStandardItemModel *allStationPlusGroupModel;
    QStandardItemModel *allBaselinePlusGroupModel;
    QStandardItemModel *allSatellitePlusGroupModel;
    QStandardItemModel *allSpacecraftPlusGroupModel;

    std::map<std::string, std::vector<std::string>> *groupSta;
    std::map<std::string, std::vector<std::string>> *groupSrc;
    std::map<std::string, std::vector<std::string>> *groupBl;
    std::map<std::string, std::vector<std::string>> *groupSat;
    std::map<std::string, std::vector<std::string>> *groupSpace;


    void prepareSetupPlot();


    bool eventFilter(QObject *watched, QEvent *event);


    void displaySetupMember(QString name);


    void displayStationSetupParameter(QString name);

    void displaySourceSetupParameter(QString name);

    void displayBaselineSetupParameter(QString name);

    void drawTable(const VieVS::ParameterSetup &setup, QTreeWidgetItem *c);

    void drawSetupPlot();

    int plotParameter(QChart* targetChart, QTreeWidgetItem *item, int level, int plot, QString target);

    void setBackgroundColorOfChildrenWhite(QTreeWidgetItem *item);



};

#endif // SETUPWIDGET_H
