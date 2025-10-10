#include "simulatorwidget.h"
#include "ui_simulatorwidget.h"

SimulatorWidget::SimulatorWidget(QStandardItemModel *model, QDateTimeEdit *start, QWidget *parent) :
    QWidget(parent),
    start{start},
    ui(new Ui::SimulatorWidget),
    model_{model}
{

    ui->setupUi(this);

    unsigned long seed = std::chrono::system_clock::now().time_since_epoch().count();
    int iseed = seed%(std::numeric_limits<int>::max());
    ui->spinBox_seed->setValue(iseed);
    auto hv = ui->treeWidget_simpara->header();
    hv->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeWidget_simpara->setSelectionMode(QAbstractItemView::NoSelection);

    connect(ui->treeWidget_simpara,
            SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this,
            SLOT(toggleAll(QTreeWidgetItem*, int)));
}

SimulatorWidget::~SimulatorWidget()
{
    delete ui;
}

void SimulatorWidget::addStations(QStandardItem *dummy)
{
    if (block ){
        return;
    }
    //if(dummy != nullptr){
    //    if(dummy->column() != 0){
    //        return;
    //    }
    //}
    QStringList stations;
    stations << "__all__";
    for( int i = 0; i<model_->rowCount(); ++i){
        stations << model_->item(i,0)->text();
    }

    QTreeWidget *t = ui->treeWidget_simpara;
    t->clear();


    int r = 0;
    for(const auto &sta : stations){
        QTreeWidgetItem *item = new QTreeWidgetItem();
        bool enable = false;
        if(sta == "__all__"){
            item->setIcon(0,QIcon(":/icons/icons/station_group.png"));
            item->setCheckState(0, Qt::Checked);
            enable = true;
        }else{
            item->setFlags(item->flags() & (~Qt::ItemIsEnabled));
            item->setIcon(0,QIcon(":/icons/icons/station.png"));
        }

        item->setText(0,sta);
        t->addTopLevelItem(item);

        int c = 1;
        QDoubleSpinBox *wn = new QDoubleSpinBox();
        wn->setRange(0,250);
        wn->setSingleStep(2.5);
        wn->setValue(17.68);
        wn->setDecimals(2);
        wn->setSuffix(" [ps]");
        wn->setEnabled(enable);
        t->setItemWidget(item,c++,wn);

        QDoubleSpinBox *clockASD = new QDoubleSpinBox();
        clockASD->setRange(0,100);
        clockASD->setSingleStep(0.1);
        clockASD->setDecimals(4);
        clockASD->setValue(1.0);
        clockASD->setSuffix("e-14 [s]");
        clockASD->setEnabled(enable);
        t->setItemWidget(item,c++,clockASD);

        QDoubleSpinBox *clockDur = new QDoubleSpinBox();
        clockDur->setRange(0,1440);
        clockDur->setSingleStep(5);
        clockDur->setDecimals(0);
        clockDur->setValue(50);
        clockDur->setSuffix(" [min]");
        clockDur->setEnabled(enable);
        t->setItemWidget(item,c++,clockDur);

        QDoubleSpinBox *tropo_Cn = new QDoubleSpinBox();
        tropo_Cn->setRange(0,10);
        tropo_Cn->setSingleStep(.1);
        tropo_Cn->setDecimals(2);
        tropo_Cn->setValue(1.8);
        tropo_Cn->setSuffix("e-7 [m^-1/3]");
        tropo_Cn->setEnabled(enable);
        t->setItemWidget(item,c++,tropo_Cn);

        QDoubleSpinBox *tropo_H = new QDoubleSpinBox();
        tropo_H->setRange(0,10000);
        tropo_H->setSingleStep(500);
        tropo_H->setDecimals(0);
        tropo_H->setValue(2000);
        tropo_H->setSuffix(" [m]");
        tropo_H->setEnabled(enable);
        t->setItemWidget(item,c++,tropo_H);

        QDoubleSpinBox *tropo_dH = new QDoubleSpinBox();
        tropo_dH->setRange(0,1000);
        tropo_dH->setSingleStep(50);
        tropo_dH->setDecimals(0);
        tropo_dH->setValue(200);
        tropo_dH->setSuffix(" [m]");
        tropo_dH->setEnabled(enable);
        t->setItemWidget(item,c++,tropo_dH);

        QDoubleSpinBox *tropo_dHseg = new QDoubleSpinBox();
        tropo_dHseg->setRange(0,24);
        tropo_dHseg->setSingleStep(.5);
        tropo_dHseg->setDecimals(2);
        tropo_dHseg->setValue(2);
        tropo_dHseg->setSuffix(" [h]");
        tropo_dHseg->setEnabled(enable);
        t->setItemWidget(item,c++,tropo_dHseg);

        QDoubleSpinBox *tropo_ve = new QDoubleSpinBox();
        tropo_ve->setRange(-100,100);
        tropo_ve->setSingleStep(1);
        tropo_ve->setDecimals(1);
        tropo_ve->setValue(8);
        tropo_ve->setSuffix(" [m/s]");
        tropo_ve->setEnabled(enable);
        t->setItemWidget(item,c++,tropo_ve);

        QDoubleSpinBox *tropo_vn = new QDoubleSpinBox();
        tropo_vn->setRange(-100,100);
        tropo_vn->setSingleStep(1);
        tropo_vn->setDecimals(1);
        tropo_vn->setValue(0);
        tropo_vn->setSuffix(" [m/s]");
        tropo_vn->setEnabled(enable);
        t->setItemWidget(item,c++,tropo_vn);

        QDoubleSpinBox *tropo_wzd0 = new QDoubleSpinBox();
        tropo_wzd0->setRange(0,1000);
        tropo_wzd0->setSingleStep(25);
        tropo_wzd0->setDecimals(1);
        tropo_wzd0->setValue(150);
        tropo_wzd0->setSuffix(" [mm]");
        tropo_wzd0->setEnabled(enable);
        t->setItemWidget(item,c++,tropo_wzd0);

        if(r == 0){
            connect(wn, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [t](){
                int c = 1;
                int rmax = t->topLevelItemCount();
                QTreeWidgetItem *ref = t->topLevelItem(0);
                double val = qobject_cast<QDoubleSpinBox *>(t->itemWidget(ref,c))->value();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t->topLevelItem(r);
                    qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c))->setValue(val);
                }
            });

            connect(clockASD, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [t](){
                int c = 2;
                int rmax = t->topLevelItemCount();
                QTreeWidgetItem *ref = t->topLevelItem(0);
                double val = qobject_cast<QDoubleSpinBox *>(t->itemWidget(ref,c))->value();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t->topLevelItem(r);
                    qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c))->setValue(val);
                }
            });
            connect(clockDur, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [t](){
                int c = 3;
                int rmax = t->topLevelItemCount();
                QTreeWidgetItem *ref = t->topLevelItem(0);
                double val = qobject_cast<QDoubleSpinBox *>(t->itemWidget(ref,c))->value();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t->topLevelItem(r);
                    qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c))->setValue(val);
                }
            });

            connect(tropo_Cn, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [t](){
                int c = 4;
                int rmax = t->topLevelItemCount();
                QTreeWidgetItem *ref = t->topLevelItem(0);
                double val = qobject_cast<QDoubleSpinBox *>(t->itemWidget(ref,c))->value();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t->topLevelItem(r);
                    qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c))->setValue(val);
                }
            });
            connect(tropo_H, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [t](){
                int c = 5;
                int rmax = t->topLevelItemCount();
                QTreeWidgetItem *ref = t->topLevelItem(0);
                double val = qobject_cast<QDoubleSpinBox *>(t->itemWidget(ref,c))->value();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t->topLevelItem(r);
                    qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c))->setValue(val);
                }
            });
            connect(tropo_dH, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [t](){
                int c = 6;
                int rmax = t->topLevelItemCount();
                QTreeWidgetItem *ref = t->topLevelItem(0);
                double val = qobject_cast<QDoubleSpinBox *>(t->itemWidget(ref,c))->value();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t->topLevelItem(r);
                    qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c))->setValue(val);
                }
            });
            connect(tropo_dHseg, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [t](){
                int c = 7;
                int rmax = t->topLevelItemCount();
                QTreeWidgetItem *ref = t->topLevelItem(0);
                double val = qobject_cast<QDoubleSpinBox *>(t->itemWidget(ref,c))->value();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t->topLevelItem(r);
                    qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c))->setValue(val);
                }
            });
            connect(tropo_ve, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [t](){
                int c = 8;
                int rmax = t->topLevelItemCount();
                QTreeWidgetItem *ref = t->topLevelItem(0);
                double val = qobject_cast<QDoubleSpinBox *>(t->itemWidget(ref,c))->value();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t->topLevelItem(r);
                    qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c))->setValue(val);
                }
            });
            connect(tropo_vn, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [t](){
                int c = 9;
                int rmax = t->topLevelItemCount();
                QTreeWidgetItem *ref = t->topLevelItem(0);
                double val = qobject_cast<QDoubleSpinBox *>(t->itemWidget(ref,c))->value();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t->topLevelItem(r);
                    qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c))->setValue(val);
                }
            });
            connect(tropo_wzd0, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [t](){
                int c = 10;
                int rmax = t->topLevelItemCount();
                QTreeWidgetItem *ref = t->topLevelItem(0);
                double val = qobject_cast<QDoubleSpinBox *>(t->itemWidget(ref,c))->value();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t->topLevelItem(r);
                    qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c))->setValue(val);
                }
            });
        }

        ++r;
    }
}

