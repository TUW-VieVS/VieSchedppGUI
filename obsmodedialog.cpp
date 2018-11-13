#include "obsmodedialog.h"
#include "ui_obsmodedialog.h"

ObsModeDialog::ObsModeDialog(VieVS::ObservingMode obsMode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ObsModeDialog)
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(0);

    bandIds_ = new QStringListModel(this);
    ifIds_ = new QStringListModel(this);
    bbcIds_ = new QStringListModel(this);
    channelIds_ = new QStringListModel(this);

    modesNames_ = new QStringListModel(this);
    ifNames_ = new QStringListModel(this);
    bbcNames_ = new QStringListModel(this);
    freqNames_ = new QStringListModel(this);
    tracksNames_ = new QStringListModel(this);
    phaseCalNames_ = new QStringListModel(this);
    trackFrameFormatNames_ = new QStringListModel(this);

    QStringList pc;
    pc << "&U_cal";
    phaseCalIds_ = new QStringListModel(pc,this);

    QStringList bandIds;
    for(const auto &any : obsMode.getAllBands()){
        bandIds.append(QString::fromStdString(any));
    }
    bandIds_->setStringList(bandIds);

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
        modes_.push_back(std::make_shared<VieVS::Mode>(any->getName(), any->getNSta()));
//        modes_.push_back(std::make_shared<VieVS::Mode>(*any));
        modes << QString::fromStdString(any->getName());
    }
    modesNames_->setStringList(modes);
    ui->comboBox_selectMode->setModel(modesNames_);

    connect(ui->comboBox_selectIfBlock, SIGNAL(currentIndexChanged(int)), this, SLOT(updateIf(int)));
    QStringList ifs;
    for(const auto &any : obsMode.getIfs()){
        std::shared_ptr<VieVS::If> newBlock = std::make_shared<VieVS::If>(*any);
        ifs_.push_back(newBlock);
        for(int i=0; i<modes_.size(); ++i){
            std::shared_ptr<VieVS::Mode> newMode = modes_.at(i);
            std::shared_ptr<const VieVS::Mode> oldMode = obsMode.getModes().at(i);
            newMode->addBlock(newBlock, *oldMode->getAllStationsWithBlock(any));
        }
        ifs << QString::fromStdString(any->getName());
    }
    ifNames_->setStringList(ifs);
    ui->comboBox_selectIfBlock->setModel(ifNames_);

    connect(ui->comboBox_selectBbcBlock, SIGNAL(currentIndexChanged(int)), this, SLOT(updateBbc(int)));
    QStringList bbc;
    for(const auto &any : obsMode.getBbcs()){
        std::shared_ptr<VieVS::Bbc> newBlock = std::make_shared<VieVS::Bbc>(*any);
        bbcs_.push_back(newBlock);
        for(int i=0; i<modes_.size(); ++i){
            std::shared_ptr<VieVS::Mode> newMode = modes_.at(i);
            std::shared_ptr<const VieVS::Mode> oldMode = obsMode.getModes().at(i);
            newMode->addBlock(newBlock, *oldMode->getAllStationsWithBlock(any));
        }

        bbc << QString::fromStdString(any->getName());
    }
    bbcNames_->setStringList(bbc);
    ui->comboBox_selectBbcBlock->setModel(bbcNames_);

    connect(ui->comboBox_selectFreqBlock, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFreq(int)));
    QStringList freq;
    for(const auto &any : obsMode.getFreqs()){
        std::shared_ptr<VieVS::Freq> newBlock = std::make_shared<VieVS::Freq>(*any);
        freqs_.push_back(newBlock);
        for(int i=0; i<modes_.size(); ++i){
            std::shared_ptr<VieVS::Mode> newMode = modes_.at(i);
            std::shared_ptr<const VieVS::Mode> oldMode = obsMode.getModes().at(i);
            newMode->addBlock(newBlock, *oldMode->getAllStationsWithBlock(any));
        }
        freq << QString::fromStdString(any->getName());
    }
    freqNames_->setStringList(freq);
    ui->comboBox_selectFreqBlock->setModel(freqNames_);

    connect(ui->comboBox_selectTracksBlock, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTracks(int)));
    QStringList tracks;
    for(const auto &any : obsMode.getTracks()){
        std::shared_ptr<VieVS::Track> newBlock = std::make_shared<VieVS::Track>(*any);
        tracks_.push_back(newBlock);
        for(int i=0; i<modes_.size(); ++i){
            std::shared_ptr<VieVS::Mode> newMode = modes_.at(i);
            std::shared_ptr<const VieVS::Mode> oldMode = obsMode.getModes().at(i);
            newMode->addBlock(newBlock, *oldMode->getAllStationsWithBlock(any));
        }

        tracks << QString::fromStdString(any->getName());
    }
    tracksNames_->setStringList(tracks);
    ui->comboBox_selectTracksBlock->setModel(tracksNames_);

    QStringList trackFrameFormat;
    for(const auto &any : obsMode.getTrackFrameFormats()){
        std::shared_ptr<std::string> newBlock = std::make_shared<std::string>(*any);
        trackFrameFormats_.push_back(newBlock);
        for(int i=0; i<modes_.size(); ++i){
            std::shared_ptr<VieVS::Mode> newMode = modes_.at(i);
            std::shared_ptr<const VieVS::Mode> oldMode = obsMode.getModes().at(i);
            newMode->addBlock(newBlock, *oldMode->getAllStationsWithBlock(any));
        }
        trackFrameFormat << QString::fromStdString(*any);
    }
    trackFrameFormatNames_->setStringList(trackFrameFormat);
    ui->comboBox_selectTrackFrameFormat->setModel(trackFrameFormatNames_);

    QStringList phaceCalNames;
    phaceCalNames << "Standard";
    phaseCalNames_->setStringList(phaceCalNames);
    ui->comboBox_selectPhaseCal->setModel(phaseCalNames_);


    ui->comboBox_bands->setModel(bandIds_);

    connect(ui->pushButton_addFanout, SIGNAL(clicked()), this, SLOT(insertAndErase()));
    connect(ui->pushButton_defineChannel, SIGNAL(clicked()), this, SLOT(insertAndErase()));
    connect(ui->pushButton_defineIf, SIGNAL(clicked()), this, SLOT(insertAndErase()));
    connect(ui->pushButton_assignBbc, SIGNAL(clicked()), this, SLOT(insertAndErase()));

    connect(ui->pushButton_removeFanout, SIGNAL(clicked()), this, SLOT(insertAndErase()));
    connect(ui->pushButton_removeChannel, SIGNAL(clicked()), this, SLOT(insertAndErase()));
    connect(ui->pushButton_removeIf, SIGNAL(clicked()), this, SLOT(insertAndErase()));
    connect(ui->pushButton_removeBbc, SIGNAL(clicked()), this, SLOT(insertAndErase()));

    connect(ui->pushButton_renameBbcBlock, SIGNAL(clicked()), this, SLOT(rename()));
    connect(ui->pushButton_renameFreqBlock, SIGNAL(clicked()), this, SLOT(rename()));
    connect(ui->pushButton_renameIfBlock, SIGNAL(clicked()), this, SLOT(rename()));
    connect(ui->pushButton_renameModeBlock, SIGNAL(clicked()), this, SLOT(rename()));
    connect(ui->pushButton_renamePhaseCal, SIGNAL(clicked()), this, SLOT(rename()));
    connect(ui->pushButton_renameTrackFrameFormat, SIGNAL(clicked()), this, SLOT(rename()));
    connect(ui->pushButton_renameTracksBock, SIGNAL(clicked()), this, SLOT(rename()));

    connect(ui->pushButton_addMode, SIGNAL(clicked()), this, SLOT(addNewBlock()));
    connect(ui->pushButton_addFreqBlock, SIGNAL(clicked()), this, SLOT(addNewBlock()));
    connect(ui->pushButton_addBbcBlock, SIGNAL(clicked()), this, SLOT(addNewBlock()));
    connect(ui->pushButton_addIfBlock, SIGNAL(clicked()), this, SLOT(addNewBlock()));
    connect(ui->pushButton_addTracksBlock, SIGNAL(clicked()), this, SLOT(addNewBlock()));
    connect(ui->pushButton_addTrackFrameFormat, SIGNAL(clicked()), this, SLOT(addNewBlock()));
    connect(ui->pushButton_addPhaseCalBlcok, SIGNAL(clicked()), this, SLOT(addNewBlock()));

    connect(ui->pushButton_removeMode, SIGNAL(clicked()), this, SLOT(removeBlock()));
    connect(ui->pushButton_removeFreqBlock, SIGNAL(clicked()), this, SLOT(removeBlock()));
    connect(ui->pushButton_removeBbcBlock, SIGNAL(clicked()), this, SLOT(removeBlock()));
    connect(ui->pushButton_removeIfBlock, SIGNAL(clicked()), this, SLOT(removeBlock()));
    connect(ui->pushButton_removeTracksBlock, SIGNAL(clicked()), this, SLOT(removeBlock()));
    connect(ui->pushButton_removeTrackFrameFormatBlock, SIGNAL(clicked()), this, SLOT(removeBlock()));
    connect(ui->pushButton_removePhaseCalBlock, SIGNAL(clicked()), this, SLOT(removeBlock()));

    connect(model_bbc_, SIGNAL(idChanged()), this, SLOT(updateIds()));
    connect(model_tracks_, SIGNAL(idChanged()), this, SLOT(updateIds()));
    connect(model_if_, SIGNAL(idChanged()), this, SLOT(updateIds()));

    model_bbc_->idChanged();
    model_tracks_->idChanged();
    model_if_->idChanged();

    connect(ui->spinBox_bits, SIGNAL(valueChanged(int)), this, SLOT(changeBits(int)));

    connect(ui->pushButton_summary, SIGNAL(clicked()), this, SLOT(dispSummary()));
}

