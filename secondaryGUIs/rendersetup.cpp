#include "rendersetup.h"
#include "ui_rendersetup.h"

RenderSetup::RenderSetup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenderSetup)
{
    ui->setupUi(this);
}

RenderSetup::~RenderSetup()
{
    delete ui;
}
