#ifndef MASTERSESSIONVIEWER_H
#define MASTERSESSIONVIEWER_H

#include <QDialog>
#include <QDateTime>
#include <QTableWidgetItem>
#include <QMessageBox>

namespace Ui {
class masterSessionViewer;
}

class masterSessionViewer : public QDialog
{
    Q_OBJECT

public:
    explicit masterSessionViewer(QString loadText, QWidget *parent = nullptr);
    ~masterSessionViewer();

    void addSessions(QVector<std::pair<int, QString>> sessions);

    QString getSessionCode();

    bool getSaveClicked(){ return updateSave; }

    QString getSaveText(){ return saveText; }

private:
    Ui::masterSessionViewer *ui;
    QVector<std::pair<int, QString>> sessions_;

    QTableWidgetItem* createTableWidgetItem( const QString& text, Qt::AlignmentFlag = Qt::AlignLeft ) const;
    bool updateSave = false;
    QString saveText = "";

private slots:
    void updateTable();

    void highlight();



    void on_pushButton_clicked();
};

#endif // MASTERSESSIONVIEWER_H
