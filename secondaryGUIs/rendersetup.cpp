#include "rendersetup.h"
#include "ui_rendersetup.h"

RenderSetup::RenderSetup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenderSetup)
{
    ui->setupUi(this);

    connect(ui->groupBox_timeSeries, SIGNAL(toggled(bool)), this, SLOT(updatePlotCounter()));
    connect(ui->spinBox_increment, SIGNAL(valueChanged(int)), this, SLOT(updatePlotCounter()));
    connect(ui->spinBox_width, SIGNAL(valueChanged(int)), ui->spinBox_height, SLOT(setValue(int)));
    connect(ui->spinBox_height, SIGNAL(valueChanged(int)), ui->spinBox_width, SLOT(setValue(int)));
}

RenderSetup::~RenderSetup()
{
    delete ui;
}

void RenderSetup::addTimes(int duration)
{
    duration_ = duration;
}

void RenderSetup::addList(QStandardItemModel *list)
{
    ui->listView->setModel(list);
    QItemSelectionModel* selectionModel = ui->listView->selectionModel();

    connect(selectionModel,SIGNAL(selectionChanged (const QItemSelection& , const QItemSelection &)),this,SLOT(updatePlotCounter()));
}

void RenderSetup::setOutDir(QString dir)
{
    ui->lineEdit_output->setText(dir);
}

QVector<int> RenderSetup::selected()
{
    auto list = ui->listView->selectionModel()->selectedIndexes();

    QVector<int> v;
    for(const auto &any: list){
        v.push_back(any.row());
    }
    return v;
}

QVector<QPair<int, int> > RenderSetup::times()
{
    QVector<QPair<int, int>> v;
    if(ui->groupBox_timeSeries->isChecked()){
        for(int i=0; i<duration_; i+=ui->spinBox_increment->value()){
            v.push_back({i, i+ui->spinBox_timespan->value()});
        }
    }else{
        v.push_back({0,duration_});
    }
    return  v;
}

QString RenderSetup::outDir()
{
    return ui->lineEdit_output->text();
}

QPair<int, int> RenderSetup::resolution()
{
    return {ui->spinBox_width->value(), ui->spinBox_height->value()};
}

void RenderSetup::setDefaultFormat(int w, int h)
{
    auto status = ui->checkBox_fixRatio->checkState();
    ui->checkBox_fixRatio->setCheckState(Qt::Unchecked);
    ui->spinBox_width->setValue(w);
    ui->spinBox_height->setValue(h);
    ui->checkBox_fixRatio->setCheckState(status);
}

void RenderSetup::on_checkBox_fixRatio_toggled(bool checked)
{
    if(checked){
        connect(ui->spinBox_width, SIGNAL(valueChanged(int)), ui->spinBox_height, SLOT(setValue(int)));
        connect(ui->spinBox_height, SIGNAL(valueChanged(int)), ui->spinBox_width, SLOT(setValue(int)));
    }else{
        disconnect(ui->spinBox_width, SIGNAL(valueChanged(int)), ui->spinBox_height, SLOT(setValue(int)));
        disconnect(ui->spinBox_height, SIGNAL(valueChanged(int)), ui->spinBox_width, SLOT(setValue(int)));
    }
}

void RenderSetup::updatePlotCounter()
{
    auto list = ui->listView->selectionModel()->selectedIndexes();
    int n = list.size();

    if(ui->groupBox_timeSeries->isChecked()){
        int nt = duration_/ui->spinBox_increment->value();
        n *= nt;
    }

    ui->spinBox_numberOfPlots->setValue(n);
}

void RenderSetup::on_pushButton_browse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                "/home",
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);

    if(!dir.isEmpty()){
        ui->lineEdit_output->setText(dir);
    }
}
