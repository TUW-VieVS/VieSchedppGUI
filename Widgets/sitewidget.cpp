#include "sitewidget.h"
#include "ui_sitewidget.h"

SiteWidget::SiteWidget(QStandardItemModel *stations, QWidget *parent) :
    QWidget(parent),
    stations_{stations},
    ui(new Ui::SiteWidget)
{
    ui->setupUi(this);
    ui->tableWidget_sites->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    updateCounter();
}

SiteWidget::~SiteWidget()
{
    delete ui;
}

void SiteWidget::setup()
{
    int n = stations_->rowCount();
    ui->tableWidget_sites->setRowCount(n);
    QStringList stations;
    for( int i = 0; i<n; ++i){
        QString name = stations_->item(i,0)->text();
        stations << name;
    }
    stations.sort();

    for( int i = 0; i<n; ++i){
        QString name = stations[i];
        QTableWidgetItem *itm = new QTableWidgetItem(QIcon(":/icons/icons/station.png"), name);
        ui->tableWidget_sites->setItem(i,0,itm);

        AlphabetSpinnBoxSites *sp = new AlphabetSpinnBoxSites();
        sp->setRange(0,n-1);
        sp->setValue(i);
        connect(sp, SIGNAL(valueChanged(int)), this, SLOT(updateCounter()));
        ui->tableWidget_sites->setCellWidget(i,1,sp);
    }

}

void SiteWidget::updateCounter()
{
    QList<QColor> colorPalette = {
        QColor("#E57373"), // Red
        QColor("#64B5F6"), // Blue
        QColor("#81C784"), // Green
        QColor("#FFD54F"), // Yellow
        QColor("#BA68C8"), // Purple
        QColor("#4DB6AC"), // Teal
        QColor("#FF8A65"), // Orange
        QColor("#A1887F"), // Brown
        QColor("#90A4AE"), // Gray-Blue
        QColor("#DCE775")  // Lime
    };

    int n = stations_->rowCount();
    QVector<int>counter(n, 0);
    for( int i = 0; i<n; ++i){
        QSpinBox *sp = qobject_cast<QSpinBox *>(ui->tableWidget_sites->cellWidget(i,1));
        int id = sp->value();
        ++counter[id];
    }
    QMap<int, int> id2color;
    int icolor = 0;
    for( int i = 0; i<n; ++i){

        QSpinBox *sp = qobject_cast<QSpinBox *>(ui->tableWidget_sites->cellWidget(i,1));
        int id = sp->value();
        int val = counter[id];

        if( val > 1 ){
            int color;
            if ( id2color.find(id) != id2color.end()){
                color = id2color[id];
            }else{
                color = icolor;
                id2color[id] = color;
                icolor += 1;
                icolor %= 10;
            }
            sp->setProperty("highlightIndex", color);
        }else{
            sp->setProperty("highlightIndex", -1);
        }
        sp->style()->unpolish(sp);
        sp->style()->polish(sp);
        sp->update();
    }
}


void SiteWidget::addStations(QStandardItem *)
{
    if (!block){
        setup();
    }
}


boost::property_tree::ptree SiteWidget::toXML()
{
    boost::property_tree::ptree tree;
    const auto *t = ui->tableWidget_sites;
    QSet<QString> ids;
    for(int i = 0; i<t->rowCount(); ++i){
        QString id = qobject_cast<QSpinBox *>(t->cellWidget(i,1))->text();
        ids.insert(id);
    }
    if ( ids.size() == t->rowCount() ){
        return tree;
    }

    QStringList processed;
    for(int i = 0; i<t->rowCount(); ++i){
        QString id = qobject_cast<QSpinBox *>(t->cellWidget(i,1))->text();
        if ( processed.contains(id) ){
            continue;
        }
        processed.push_back(id);

        boost::property_tree::ptree site;
        site.add("<xmlattr>.ID", id.toStdString());
        boost::property_tree::ptree statree;
        for(int i = 0; i<t->rowCount(); ++i){
            QString name = t->item(i,0)->text();
            QString txt = qobject_cast<QSpinBox *>(t->cellWidget(i,1))->text();
            if( txt == id ){
                site.add("station", name.toStdString());
            }
        }
        tree.add_child("site", site);
    }
    return tree;
}

void SiteWidget::fromXML(const boost::property_tree::ptree &tree)
{
    QTableWidget *t = ui->tableWidget_sites;

    for( const auto &any : tree){
        QString name = QString::fromStdString(any.second.get("<xmlattr>.ID",""));
        int id_ = AlphabetSpinnBoxSites::qstr2int(name);

        for ( const auto & stas : any.second){
            QString name = QString::fromStdString(stas.second.data());
            for ( int i =0; i<t->rowCount();++i){
                QString sta = t->item(i,0)->text();
                if ( name == sta){
                    qobject_cast<QSpinBox *>(t->cellWidget(i,1))->setValue(id_);
                    break;
                }
            }
        }
    }
}

void SiteWidget::on_pushButton_clicked()
{
    setup();
    int n = stations_->rowCount();
    for (int i = 0; i<n; ++i){
        double x1 = stations_->index(i, 16).data().toDouble();
        double y1 = stations_->index(i, 17).data().toDouble();
        double z1 = stations_->index(i, 18).data().toDouble();
        for (int j = 0; j<n; ++j){
            double x2 = stations_->index(j, 16).data().toDouble();
            double y2 = stations_->index(j, 17).data().toDouble();
            double z2 = stations_->index(j, 18).data().toDouble();

            double dist = qSqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)+(z2-z1)*(z2-z1));
            if (dist <= ui->spinBox_dist->value()){
                QSpinBox *sp1 = qobject_cast<QSpinBox *>(ui->tableWidget_sites->cellWidget(i,1));
                int id = sp1->value();
                QSpinBox *sp2 = qobject_cast<QSpinBox *>(ui->tableWidget_sites->cellWidget(j,1));
                sp2->setValue(id);
            }
        }
    }
}

