#include "tleformat.h"
#include "ui_tleformat.h"

TleFormat::TleFormat(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TleFormat)
{
    ui->setupUi(this);
}

TleFormat::~TleFormat()
{
    delete ui;
}