ObsModeDialog::~ObsModeDialog()
{
    delete ui;
}

void ObsModeDialog::updateMode(int i)
{
    if(i>=0 && i<modes_.size()){
        model_mode_->setMode(modes_.at(i));
    }else{
        model_mode_->setMode(std::make_shared<VieVS::Mode>("empty",stations_.size()));
    }
}

void ObsModeDialog::updateFreq(int i)
{
    if(i>=0 && i<freqs_.size()){
        model_freq_->setFreq(freqs_.at(i));
    }else{
        model_freq_->setFreq(std::make_shared<VieVS::Freq>("empty"));
    }
    ui->label_nrFreq->setText(QString("number of channels: %1").arg(model_freq_->nrItems()));
}

void ObsModeDialog::updateIf(int i)
{
    if(i>=0 && i<ifs_.size()){
        model_if_->setIf(ifs_.at(i));
    }else{
        model_if_->setIf(std::make_shared<VieVS::If>("empty"));
    }
    ui->label_nrIfs->setText(QString("number of IF definitions: %1").arg(model_if_->nrItems()));
}

void ObsModeDialog::updateBbc(int i)
{
    if(i>=0 && i<bbcs_.size()){
        model_bbc_->setBbc(bbcs_.at(i));
    }else{
        model_bbc_->setBbc(std::make_shared<VieVS::Bbc>("empty"));
    }
    ui->label_nrBbcs->setText(QString("number of assigned BBCs: %1").arg(model_bbc_->nrItems()));
}

