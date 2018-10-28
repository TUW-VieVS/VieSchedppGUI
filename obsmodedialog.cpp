#include "obsmodedialog.h"
#include "ui_obsmodedialog.h"

ObsModeDialog::ObsModeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ObsModeDialog)
{
    ui->setupUi(this);
}

ObsModeDialog::~ObsModeDialog()
{
    delete ui;
}
