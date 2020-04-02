#include "priorities.h"
#include "ui_priorities.h"

Priorities::Priorities(QStandardItemModel *model, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Priorities),
    model_{model}
{
    ui->setupUi(this);
    setup();
    connect(ui->checkBox_EOP, SIGNAL(toggled(bool)), this, SLOT(setup()));
    connect(ui->checkBox_stations, SIGNAL(toggled(bool)), this, SLOT(setup()));
}

Priorities::~Priorities()
{
    delete ui;
}

boost::property_tree::ptree Priorities::toXML()
{
    auto *t = ui->tableWidget_params;

    boost::property_tree::ptree tree;
    if(ui->radioButton_mfe->isChecked()){
        tree.add("priorities.type","mean formal errors");
    }else if(ui->radioButton_rep->isChecked()){
        tree.add("priorities.type","repeatabilities");
    }else{
        tree.add("priorities.type","none");
    }
    tree.add("priorities.percentile",ui->doubleSpinBox_quantile->value());

    for( int i = 0; i<t->rowCount(); ++i){
        boost::property_tree::ptree t2;
        auto *itm = qobject_cast<QDoubleSpinBox *>(t->cellWidget(i,0));
        if(itm == nullptr){
            continue;
        }
        double v = itm->value();
        t2.add("variable", v);
        t2.add("variable.<xmlattr>.name", t->verticalHeaderItem(i)->text().toStdString());
        tree.add_child("priorities.variable",t2.get_child("variable"));
    }
    return tree;
}

void Priorities::fromXML(const boost::property_tree::ptree &tree)
{
    QString type = QString::fromStdString(tree.get("priorities.type","none"));
    if (type == "none"){
        ui->radioButton_noRec->setChecked(true);
    }else if(type == "repeatabilities"){
        ui->radioButton_rep->setChecked(true);
    }else if(type == "mean formal errors"){
        ui->radioButton_mfe->setChecked(true);
    }
    ui->doubleSpinBox_quantile->setValue(tree.get("priorities.percentile",0.75));

    auto *t = ui->tableWidget_params;
    t->clear();
    t->setRowCount(0);
    t->setHorizontalHeaderItem(0,new QTableWidgetItem("priority"));
    t->setHorizontalHeaderItem(1,new QTableWidgetItem(""));
    for( const auto &any : tree){
        if(any.first != "variable"){
            continue;
        }
        QString name = QString::fromStdString(any.second.get("<xmlattr>.name",""));
        double val = QString::fromStdString(any.second.data()).toDouble();
        addRow(name,val);

    }
    paintBars();

}

void Priorities::addStations(QStandardItem *)
{
    setup();
}

void Priorities::addRow(QString name, double val)
{
    auto *t = ui->tableWidget_params;
    int r = t->rowCount();
    t->insertRow(r);
    t->setVerticalHeaderItem(r, new QTableWidgetItem(name));

    QDoubleSpinBox *a = new QDoubleSpinBox();
    a->setValue(val);
    a->setRange(0,100);
    a->setSingleStep(.25);
    t->setCellWidget(r,0,a);
    connect(a, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Priorities::paintBars);

    QProgressBar *b = new QProgressBar();
    b->setOrientation(Qt::Horizontal);
    b->setRange(0,100);
    b->setTextVisible(true);
    t->setCellWidget(r,1,b);


    paintBars();
}

void Priorities::setup()
{
    auto *t = ui->tableWidget_params;
    t->clear();
    t->setRowCount(0);
    t->setHorizontalHeaderItem(0,new QTableWidgetItem("priority"));
    t->setHorizontalHeaderItem(1,new QTableWidgetItem(""));

    addRow("#obs");
    if(ui->checkBox_EOP->checkState() == Qt::Checked){
        addRow("XPO");
        addRow("YPO");
        addRow("dUT1");
        addRow("NUTX");
        addRow("NUTY");
    }else{
        addRow("EOP");
    }

    if(ui->checkBox_stations->checkState() == Qt::Checked){
        QStringList stations;
        for( int i = 0; i<model_->rowCount(); ++i){
            addRow( model_->item(i,0)->text());
        }
    }else{
        addRow("stations");
    }

}

void Priorities::paintBars()
{
    auto *t = ui->tableWidget_params;
    double total = 0;
    QVector<double> vals;
    for(int i = 0; i<t->rowCount(); ++i){
        auto *itm = qobject_cast<QDoubleSpinBox *>(t->cellWidget(i,0));
        if(itm == nullptr){
            continue;
        }
        double v = itm->value();
        vals << v;
        total+=v;
    }

    for(int i = 0; i<t->rowCount(); ++i){
        double p = 0;
        if(total > 0){
            p = vals[i]/total*100;
        }
        qobject_cast<QProgressBar *>(t->cellWidget(i,1))->setValue(std::lround(p));
        qobject_cast<QProgressBar *>(t->cellWidget(i,1))->setFormat(QString("%1%").arg(p,0,'f',2));
    }
}