void ObsModeDialog::updateTracks(int i)
{
    if(i>=0 && i<tracks_.size()){
        model_tracks_->setTracks(tracks_.at(i));
        ui->comboBox->setCurrentIndex(model_tracks_->getFanout()-1);
    }else{
        model_tracks_->setTracks(std::make_shared<VieVS::Track>("empty"));
    }
    ui->label_nrTracks->setText(QString("number of fanouts: %1").arg(model_tracks_->nrItems()));
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
    model_bbc_ = new Model_Bbc(ifIds_, this);
    view->setModel(model_bbc_);

    SpinBoxDelegate *spinBoxDelegate = new SpinBoxDelegate("", this);
    ComboBoxDelegate *comboBoxDelegate_ifs = new ComboBoxDelegate(ifIds_, this);

    view->setItemDelegateForColumn(1,spinBoxDelegate);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setItemDelegateForColumn(2,comboBoxDelegate_ifs);
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
    model_freq_ = new Model_Freq(bandIds_, channelIds_, bbcIds_, this);
    view->setModel(model_freq_);

    DoubleSpinBoxDelegate *doubleSpinBoxDelegate_MHz = new DoubleSpinBoxDelegate(" [MHz]",this);
    ComboBoxDelegate *comboBoxDelegate_netSidebands = new ComboBoxDelegate(Model_Freq::getNetSidebands(), this);
    ComboBoxDelegate *comboBoxDelegate_bbcs = new ComboBoxDelegate(bbcIds_, this);
    ComboBoxDelegate *comboBoxDelegate_bands = new ComboBoxDelegate(bandIds_, this);
    ComboBoxDelegate *comboBoxDelegate_channelIds_ = new ComboBoxDelegate(channelIds_, this);
    ComboBoxDelegate *comboBoxDelegate_pcal = new ComboBoxDelegate(phaseCalIds_, this);

    view->setItemDelegateForColumn(0,comboBoxDelegate_bands);
    view->setItemDelegateForColumn(1,doubleSpinBoxDelegate_MHz);
    view->setItemDelegateForColumn(2,comboBoxDelegate_netSidebands);
    view->setItemDelegateForColumn(3,doubleSpinBoxDelegate_MHz);
    view->setItemDelegateForColumn(4,comboBoxDelegate_channelIds_);
    view->setItemDelegateForColumn(5,comboBoxDelegate_bbcs);
    view->setItemDelegateForColumn(6,comboBoxDelegate_pcal);

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
    if(selected.isEmpty()){
        QMessageBox::information(this,"no selection","Please select items which should be deleted in the list below first.");
    }else{
        for(int k = 0; k < (selected.size()/2); k++){
            selected.swap(k,selected.size()-(1+k));
        }

        for(const auto &any : selected){
            view->model()->removeRows(any.row(),1);
        }
    }

}

