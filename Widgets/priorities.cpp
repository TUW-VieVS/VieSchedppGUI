#include "priorities.h"
#include "ui_priorities.h"

Priorities::Priorities(QStandardItemModel *stations, QStandardItemModel *sources, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Priorities),
    stations_{stations},
    sources_{sources}
{
    ui->setupUi(this);
    setup();

}

Priorities::~Priorities()
{
    delete ui;
}

boost::property_tree::ptree Priorities::toXML()
{
    auto *t = ui->treeWidget_params;

    boost::property_tree::ptree tree;
    int v = ui->horizontalSlider_recom->value();
    tree.add("priorities.fraction", v*10);
    tree.add("priorities.percentile",ui->doubleSpinBox_quantile->value());


    QTreeWidgetItem *itm_nobs = t->topLevelItem(0);
    auto *dsp_nobs = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_nobs,1));
    boost::property_tree::ptree t_nobs;
    t_nobs.add("variable", dsp_nobs->value());
    t_nobs.add("variable.<xmlattr>.name", itm_nobs->text(0).toStdString());
    tree.add_child("priorities.variable",t_nobs.get_child("variable"));



    QTreeWidgetItem *itm_eop = t->topLevelItem(1);
    for( int i = 0; i<itm_eop->childCount(); ++i){
        QTreeWidgetItem *itm = itm_eop->child(i);
        auto *dsp = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,1));
        boost::property_tree::ptree t2;

        t2.add("variable", dsp->value());
        t2.add("variable.<xmlattr>.name", itm->text(0).toStdString());
        tree.add_child("priorities.variable",t2.get_child("variable"));
    }

    QTreeWidgetItem *itm_sta = t->topLevelItem(2);
    double sta_first_val = -1;
    bool sta_allTheSame = true;
    for( int i = 0; i<itm_sta->childCount(); ++i){
        QTreeWidgetItem *itm = itm_sta->child(i);
        auto *dsp = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,1));
        double val = dsp->value();
        if (sta_first_val == -1){
            sta_first_val = val;
        }else{
            if(val != sta_first_val){
                sta_allTheSame = false;
                break;
            }
        }
    }
    if( sta_allTheSame){
        boost::property_tree::ptree t2;
        t2.add("variable", sta_first_val);
        t2.add("variable.<xmlattr>.name", "stations");
        tree.add_child("priorities.variable",t2.get_child("variable"));
    }else{
        for( int i = 0; i<itm_sta->childCount(); ++i){
            QTreeWidgetItem *itm = itm_sta->child(i);
            auto *dsp = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,1));
            boost::property_tree::ptree t2;

            t2.add("variable", dsp->value());
            t2.add("variable.<xmlattr>.name", itm->text(0).toStdString());
            tree.add_child("priorities.variable",t2.get_child("variable"));
        }
    }


    QTreeWidgetItem *itm_src = t->topLevelItem(3);
    double src_first_val = -1;
    bool src_allTheSame = true;
    for( int i = 0; i<itm_src->childCount(); ++i){
        QTreeWidgetItem *itm = itm_src->child(i);
        auto *dsp = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,1));
        double val = dsp->value();
        if (src_first_val == -1){
            src_first_val = val;
        }else{
            if(val != src_first_val){
                src_allTheSame = false;
                break;
            }
        }
    }
    if( src_allTheSame){
        boost::property_tree::ptree t2;
        t2.add("variable", src_first_val);
        t2.add("variable.<xmlattr>.name", "sources");
        tree.add_child("priorities.variable",t2.get_child("variable"));
    }else{
        for( int i = 0; i<itm_src->childCount(); ++i){
            QTreeWidgetItem *itm = itm_src->child(i);
            auto *dsp = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,1));
            boost::property_tree::ptree t2;

            t2.add("variable", dsp->value());
            t2.add("variable.<xmlattr>.name", itm->text(0).toStdString());
            tree.add_child("priorities.variable",t2.get_child("variable"));
        }
    }

    QTreeWidgetItem *itm_scale = t->topLevelItem(4);
    auto *dsp_scale = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_scale,1));
    boost::property_tree::ptree t_scale;
    t_scale.add("variable", dsp_scale->value());
    t_scale.add("variable.<xmlattr>.name", itm_scale->text(0).toStdString());
    tree.add_child("priorities.variable",t_scale.get_child("variable"));

    return tree;
}

