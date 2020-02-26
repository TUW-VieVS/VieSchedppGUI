#include "solver.h"
#include "ui_solver.h"

Solver::Solver(QStandardItemModel *model, QWidget *parent) :
      QWidget(parent),
      ui(new Ui::Solver),
      model_{model}
{
    ui->setupUi(this);

    auto hv1 = ui->treeWidget_sta_clock->header();
    hv1->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeWidget_sta_clock->setSelectionMode(QAbstractItemView::NoSelection);

    auto hv2 = ui->treeWidget_sta_coord->header();
    hv2->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeWidget_sta_coord->setSelectionMode(QAbstractItemView::NoSelection);

    auto hv3 = ui->treeWidget_sta_tropo->header();
    hv3->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeWidget_sta_tropo->setSelectionMode(QAbstractItemView::NoSelection);

    auto hv4 = ui->treeWidget_src_coord->header();
    hv4->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeWidget_src_coord->setSelectionMode(QAbstractItemView::NoSelection);

    connect(ui->treeWidget_sta_coord,
            SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this,
            SLOT(toggleAll_sta_coord(QTreeWidgetItem*, int)));

    connect(ui->treeWidget_sta_clock,
            SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this,
            SLOT(toggleAll_sta_clock(QTreeWidgetItem*, int)));

    connect(ui->treeWidget_sta_tropo,
            SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this,
            SLOT(toggleAll_sta_tropo(QTreeWidgetItem*, int)));

    ui->comboBox_ref_clock->setModel(model_);
}

Solver::~Solver()
{
    delete ui;
}