boost::property_tree::ptree SimulatorWidget::toXML()
{
    QTreeWidget *t = ui->treeWidget_simpara;
    int rmax = t->topLevelItemCount();

    boost::property_tree::ptree tree;
    tree.add("simulator.number_of_simulations", ui->spinBox_simulations->value());
    if(ui->checkBox_seed->isChecked()){
        tree.add("simulator.seed", ui->spinBox_seed->value());
    }
    if(t->topLevelItem(0)->checkState(0) == Qt::Checked){
          QTreeWidgetItem *itm = t->topLevelItem(0);
          int c = 1;
          double wn = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();

          double clockASD = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();
          double clockDur = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();

          double tropo_Cn = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();
          double tropo_H = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();
          double tropo_dH = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();
          double tropo_dHseg = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();
          double tropo_ve = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();
          double tropo_vn = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();
          double tropo_wzd0 = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();

          tree.add("simulator.station.wn",wn);
          tree.add("simulator.station.<xmlattr>.name","__all__");

          tree.add("simulator.station.clockASD",clockASD);
          tree.add("simulator.station.clockDur",clockDur);

          tree.add("simulator.station.tropo_Cn",tropo_Cn);
          tree.add("simulator.station.tropo_H",tropo_H);
          tree.add("simulator.station.tropo_dH",tropo_dH);
          tree.add("simulator.station.tropo_dHseg",tropo_dHseg);
          tree.add("simulator.station.tropo_ve",tropo_ve);
          tree.add("simulator.station.tropo_vn",tropo_vn);
          tree.add("simulator.station.tropo_wzd0",tropo_wzd0);
    }else{

        for (int r = 1; r<rmax; ++r){
            boost::property_tree::ptree staTree;
            QTreeWidgetItem *itm = t->topLevelItem(r);
            int c = 1;
            double wn = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();

            double clockASD = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();
            double clockDur = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();

            double tropo_Cn = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();
            double tropo_H = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();
            double tropo_dH = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();
            double tropo_dHseg = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();
            double tropo_ve = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();
            double tropo_vn = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();
            double tropo_wzd0 = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,c++))->value();

            QString itmName = itm->text(0);
            staTree.add("station.<xmlattr>.name", itmName.toStdString());

            staTree.add("station.wn",wn);

            staTree.add("station.clockASD",clockASD);
            staTree.add("station.clockDur",clockDur);

            staTree.add("station.tropo_Cn",tropo_Cn);
            staTree.add("station.tropo_H",tropo_H);
            staTree.add("station.tropo_dH",tropo_dH);
            staTree.add("station.tropo_dHseg",tropo_dHseg);
            staTree.add("station.tropo_ve",tropo_ve);
            staTree.add("station.tropo_vn",tropo_vn);
            staTree.add("station.tropo_wzd0",tropo_wzd0);

            tree.add_child("simulator.station",staTree.get_child("station"));
        }
    }
    tree.add("simulator.output.obs_minus_comp", ui->checkBox_obs_minus_comp->isChecked());
    return tree;
}

