#include "parsedowntimes.h"
#include "ui_parsedowntimes.h"

ParseDownTimes::ParseDownTimes(QVector<QString> sta, QDateTime sessionStart, QDateTime sessionEnd, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParseDownTimes)
{
    ui->setupUi(this);

    this->sessionStart = sessionStart;
    this->sessionEnd = sessionEnd;

    ui->tableWidget_downTimes->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    for(const auto & any:sta){
        ui->comboBox->addItem(QIcon(":/icons/icons/station.png"),any);
    }

}

ParseDownTimes::~ParseDownTimes()
{
    delete ui;
}

QVector<std::pair<QString, std::pair<int, int> > > ParseDownTimes::getDownTimes()
{
    QVector<std::pair<QString, std::pair<int, int> > > v;
    QTableWidget *t = ui->tableWidget_downTimes;
    QString station = ui->comboBox->currentText();
    for(int i =0; i<t->rowCount(); ++i){
        QDateTime s = QDateTime::fromString(t->item(i,0)->text(),"dd.MM.yyyy hh:mm");
        QDateTime e = QDateTime::fromString(t->item(i,1)->text(),"dd.MM.yyyy hh:mm");
        int start = sessionStart.secsTo(s);
        int end   = sessionStart.secsTo(e);

        v.append({station,{start,end}});
    }
    return v;
}

void ParseDownTimes::on_pushButton_parse_clicked()
{
    QString txt = ui->plainTextEdit_insert->toPlainText();
    QStringList lines = txt.split("\n");

    QRegularExpression r1("(\\d\\d\\d\\d).(0?\\d|1[0-2]).([0-2]\\d|3[0-1]).([0-1]\\d|2[0-3]):([0-5][0-9])");
//    QRegularExpression r2("(\\d\\d\\d\\d).(0?\\d|1[0-2]).([0-2]\\d|3[0-1]).([0-1]\\d|2[0-3]).([0-5][0-9])");
    QVector<QRegularExpression> r;
    r << r1;
//    r << r2;

    QTableWidget *t = ui->tableWidget_downTimes;
    for(const auto &any : r){
        for (const auto &line : lines){
            auto down = getStartEndTime(any,line);
            if(down.is_initialized()){
                int r = ui->tableWidget_downTimes->rowCount();

                QString t1 = down->first.toString("dd.MM.yyyy hh:mm");
                QString t2 = down->second.toString("dd.MM.yyyy hh:mm");
                bool duplicate = false;
                for(int i = 0; i<r; ++i){
                    if(t->item(i,0)->text() == t1 && t->item(i,1)->text() == t2){
                        duplicate = true;
                        break;
                    }
                }
                if(duplicate){
                    continue;
                }


                t->insertRow(r);

                t->setItem(r,0,new QTableWidgetItem(t1));
                t->setItem(r,1,new QTableWidgetItem(t2));
                t->setItem(r,2,new QTableWidgetItem(QString("%1 [min]").arg(down->first.secsTo(down->second)/60)));
            }
        }
    }
}

void ParseDownTimes::on_pushButton_clear_clicked()
{
    QTableWidget *t = ui->tableWidget_downTimes;
    t->setRowCount(0);
}

boost::optional<std::pair<QDateTime,QDateTime>> ParseDownTimes::getStartEndTime(QRegularExpression r, QString line)
{
    QRegularExpressionMatchIterator i = r.globalMatch(line);

    QDateTime start;
    QDateTime end;
    if ( i.hasNext() ){
        QRegularExpressionMatch match = i.next();
        int year = match.captured(1).toInt();
        int month = match.captured(2).toInt();
        int day = match.captured(3).toInt();
        int hour = match.captured(4).toInt();
        int min = match.captured(5).toInt();
        start = QDateTime(QDate(year, month, day), QTime(hour, min));
        start = start.addSecs(ui->doubleSpinBox_buffer->value()*-60);

        int i = sessionStart.secsTo(start);
        start = sessionStart.addSecs(std::max({0ll,sessionStart.secsTo(start)}));

    }
    if ( i.hasNext() ){
        QRegularExpressionMatch match = i.next();
        int year = match.captured(1).toInt();
        int month = match.captured(2).toInt();
        int day = match.captured(3).toInt();
        int hour = match.captured(4).toInt();
        int min = match.captured(5).toInt();
        end = QDateTime(QDate(year, month, day), QTime(hour, min));
        end = end.addSecs(ui->doubleSpinBox_buffer->value()*60);


        int i = sessionStart.secsTo(end);
        end = sessionStart.addSecs(std::min({sessionStart.secsTo(sessionEnd),sessionStart.secsTo(end)}));
    }
    if (i.hasNext()){
        return boost::none;
    }
    if (start.secsTo(end)<=0){
        return boost::none;
    }

    return std::pair<QDateTime,QDateTime>({start,end});
}