boost::property_tree::ptree Solver::toXML()
{
    QTreeWidget *t_clock = ui->treeWidget_sta_clock;
    QTreeWidget *t_coord = ui->treeWidget_sta_coord;
    QTreeWidget *t_tropo = ui->treeWidget_sta_tropo;

    bool all_sta = t_clock->topLevelItem(0)->checkState(0) == Qt::Checked &&
                   t_coord->topLevelItem(0)->checkState(0) == Qt::Checked &&
                   t_tropo->topLevelItem(0)->checkState(0) == Qt::Checked;


    int rmax = t_clock->topLevelItemCount();

    boost::property_tree::ptree tree;

    if(ui->checkBox_XPO->checkState() == Qt::Checked){
        tree.add("solver.EOP.XPO.interval",ui->doubleSpinBox_xpo_int->value());
        tree.add("solver.EOP.XPO.constraint",ui->doubleSpinBox_xpo_const->value());
    }
    if(ui->checkBox_YPO->checkState() == Qt::Checked){
        tree.add("solver.EOP.YPO.interval",ui->doubleSpinBox_ypo_int->value());
        tree.add("solver.EOP.YPO.constraint",ui->doubleSpinBox_ypo_const->value());
    }
    if(ui->checkBox_dUT1->checkState() == Qt::Checked){
        tree.add("solver.EOP.dUT1.interval",ui->doubleSpinBox_dut1_int->value());
        tree.add("solver.EOP.dUT1.constraint",ui->doubleSpinBox_dut1_const->value());
    }
    if(ui->checkBox_NUTX->checkState() == Qt::Checked){
        tree.add("solver.EOP.NUTX.interval",ui->doubleSpinBox_nutx_int->value());
        tree.add("solver.EOP.NUTX.constraint",ui->doubleSpinBox_nutx_const->value());
    }
    if(ui->checkBox_NUTY->checkState() == Qt::Checked){
        tree.add("solver.EOP.NUTY.interval",ui->doubleSpinBox_nuty_int->value());
        tree.add("solver.EOP.NUTY.constraint",ui->doubleSpinBox_nuty_const->value());
    }

    tree.add("solver.reference_clock", ui->comboBox_ref_clock->currentText().toStdString());

    if(all_sta){
        int c = 1;
        QTreeWidgetItem *itm_coord = t_coord->topLevelItem(0);
        bool coord = qobject_cast<QCheckBox *>(t_coord->itemWidget(itm_coord,c++))->checkState() == Qt::Checked;
        bool datum = qobject_cast<QCheckBox *>(t_coord->itemWidget(itm_coord,c++))->checkState() == Qt::Checked;

        c = 1;
        QTreeWidgetItem *itm_clock = t_clock->topLevelItem(0);
        bool linear_clk  = qobject_cast<QCheckBox *>(t_clock->itemWidget(itm_clock,c++))->checkState() == Qt::Checked;
        bool quadratic_clk = qobject_cast<QCheckBox *>(t_clock->itemWidget(itm_clock,c++))->checkState() == Qt::Checked;
        bool pwl_clk  = qobject_cast<QCheckBox *>(t_clock->itemWidget(itm_clock,c++))->checkState() == Qt::Checked;
        double pwl_clk_int = qobject_cast<QDoubleSpinBox *>(t_clock->itemWidget(itm_clock,c++))->value();
        double pwl_clk_const = qobject_cast<QDoubleSpinBox *>(t_clock->itemWidget(itm_clock,c++))->value();

        c = 1;
        QTreeWidgetItem *itm_tropo = t_tropo->topLevelItem(0);
        bool zwd  = qobject_cast<QCheckBox *>(t_tropo->itemWidget(itm_tropo,c++))->checkState() == Qt::Checked;
        double zwd_int = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->value();
        double zwd_const = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->value();
        bool ngr  = qobject_cast<QCheckBox *>(t_tropo->itemWidget(itm_tropo,c++))->checkState() == Qt::Checked;
        double ngr_int = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->value();
        double ngr_const = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->value();
        bool egr  = qobject_cast<QCheckBox *>(t_tropo->itemWidget(itm_tropo,c++))->checkState() == Qt::Checked;
        double egr_int = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->value();
        double egr_const = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->value();

        tree.add("solver.station.<xmlattr>.name","__all__");
        tree.add("solver.station.coordinates",coord);
        tree.add("solver.station.datum",datum);

        tree.add("solver.station.linear_clock",linear_clk);
        tree.add("solver.station.quadratic_clock",quadratic_clk);
        if(pwl_clk){
            tree.add("solver.station.PWL_clock.interval",pwl_clk_int);
            tree.add("solver.station.PWL_clock.constraint",pwl_clk_const);
        }

        if(zwd){
            tree.add("solver.station.PWL_ZWD.interval",zwd_int);
            tree.add("solver.station.PWL_ZWD.constraint",zwd_const);
        }
        if(ngr){
            tree.add("solver.station.PWL_NGR.interval",ngr_int);
            tree.add("solver.station.PWL_NGR.constraint",ngr_const);
        }
        if(egr){
            tree.add("solver.station.PWL_EGR.interval",egr_int);
            tree.add("solver.station.PWL_EGR.constraint",egr_const);
        }

    }else{

        for (int r = 1; r<rmax; ++r){
            boost::property_tree::ptree staTree;
            int c = 1;
            QTreeWidgetItem *itm_coord = t_coord->topLevelItem(0);
            bool coord = qobject_cast<QCheckBox *>(t_coord->itemWidget(itm_coord,c++))->checkState() == Qt::Checked;
            bool datum = qobject_cast<QCheckBox *>(t_coord->itemWidget(itm_coord,c++))->checkState() == Qt::Checked;

            c = 1;
            QTreeWidgetItem *itm_clock = t_clock->topLevelItem(0);
            bool linear_clk  = qobject_cast<QCheckBox *>(t_clock->itemWidget(itm_clock,c++))->checkState() == Qt::Checked;
            bool quadratic_clk = qobject_cast<QCheckBox *>(t_clock->itemWidget(itm_clock,c++))->checkState() == Qt::Checked;
            bool pwl_clk  = qobject_cast<QCheckBox *>(t_clock->itemWidget(itm_clock,c++))->checkState() == Qt::Checked;
            double pwl_clk_int = qobject_cast<QDoubleSpinBox *>(t_clock->itemWidget(itm_clock,c++))->value();
            double pwl_clk_const = qobject_cast<QDoubleSpinBox *>(t_clock->itemWidget(itm_clock,c++))->value();

            c = 1;
            QTreeWidgetItem *itm_tropo = t_tropo->topLevelItem(0);
            bool zwd  = qobject_cast<QCheckBox *>(t_tropo->itemWidget(itm_tropo,c++))->checkState() == Qt::Checked;
            double zwd_int = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->value();
            double zwd_const = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->value();
            bool ngr  = qobject_cast<QCheckBox *>(t_tropo->itemWidget(itm_tropo,c++))->checkState() == Qt::Checked;
            double ngr_int = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->value();
            double ngr_const = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->value();
            bool egr  = qobject_cast<QCheckBox *>(t_tropo->itemWidget(itm_tropo,c++))->checkState() == Qt::Checked;
            double egr_int = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->value();
            double egr_const = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->value();

            staTree.add("solver.station.<xmlattr>.name","__all__");
            staTree.add("solver.station.coordinates",coord);
            staTree.add("solver.station.datum",datum);

            staTree.add("solver.station.linear_clock",linear_clk);
            staTree.add("solver.station.quadratic_clock",quadratic_clk);
            if(pwl_clk){
                staTree.add("solver.station.PWL_clock.interval",pwl_clk_int);
                staTree.add("solver.station.PWL_clock.constraint",pwl_clk_const);
            }

            if(zwd){
                staTree.add("solver.station.PWL_ZWD.interval",zwd_int);
                staTree.add("solver.station.PWL_ZWD.constraint",zwd_const);
            }
            if(ngr){
                staTree.add("solver.station.PWL_NGR.interval",ngr_int);
                staTree.add("solver.station.PWL_NGR.constraint",ngr_const);
            }
            if(egr){
                staTree.add("solver.station.PWL_EGR.interval",egr_int);
                staTree.add("solver.station.PWL_EGR.constraint",egr_const);
            }
            tree.add_child("solver.station",staTree.get_child("station"));
        }
    }
    return tree;
}