void ObsModeDialog::insertAndErase()
{
    QObject *s = sender();

    if(s == ui->pushButton_addFanout){            // insert
        insertModelEntries(ui->tableView_tracks);
        ui->label_nrTracks->setText(QString("number of fanouts: %1").arg(model_tracks_->nrItems()));
    }else if(s == ui->pushButton_defineChannel){
        insertModelEntries(ui->tableView_freq);
        ui->label_nrFreq->setText(QString("number of channels: %1").arg(model_freq_->nrItems()));
    }else if(s == ui->pushButton_defineIf){
        insertModelEntries(ui->tableView_if);
        ui->label_nrIfs->setText(QString("number of IF definitions: %1").arg(model_if_->nrItems()));
    }else if(s == ui->pushButton_assignBbc){
        insertModelEntries(ui->tableView_bbc);
        ui->label_nrBbcs->setText(QString("number of assigned BBCs: %1").arg(model_bbc_->nrItems()));
    }else if(s == ui->pushButton_removeFanout){   // remove
        ereaseModelEntries(ui->tableView_tracks);
        ui->label_nrTracks->setText(QString("number of fanouts: %1").arg(model_tracks_->nrItems()));
    }else if(s == ui->pushButton_removeChannel){
        ereaseModelEntries(ui->tableView_freq);
        ui->label_nrFreq->setText(QString("number of channels: %1").arg(model_freq_->nrItems()));
    }else if(s == ui->pushButton_removeIf){
        ereaseModelEntries(ui->tableView_if);
        ui->label_nrIfs->setText(QString("number of IF definitions: %1").arg(model_if_->nrItems()));
    }else if(s == ui->pushButton_removeBbc){
        ereaseModelEntries(ui->tableView_bbc);
        ui->label_nrBbcs->setText(QString("number of assigned BBCs: %1").arg(model_bbc_->nrItems()));
    }

}

