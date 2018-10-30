#ifndef OBSMODEDIALOG_H
#define OBSMODEDIALOG_H

#include <QDialog>
#include "../VieSchedpp/ObservingMode.h"

namespace Ui {
class ObsModeDialog;
}

class ObsModeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ObsModeDialog(QWidget *parent = nullptr);
    ~ObsModeDialog();

private:
    Ui::ObsModeDialog *ui;
};

#endif // OBSMODEDIALOG_H