void Solver::fromXML(const boost::property_tree::ptree &tree)
{

}

void Solver::addStations(QStandardItem *dummy)
{
    if(dummy != nullptr && dummy->column() != 0){
        return;
    }
    QStringList stations;
    stations << "__all__";
    for( int i = 0; i<model_->rowCount(); ++i){
        stations << model_->item(i,0)->text();
    }

    QTreeWidget *t_coord = ui->treeWidget_sta_coord;
    QTreeWidget *t_clock = ui->treeWidget_sta_clock;
    QTreeWidget *t_tropo = ui->treeWidget_sta_tropo;
    t_coord->clear();
    t_clock->clear();
    t_tropo->clear();

    int r = 0;
    for(const auto &sta : stations){
        // ####### coord ########
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
        t_coord->addTopLevelItem(item);

        int c = 1;

        QCheckBox *coord = new QCheckBox();
        coord->setStyleSheet("margin-left:20%; margin-right:00%;");
        coord->setChecked(true);
        coord->setEnabled(enable);
        t_coord->setItemWidget(item,c++,coord);

        QCheckBox *datum = new QCheckBox();
        datum->setStyleSheet("margin-left:30%; margin-right:00%;");
        datum->setChecked(true);
        datum->setEnabled(enable);
        t_coord->setItemWidget(item,c++,datum);

        connect(coord, &QCheckBox::toggled, [coord, datum](){
            if( coord->isEnabled() && coord->checkState() == Qt::Checked){
                datum ->setEnabled(true);
            }else {
                datum ->setEnabled(false);
            }
        });

        if(r == 0){
            connect(coord, &QCheckBox::toggled, [t_coord](){
                int c = 1;
                int rmax = t_coord->topLevelItemCount();
                QTreeWidgetItem *ref = t_coord->topLevelItem(0);
                Qt::CheckState val = qobject_cast<QCheckBox *>(t_coord->itemWidget(ref,c))->checkState();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t_coord->topLevelItem(r);
                    qobject_cast<QCheckBox *>(t_coord->itemWidget(itm,c))->setCheckState(val);
                }
            });

            connect(datum, &QCheckBox::toggled, [t_coord](){
                int c = 2;
                int rmax = t_coord->topLevelItemCount();
                QTreeWidgetItem *ref = t_coord->topLevelItem(0);
                Qt::CheckState val = qobject_cast<QCheckBox *>(t_coord->itemWidget(ref,c))->checkState();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t_coord->topLevelItem(r);
                    qobject_cast<QCheckBox *>(t_coord->itemWidget(itm,c))->setCheckState(val);
                }
            });
        }
        ++r;
    }

    r = 0;
    for(const auto &sta : stations){
        // ####### coord ########
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
        t_clock->addTopLevelItem(item);

        int c = 1;
        QCheckBox *linear = new QCheckBox();
        linear->setStyleSheet("margin-left:30%; margin-right:00%;");
        linear->setChecked(true);
        linear->setEnabled(enable);
        t_clock->setItemWidget(item,c++,linear);

        QCheckBox *quadratic = new QCheckBox();
        quadratic->setStyleSheet("margin-left:45%; margin-right:00%;");
        quadratic->setChecked(true);
        quadratic->setEnabled(enable);
        t_clock->setItemWidget(item,c++,quadratic);

        QCheckBox *pwl = new QCheckBox();
        pwl->setStyleSheet("margin-left:30%; margin-right:00%;");
        pwl->setChecked(true);
        pwl->setEnabled(enable);
        t_clock->setItemWidget(item,c++,pwl);

        QDoubleSpinBox *interval = new QDoubleSpinBox();
        interval->setRange(5,1440);
        interval->setSingleStep(5);
        interval->setDecimals(0);
        interval->setValue(60);
        interval->setSuffix(" [min]");
        interval->setEnabled(enable);
        t_clock->setItemWidget(item,c++,interval);

        QDoubleSpinBox *constraint = new QDoubleSpinBox();
        constraint->setRange(0.01,1000);
        constraint->setSingleStep(1);
        constraint->setStepType(QAbstractSpinBox::StepType::AdaptiveDecimalStepType);
        constraint->setDecimals(2);
        constraint->setValue(1.3);
        constraint->setSuffix(" [cm]");
        constraint->setEnabled(enable);
        t_clock->setItemWidget(item,c++,constraint);


        connect(pwl, &QCheckBox::toggled, [pwl, interval](){
            if( pwl->isEnabled() && pwl->checkState() == Qt::Checked){
                interval ->setEnabled(true);
            }else {
                interval ->setEnabled(false);
            }
        });
        connect(pwl, &QCheckBox::toggled, [pwl, constraint](){
            if( pwl->isEnabled() && pwl->checkState() == Qt::Checked){
                constraint ->setEnabled(true);
            }else {
                constraint ->setEnabled(false);
            }
        });

        if(r == 0){
            connect(linear, &QCheckBox::toggled, [t_clock](){
                int c = 1;
                int rmax = t_clock->topLevelItemCount();
                QTreeWidgetItem *ref = t_clock->topLevelItem(0);
                Qt::CheckState val = qobject_cast<QCheckBox *>(t_clock->itemWidget(ref,c))->checkState();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t_clock->topLevelItem(r);
                    qobject_cast<QCheckBox *>(t_clock->itemWidget(itm,c))->setCheckState(val);
                }
            });
            connect(quadratic, &QCheckBox::toggled, [t_clock](){
                int c = 2;
                int rmax = t_clock->topLevelItemCount();
                QTreeWidgetItem *ref = t_clock->topLevelItem(0);
                Qt::CheckState val = qobject_cast<QCheckBox *>(t_clock->itemWidget(ref,c))->checkState();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t_clock->topLevelItem(r);
                    qobject_cast<QCheckBox *>(t_clock->itemWidget(itm,c))->setCheckState(val);
                }
            });
            connect(pwl, &QCheckBox::toggled, [t_clock](){
                int c = 3;
                int rmax = t_clock->topLevelItemCount();
                QTreeWidgetItem *ref = t_clock->topLevelItem(0);
                Qt::CheckState val = qobject_cast<QCheckBox *>(t_clock->itemWidget(ref,c))->checkState();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t_clock->topLevelItem(r);
                    qobject_cast<QCheckBox *>(t_clock->itemWidget(itm,c))->setCheckState(val);
                }
            });
            connect(interval, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [t_clock](){
                int c = 4;
                int rmax = t_clock->topLevelItemCount();
                QTreeWidgetItem *ref = t_clock->topLevelItem(0);
                double val = qobject_cast<QDoubleSpinBox *>(t_clock->itemWidget(ref,c))->value();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t_clock->topLevelItem(r);
                    qobject_cast<QDoubleSpinBox *>(t_clock->itemWidget(itm,c))->setValue(val);
                }
            });
            connect(constraint, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [t_clock](){
                int c = 5;
                int rmax = t_clock->topLevelItemCount();
                QTreeWidgetItem *ref = t_clock->topLevelItem(0);
                double val = qobject_cast<QDoubleSpinBox *>(t_clock->itemWidget(ref,c))->value();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t_clock->topLevelItem(r);
                    qobject_cast<QDoubleSpinBox *>(t_clock->itemWidget(itm,c))->setValue(val);
                }
            });
        }
        ++r;
    }

    r = 0;
    for(const auto &sta : stations){
        // ####### tropo ########
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
        t_tropo->addTopLevelItem(item);

        int c = 1;
        QCheckBox *zwd = new QCheckBox();
        zwd->setStyleSheet("margin-left:30%; margin-right:00%;");
        zwd->setChecked(true);
        zwd->setEnabled(enable);
        t_tropo->setItemWidget(item,c++,zwd);

        QDoubleSpinBox *zwd_int = new QDoubleSpinBox();
        zwd_int->setRange(5,1440);
        zwd_int->setSingleStep(5);
        zwd_int->setDecimals(0);
        zwd_int->setValue(30);
        zwd_int->setSuffix(" [min]");
        zwd_int->setEnabled(enable);
        t_tropo->setItemWidget(item,c++,zwd_int);

        QDoubleSpinBox *zwd_const = new QDoubleSpinBox();
        zwd_const->setRange(0.01,1000);
        zwd_const->setSingleStep(1);
        zwd_const->setStepType(QAbstractSpinBox::StepType::AdaptiveDecimalStepType);
        zwd_const->setDecimals(2);
        zwd_const->setValue(1.5);
        zwd_const->setSuffix(" [cm]");
        zwd_const->setEnabled(enable);
        t_tropo->setItemWidget(item,c++,zwd_const);


        connect(zwd, &QCheckBox::toggled, [zwd, zwd_int](){
            if( zwd->isEnabled() && zwd->checkState() == Qt::Checked){
                zwd_int ->setEnabled(true);
            }else {
                zwd_int ->setEnabled(false);
            }
        });
        connect(zwd, &QCheckBox::toggled, [zwd, zwd_const](){
            if( zwd->isEnabled() && zwd->checkState() == Qt::Checked){
                zwd_const ->setEnabled(true);
            }else {
                zwd_const ->setEnabled(false);
            }
        });

        QCheckBox *ngr = new QCheckBox();
        ngr->setStyleSheet("margin-left:20%; margin-right:00%;");
        ngr->setChecked(true);
        ngr->setEnabled(enable);
        t_tropo->setItemWidget(item,c++,ngr);

        QDoubleSpinBox *ngr_int = new QDoubleSpinBox();
        ngr_int->setRange(5,1440);
        ngr_int->setSingleStep(30);
        ngr_int->setDecimals(0);
        ngr_int->setValue(180);
        ngr_int->setSuffix(" [min]");
        ngr_int->setEnabled(enable);
        t_tropo->setItemWidget(item,c++,ngr_int);

        QDoubleSpinBox *ngr_const = new QDoubleSpinBox();
        ngr_const->setRange(0.01,1000);
        ngr_const->setSingleStep(1);
        ngr_const->setStepType(QAbstractSpinBox::StepType::AdaptiveDecimalStepType);
        ngr_const->setDecimals(2);
        ngr_const->setValue(0.05);
        ngr_const->setSuffix(" [cm]");
        ngr_const->setEnabled(enable);
        t_tropo->setItemWidget(item,c++,ngr_const);


        connect(ngr, &QCheckBox::toggled, [ngr, ngr_int](){
            if( ngr->isEnabled() && ngr->checkState() == Qt::Checked){
                ngr_int ->setEnabled(true);
            }else {
                ngr_int ->setEnabled(false);
            }
        });
        connect(ngr, &QCheckBox::toggled, [ngr, ngr_const](){
            if( ngr->isEnabled() && ngr->checkState() == Qt::Checked){
                ngr_const ->setEnabled(true);
            }else {
                ngr_const ->setEnabled(false);
            }
        });


        QCheckBox *egr = new QCheckBox();
        egr->setStyleSheet("margin-left:20%; margin-right:00%;");
        egr->setChecked(true);
        egr->setEnabled(enable);
        t_tropo->setItemWidget(item,c++,egr);

        QDoubleSpinBox *egr_int = new QDoubleSpinBox();
        egr_int->setRange(5,1440);
        egr_int->setSingleStep(5);
        egr_int->setDecimals(0);
        egr_int->setValue(180);
        egr_int->setSuffix(" [min]");
        egr_int->setEnabled(enable);
        t_tropo->setItemWidget(item,c++,egr_int);

        QDoubleSpinBox *egr_const = new QDoubleSpinBox();
        egr_const->setRange(0.01,1000);
        egr_const->setSingleStep(1);
        egr_const->setStepType(QAbstractSpinBox::StepType::AdaptiveDecimalStepType);
        egr_const->setDecimals(2);
        egr_const->setValue(0.05);
        egr_const->setSuffix(" [cm]");
        egr_const->setEnabled(enable);
        t_tropo->setItemWidget(item,c++,egr_const);


        connect(egr, &QCheckBox::toggled, [egr, egr_int](){
            if( egr->isEnabled() && egr->checkState() == Qt::Checked){
                egr_int ->setEnabled(true);
            }else {
                egr_int ->setEnabled(false);
            }
        });
        connect(egr, &QCheckBox::toggled, [egr, egr_const](){
            if( egr->isEnabled() && egr->checkState() == Qt::Checked){
                egr_const ->setEnabled(true);
            }else {
                egr_const ->setEnabled(false);
            }
        });

        if(r == 0){
            connect(zwd, &QCheckBox::toggled, [t_tropo](){
                int c = 1;
                int rmax = t_tropo->topLevelItemCount();
                QTreeWidgetItem *ref = t_tropo->topLevelItem(0);
                Qt::CheckState val = qobject_cast<QCheckBox *>(t_tropo->itemWidget(ref,c))->checkState();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t_tropo->topLevelItem(r);
                    qobject_cast<QCheckBox *>(t_tropo->itemWidget(itm,c))->setCheckState(val);
                }
            });
            connect(zwd_int, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [t_tropo](){
                int c = 2;
                int rmax = t_tropo->topLevelItemCount();
                QTreeWidgetItem *ref = t_tropo->topLevelItem(0);
                double val = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(ref,c))->value();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t_tropo->topLevelItem(r);
                    qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm,c))->setValue(val);
                }
            });
            connect(zwd_const, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [t_tropo](){
                int c = 3;
                int rmax = t_tropo->topLevelItemCount();
                QTreeWidgetItem *ref = t_tropo->topLevelItem(0);
                double val = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(ref,c))->value();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t_tropo->topLevelItem(r);
                    qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm,c))->setValue(val);
                }
            });


            connect(ngr, &QCheckBox::toggled, [t_tropo](){
                int c = 4;
                int rmax = t_tropo->topLevelItemCount();
                QTreeWidgetItem *ref = t_tropo->topLevelItem(0);
                Qt::CheckState val = qobject_cast<QCheckBox *>(t_tropo->itemWidget(ref,c))->checkState();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t_tropo->topLevelItem(r);
                    qobject_cast<QCheckBox *>(t_tropo->itemWidget(itm,c))->setCheckState(val);
                }
            });
            connect(ngr_int, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [t_tropo](){
                int c = 5;
                int rmax = t_tropo->topLevelItemCount();
                QTreeWidgetItem *ref = t_tropo->topLevelItem(0);
                double val = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(ref,c))->value();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t_tropo->topLevelItem(r);
                    qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm,c))->setValue(val);
                }
            });
            connect(ngr_const, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [t_tropo](){
                int c = 6;
                int rmax = t_tropo->topLevelItemCount();
                QTreeWidgetItem *ref = t_tropo->topLevelItem(0);
                double val = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(ref,c))->value();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t_tropo->topLevelItem(r);
                    qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm,c))->setValue(val);
                }
            });


            connect(egr, &QCheckBox::toggled, [t_tropo](){
                int c = 7;
                int rmax = t_tropo->topLevelItemCount();
                QTreeWidgetItem *ref = t_tropo->topLevelItem(0);
                Qt::CheckState val = qobject_cast<QCheckBox *>(t_tropo->itemWidget(ref,c))->checkState();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t_tropo->topLevelItem(r);
                    qobject_cast<QCheckBox *>(t_tropo->itemWidget(itm,c))->setCheckState(val);
                }
            });
            connect(egr_int, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [t_tropo](){
                int c = 8;
                int rmax = t_tropo->topLevelItemCount();
                QTreeWidgetItem *ref = t_tropo->topLevelItem(0);
                double val = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(ref,c))->value();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t_tropo->topLevelItem(r);
                    qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm,c))->setValue(val);
                }
            });
            connect(egr_const, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [t_tropo](){
                int c = 9;
                int rmax = t_tropo->topLevelItemCount();
                QTreeWidgetItem *ref = t_tropo->topLevelItem(0);
                double val = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(ref,c))->value();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t_tropo->topLevelItem(r);
                    qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm,c))->setValue(val);
                }
            });

        }
        ++r;
    }
}

void Solver::toggleAll_sta_coord(QTreeWidgetItem *item, int column)
{
    if(item->text(0) == "__all__" && column == 0){
        bool checked = item->checkState(0);

        QTreeWidget *t = ui->treeWidget_sta_coord;
        int rmax = t->topLevelItemCount();
        int cmax = t->columnCount();
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

void Solver::toggleAll_sta_clock(QTreeWidgetItem *item, int column)
{
    if(item->text(0) == "__all__" && column == 0){
        bool checked = item->checkState(0);

        QTreeWidget *t = ui->treeWidget_sta_clock;
        int rmax = t->topLevelItemCount();
        int cmax = t->columnCount();
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

void Solver::toggleAll_sta_tropo(QTreeWidgetItem *item, int column)
{
    if(item->text(0) == "__all__" && column == 0){
        bool checked = item->checkState(0);

        QTreeWidget *t = ui->treeWidget_sta_tropo;
        int rmax = t->topLevelItemCount();
        int cmax = t->columnCount();
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


