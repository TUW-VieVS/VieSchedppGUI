#include "obsmodedialog.h"
#include "ui_obsmodedialog.h"

ObsModeDialog::ObsModeDialog(VieVS::ObservingMode obsMode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ObsModeDialog)
{
    ui->setupUi(this);

    modesNames_ = new QStringListModel(this);
    ifNames_ = new QStringListModel(this);
    bbcNames_ = new QStringListModel(this);
    freqNames_ = new QStringListModel(this);
    tracksNames_ = new QStringListModel(this);
    phaseCalNames_ = new QStringListModel(this);
    trackFrameFormatNames_ = new QStringListModel(this);


    for(const auto &any : obsMode.getStationNames()){
        stations_.append(QString::fromStdString(any));
    }

    setupViewMode(ui->tableView_mode);
    setupViewIf(ui->tableView_if);
    setupViewBbc(ui->tableView_bbc);
    setupViewTracks(ui->tableView_tracks);
    setupViewFreq(ui->tableView_freq);

    connect(ui->comboBox_selectMode, SIGNAL(currentIndexChanged(int)), this, SLOT(updateMode(int)));
    QStringList modes;
    for(const auto &any : obsMode.getModes()){
        modes_.push_back(std::make_shared<VieVS::Mode>(*any));
        modes << QString::fromStdString(any->getName());
    }
    modesNames_->setStringList(modes);
    ui->comboBox_selectMode->setModel(modesNames_);

    connect(ui->comboBox_selectIfBlock, SIGNAL(currentIndexChanged(int)), this, SLOT(updateIf(int)));
    QStringList ifs;
    for(const auto &any : obsMode.getIfs()){
        ifs_.push_back(std::make_shared<VieVS::If>(*any));
        ifs << QString::fromStdString(any->getName());
    }
    ifNames_->setStringList(ifs);
    ui->comboBox_selectIfBlock->setModel(ifNames_);

    connect(ui->comboBox_selectBbcBlock, SIGNAL(currentIndexChanged(int)), this, SLOT(updateBbc(int)));
    QStringList bbc;
    for(const auto &any : obsMode.getBbcs()){
        bbcs_.push_back(std::make_shared<VieVS::Bbc>(*any));
        bbc << QString::fromStdString(any->getName());
    }
    bbcNames_->setStringList(bbc);
    ui->comboBox_selectBbcBlock->setModel(bbcNames_);

    connect(ui->comboBox_selectFreqBlock, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFreq(int)));
    QStringList freq;
    for(const auto &any : obsMode.getFreqs()){
        freqs_.push_back(std::make_shared<VieVS::Freq>(*any));
        freq << QString::fromStdString(any->getName());
    }
    freqNames_->setStringList(freq);
    ui->comboBox_selectFreqBlock->setModel(freqNames_);

    connect(ui->comboBox_selectTracksBlock, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTracks(int)));
    QStringList tracks;
    for(const auto &any : obsMode.getTracks()){
        tracks_.push_back(std::make_shared<VieVS::Track>(*any));
        tracks << QString::fromStdString(any->getName());
    }
    tracksNames_->setStringList(tracks);
    ui->comboBox_selectTracksBlock->setModel(tracksNames_);

    QStringList trackFrameFormat;
    for(const auto &any : obsMode.getTrackFrameFormats()){
        trackFrameFormats_.push_back(std::make_shared<std::string>(*any));
        trackFrameFormat << QString::fromStdString(*any.get());
    }
    trackFrameFormatNames_->setStringList(trackFrameFormat);
    ui->comboBox_selectTrackFrameFormat->setModel(trackFrameFormatNames_);

    QStringList phaceCalNames;
    phaceCalNames << "Standard";
    phaseCalNames_->setStringList(phaceCalNames);
    ui->comboBox_selectPhaseCal->setModel(phaseCalNames_);


    connect(ui->pushButton_addFanout, SIGNAL(clicked()), this, SLOT(insertAndErase()));
    connect(ui->pushButton_defineChannel, SIGNAL(clicked()), this, SLOT(insertAndErase()));
    connect(ui->pushButton_defineIf, SIGNAL(clicked()), this, SLOT(insertAndErase()));
    connect(ui->pushButton_assignBbc, SIGNAL(clicked()), this, SLOT(insertAndErase()));

    connect(ui->pushButton_removeFanout, SIGNAL(clicked()), this, SLOT(insertAndErase()));
    connect(ui->pushButton_removeChannel, SIGNAL(clicked()), this, SLOT(insertAndErase()));
    connect(ui->pushButton_removeIf, SIGNAL(clicked()), this, SLOT(insertAndErase()));
    connect(ui->pushButton_removeBbc, SIGNAL(clicked()), this, SLOT(insertAndErase()));
}

