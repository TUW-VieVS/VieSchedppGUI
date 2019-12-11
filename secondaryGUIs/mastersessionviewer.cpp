#include "mastersessionviewer.h"
#include "ui_mastersessionviewer.h"


masterSessionViewer::masterSessionViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::masterSessionViewer)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    ui->tableWidget_VGOS->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget_24hSX->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget_IntensiveSX->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

masterSessionViewer::~masterSessionViewer()
{
    delete ui;
}

void masterSessionViewer::addSessions(QVector<std::pair<int, QString> > sessions)
{
    sessions_ = sessions;
    updateTable();
}

QString masterSessionViewer::getSessionCode()
{
    int idx = ui->tabWidget->currentIndex();
    QTableWidget *t;
    if(idx == 0){
        t = ui->tableWidget_24hSX;
    }else if(idx == 1){
        t = ui->tableWidget_IntensiveSX;
    }else{
        t = ui->tableWidget_VGOS;
    }

    auto selected = t->selectedItems();
    if(!selected.isEmpty()){
        int row = t->selectedItems().at(0)->row();
        return t->item(row,1)->text();
    }else{
        return "";
    }
}

void masterSessionViewer::updateTable()
{
    ui->tableWidget_IntensiveSX->setRowCount(0);
    ui->tableWidget_24hSX->setRowCount(0);
    ui->tableWidget_VGOS->setRowCount(0);

    for(const auto &any : sessions_){
        int code = any.first;
        QStringList content = any.second.split('|',QString::SplitBehavior::SkipEmptyParts);

        QTableWidget *t;
        if(code == 0){
            t = ui->tableWidget_24hSX;
        }else if(code == 1){
            t = ui->tableWidget_IntensiveSX;
        }else{
            t = ui->tableWidget_VGOS;
        }


        int r = t->rowCount();
        t->insertRow(r);

        QDateTime now = QDateTime::currentDateTime();
        int nowYear = now.date().year();
        int nowDoy = now.date().dayOfYear();

        int tDoy = content[3].toInt();
        int tyear = nowYear;
        if(nowDoy > 330 && tDoy < 35){
            ++tyear;
        }
        QDate tDate(tyear,1,1);
        tDate = tDate.addDays(tDoy-1);

        auto hm = content[4].split(':');
        int tHour = hm.at(0).toInt();
        int tMin = hm.at(1).toInt();
        QTime tTime(tHour,tMin);

        QDateTime tStart(tDate,tTime);

        QString tStations = content[6].split(' ').at(0);
        QString tStationsOut = QString("(%1) ").arg(tStations.length()/2);
        for (int c = 0; c < tStations.length(); ++c) {
            tStationsOut += tStations[c];
            if ( c % 2){
                tStationsOut += " ";
            }
        }


        t->setItem(r, 0, new QTableWidgetItem( content[0] ));
        QTableWidgetItem *twi = new QTableWidgetItem( content[1] );
        auto font = twi->font();
        font.setBold(true);
        twi->setFont(font);
        t->setItem(r, 1, twi);
        t->setItem(r, 2, createTableWidgetItem( content[7], Qt::AlignRight ));
        t->setItem(r, 3, new QTableWidgetItem( tStart.toString("dd.MM.yyyy HH:mm") ));
        t->setItem(r, 4, createTableWidgetItem( content[3] , Qt::AlignCenter ));
        t->setItem(r, 5, createTableWidgetItem( content[4].trimmed(), Qt::AlignCenter ));
        t->setItem(r, 6, new QTableWidgetItem( tStationsOut ));
        t->setItem(r, 7, createTableWidgetItem( content[8], Qt::AlignRight ));
        t->setItem(r, 8, createTableWidgetItem( content[11], Qt::AlignRight ));

    }
}


QTableWidgetItem* masterSessionViewer::createTableWidgetItem( const QString& text, Qt::AlignmentFlag align ) const
{
    QTableWidgetItem* item = new QTableWidgetItem( text );
    auto a = align | Qt::AlignVCenter;
    item->setTextAlignment( a );
    return item;
}

