#ifndef MASTERSESSIONVIEWER_H
#define MASTERSESSIONVIEWER_H

#include <QDialog>
#include <QDateTime>
#include <QTableWidgetItem>

namespace Ui {
class masterSessionViewer;
}

class masterSessionViewer : public QDialog
{
    Q_OBJECT

public:
    explicit masterSessionViewer(QWidget *parent = nullptr);
    ~masterSessionViewer();

    void addSessions(QVector<std::pair<int, QString>> sessions);

    QString getSessionCode();

private:
    Ui::masterSessionViewer *ui;
    QVector<std::pair<int, QString>> sessions_;

    QTableWidgetItem* createTableWidgetItem( const QString& text, Qt::AlignmentFlag = Qt::AlignLeft ) const;

private slots:
    void updateTable();




};

#endif // MASTERSESSIONVIEWER_H
