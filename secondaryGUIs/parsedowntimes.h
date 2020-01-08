#ifndef PARSEDOWNTIMES_H
#define PARSEDOWNTIMES_H

#include <QDialog>
#include <QDateTime>
#include <algorithm>
#include "boost/optional.hpp"

namespace Ui {
class ParseDownTimes;
}

class ParseDownTimes : public QDialog
{
    Q_OBJECT

public:
    explicit ParseDownTimes(QVector<QString> sta, QDateTime sessionStart, QDateTime sessionEnd, QWidget *parent = nullptr);
    ~ParseDownTimes();

    QVector<std::pair<QString, std::pair<int,int>>> getDownTimes();

private slots:
    void on_pushButton_parse_clicked();

    void on_pushButton_clear_clicked();

private:
    Ui::ParseDownTimes *ui;
    QDateTime sessionStart;
    QDateTime sessionEnd;

    boost::optional<std::pair<QDateTime,QDateTime>> getStartEndTime(QRegularExpression r, QString line);
};

#endif // PARSEDOWNTIMES_H