void ObsModeDialog::on_comboBox_currentIndexChanged(int index)
{
    model_tracks_->setFanout(index+1);
    model_tracks_->layoutChanged();
}

void ObsModeDialog::rename()
{
    QObject *s = sender();
    QString currentText;
    if(s == ui->pushButton_renameModeBlock){
        currentText = ui->comboBox_selectMode->currentText();
    }else if(s == ui->pushButton_renameFreqBlock){
        currentText = ui->comboBox_selectFreqBlock->currentText();
    }else if(s == ui->pushButton_renameBbcBlock){
        currentText = ui->comboBox_selectBbcBlock->currentText();
    }else if(s == ui->pushButton_renameIfBlock){
        currentText = ui->comboBox_selectIfBlock->currentText();
    }else if(s == ui->pushButton_renameTracksBock){
        currentText = ui->comboBox_selectTracksBlock->currentText();
    }else if(s == ui->pushButton_renameTrackFrameFormat){
        currentText = ui->comboBox_selectTrackFrameFormat->currentText();
    }else if(s == ui->pushButton_renamePhaseCal){
        currentText = ui->comboBox_selectPhaseCal->currentText();
    }

    if(currentText.isEmpty()){
        return;
    }

    bool ok;
    QString name = QInputDialog::getText(this, tr("rename block"),
                                         tr("new name:"), QLineEdit::Normal,
                                         currentText, &ok);
    if (ok && !name.isEmpty()){
        if(s == ui->pushButton_renameModeBlock){
            QComboBox *comboBox = ui->comboBox_selectMode;
            int idx = comboBox->currentIndex();
            modes_[idx]->changeName(name.toStdString());
            QStringList tmp = modesNames_->stringList();
            tmp[idx] = name;
            modesNames_->setStringList(tmp);
            comboBox->setCurrentIndex(idx);
        }else if(s == ui->pushButton_renameFreqBlock){
            QComboBox *comboBox = ui->comboBox_selectFreqBlock;
            int idx = comboBox->currentIndex();
            freqs_[idx]->changeName(name.toStdString());
            QStringList tmp = freqNames_->stringList();
            tmp[idx] = name;
            freqNames_->setStringList(tmp);
            comboBox->setCurrentIndex(idx);
        }else if(s == ui->pushButton_renameBbcBlock){
            QComboBox *comboBox = ui->comboBox_selectBbcBlock;
            int idx = comboBox->currentIndex();
            bbcs_[idx]->changeName(name.toStdString());
            QStringList tmp = bbcNames_->stringList();
            tmp[idx] = name;
            bbcNames_->setStringList(tmp);
            comboBox->setCurrentIndex(idx);
        }else if(s == ui->pushButton_renameIfBlock){
            QComboBox *comboBox = ui->comboBox_selectIfBlock;
            int idx = comboBox->currentIndex();
            ifs_[idx]->changeName(name.toStdString());
            QStringList tmp = ifNames_->stringList();
            tmp[idx] = name;
            ifNames_->setStringList(tmp);
            comboBox->setCurrentIndex(idx);
        }else if(s == ui->pushButton_renameTracksBock){
            QComboBox *comboBox = ui->comboBox_selectTracksBlock;
            int idx = comboBox->currentIndex();
            tracks_[idx]->changeName(name.toStdString());
            QStringList tmp = tracksNames_->stringList();
            tmp[idx] = name;
            tracksNames_->setStringList(tmp);
            comboBox->setCurrentIndex(idx);
        }else if(s == ui->pushButton_renameTrackFrameFormat){
            QComboBox *comboBox = ui->comboBox_selectTrackFrameFormat;
            int idx = comboBox->currentIndex();
            trackFrameFormats_[idx] = std::make_shared<std::string>(name.toStdString());
            QStringList tmp = trackFrameFormatNames_->stringList();
            tmp[idx] = name;
            trackFrameFormatNames_->setStringList(tmp);
            comboBox->setCurrentIndex(idx);
        }else if(s == ui->pushButton_renamePhaseCal){
            // *** todo ***
        }
    }
}