void SimulatorWidget::fromXML(const boost::property_tree::ptree &tree)
{
    QTreeWidget *t = ui->treeWidget_simpara;
    int rmax = t->topLevelItemCount();
    ui->spinBox_simulations->setValue(tree.get("number_of_simulations",1000));

    auto tmp = tree.get_optional<int>("seed");
    if(tmp.is_initialized()){
        ui->checkBox_seed->setCheckState(Qt::Checked);
        ui->spinBox_seed->setValue(*tmp);
    }

    for(const auto &any : tree){
        if(any.first == "station"){
            QString name = QString::fromStdString(any.second.get("<xmlattr>.name",""));
            if(name == "__all__"){
                QTreeWidgetItem *itm = t->topLevelItem(0);
                itm->setCheckState(0,Qt::Checked);

                double wn = any.second.get("wn",17.68);

                double clockASD = any.second.get("clockASD",1.);
                double clockDur = any.second.get("clockDur",50.);

                double tropo_Cn = any.second.get("tropo_Cn",1.8);
                double tropo_H = any.second.get("tropo_H",2000.);
                double tropo_dH = any.second.get("tropo_dH",200.);
                double tropo_dHseg = any.second.get("tropo_dHseg",2.);
                double tropo_ve = any.second.get("tropo_ve",8.);
                double tropo_vn = any.second.get("tropo_vn",0.);
                double tropo_wzd0 = any.second.get("tropo_wzd0",150.);

                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,1))->setValue(wn);

                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,2))->setValue(clockASD);
                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,3))->setValue(clockDur);

                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,4))->setValue(tropo_Cn);
                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,5))->setValue(tropo_H);
                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,6))->setValue(tropo_dH);
                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,7))->setValue(tropo_dHseg);
                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,8))->setValue(tropo_ve);
                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,9))->setValue(tropo_vn);
                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,10))->setValue(tropo_wzd0);

            }else{
                t->topLevelItem(0)->setCheckState(0,Qt::Unchecked);
                QTreeWidgetItem *itm = nullptr;
                for(int r=1; r<rmax; ++r){
                    if(t->topLevelItem(r)->text(0) == name){
                        itm = t->topLevelItem(r);
                        break;
                    }
                }

                if(itm == nullptr){
                    continue;
                }

                double wn = any.second.get("wn",17.68);

                double clockASD = any.second.get("clockASD",1.);
                double clockDur = any.second.get("clockDur",50.);

                double tropo_Cn = any.second.get("tropo_Cn",1.8);
                double tropo_H = any.second.get("tropo_H",2000.);
                double tropo_dH = any.second.get("tropo_dH",200.);
                double tropo_dHseg = any.second.get("tropo_dHseg",2.);
                double tropo_ve = any.second.get("tropo_ve",8.);
                double tropo_vn = any.second.get("tropo_vn",0.);
                double tropo_wzd0 = any.second.get("tropo_wzd0",150.);

                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,1))->setValue(wn);

                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,2))->setValue(clockASD);
                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,3))->setValue(clockDur);

                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,4))->setValue(tropo_Cn);
                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,5))->setValue(tropo_H);
                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,6))->setValue(tropo_dH);
                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,7))->setValue(tropo_dHseg);
                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,8))->setValue(tropo_ve);
                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,9))->setValue(tropo_vn);
                qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,10))->setValue(tropo_wzd0);
            }
        }
    }

    ui->checkBox_obs_minus_comp->setChecked(tree.get("output.obs_minus_comp", false));

}