void Priorities::fromXML(const boost::property_tree::ptree &tree)
{
    int v = tree.get("priorities.fraction", 70);
    ui->horizontalSlider_recom->setValue(v/10);

    ui->doubleSpinBox_quantile->setValue(tree.get("priorities.percentile",0.75));
    auto *t = ui->treeWidget_params;


    QTreeWidgetItem *itm_src = t->topLevelItem(2);
    for( int i = 0; i<itm_src->childCount(); ++i){
        QTreeWidgetItem *itm = itm_src->child(i);
        auto *dsp = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,1));
        dsp->setValue(0);
    }



    for( const auto &any : tree){
        if(any.first != "variable"){
            continue;
        }
        QString name = QString::fromStdString(any.second.get("<xmlattr>.name",""));
        double val = QString::fromStdString(any.second.data()).toDouble();


        for(int tli = 0; tli<t->topLevelItemCount(); ++tli){
            QTreeWidgetItem *itm_tl = t->topLevelItem(tli);
            QDoubleSpinBox *dsp_tl = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_tl, 1));
            if(itm_tl->text(0) == name){

                if ( name == "stations" || name == "sources"){
                    val *= itm_tl->childCount();
                }

                dsp_tl->setValue(val);
                break;
            }else{
                for(int c = 0; c<itm_tl->childCount(); ++c){
                    QTreeWidgetItem *itm_c = itm_tl->child(c);
                    QDoubleSpinBox *dsp_c = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_c, 1));
                    QString itm_name = itm_c->text(0);
                    if( itm_name == name){
                        dsp_c->setValue(val);
                        break;
                    }
                }
            }
        }
    }
    paintBars();

}

void Priorities::addStations(QStandardItem *)
{
    if ( !block ){
        setup();
    }
}

void Priorities::addSources(QStandardItem *)
{
    if ( !block ){
        setup();
    }
}

void Priorities::setup()
{
    ui->treeWidget_params->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    auto *t = ui->treeWidget_params;

    auto *sta = t->topLevelItem(2);
    qDeleteAll(sta->takeChildren());
    sta->setIcon(0, QIcon(":/icons/icons/station_group.png"));

    auto *src = t->topLevelItem(3);
    qDeleteAll(src->takeChildren());
    src->setIcon(0, QIcon(":/icons/icons/source_group.png"));

    for( int i = 0; i<stations_->rowCount(); ++i){
        QString name = stations_->item(i,0)->text();
        QTreeWidgetItem *itm = new QTreeWidgetItem(QStringList() << name);
        itm->setIcon(0, QIcon(":/icons/icons/station.png"));
        sta->addChild(itm);
    }

    for( int i = 0; i<sources_->rowCount(); ++i){
        QString name = sources_->item(i,0)->text();
        QTreeWidgetItem *itm = new QTreeWidgetItem(QStringList() << name);
        itm->setIcon(0, QIcon(":/icons/icons/source.png"));
        src->addChild(itm);
    }

    for(int tli = 0; tli<t->topLevelItemCount(); ++tli){
        QTreeWidgetItem *itm_tl = t->topLevelItem(tli);
        QDoubleSpinBox *a = new QDoubleSpinBox();
        a->setValue(1);
        a->setDecimals(4);
        if(tli == 4 || tli == 3){
            a->setValue(0);
        }
        a->setRange(0,100);
        a->setSingleStep(.1);
        t->setItemWidget(itm_tl, 1, a);
        if(tli == 0){
            connect(a, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Priorities::paintBars);
        }
        if(tli == 1){
            connect(a, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Priorities::averageEOP);
        }
        if(tli == 2){
            connect(a, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Priorities::averageSta);
        }
        if(tli == 3){
            connect(a, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Priorities::averageSrc);
            itm_tl->setText(0,"sources");
        }
        if(tli == 4){
            connect(a, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Priorities::paintBars);
            itm_tl->setText(0,"scale");
        }
        QProgressBar *b = new QProgressBar();
        b->setOrientation(Qt::Horizontal);
        b->setRange(0,100);
        b->setTextVisible(true);
        t->setItemWidget(itm_tl, 2, b);

        for(int c=0; c<itm_tl->childCount(); ++c){
            QTreeWidgetItem *itm_c = itm_tl->child(c);
            QDoubleSpinBox *a = new QDoubleSpinBox();
            if(tli == 1){
                a->setValue(1./5.);
            }
            if(tli == 2){
                a->setValue(1./stations_->rowCount());
            }


            a->setRange(0,100);
            a->setDecimals(6);
            a->setSingleStep(.1);
            t->setItemWidget(itm_c, 1, a);
            connect(a, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Priorities::paintBars);

            QProgressBar *b = new QProgressBar();
            b->setOrientation(Qt::Horizontal);
            b->setRange(0,100);
            b->setTextVisible(true);
            t->setItemWidget(itm_c, 2, b);
        }
    }
    t->expandAll();
    paintBars();
}

void Priorities::averageEOP()
{
    blockPaint = true;
    auto *t = ui->treeWidget_params;
    QTreeWidgetItem *itm = t->topLevelItem(1);
    double v = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,1))->value();

    double total = 0;
    for(int i = 0; i<itm->childCount(); ++i){
        QTreeWidgetItem *itm_c = itm->child(i);
        total += qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_c,1))->value();
    }

    for(int i = 0; i<itm->childCount(); ++i){
        QTreeWidgetItem *itm_c = itm->child(i);
        auto dsp = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_c,1));
        if( total == 0){
            dsp->setValue(v/itm->childCount());
        } else{
            double v_c = dsp->value();
            v_c = v_c/total;
            dsp->setValue(v*v_c);
        }
    }
    blockPaint = false;
    paintBars();
}