void ObsModeDialog::addNewBlock()
{
    QObject *s = sender();

    if(s == ui->pushButton_addMode){
        int n = modes_.size();
        int nsta = stations_.size();
        std::string name = std::string("MODE#").append(std::to_string(n));
        auto itm = std::make_shared<VieVS::Mode>(name, nsta);
        for(const auto &any : freqs_){
            itm->addBlock(any,std::vector<unsigned long>());
        }
        for(const auto &any : bbcs_){
            itm->addBlock(any,std::vector<unsigned long>());
        }
        for(const auto &any : ifs_){
            itm->addBlock(any,std::vector<unsigned long>());
        }
        for(const auto &any : tracks_){
            itm->addBlock(any,std::vector<unsigned long>());
        }
        for(const auto &any : trackFrameFormats_){
            itm->addBlock(any,std::vector<unsigned long>());
        }
//        for(const auto &any : phaseCals_){
//            itm->addBlock(any,std::vector<unsigned long>());
//        }
        modes_.push_back(itm);
        QStringList tmp = modesNames_->stringList();
        tmp.append(QString::fromStdString(name));
        modesNames_->setStringList(tmp);
        ui->comboBox_selectMode->setCurrentIndex(n);

    }else if( s == ui->pushButton_addFreqBlock){
        int n = freqs_.size();
        std::string name = std::string("FREQ#").append(std::to_string(n));
        auto itm = std::make_shared<VieVS::Freq>(name);
        freqs_.push_back(itm);
        for(auto &any : modes_){
            any->addBlock(itm,std::vector<unsigned long>());
        }
        QStringList tmp = freqNames_->stringList();
        tmp.append(QString::fromStdString(name));
        freqNames_->setStringList(tmp);
        ui->comboBox_selectFreqBlock->setCurrentIndex(n);

    }else if( s == ui->pushButton_addBbcBlock){
        int n = bbcs_.size();
        std::string name = std::string("BBC#").append(std::to_string(n));
        auto itm = std::make_shared<VieVS::Bbc>(name);
        bbcs_.push_back(itm);
        for(auto &any : modes_){
            any->addBlock(itm,std::vector<unsigned long>());
        }
        QStringList tmp = bbcNames_->stringList();
        tmp.append(QString::fromStdString(name));
        bbcNames_->setStringList(tmp);
        ui->comboBox_selectBbcBlock->setCurrentIndex(n);

    }else if( s == ui->pushButton_addIfBlock){
        int n = ifs_.size();
        std::string name = std::string("IF#").append(std::to_string(n));
        auto itm = std::make_shared<VieVS::If>(name);
        ifs_.push_back(itm);
        for(auto &any : modes_){
            any->addBlock(itm,std::vector<unsigned long>());
        }
        QStringList tmp = ifNames_->stringList();
        tmp.append(QString::fromStdString(name));
        ifNames_->setStringList(tmp);
        ui->comboBox_selectIfBlock->setCurrentIndex(n);

    }else if( s == ui->pushButton_addTracksBlock){
        int n = tracks_.size();
        std::string name = std::string("TRACKS#").append(std::to_string(n));
        auto itm = std::make_shared<VieVS::Track>(name);
        tracks_.push_back(itm);
        for(auto &any : modes_){
            any->addBlock(itm,std::vector<unsigned long>());
        }
        QStringList tmp = tracksNames_->stringList();
        tmp.append(QString::fromStdString(name));
        tracksNames_->setStringList(tmp);
        ui->comboBox_selectTracksBlock->setCurrentIndex(n);

    }else if( s == ui->pushButton_addTrackFrameFormat){
        int n = trackFrameFormats_.size();
        std::string name = std::string("trackFrameFormat#").append(std::to_string(n));
        auto itm = std::make_shared<std::string>(name);
        trackFrameFormats_.push_back(itm);
        for(auto &any : modes_){
            any->addBlock(itm,std::vector<unsigned long>());
        }
        QStringList tmp = trackFrameFormatNames_->stringList();
        tmp.append(QString::fromStdString(name));
        trackFrameFormatNames_->setStringList(tmp);
        ui->comboBox_selectTrackFrameFormat->setCurrentIndex(n);

    }else if( s == ui->pushButton_addPhaceCalDetect){
        // *** todo ***
    }
}