void SimulatorWidget::toggleAll(QTreeWidgetItem *item, int column)
{
    if(item->text(0) == "__all__" && column == 0){
        bool checked = item->checkState(0);

        QTreeWidget *t = ui->treeWidget_simpara;
        int rmax = t->topLevelItemCount();
        int cmax = t->columnCount()-1;
        for(int r=0; r<rmax; ++r){
            bool flag = !checked;

            QTreeWidgetItem *itm = t->topLevelItem(r);
            if(r==0){
                flag = checked;
            }
            int c = 1;

            if(r>0){
                if(flag){
                    itm->setFlags(item->flags() | Qt::ItemIsEnabled);
                }else{
                    itm->setFlags(item->flags() & (~Qt::ItemIsEnabled));
                }
            }
            for (int c = 1; c<cmax; ++c){
                t->itemWidget(itm,c)->setEnabled(flag);
            }
        }
    }
}

void SimulatorWidget::on_pushButton_wnTable_clicked()
{
    QDialog *d = new QDialog(this);
    QHBoxLayout *l = new QHBoxLayout();
    QTreeWidget *t = new QTreeWidget(d);
    l->addWidget(t);
    d->setLayout(l);

    t->setColumnCount(2);
    t->setHeaderItem(new QTreeWidgetItem(QStringList() << "wn per paseline" << "wn per station"));
    for(double d = 0.1; d< 1; d+=.1){
        QTreeWidgetItem *itm = new QTreeWidgetItem();
        itm->setText(0, QString::number(d,'f',2) + " [ps]");
        itm->setTextAlignment(0,Qt::AlignRight | Qt::AlignVCenter);
        itm->setText(1, QString::number(d/std::sqrt(2),'f',2) + " [ps]");
        itm->setTextAlignment(1,Qt::AlignRight | Qt::AlignVCenter);
        t->addTopLevelItem(itm);
    }
    for(double d = 1; d< 10; ++d){
        QTreeWidgetItem *itm = new QTreeWidgetItem();
        itm->setText(0, QString::number(d,'f',2) + " [ps]");
        itm->setTextAlignment(0,Qt::AlignRight | Qt::AlignVCenter);
        itm->setText(1, QString::number(d/std::sqrt(2),'f',2) + " [ps]");
        itm->setTextAlignment(1,Qt::AlignRight | Qt::AlignVCenter);
        t->addTopLevelItem(itm);
    }
    for(double d = 10; d< 20; d+=2){
        QTreeWidgetItem *itm = new QTreeWidgetItem();
        itm->setText(0, QString::number(d,'f',2) + " [ps]");
        itm->setTextAlignment(0,Qt::AlignRight | Qt::AlignVCenter);
        itm->setText(1, QString::number(d/std::sqrt(2),'f',2) + " [ps]");
        itm->setTextAlignment(1,Qt::AlignRight | Qt::AlignVCenter);
        t->addTopLevelItem(itm);
    }
    for(double d = 20; d< 50; d+=5){
        QTreeWidgetItem *itm = new QTreeWidgetItem();
        itm->setText(0, QString::number(d,'f',2) + " [ps]");
        itm->setTextAlignment(0,Qt::AlignRight | Qt::AlignVCenter);
        itm->setText(1, QString::number(d/std::sqrt(2),'f',2) + " [ps]");
        itm->setTextAlignment(1,Qt::AlignRight | Qt::AlignVCenter);
        t->addTopLevelItem(itm);
    }
    for(double d = 50; d< 100; d+=10){
        QTreeWidgetItem *itm = new QTreeWidgetItem();
        itm->setText(0, QString::number(d,'f',2) + " [ps]");
        itm->setTextAlignment(0,Qt::AlignRight | Qt::AlignVCenter);
        itm->setText(1, QString::number(d/std::sqrt(2),'f',2) + " [ps]");
        itm->setTextAlignment(1,Qt::AlignRight | Qt::AlignVCenter);
        t->addTopLevelItem(itm);
    }
    t->header()->resizeSections(QHeaderView::ResizeToContents);

    d->show();
}