ObsModeDialog::~ObsModeDialog()
{
    delete ui;
}

void ObsModeDialog::updateMode(int i)
{
    model_mode_->setMode(modes_.at(i));
}

void ObsModeDialog::updateFreq(int i)
{
    model_freq_->setFreq(freqs_.at(i));
}

void ObsModeDialog::updateIf(int i)
{
    model_if_->setIf(ifs_.at(i));
}

void ObsModeDialog::updateBbc(int i)
{
    model_bbc_->setBbc(bbcs_.at(i));
}

void ObsModeDialog::updateTracks(int i)
{
    model_tracks_->setTracks(tracks_.at(i));
    ui->comboBox->setCurrentIndex(model_tracks_->getFanout()-1);
}

void ObsModeDialog::updatePhaceCal(int i)
{

}

void ObsModeDialog::setupViewMode(QTableView *view)
{
    model_mode_ = new Model_Mode(stations_, this);
    view->setModel(model_mode_);

    ComboBoxDelegate *ComboBoxDelegate_freq = new ComboBoxDelegate(freqNames_, this);
    ComboBoxDelegate *ComboBoxDelegate_bbc = new ComboBoxDelegate(bbcNames_, this);
    ComboBoxDelegate *ComboBoxDelegate_if = new ComboBoxDelegate(ifNames_, this);
    ComboBoxDelegate *ComboBoxDelegate_tracks = new ComboBoxDelegate(tracksNames_, this);
    ComboBoxDelegate *ComboBoxDelegate_phaseCal = new ComboBoxDelegate(phaseCalNames_, this);
    ComboBoxDelegate *ComboBoxDelegate_tff = new ComboBoxDelegate(trackFrameFormatNames_, this);

    view->setItemDelegateForColumn(1,ComboBoxDelegate_freq);
    view->setItemDelegateForColumn(2,ComboBoxDelegate_bbc);
    view->setItemDelegateForColumn(3,ComboBoxDelegate_if);
    view->setItemDelegateForColumn(4,ComboBoxDelegate_tracks);
    view->setItemDelegateForColumn(5,ComboBoxDelegate_phaseCal);
    view->setItemDelegateForColumn(6,ComboBoxDelegate_tff);
    auto hv1 = view->horizontalHeader();
    hv1->setSectionResizeMode(QHeaderView::ResizeToContents);

}

void ObsModeDialog::setupViewIf(QTableView *view)
{
    model_if_ = new Model_If(this);
    view->setModel(model_if_);

    DoubleSpinBoxDelegate *doubleSpinBoxDelegate_MHz = new DoubleSpinBoxDelegate(" [MHz]",this);
    DoubleSpinBoxDelegate *doubleSpinBoxDelegate_Hz = new DoubleSpinBoxDelegate(" [Hz]",this);

    ComboBoxDelegate *ComboBoxDelegate_pol = new ComboBoxDelegate(Model_If::getPolarizations(), this);
    ComboBoxDelegate *ComboBoxDelegate_net = new ComboBoxDelegate(Model_If::getNetSidebands(), this);

    view->setItemDelegateForColumn(2,ComboBoxDelegate_pol);
    view->setItemDelegateForColumn(3,doubleSpinBoxDelegate_MHz);
    view->setItemDelegateForColumn(4,ComboBoxDelegate_net);
    view->setItemDelegateForColumn(5,doubleSpinBoxDelegate_MHz);
    view->setItemDelegateForColumn(6,doubleSpinBoxDelegate_Hz);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    auto hv1 = view->horizontalHeader();
    hv1->setSectionResizeMode(QHeaderView::ResizeToContents);

}

