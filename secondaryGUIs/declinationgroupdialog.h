#ifndef DECLINATIONGROUPDIALOG_H
#define DECLINATIONGROUPDIALOG_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class DeclinationGroupDialog;
}

class DeclinationGroupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeclinationGroupDialog(QWidget *parent = nullptr);
    ~DeclinationGroupDialog();

    QList<double> thresholds() const;

private slots:
    void addThreshold();
    void removeSelected();

private:
    Ui::DeclinationGroupDialog *ui;
};

#endif // DECLINATIONGROUPDIALOG_H