void SimulatorWidget::on_pushButton_Cn_clicked()
{
    int month = start->date().month();

    // 1. Uncheck first-level item at (0,0)
    QTreeWidgetItem *firstItem = ui->treeWidget_simpara->topLevelItem(0);
    if (firstItem) {
        firstItem->setCheckState(0, Qt::Unchecked);
    }

    // 2. Collect station names from column 1 starting row 2
    QStringList stationList;
    int rowCount = ui->treeWidget_simpara->topLevelItemCount();
    for (int i = 1; i < rowCount; ++i) {  // start from row 2
        QTreeWidgetItem *item = ui->treeWidget_simpara->topLevelItem(i);
        if (item) {
            QString station = item->text(0);  // column 1
            if (!station.isEmpty())
                stationList << station;
        }
    }
    QString warnings;
    // 3. Parse catalog file
    QFile file_Cn("./AUTO_DOWNLOAD_CATALOGS/VLBI_Cn.cat");
    QMap<QString, QVector<double>> stationData_Cn;
    if (!file_Cn.open(QIODevice::ReadOnly | QIODevice::Text)) {
        warnings += "Could not open VLBI_Cn.cat file!\n";
    } else {
        QTextStream in_Cn(&file_Cn);
        while (!in_Cn.atEnd()) {
            QString line = in_Cn.readLine().trimmed();
            if (line.startsWith("*") || line.isEmpty())
                continue;

            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() < 14)  // Name + ID + 12 months
                continue;

            QString stationName = parts[0];
            QVector<double> values;
            for (int m = 2; m < parts.size(); ++m) { // skip Name + ID
                values.append(parts[m].toDouble());
            }
            stationData_Cn[stationName] = values;
        }
        file_Cn.close();
    }



    // 3. Parse catalog file
    QFile file_v("./AUTO_DOWNLOAD_CATALOGS/VLBI_v.cat");
    QMap<QString, QVector<double>> stationData_v;
    if (!file_v.open(QIODevice::ReadOnly | QIODevice::Text)) {
        warnings += "Could not open VLBI_v.cat file!\n";
    }else{
        QTextStream in_v(&file_v);

        while (!in_v.atEnd()) {
            QString line = in_v.readLine().trimmed();
            if (line.startsWith("*") || line.isEmpty())
                continue;

            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() < 14)  // Name + ID + 12 months
                continue;

            QString stationName = parts[0];
            QVector<double> values;
            for (int m = 2; m < parts.size(); ++m) { // skip Name + ID
                values.append(parts[m].toDouble());
            }
            stationData_v[stationName] = values;
        }
        file_v.close();
    }

    int monthIndex = month - 1;
    // 4. Assign values into column 5 (index 4)
    for (int i = 1; i < rowCount; ++i) {
        QTreeWidgetItem *item = ui->treeWidget_simpara->topLevelItem(i);
        if (!item) continue;

        QString station = item->text(0);
        double value_Cn = 1.8;  // default

        if (stationData_Cn.contains(station)) {
            QVector<double> values = stationData_Cn[station];
            if (monthIndex >= 0 && monthIndex < values.size())
                value_Cn = values[monthIndex];
        }else{
            warnings += "Missing Cn information for station " + station + "\n";
        }

        QWidget *widget_Cn = ui->treeWidget_simpara->itemWidget(item, 4); // column index 4
        if (QDoubleSpinBox *spin_Cn = qobject_cast<QDoubleSpinBox*>(widget_Cn)) {
            spin_Cn->setValue(value_Cn);
        }

        double value_v = 8;  // default
        if (stationData_v.contains(station)) {
            QVector<double> values = stationData_v[station];
            if (monthIndex >= 0 && monthIndex < values.size())
                value_v = values[monthIndex];
        }else{
            warnings += "Missing v information for station " + station + "\n";
        }

        QWidget *widget_v = ui->treeWidget_simpara->itemWidget(item, 8); // column index 4
        if (QDoubleSpinBox *spin_v = qobject_cast<QDoubleSpinBox*>(widget_v)) {
            spin_v->setValue(value_v);
        }
    }
    if (!warnings.isEmpty()){
        QMessageBox::warning(
            this,
            "Missing data",
            warnings
        );
    }
}


