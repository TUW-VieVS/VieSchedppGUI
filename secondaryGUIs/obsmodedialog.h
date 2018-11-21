#ifndef OBSMODEDIALOG_H
#define OBSMODEDIALOG_H

#include <QDialog>
#include <QStringListModel>
#include <QTableView>
#include <QInputDialog>
#include <QMessageBox>
#include <QTableWidget>

#include <Models/model_mode.h>
#include <Models/model_if.h>
#include <Models/model_freq.h>
#include <Models/model_bbc.h>
#include <Models/model_tracks.h>

#include <Delegates/doublespinboxdelegate.h>
#include <Delegates/comboboxdelegate.h>
#include <Delegates/spinboxdelegate.h>

#include "../VieSchedpp/ObservingMode/ObservingMode.h"
//#include "utility"

namespace Ui {
class ObsModeDialog;
}

class ObsModeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ObsModeDialog(VieVS::ObservingMode obsMode, QWidget *parent = nullptr);

    ~ObsModeDialog();

    VieVS::ObservingMode getObservingMode();

private slots:
    void updateMode(int i);

    void updateFreq(int i);

    void updateIf(int i);

    void updateBbc(int i);

    void updateTracks(int i);

    void updatePhaceCal(int i);

    void setupViewMode(QTableView *view);

    void setupViewIf(QTableView *view);

    void setupViewBbc(QTableView *view);

    void setupViewTracks(QTableView *view);

    void setupViewFreq(QTableView *view);

    void insertModelEntries(QTableView *view);

    void ereaseModelEntries(QTableView *view);

    void insertAndErase();

    void on_comboBox_currentIndexChanged(int index);

    void rename();

    void addNewBlock();

    void removeBlock();

    void updateIds();

    void on_pushButton_addNewBand_clicked();

    void on_pushButton_removeBand_clicked();

    void dispSummary();

private:
    Ui::ObsModeDialog *ui;

    std::vector<std::shared_ptr<VieVS::Mode>> modes_;
    std::vector<std::shared_ptr<VieVS::If>> ifs_;
    std::vector<std::shared_ptr<VieVS::Bbc>> bbcs_;
    std::vector<std::shared_ptr<VieVS::Freq>> freqs_;
    std::vector<std::shared_ptr<VieVS::Track>> tracks_;
    std::vector<std::shared_ptr<std::string>> trackFrameFormats_;

    QVector<QString> stations_;

    Model_Mode * model_mode_;
    Model_If *model_if_;
    Model_Bbc *model_bbc_;
    Model_Tracks *model_tracks_;
    Model_Freq *model_freq_;

    QStringListModel *modesNames_;
    QStringListModel *ifNames_;
    QStringListModel *bbcNames_;
    QStringListModel *freqNames_;
    QStringListModel *tracksNames_;
    QStringListModel *phaseCalNames_;
    QStringListModel *trackFrameFormatNames_;

    QStringListModel *bandIds_;
    QStringListModel *ifIds_;
    QStringListModel *bbcIds_;
    QStringListModel *channelIds_;
    QStringListModel *phaseCalIds_;
};

#endif // OBSMODEDIALOG_H
