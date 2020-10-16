#include "solverwidget.h"
#include "ui_solverwidget.h"


SolverWidget::SolverWidget(QStandardItemModel *station_model, QStandardItemModel *source_model, QWidget *parent) :
      QWidget(parent),
      ui(new Ui::SolverWidget),
      station_model_{station_model},
      source_model_{source_model}
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

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

    ui->tableWidget_solver->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->treeWidget_sta_coord,
            SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this,
            SLOT(toggleAll_sta_coord(QTreeWidgetItem*, int)));

    connect(ui->treeWidget_src_coord,
            SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this,
            SLOT(toggleAll_src_coord(QTreeWidgetItem*, int)));

    connect(ui->treeWidget_sta_clock,
            SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this,
            SLOT(toggleAll_sta_clock(QTreeWidgetItem*, int)));

    connect(ui->treeWidget_sta_tropo,
            SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this,
            SLOT(toggleAll_sta_tropo(QTreeWidgetItem*, int)));

//    connect(ui->treeWidget_sta_coord,
//            SIGNAL(itemChanged(QTreeWidgetItem*, int)),
//            this,
//            SLOT(checkDatum()));

    ui->comboBox_ref_clock->setModel(station_model);
}

SolverWidget::~SolverWidget()
{
    delete ui;
}

