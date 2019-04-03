#ifndef RENDERSETUP_H
#define RENDERSETUP_H

#include <QDialog>

namespace Ui {
class RenderSetup;
}

class RenderSetup : public QDialog
{
    Q_OBJECT

public:
    explicit RenderSetup(QWidget *parent = nullptr);
    ~RenderSetup();

private:
    Ui::RenderSetup *ui;
};

#endif // RENDERSETUP_H
