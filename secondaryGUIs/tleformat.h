#ifndef TLEFORMAT_H
#define TLEFORMAT_H

#include <QDialog>

namespace Ui {
class TleFormat;
}

class TleFormat : public QDialog
{
    Q_OBJECT

public:
    explicit TleFormat(QWidget *parent = nullptr);
    ~TleFormat();

private:
    Ui::TleFormat *ui;
};

#endif // TLEFORMAT_H