boost::property_tree::ptree SolverWidget::toXML()
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
    if(ui->checkBox_scale->checkState() == Qt::Checked){
        tree.add("solver.EOP.scale","true");
    }

    if (ui->radioButton_force_ref_clock->isChecked()){
        std::string refClock = ui->comboBox_ref_clock->currentText().toStdString();
        tree.add("solver.reference_clock", refClock);
    }

    if(all_sta){
        int c = 1;
        QTreeWidgetItem *itm_coord = t_coord->topLevelItem(0);
        bool coord = qobject_cast<QCheckBox *>(t_coord->itemWidget(itm_coord,c++))->checkState() == Qt::Checked;
        bool datum = qobject_cast<QCheckBox *>(t_coord->itemWidget(itm_coord,c++))->checkState() == Qt::Checked;
        if(!coord){
            datum = false;
        }

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
            QTreeWidgetItem *itm_coord = t_coord->topLevelItem(r);
            int c = 1;
            bool coord = qobject_cast<QCheckBox *>(t_coord->itemWidget(itm_coord,c++))->checkState() == Qt::Checked;
            bool datum = qobject_cast<QCheckBox *>(t_coord->itemWidget(itm_coord,c++))->checkState() == Qt::Checked;
            if(!coord){
                datum = false;
            }

            c = 1;
            QTreeWidgetItem *itm_clock = t_clock->topLevelItem(r);
            bool linear_clk      = qobject_cast<QCheckBox *>(t_clock->itemWidget(itm_clock,c++))->checkState() == Qt::Checked;
            bool quadratic_clk   = qobject_cast<QCheckBox *>(t_clock->itemWidget(itm_clock,c++))->checkState() == Qt::Checked;
            bool pwl_clk         = qobject_cast<QCheckBox *>(t_clock->itemWidget(itm_clock,c++))->checkState() == Qt::Checked;
            double pwl_clk_int   = qobject_cast<QDoubleSpinBox *>(t_clock->itemWidget(itm_clock,c++))->value();
            double pwl_clk_const = qobject_cast<QDoubleSpinBox *>(t_clock->itemWidget(itm_clock,c++))->value();

            c = 1;
            QTreeWidgetItem *itm_tropo = t_tropo->topLevelItem(r);
            bool zwd         = qobject_cast<QCheckBox *>(t_tropo->itemWidget(itm_tropo,c++))->checkState() == Qt::Checked;
            double zwd_int   = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->value();
            double zwd_const = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->value();
            bool ngr         = qobject_cast<QCheckBox *>(t_tropo->itemWidget(itm_tropo,c++))->checkState() == Qt::Checked;
            double ngr_int   = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->value();
            double ngr_const = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->value();
            bool egr         = qobject_cast<QCheckBox *>(t_tropo->itemWidget(itm_tropo,c++))->checkState() == Qt::Checked;
            double egr_int   = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->value();
            double egr_const = qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->value();

            QString itmName = itm_tropo->text(0);
            staTree.add("station.<xmlattr>.name", itmName.toStdString());
            staTree.add("station.coordinates",coord);
            staTree.add("station.datum",datum);

            staTree.add("station.linear_clock",linear_clk);
            staTree.add("station.quadratic_clock",quadratic_clk);
            if(pwl_clk){
                staTree.add("station.PWL_clock.interval",pwl_clk_int);
                staTree.add("station.PWL_clock.constraint",pwl_clk_const);
            }

            if(zwd){
                staTree.add("station.PWL_ZWD.interval",zwd_int);
                staTree.add("station.PWL_ZWD.constraint",zwd_const);
            }
            if(ngr){
                staTree.add("station.PWL_NGR.interval",ngr_int);
                staTree.add("station.PWL_NGR.constraint",ngr_const);
            }
            if(egr){
                staTree.add("station.PWL_EGR.interval",egr_int);
                staTree.add("station.PWL_EGR.constraint",egr_const);
            }
            tree.add_child("solver.station",staTree.get_child("station"));
        }
    }

    tree.add("solver.source.minScans", ui->spinBox_src_minScans->value() );
    tree.add("solver.source.minObs", ui->spinBox_src_minObs->value() );
    tree.add("solver.source.minObs_datum", ui->spinBox_src_minObs_datum->value() );

    for(int i = 0; i<ui->treeWidget_src_coord->topLevelItemCount(); ++i){
        QTreeWidgetItem *itm_coord = ui->treeWidget_src_coord->topLevelItem(i);
        if(i == 0){
            if(itm_coord->checkState(0) == Qt::Checked){
                bool coord = qobject_cast<QCheckBox *>(ui->treeWidget_src_coord->itemWidget(itm_coord,1))->checkState() == Qt::Checked;
                bool datum = false;
                if(coord){
                    datum = qobject_cast<QCheckBox *>(ui->treeWidget_src_coord->itemWidget(itm_coord,2))->checkState() == Qt::Checked;
                }
                if(coord){
                    tree.add("solver.source.estimate", "__all__");
                }else{
                    tree.add("solver.source.estimate", "__none__");
                }

                tree.add("solver.source.datum", "__all__");
                break;
            }else{
                continue;
            }
        }

        std::string name = itm_coord->text(0).toStdString();

        bool coord = qobject_cast<QCheckBox *>(ui->treeWidget_src_coord->itemWidget(itm_coord,1))->checkState() == Qt::Checked;
        bool datum = false;
        if(coord){
            datum = qobject_cast<QCheckBox *>(ui->treeWidget_src_coord->itemWidget(itm_coord,2))->checkState() == Qt::Checked;
        }
        boost::property_tree::ptree srcTree;

        if(coord){
            srcTree.add("name",name);
            tree.add_child("solver.source.estimate.name", srcTree.get_child("name"));
        }
        if(datum){
            srcTree.add("name",name);
            tree.add_child("solver.source.datum.name", srcTree.get_child("name"));
        }
    }
    if(ui->comboBox_solver->currentText() != "complete orthogonal decomposition (default)"){
        std::string s;
        if(ui->comboBox_solver->currentText() == "Householder QR decomposition"){
            s = "householderQr";
        }else if(ui->comboBox_solver->currentText() == "robust Cholesky decomposition with pivoting (LDLT)"){
            s = "ldlt";
        }else if(ui->comboBox_solver->currentText() == "LU decomposition with partial pivoting"){
            s = "partialPivLu";
        }
        tree.add("solver.algorithm", s);
    }
    return tree;
}