void Priorities::averageSta()
{
    blockPaint = true;
    auto *t = ui->treeWidget_params;
    QTreeWidgetItem *itm = t->topLevelItem(2);
    double v = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,1))->value();

    double total = 0;
    for(int i = 0; i<itm->childCount(); ++i){
        QTreeWidgetItem *itm_c = itm->child(i);
        total += qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_c,1))->value();
    }

    for(int i = 0; i<itm->childCount(); ++i){
        QTreeWidgetItem *itm_c = itm->child(i);
        auto dsp = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_c,1));
        if( total == 0){
            dsp->setValue(v/itm->childCount());
        } else{
            double v_c = dsp->value();
            v_c = v_c/total;
            dsp->setValue(v*v_c);
        }
    }
    blockPaint = false;
    paintBars();
}

void Priorities::averageSrc()
{
    blockPaint = true;
    auto *t = ui->treeWidget_params;
    QTreeWidgetItem *itm = t->topLevelItem(3);
    double v = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm,1))->value();

    double total = 0;
    for(int i = 0; i<itm->childCount(); ++i){
        QTreeWidgetItem *itm_c = itm->child(i);
        total += qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_c,1))->value();
    }

    for(int i = 0; i<itm->childCount(); ++i){
        QTreeWidgetItem *itm_c = itm->child(i);
        auto dsp = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_c,1));
        if( total == 0){
            double new_val = v/itm->childCount();
            dsp->setValue(new_val);
        } else{
            double v_c = dsp->value();
            v_c = v_c/total;
            dsp->setValue(v*v_c);
        }
    }
    blockPaint = false;
    paintBars();
}

void Priorities::paintBars()
{
    if(blockPaint){
        return;
    }

    auto *t = ui->treeWidget_params;
    double total = 0;

    QTreeWidgetItem *itm_nobs = t->topLevelItem(0);
    auto *dsp_nobs = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_nobs,1));
    double v_nobs = dsp_nobs->value();
    total += v_nobs;

    QTreeWidgetItem *itm_scale = t->topLevelItem(4);
    auto *dsp_scale = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_scale,1));
    double v_scale = dsp_scale->value();
    total += v_scale;


    QTreeWidgetItem *itm_eop = t->topLevelItem(1);
    auto *dsp_eop = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_eop,1));
    double v_eop = 0;
    for(int c=0; c<itm_eop->childCount(); ++c){
        QTreeWidgetItem *itm_c = itm_eop->child(c);
        auto *itm2 = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_c,1));
        double v = itm2->value();
        total+=v;
        v_eop+=v;
    }
    dsp_eop->setValue(v_eop);


    QTreeWidgetItem *itm_sta = t->topLevelItem(2);
    auto *dsp_sta = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_sta,1));
    double v_sta = 0;
    for(int c=0; c<itm_sta->childCount(); ++c){
        QTreeWidgetItem *itm_c = itm_sta->child(c);
        auto *itm2 = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_c,1));
        double v = itm2->value();
        total+=v;
        v_sta+=v;
    }
    dsp_sta->setValue(v_sta);

    QTreeWidgetItem *itm_src = t->topLevelItem(3);
    auto *dsp_src = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_src,1));
    double v_src = 0;
    for(int c=0; c<itm_src->childCount(); ++c){
        QTreeWidgetItem *itm_c = itm_src->child(c);
        auto *itm2 = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_c,1));
        double v = itm2->value();
        total+=v;
        v_src+=v;
    }
    dsp_src->setValue(v_src);
    for(int i = 0; i<t->topLevelItemCount(); ++i){
        QTreeWidgetItem *itm_tli = t->topLevelItem(i);
        auto *dsp_tli = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_tli,1));
        double v = dsp_tli->value();

        double p = 0;
        if(total > 0){
            p = v/total*100;
        }
        qobject_cast<QProgressBar *>(t->itemWidget(itm_tli,2))->setValue(std::lround(p));
        qobject_cast<QProgressBar *>(t->itemWidget(itm_tli,2))->setFormat(QString("%1%").arg(p,0,'f',2));


        for(int j = 0; j<itm_tli->childCount(); ++j){
            QTreeWidgetItem *itm_c = itm_tli->child(j);
            auto *dsp_c = qobject_cast<QDoubleSpinBox *>(t->itemWidget(itm_c,1));
            double v_c = dsp_c->value();

            double p = 0;
            if(total > 0){
                p = v_c/total*100;
            }
            qobject_cast<QProgressBar *>(t->itemWidget(itm_c,2))->setValue(std::lround(p));
            qobject_cast<QProgressBar *>(t->itemWidget(itm_c,2))->setFormat(QString("%1%").arg(p,0,'f',2));
        }
    }

}
