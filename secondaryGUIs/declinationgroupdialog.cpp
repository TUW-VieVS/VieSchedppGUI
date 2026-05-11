#include "declinationgroupdialog.h"
#include "secondaryGUIs/ui_declinationgroupdialog.h"
#include "ui_declinationgroupdialog.h"

DeclinationGroupDialog::DeclinationGroupDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DeclinationGroupDialog)
{
    ui->setupUi(this);

    setWindowTitle("Declination Groups");

    connect(ui->pushButton_add,
            &QPushButton::clicked,
            this,
            &DeclinationGroupDialog::addThreshold);

    connect(ui->pushButton_remove,
            &QPushButton::clicked,
            this,
            &DeclinationGroupDialog::removeSelected);

    connect(ui->buttonBox,
            &QDialogButtonBox::accepted,
            this,
            &QDialog::accept);

    connect(ui->buttonBox,
            &QDialogButtonBox::rejected,
            this,
            &QDialog::reject);
}

DeclinationGroupDialog::~DeclinationGroupDialog()
{
    delete ui;
}

void DeclinationGroupDialog::addThreshold()
{
    double value = ui->doubleSpinBox_threshold->value();

    // collect existing values
    QList<double> values;

    for (int i = 0; i < ui->listWidget_thresholds->count(); ++i){
        values << ui->listWidget_thresholds->item(i)->text().toDouble();
    }

    // reject duplicates
    if (values.contains(value))
    {
        QMessageBox::warning(this, "Duplicate Threshold", "Threshold already exists.");
        return;
    }

    values << value;

    // sort automatically
    std::sort(values.begin(), values.end(), std::greater<double>());

    // rebuild list
    ui->listWidget_thresholds->clear();

    for (double v : values){
        ui->listWidget_thresholds->addItem(QString::number(v, 'f', 2));
    }
}

void DeclinationGroupDialog::removeSelected()
{
    int row = ui->listWidget_thresholds->currentRow();

    if (row < 0)
        return;

    QListWidgetItem* item = ui->listWidget_thresholds->item(row);

    double value = item->text().toDouble();

    // protect fixed limits
    if (value == -90.0 || value == 90.0){
        QMessageBox::warning( this, "Protected Threshold", "The limits -90° and +90° cannot be removed.");
        return;
    }

    delete ui->listWidget_thresholds->takeItem(row);
}
QList<double> DeclinationGroupDialog::thresholds() const
{
    QList<double> values;

    for (int i = 0; i < ui->listWidget_thresholds->count(); ++i){
        values << ui->listWidget_thresholds->item(i)->text().toDouble();
    }
    std::sort(values.begin(), values.end());

    return values;
}