void SolverWidget::fromXML(const boost::property_tree::ptree &tree)
{
    std::string solver = tree.get("algorithm","");
    if( !solver.empty() ){
        if(solver == "householderQr"){
            ui->comboBox_solver->setCurrentText("Householder QR decomposition");
        } else if(solver == "ldlt"){
            ui->comboBox_solver->setCurrentText("robust Cholesky decomposition with pivoting (LDLT)");
        } else if(solver == "partialPivLu"){
            ui->comboBox_solver->setCurrentText("LU decomposition with partial pivoting");
        }else{
            ui->comboBox_solver->setCurrentText("complete orthogonal decomposition (default)");
        }
    }else{
        ui->comboBox_solver->setCurrentText("complete orthogonal decomposition (default)");
    }


    // read EOP
    double xpo = tree.get("EOP.XPO.interval",-1.0);
    if(xpo != -1){
        ui->checkBox_XPO->setChecked(true);
        ui->doubleSpinBox_xpo_int->setValue(xpo);
        ui->doubleSpinBox_xpo_const->setValue(tree.get("EOP.XPO.constraint",.0001));
    }else{
        ui->checkBox_XPO->setChecked(false);
    }
    double ypo = tree.get("EOP.YPO.interval",-1.0);
    if(ypo != -1){
        ui->checkBox_YPO->setChecked(true);
        ui->doubleSpinBox_ypo_int->setValue(ypo);
        ui->doubleSpinBox_ypo_const->setValue(tree.get("EOP.YPO.constraint",.0001));
    }else{
        ui->checkBox_YPO->setChecked(false);
    }
    double dut1 = tree.get("EOP.dUT1.interval",-1.0);
    if(dut1 != -1){
        ui->checkBox_dUT1->setChecked(true);
        ui->doubleSpinBox_dut1_int->setValue(dut1);
        ui->doubleSpinBox_dut1_const->setValue(tree.get("EOP.dUT1.constraint",.0001));
    }else{
        ui->checkBox_dUT1->setChecked(false);
    }
    double nutx = tree.get("EOP.NUTX.interval",-1.0);
    if(nutx != -1){
        ui->checkBox_NUTX->setChecked(true);
        ui->doubleSpinBox_nutx_int->setValue(nutx);
        ui->doubleSpinBox_nutx_const->setValue(tree.get("EOP.NUTX.constraint",.0001));
    }else{
        ui->checkBox_NUTX->setChecked(false);
    }
    double nuty = tree.get("EOP.NUTY.interval",-1.0);
    if(nuty != -1){
        ui->checkBox_NUTY->setChecked(true);
        ui->doubleSpinBox_nuty_int->setValue(nuty);
        ui->doubleSpinBox_nuty_const->setValue(tree.get("EOP.NUTY.constraint",.0001));
    }else{
        ui->checkBox_NUTY->setChecked(false);
    }
    bool scale = tree.get("EOP.scale",false);
    if(scale){
        ui->checkBox_scale->setChecked(true);
    }else{
        ui->checkBox_scale->setChecked(false);
    }

    for(const auto &any : tree){

        // read stations
        if(any.first == "station"){
            QString name = QString::fromStdString(any.second.get("<xmlattr>.name","__all__"));

            // clock
            {
                QTreeWidget *t_clock = ui->treeWidget_sta_clock;
                QTreeWidgetItem *itm_clock;
                for(int i = 0; i<t_clock->topLevelItemCount(); ++i){
                    itm_clock = t_clock->topLevelItem(i);
                    if(i == 0){
                        if(name == "__all__"){
                            itm_clock->setCheckState(0,Qt::Checked);
                        }else{
                            itm_clock->setCheckState(0,Qt::Unchecked);
                        }
                    }
                    if(itm_clock->text(0) == name){
                        break;
                    }
                }

                if(itm_clock != nullptr){
                    int c = 1;
                    qobject_cast<QCheckBox *>(t_clock->itemWidget(itm_clock,c++))->setChecked(any.second.get("linear_clock",true));
                    qobject_cast<QCheckBox *>(t_clock->itemWidget(itm_clock,c++))->setChecked(any.second.get("quadratic_clock",true));

                    const auto &pwl = any.second.get_child_optional("PWL_clock");
                    if(pwl.is_initialized()){
                        qobject_cast<QCheckBox *>(t_clock->itemWidget(itm_clock,c++))->setChecked(true);
                        qobject_cast<QDoubleSpinBox *>(t_clock->itemWidget(itm_clock,c++))->setValue(any.second.get("PWL_clock.interval",60.));
                        qobject_cast<QDoubleSpinBox *>(t_clock->itemWidget(itm_clock,c++))->setValue(any.second.get("PWL_clock.constraint",1.3));
                    }else{
                        qobject_cast<QCheckBox *>(t_clock->itemWidget(itm_clock,c++))->setChecked(false);
                    }
                }
            }

            // coord
            {
                QTreeWidget *t_coord = ui->treeWidget_sta_coord;
                QTreeWidgetItem *itm_coord;
                for(int i = 0; i<t_coord->topLevelItemCount(); ++i){
                    itm_coord = t_coord->topLevelItem(i);
                    if(i == 0){
                        if(name == "__all__"){
                            itm_coord->setCheckState(0,Qt::Checked);
                        }else{
                            itm_coord->setCheckState(0,Qt::Unchecked);
                        }
                    }
                    if(itm_coord->text(0) == name){
                        break;
                    }
                }

                if(itm_coord != nullptr){
                    int c = 1;
                    qobject_cast<QCheckBox *>(t_coord->itemWidget(itm_coord,c++))->setChecked(any.second.get("coordinates",true));
                    qobject_cast<QCheckBox *>(t_coord->itemWidget(itm_coord,c++))->setChecked(any.second.get("datum",true));
                }
            }

            // tropo
            {
                QTreeWidget *t_tropo = ui->treeWidget_sta_tropo;

                QTreeWidgetItem *itm_tropo;
                for(int i = 0; i<t_tropo->topLevelItemCount(); ++i){
                    itm_tropo = t_tropo->topLevelItem(i);
                    if(i == 0){
                        if(name == "__all__"){
                            itm_tropo->setCheckState(0,Qt::Checked);
                        }else{
                            itm_tropo->setCheckState(0,Qt::Unchecked);
                        }
                    }

                    if(itm_tropo->text(0) == name){
                        break;
                    }
                }

                if(itm_tropo != nullptr){
                    int c = 1;
                    const auto &zwd = any.second.get_child_optional("PWL_ZWD");
                    if(zwd.is_initialized()){
                        qobject_cast<QCheckBox *>(t_tropo->itemWidget(itm_tropo,c++))->setChecked(true);
                        qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->setValue(any.second.get("PWL_ZWD.interval",30.));
                        qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->setValue(any.second.get("PWL_ZWD.constraint",1.5));
                    }else{
                        qobject_cast<QCheckBox *>(t_tropo->itemWidget(itm_tropo,c++))->setChecked(false);
                        ++c;
                        ++c;
                    }

                    const auto &ngr = any.second.get_child_optional("PWL_NGR");
                    if(ngr.is_initialized()){
                        qobject_cast<QCheckBox *>(t_tropo->itemWidget(itm_tropo,c++))->setChecked(true);
                        qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->setValue(any.second.get("PWL_NGR.interval",180.));
                        qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->setValue(any.second.get("PWL_NGR.constraint",0.05));
                    }else{
                        qobject_cast<QCheckBox *>(t_tropo->itemWidget(itm_tropo,c++))->setChecked(false);
                        ++c;
                        ++c;
                    }

                    const auto &egr = any.second.get_child_optional("PWL_EGR");
                    if(egr.is_initialized()){
                        qobject_cast<QCheckBox *>(t_tropo->itemWidget(itm_tropo,c++))->setChecked(true);
                        qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->setValue(any.second.get("PWL_EGR.interval",180.));
                        qobject_cast<QDoubleSpinBox *>(t_tropo->itemWidget(itm_tropo,c++))->setValue(any.second.get("PWL_EGR.constraint",0.05));
                    }else{
                        qobject_cast<QCheckBox *>(t_tropo->itemWidget(itm_tropo,c++))->setChecked(false);
                        ++c;
                        ++c;
                    }
                }
            }
        }

        QString refClock = QString::fromStdString(tree.get("reference_clock",""));
        if(!refClock.isEmpty()){
            ui->radioButton_force_ref_clock->setChecked(true);
            ui->comboBox_ref_clock->setCurrentText(refClock);
        }else{
            ui->comboBox_ref_clock->setCurrentIndex(0);
            ui->radioButton_obs_based_ref_clock->setChecked(true);
        }


        // read sources
        ui->spinBox_src_minScans->setValue(tree.get("source.minScans",3));
        ui->spinBox_src_minObs->setValue(tree.get("source.minObs",5));
        ui->spinBox_src_minObs_datum->setValue(tree.get("source.minObs_datum",25));

        if(any.first == "source"){

            QTreeWidget *t_src = ui->treeWidget_src_coord;
            t_src->topLevelItem(0)->setCheckState(0,Qt::Unchecked);

            if(any.second.get("estimate","__all__") == "__all__"){
                t_src->topLevelItem(0)->setCheckState(0,Qt::Checked);
                QTreeWidgetItem *itm_src;
                for(int i = 0; i<t_src->topLevelItemCount(); ++i){
                    itm_src = t_src->topLevelItem(i);
                    qobject_cast<QCheckBox *>(t_src->itemWidget(itm_src,1))->setChecked(true);
                }
            }
            if(any.second.get("estimate","__all__") == "__none__"){
                t_src->topLevelItem(0)->setCheckState(0,Qt::Checked);
                QTreeWidgetItem *itm_src;
                for(int i = 0; i<t_src->topLevelItemCount(); ++i){
                    itm_src = t_src->topLevelItem(i);
                    qobject_cast<QCheckBox *>(t_src->itemWidget(itm_src,1))->setChecked(false);
                }
            }
            if(any.second.get("datum","__all__") == "__all__"){
                t_src->topLevelItem(0)->setCheckState(0,Qt::Checked);
                QTreeWidgetItem *itm_src;
                for(int i = 0; i<t_src->topLevelItemCount(); ++i){
                    itm_src = t_src->topLevelItem(i);
                    qobject_cast<QCheckBox *>(t_src->itemWidget(itm_src,2))->setChecked(true);
                }
            }
            QStringList estimateSource;
            QStringList datumSource;

            for(const auto &any2 : any.second){
                if(any2.first == "estimate"){
                    for(const auto &any3 : any2.second){
                        if(any3.first == "name"){
                            QString name = QString::fromStdString(any3.second.data());
                            estimateSource << name;
                        }
                    }
                }
                if(any2.first == "datum"){
                    for(const auto &any3 : any2.second){
                        if(any3.first == "name"){
                            QString name = QString::fromStdString(any3.second.data());
                            datumSource << name;
                        }
                    }
                }
            }


            QTreeWidgetItem *itm_src;
            for(int i = 0; i<t_src->topLevelItemCount(); ++i){
                itm_src = t_src->topLevelItem(i);
                if(i == 0){
                    continue;
                }

                if(estimateSource.contains(itm_src->text(0))){
                    qobject_cast<QCheckBox *>(t_src->itemWidget(itm_src,1))->setChecked(true);
                }else{
                    qobject_cast<QCheckBox *>(t_src->itemWidget(itm_src,1))->setChecked(false);
                }
                if(datumSource.contains(itm_src->text(0))){
                    qobject_cast<QCheckBox *>(t_src->itemWidget(itm_src,2))->setChecked(true);
                }else{
                    qobject_cast<QCheckBox *>(t_src->itemWidget(itm_src,2))->setChecked(false);
                }
            }
        }
    }
}

