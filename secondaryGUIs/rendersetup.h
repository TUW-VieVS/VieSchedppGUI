#ifndef RENDERSETUP_H
#define RENDERSETUP_H

#include <QDialog>
#include <QStandardItemModel>
#include <QFileDialog>

namespace Ui {
class RenderSetup;
}

class RenderSetup : public QDialog
{
    Q_OBJECT

public:
    explicit RenderSetup(QWidget *parent = nullptr);
    ~RenderSetup();

    void addTimes(int duration);

    void addList(QStandardItemModel *list);

    void setOutDir(QString dir);

    QVector<int> selected();

    QVector<QPair<int,int>> times();

    QString outDir();

    QPair<int,int> resolution();

    void setDefaultFormat(int w, int h);

private slots:

    void on_checkBox_fixRatio_toggled(bool checked);

    void updatePlotCounter();

    void on_pushButton_browse_clicked();

private:
    Ui::RenderSetup *ui;
    int duration_ = 0;
};

#endif // RENDERSETUP_H
