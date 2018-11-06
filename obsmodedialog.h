#ifndef OBSMODEDIALOG_H
#define OBSMODEDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include "../VieSchedpp/ObservingMode.h"

namespace Ui {
class ObsModeDialog;
}

class ObsModeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ObsModeDialog(VieVS::ObservingMode obsMode, QWidget *parent = nullptr);
    ~ObsModeDialog();

private slots:
    void updateMode(int i);
    void updateFreq(int i);
    void updateIf(int i);
    void updateBbc(int i);
    void updateTracks(int i);
    void updatePhaceCal(int i);

private:
    Ui::ObsModeDialog *ui;

    std::vector<VieVS::Mode> modes_;
    std::vector<VieVS::If> ifs_;
    std::vector<VieVS::Bbc> bbcs_;
    std::vector<VieVS::Freq> freqs_;
    std::vector<VieVS::Track> tracks_;
    std::vector<std::string> trackFrameFormats_;

    QStandardItemModel stations_;
};

#endif // OBSMODEDIALOG_H