void SolverWidget::addStations(QStandardItem *dummy)
{
    if(dummy != nullptr && dummy->column() != 0){
        return;
    }
    QStringList stations;
    stations << "__all__";
    for( int i = 0; i<station_model_->rowCount(); ++i){
        stations << station_model_->item(i,0)->text();
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
        connect(coord,SIGNAL(toggled(bool)),this,SLOT(checkDatum()));

        QCheckBox *datum = new QCheckBox();
        datum->setStyleSheet("margin-left:30%; margin-right:00%;");
        datum->setChecked(true);
        datum->setEnabled(enable);
        t_coord->setItemWidget(item,c++,datum);
        connect(coord,SIGNAL(toggled(bool)),this,SLOT(checkDatum()));
        connect(datum,SIGNAL(toggled(bool)),this,SLOT(checkDatum()));

        connect(coord, &QCheckBox::toggled, [coord, datum](){
            if( coord->isEnabled() && coord->checkState() == Qt::Checked){
                datum ->setEnabled(true);
            }else {
                datum ->setEnabled(false);
            }
        });

        if(r == 0){
            connect(coord, &QCheckBox::toggled, [this, t_coord](){
                int c = 1;
                int rmax = t_coord->topLevelItemCount();
                QTreeWidgetItem *ref = t_coord->topLevelItem(0);
                Qt::CheckState val = qobject_cast<QCheckBox *>(t_coord->itemWidget(ref,c))->checkState();

                for(int r=1; r<rmax; ++r){
                    QTreeWidgetItem *itm = t_coord->topLevelItem(r);
                    qobject_cast<QCheckBox *>(t_coord->itemWidget(itm,c))->setCheckState(val);
                }
            });

            connect(datum, &QCheckBox::toggled, [this, t_coord](){
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
        constraint->setSingleStep(.1);
        //constraint->setStepType(QAbstractSpinBox::StepType::AdaptiveDecimalStepType);
        constraint->setDecimals(3);
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
        zwd_const->setSingleStep(0.1);
        //zwd_const->setStepType(QAbstractSpinBox::StepType::AdaptiveDecimalStepType);
        zwd_const->setDecimals(3);
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
        ngr_const->setSingleStep(.01);
        //ngr_const->setStepType(QAbstractSpinBox::StepType::AdaptiveDecimalStepType);
        ngr_const->setDecimals(3);
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
        egr_int->setSingleStep(30);
        egr_int->setDecimals(0);
        egr_int->setValue(180);
        egr_int->setSuffix(" [min]");
        egr_int->setEnabled(enable);
        t_tropo->setItemWidget(item,c++,egr_int);

        QDoubleSpinBox *egr_const = new QDoubleSpinBox();
        egr_const->setRange(0.01,1000);
        egr_const->setSingleStep(.01);
        //egr_const->setStepType(QAbstractSpinBox::StepType::AdaptiveDecimalStepType);
        egr_const->setDecimals(3);
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

    ui->comboBox_ref_clock->setCurrentIndex(0);
}

void SolverWidget::toggleAll_sta_coord(QTreeWidgetItem *item, int column)
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
    checkDatum();
}

void SolverWidget::addSources(QStandardItem *dummy)
{
    if(dummy != nullptr && dummy->column() != 0){
        return;
    }
    QStringList sources;
    sources << "__all__";
    for( int i = 0; i<source_model_->rowCount(); ++i){
        sources << source_model_->item(i,0)->text();
    }

    QTreeWidget *t_coord = ui->treeWidget_src_coord;
    t_coord->clear();

    int r = 0;
    for(const auto &src : sources){
        // ####### coord ########
        QTreeWidgetItem *item = new QTreeWidgetItem();
        bool enable = false;
        if(src == "__all__"){
            item->setIcon(0,QIcon(":/icons/icons/source_group.png"));
            item->setCheckState(0, Qt::Checked);
            enable = true;
        }else{
            item->setFlags(item->flags() & (~Qt::ItemIsEnabled));
            item->setIcon(0,QIcon(":/icons/icons/source.png"));
        }

        item->setText(0,src);
        t_coord->addTopLevelItem(item);

        int c = 1;

        QCheckBox *coord = new QCheckBox();
        coord->setStyleSheet("margin-left:20%; margin-right:00%;");
        if(src == "__all__"){
            coord->setChecked(false);
        }else{
            coord->setChecked(true);
        }
        coord->setEnabled(enable);
        t_coord->setItemWidget(item,c++,coord);

        QCheckBox *datum = new QCheckBox();
        datum->setStyleSheet("margin-left:30%; margin-right:00%;");
        datum->setChecked(true);
        if(src == "__all__"){
            datum->setEnabled(false);
        }else{
            datum->setEnabled(enable);
        }
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
}

void SolverWidget::toggleAll_src_coord(QTreeWidgetItem *item, int column)
{
    if(item->text(0) == "__all__" && column == 0){
        bool checked = item->checkState(0);

        QTreeWidget *t = ui->treeWidget_src_coord;
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



void SolverWidget::toggleAll_sta_clock(QTreeWidgetItem *item, int column)
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

void SolverWidget::toggleAll_sta_tropo(QTreeWidgetItem *item, int column)
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


void SolverWidget::on_pushButton_3_clicked()
{
    QStringList icrf3d;
    icrf3d       << "0002-478" << "0007+106" << "0009-148" << "0010+405" << "0013-005" << "0017+200" << "0016+731"
                 << "0035-252" << "0038-326" << "0044-846" << "0046+316" << "0047+023" << "0048-097" << "0054+161"
                 << "0059+581" << "0104-408" << "0107-610" << "0110+495"
                 << "0133+476" << "0149+218" << "0159+723"
                 << "0202-172" << "0202+319" << "0208-512" << "0215+015" << "0221+067" << "0227-369" << "0230-790" << "0227+403"
                 << "0234-301" << "0235-618" << "0235+164" << "0237-027" << "0239+175" << "0256-005" << "0300+470" << "0302+625" << "0305+039"
                 << "0308-611" << "0307+380" << "0312+100" << "0316-444" << "0322+222" << "0332-403" << "0334-131" << "0346-279" << "0347-211"
                 << "0346+800" << "0355-669" << "0400-319" << "0402-362" << "0403-132" << "0406-127" << "0415+398" << "0420+022"
                 << "0430+289" << "0437-454" << "0454-810" << "0454-234"
                 << "0458-020" << "0506-612" << "0454+844" << "0506+101" << "0507+179" << "0510+559" << "0515+208" << "0522-611"
                 << "0524-485" << "0524+034" << "0530-727" << "0529+483" << "0534-340" << "0536+145" << "0537-286" << "0539-057" << "0544+273" << "0548+084" << "0552+398" << "0556+238"
                 << "0605-085" << "0607-157" << "0613+570" << "0615+820" << "0627-199"
                 << "0642-349" << "0641+392" << "0646-306" << "0648-165" << "0657+172" << "0700-465" << "0700-197"
                 << "0716+714" << "0727-115" << "0738-674" << "0736+017" << "0738+491" << "0742-562"
                 << "0743-006" << "0743+259" << "0743+277" << "0748+126" << "0749+540" << "0759+183" << "0802-010" << "0800+618" << "0804-267"
                 << "0804+499" << "0805+410" << "0809-493" << "0808+019" << "0818-128" << "0826-373" << "0829+089" << "0834-201" << "0834+250"
                 << "0847-120" << "0855-716"
                 << "0912+297" << "0918-534" << "0920+390" << "0926-039" << "0930-080"
                 << "0943+105" << "0951+268" << "0955+476" << "0954+658" << "1004-500" << "1004-217"
                 << "1012+232" << "1015+057" << "1016-311" << "1022-665" << "1022+194" << "1027-186" << "1034-374" << "1036-529" << "1040+244"
                 << "1042+071" << "1053+704" << "1059+282" << "1101-536"
                 << "1111+149" << "1116-462" << "1124-186" << "1130+009" << "1133-032"
                 << "1143-696" << "1143-245" << "1143-332" << "1144+402" << "1144-379" << "1149-084" << "1150+497"
                 << "1219+044" << "1221+809" << "1222+131" << "1226+373" << "1227+255"
                 << "1236+077" << "1243-160" << "1243-072" << "1244-255" << "1245-457" << "1251-713" << "1300+580"
                 << "1306-395" << "1308+328" << "1312-533" << "1313-333" << "1324+224" << "1325+126" << "1325-558"
                 << "1330+476" << "1330+022" << "1334-127" << "1348+308" << "1351-018" << "1357+769"
                 << "1406-076" << "1406-267" << "1412-368" << "1418+546" << "1420-679" << "1423+146" << "1424-418"
                 << "1428+370" << "1435-218" << "1443-162" << "1448+762" << "1448-648" << "1451-400"
                 << "1502+106" << "1504+377" << "1508+572" << "1510-089" << "1511-476" << "1511-558" << "1514+197" << "1520+437" << "1520+319" << "1519-273"
                 << "1538+149" << "1555+001" << "1557+032" << "1556-245" << "1600-445" << "1602-115"
                 << "1606+106" << "1606-398" << "1608+243" << "1619-680" << "1623+578" << "1624-617" << "1636+473"
                 << "1639-062" << "1642+690" << "1647-296" << "1657-261" << "1659+399" << "1705+018" << "1706-174"
                 << "1717+178" << "1718-259" << "1725+044" << "1730-130" << "1737-081"
                 << "1741-038" << "1745+624" << "1746+470" << "1749+096" << "1751+288" << "1753+204" << "1754+155" << "1758+388" << "1759-396" << "1806-458"
                 << "1815-553" << "1823+689" << "1831-711"
                 << "1846+322" << "1849+670" << "1908+484" << "1908-201" << "1909+161"
                 << "1921-293" << "1925-610" << "1929+226" << "1929-457" << "1936+046" << "1936-155" << "1937-101" << "1935-692"
                 << "1949-052" << "1951+355" << "1954-388" << "2000+472" << "2000+148" << "2002-375" << "2008-159" << "2017+745"
                 << "2022-077" << "2029+121" << "2036-034" << "2037+216" << "2037-253" << "2052-474" << "2059+034"
                 << "2111+400" << "2113+293" << "2109-811" << "2121+547"
                 << "2142+110" << "2143-156" << "2142-758" << "2149+056" << "2155+312" << "2155-304" << "2209+236"
                 << "2210-257" << "2214+350" << "2215+150" << "2216-038" << "2220-351" << "2227-088" << "2229+695" << "2232-488" << "2236-572" << "2244-372"
                 << "2245-328" << "2250+190" << "2254+074"
                 << "2318+049" << "2319+317" << "2319+444" << "2325-150" << "2331-240" << "2335-027" << "2336+598"
                 << "2353-686" << "2353+816" << "2355-534" << "2355-106" << "2356+385";

    const auto *t = ui->treeWidget_src_coord;
    for(int i = 0; i<t->topLevelItemCount(); ++i){
        auto *itm = t->topLevelItem(i);
        if(i==0){
            itm->setCheckState(0, Qt::Unchecked);
            continue;
        }
        QString txt = itm->text(0);
        if(icrf3d.indexOf(txt) != -1){
            qobject_cast<QCheckBox *>(t->itemWidget(itm,2))->setCheckState(Qt::Unchecked);
        }else{
            qobject_cast<QCheckBox *>(t->itemWidget(itm,2))->setCheckState(Qt::Checked);
        }
    }

}

void SolverWidget::checkDatum()
{
    QTreeWidget *t = ui->treeWidget_sta_coord;
    int rmax = t->topLevelItemCount();
    int cdatum = 0;
    int ccoord = 0;
    for(int r=0; r<rmax; ++r){
        QTreeWidgetItem *itm = t->topLevelItem(r);
        if(r==0){
            bool checked = itm->checkState(0);
            if(checked){
                QCheckBox *coord = qobject_cast<QCheckBox *>(t->itemWidget(itm,1));
                QCheckBox *datum = qobject_cast<QCheckBox *>(t->itemWidget(itm,2));
                if((coord->isChecked() && !datum->isChecked()) || (coord->isChecked() && rmax<4)){
                    ui->label_warning->setText("<html><head/><body><p><span style=\" font-weight:600; color:#ff5500;\">[WARNING] </span><span style=\" color:#ff5500;\">missing datum definition</span></p></body></html>");
                }else{
                    ui->label_warning->setText("");
                }
                return;
            }
            continue;
        }
        QCheckBox *coord = qobject_cast<QCheckBox *>(t->itemWidget(itm,1));
        if(coord->checkState()){
            ++ccoord;
        }
        QCheckBox *datum = qobject_cast<QCheckBox *>(t->itemWidget(itm,2));
        if(coord->checkState() && datum->checkState()){
            ++cdatum;
        }
    }
    int fixed = rmax-1-ccoord; //-1 because of "__all__" row
    if(fixed+cdatum < 3){
        ui->label_warning->setText("<html><head/><body><p><span style=\" font-weight:600; color:#ff5500;\">[WARNING] </span><span style=\" color:#ff5500;\">missing datum definition</span></p></body></html>");
    }else{
        ui->label_warning->setText("");
    }
}