void ObsModeDialog::setupViewBbc(QTableView *view)
{
    model_bbc_ = new Model_Bbc(this);
    view->setModel(model_bbc_);

    SpinBoxDelegate *spinBoxDelegate = new SpinBoxDelegate("", this);

    view->setItemDelegateForColumn(1,spinBoxDelegate);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    auto hv1 = view->horizontalHeader();
    hv1->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void ObsModeDialog::setupViewTracks(QTableView *view)
{
    model_tracks_ = new Model_Tracks(this);
    view->setModel(model_tracks_);

    SpinBoxDelegate *spinBoxDelegate = new SpinBoxDelegate("", this);
    ComboBoxDelegate *comboBoxDelegate_bitstream = new ComboBoxDelegate(Model_Tracks::getBitstreams(), this);

    view->setItemDelegateForColumn(2,comboBoxDelegate_bitstream);
    view->setItemDelegateForColumn(3,spinBoxDelegate);
    view->setItemDelegateForColumn(4,spinBoxDelegate);
    view->setItemDelegateForColumn(5,spinBoxDelegate);
    view->setItemDelegateForColumn(6,spinBoxDelegate);
    view->setItemDelegateForColumn(7,spinBoxDelegate);

    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    auto hv1 = view->horizontalHeader();
    hv1->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void ObsModeDialog::setupViewFreq(QTableView *view)
{
    model_freq_ = new Model_Freq(this);
    view->setModel(model_freq_);

    DoubleSpinBoxDelegate *doubleSpinBoxDelegate_MHz = new DoubleSpinBoxDelegate(" [MHz]",this);
    ComboBoxDelegate *comboBoxDelegate_netSidebands = new ComboBoxDelegate(Model_Freq::getNetSidebands(), this);

    view->setItemDelegateForColumn(1,doubleSpinBoxDelegate_MHz);
    view->setItemDelegateForColumn(2,comboBoxDelegate_netSidebands);
    view->setItemDelegateForColumn(3,doubleSpinBoxDelegate_MHz);

    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    auto hv1 = view->horizontalHeader();
    hv1->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void ObsModeDialog::insertModelEntries(QTableView *view)
{
    int row = -1;
    if(!view->selectionModel()->selectedRows().empty()){
        row = view->selectionModel()->selectedRows().back().row();
    }
    view->model()->insertRows(row,1);

}

void ObsModeDialog::ereaseModelEntries(QTableView *view)
{
    auto selected = view->selectionModel()->selectedRows();
    for(int k = 0; k < (selected.size()/2); k++){
        selected.swap(k,selected.size()-(1+k));
    }

    for(const auto &any : selected){
        view->model()->removeRows(any.row(),1);
    }
}

void ObsModeDialog::insertAndErase()
{
    QObject *s = sender();

    if(s == ui->pushButton_addFanout){            // insert
        insertModelEntries(ui->tableView_tracks);
    }else if(s == ui->pushButton_defineChannel){
        insertModelEntries(ui->tableView_freq);
    }else if(s == ui->pushButton_defineIf){
        insertModelEntries(ui->tableView_if);
    }else if(s == ui->pushButton_assignBbc){
        insertModelEntries(ui->tableView_bbc);
    }else if(s == ui->pushButton_removeFanout){   // remove
        ereaseModelEntries(ui->tableView_tracks);
    }else if(s == ui->pushButton_removeChannel){
        ereaseModelEntries(ui->tableView_freq);
    }else if(s == ui->pushButton_removeIf){
        ereaseModelEntries(ui->tableView_if);
    }else if(s == ui->pushButton_removeBbc){
        ereaseModelEntries(ui->tableView_bbc);
    }

}

void ObsModeDialog::on_comboBox_currentIndexChanged(int index)
{
    model_tracks_->setFanout(index+1);
    model_tracks_->layoutChanged();
}