void ObsModeDialog::removeBlock()
{
    QObject *s = sender();
    if(s == ui->pushButton_removeMode){
        int idx = ui->comboBox_selectMode->currentIndex();
        if(idx<0){
            return;
        }
        modes_.erase(modes_.begin()+idx);
        QStringList tmp = modesNames_->stringList();
        tmp.removeAt(idx);
        modesNames_->setStringList(tmp);
        ui->comboBox_selectMode->setCurrentIndex(idx-1);

    }else if( s == ui->pushButton_removeFreqBlock){
        int idx = ui->comboBox_selectFreqBlock->currentIndex();
        if(idx<0){
            return;
        }
        for(auto &any : modes_){
            any->removeFreq(idx);
        }
        freqs_.erase(freqs_.begin()+idx);
        QStringList tmp = freqNames_->stringList();
        tmp.removeAt(idx);
        freqNames_->setStringList(tmp);
        ui->comboBox_selectFreqBlock->setCurrentIndex(idx-1);

    }else if( s == ui->pushButton_removeBbcBlock){
        int idx = ui->comboBox_selectBbcBlock->currentIndex();
        if(idx<0){
            return;
        }
        for(auto &any : modes_){
            any->removeBbc(idx);
        }
        bbcs_.erase(bbcs_.begin()+idx);
        QStringList tmp = bbcNames_->stringList();
        tmp.removeAt(idx);
        bbcNames_->setStringList(tmp);
        ui->comboBox_selectBbcBlock->setCurrentIndex(idx-1);
        model_bbc_->idChanged();
    }else if( s == ui->pushButton_removeIfBlock){
        int idx = ui->comboBox_selectIfBlock->currentIndex();
        if(idx<0){
            return;
        }
        for(auto &any : modes_){
            any->removeIf(idx);
        }
        ifs_.erase(ifs_.begin()+idx);
        QStringList tmp = ifNames_->stringList();
        tmp.removeAt(idx);
        ifNames_->setStringList(tmp);
        ui->comboBox_selectIfBlock->setCurrentIndex(idx-1);
        model_if_->idChanged();

    }else if( s == ui->pushButton_removeTracksBlock){
        int idx = ui->comboBox_selectTracksBlock->currentIndex();
        if(idx<0){
            return;
        }
        for(auto &any : modes_){
            any->removeTracks(idx);
        }
        tracks_.erase(tracks_.begin()+idx);
        QStringList tmp = tracksNames_->stringList();
        tmp.removeAt(idx);
        tracksNames_->setStringList(tmp);
        ui->comboBox_selectTracksBlock->setCurrentIndex(idx-1);
        model_tracks_->idChanged();

    }else if( s == ui->pushButton_removeTrackFrameFormatBlock){
        int idx = ui->comboBox_selectTrackFrameFormat->currentIndex();
        if(idx<0){
            return;
        }
        for(auto &any : modes_){
            any->removeTrackFrameFormats(idx);
        }
        trackFrameFormats_.erase(trackFrameFormats_.begin()+idx);
        QStringList tmp = trackFrameFormatNames_->stringList();
        tmp.removeAt(idx);
        trackFrameFormatNames_->setStringList(tmp);
        ui->comboBox_selectTrackFrameFormat->setCurrentIndex(idx-1);

    }else if( s == ui->pushButton_removePhaceCalDetect){
        // *** todo ***
    }

}