void SimulatorWidget::on_pushButton_zwd_clicked()
{
    int month = start->date().month();

    // 1. Uncheck first-level item at (0,0)
    QTreeWidgetItem *firstItem = ui->treeWidget_simpara->topLevelItem(0);
    if (firstItem) {
        firstItem->setCheckState(0, Qt::Unchecked);
    }

    // 2. Collect station names from column 1 starting row 2
    QStringList stationList;
    int rowCount = ui->treeWidget_simpara->topLevelItemCount();
    for (int i = 1; i < rowCount; ++i) {  // start from row 2
        QTreeWidgetItem *item = ui->treeWidget_simpara->topLevelItem(i);
        if (item) {
            QString station = item->text(0);  // column 1
            if (!station.isEmpty())
                stationList << station;
        }
    }
    QString warnings;
    // 3. Parse catalog file
    QFile file_zwd("./AUTO_DOWNLOAD_CATALOGS/VLBI_zwd0.cat");
    QMap<QString, QVector<double>> stationData_zwd;
    if (!file_zwd.open(QIODevice::ReadOnly | QIODevice::Text)) {
        warnings += "Could not open VLBI_zwd0.cat file!\n";
    } else {
        QTextStream in_zwd(&file_zwd);
        while (!in_zwd.atEnd()) {
            QString line = in_zwd.readLine().trimmed();
            if (line.startsWith("*") || line.isEmpty())
                continue;

            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() < 14)  // Name + ID + 12 months
                continue;

            QString stationName = parts[0];
            QVector<double> values;
            for (int m = 2; m < parts.size(); ++m) { // skip Name + ID
                values.append(parts[m].toDouble());
            }
            stationData_zwd[stationName] = values;
        }
        file_zwd.close();
    }

    int monthIndex = month - 1;
    // 4. Assign values into column 5 (index 4)
    for (int i = 1; i < rowCount; ++i) {
        QTreeWidgetItem *item = ui->treeWidget_simpara->topLevelItem(i);
        if (!item) continue;

        QString station = item->text(0);
        double value_zwd = 1.8;  // default

        if (stationData_zwd.contains(station)) {
            QVector<double> values = stationData_zwd[station];
            if (monthIndex >= 0 && monthIndex < values.size())
                value_zwd = values[monthIndex];
        }else{
            warnings += "Missing zwd information for station " + station + "\n";
        }

        QWidget *widget_zwd = ui->treeWidget_simpara->itemWidget(item, 10); // column index 4
        if (QDoubleSpinBox *spin_zwd = qobject_cast<QDoubleSpinBox*>(widget_zwd)) {
            spin_zwd->setValue(value_zwd);
        }
    }
    if (!warnings.isEmpty()){
        QMessageBox::warning(
            this,
            "Missing data",
            warnings
        );
    }
}

