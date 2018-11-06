#include "obsmodedialog.h"
#include "ui_obsmodedialog.h"

ObsModeDialog::ObsModeDialog(VieVS::ObservingMode obsMode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ObsModeDialog)
{
    ui->setupUi(this);

    for(const auto &any : obsMode.getModes()){
        ui->comboBox_selectMode->addItem(QString::fromStdString(any->getName()));
        modes_.push_back(*any);
    }
    connect(ui->comboBox_selectMode, SIGNAL(currentIndexChanged(int)), this, SLOT(updateMode(int)));

    for(const auto &any : obsMode.getIfs()){
        ui->comboBox_selectIfBlock->addItem(QString::fromStdString(any->getName()));
        ifs_.push_back(*any);
    }
    connect(ui->comboBox_selectIfBlock, SIGNAL(currentIndexChanged(int)), this, SLOT(updateIf(int)));

    for(const auto &any : obsMode.getBbcs()){
        ui->comboBox_selectBbcBlock->addItem(QString::fromStdString(any->getName()));
        bbcs_.push_back(*any);
    }
    connect(ui->comboBox_selectBbcBlock, SIGNAL(currentIndexChanged(int)), this, SLOT(updateBbc(int)));

    for(const auto &any : obsMode.getFreqs()){
        ui->comboBox_selectFreqBlock->addItem(QString::fromStdString(any->getName()));
        freqs_.push_back(*any);
    }
    connect(ui->comboBox_selectFreqBlock, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFreq(int)));

    for(const auto &any : obsMode.getTracks()){
        ui->comboBox_selectTracksBlock->addItem(QString::fromStdString(any->getName()));
        tracks_.push_back(*any);
    }
    connect(ui->comboBox_selectTracksBlock, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTracks(int)));

    for(const auto &any : obsMode.getTrackFrameFormats()){
        ui->comboBox_selectTrackFrameFormat->addItem(QString::fromStdString(*any));
        trackFrameFormats_.push_back(*any);
    }
//    connect(ui->comboBox_selectMode, SIGNAL(currentIndexChanged(int)), this, SLOT(updateMode(int)));

    for(const auto &any : obsMode.getStationNames()){
        stations_.appendRow(new QStandardItem(QString::fromStdString(any)));
    }
}

ObsModeDialog::~ObsModeDialog()
{
    delete ui;
}

void ObsModeDialog::updateMode(int i)
{

}

void ObsModeDialog::updateFreq(int i)
{

}

void ObsModeDialog::updateIf(int i)
{

}

void ObsModeDialog::updateBbc(int i)
{

}

void ObsModeDialog::updateTracks(int i)
{

}

void ObsModeDialog::updatePhaceCal(int i)
{

}