void ObsModeDialog::updateIds()
{
    QObject *s = sender();

    if(s == model_if_){
        QStringList ifIds = ifIds_->stringList();
        ifIds.clear();
        for(const auto &any : ifs_){
            for(const auto &any : any->getIf_defs()){
                if(!ifIds.contains(QString::fromStdString(any.getName()))){
                    ifIds << QString::fromStdString(any.getName());
                }
            }
        }
        ifIds.sort();
        ifIds_->setStringList(ifIds);

    }else if(s == model_bbc_){
        QStringList bbcIds = bbcIds_->stringList();
        bbcIds.clear();
        for(const auto &any : bbcs_){
            for(const auto &any : any->getBbc_assigns()){
                if(!bbcIds.contains(QString::fromStdString(any.getName()))){
                    bbcIds.append(QString::fromStdString(any.getName()));
                }
            }
        }
        bbcIds.sort();
        bbcIds_->setStringList(bbcIds);

    }else if(s == model_tracks_){
        QStringList channelIds = channelIds_->stringList();
        channelIds.clear();
        for(const auto &any : tracks_){
            for(const auto &any : any->getFanout_defs()){
                if(!channelIds.contains(QString::fromStdString(any.trksid_))){
                    channelIds.append(QString::fromStdString(any.trksid_));
                }
            }
        }
        channelIds.sort();
        channelIds_->setStringList(channelIds);

    }
}

void ObsModeDialog::on_pushButton_addNewBand_clicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("rename block"),
                                         tr("new name:"), QLineEdit::Normal,
                                         "", &ok);

    if(ok){
        QStringList bands = bandIds_->stringList();
        if(!bands.contains(name)){
            bands << name;
            bands.sort();
        }
        bandIds_->setStringList(bands);
    }
    ui->comboBox_bands->setCurrentText(name);
}

void ObsModeDialog::on_pushButton_removeBand_clicked()
{
    int idx = ui->comboBox_bands->currentIndex();
    if(idx>=0){
        QStringList bands = bandIds_->stringList();
        bands.erase(bands.begin()+idx);
        bandIds_->setStringList(bands);
    }
    model_freq_->layoutChanged();
}

void ObsModeDialog::dispSummary()
{
    int idx = ui->comboBox_selectMode->currentIndex();
    const auto &mode = modes_.at(idx);
    mode->calcRecordingRates();

    auto bands = bandIds_->stringList();

    int nsta = stations_.size();
    QTableWidget *t = new QTableWidget(nsta*(nsta-1)/2, bandIds_->rowCount()+2, this);
    int c = 0;
    for(int i=0; i<nsta; ++i){
        for(int j=i+1; j<nsta; ++j){
            t->setItem(c, 0, new QTableWidgetItem(stations_.at(i)));
            t->setItem(c, 1, new QTableWidgetItem(stations_.at(j)));

            for(int b = 0; b<bands.size(); ++b){
                double rec = mode->recordingRate(i,j,bands.at(b).toStdString());
                t->setItem(c, b+2, new QTableWidgetItem(QString("%1").arg(rec*1e-6,0,'f',2)));
            }
            ++c;
        }
    }
    auto hv = t->horizontalHeader();
    hv->setSectionResizeMode(QHeaderView::ResizeToContents);

    QStringList h;
    h << "station 1" << "station2";
    for(const auto & any : bands){
        h << any+" [Mbps]";
    }

    t->setHorizontalHeaderLabels( h );
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->verticalHeader()->hide();
    t->setSortingEnabled(true);

    QDialog *dial = new QDialog(this);
    QVBoxLayout *l = new QVBoxLayout(dial);
    l->insertWidget(0,t,1);
    dial->setLayout(l);
    dial->resize(600,600);
    dial->exec();
}

void ObsModeDialog::changeBits(int bits)
{
    int idx = ui->comboBox_selectTracksBlock->currentIndex();
    tracks_[idx]->setBits(bits);
}

std::shared_ptr<VieVS::ObservingMode> ObsModeDialog::getObservingMode()
{
    auto om = std::make_shared<VieVS::ObservingMode>();
    for(const auto &any : modes_){
        om->addMode(any);
    }
    for(const auto &any : freqs_){
        om->addBlock(any);
    }
    for(const auto &any : bbcs_){
        om->addBlock(any);
    }
    for(const auto &any : ifs_){
        om->addBlock(any);
    }
    for(const auto &any : tracks_){
        om->addBlock(any);
    }
    for(const auto &any : trackFrameFormats_){
        om->addBlock(*any);
    }
    return om;
}
