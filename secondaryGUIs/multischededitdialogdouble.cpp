/* 
 *  VieSched++ Very Long Baseline Interferometry (VLBI) Scheduling Software
 *  Copyright (C) 2018  Matthias Schartner
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "secondaryGUIs/multischededitdialogdouble.h"
#include "ui_multischededitdialogdouble.h"

multiSchedEditDialogDouble::multiSchedEditDialogDouble(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::multiSchedEditDialogDouble)
{
    ui->setupUi(this);

    QApplication::setWindowIcon(QIcon(":/icons/icons/VieSchedppGUI_logo.png"));
    this->setWindowTitle("VieSched++");

    ui->groupBox_member->hide();
    all = new QStandardItemModel(0,1,this);
    proxy = new QSortFilterProxyModel();
    proxy->setSourceModel(all);

    ui->listView_member->setModel(proxy);
    proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->tableWidget_values->verticalHeader()->show();
    ui->tableWidget_values->horizontalHeader()->show();
}

multiSchedEditDialogDouble::~multiSchedEditDialogDouble()
{
    delete ui;
}

QVector<double> multiSchedEditDialogDouble::getValues()
{
    QVector<double> values;
    for(int i = 0; i<ui->tableWidget_values->rowCount(); ++i){
        QDoubleSpinBox *box = qobject_cast<QDoubleSpinBox*>(ui->tableWidget_values->cellWidget(i,0));
        values << box->value();
    }
    return values;
}

void multiSchedEditDialogDouble::addMember(QStandardItemModel *model)
{
    ui->groupBox_member->show();
    for(int i = 0; i< model->rowCount(); ++i){
        all->setItem(i,model->item(i)->clone());
    }
}

void multiSchedEditDialogDouble::addSkyCoverageMember(int n)
{
    ui->groupBox_member->show();
    all->appendRow(new QStandardItem(QIcon(":/icons/icons/sky_coverage.png"),"__all__"));

    for(int i=0; i<n; ++i){
        QString txt;
        if(i<26){
            txt = QString(QChar('A'+i));
        }else if(i < 2*26){
            txt = QString(QChar('a'+(i-26)));
        }else{
            txt = QString::number(i);
        }
        all->appendRow(new QStandardItem(QIcon(":/icons/icons/sky_coverage.png"),txt));
    }
}

QStandardItem *multiSchedEditDialogDouble::getMember()
{
    return all->item(ui->listView_member->selectionModel()->selectedIndexes().at(0).row());
}

void multiSchedEditDialogDouble::addDefaultValues(const QVector<double> &vals, bool random, bool weightFactors)
{
    double min = *std::min_element(vals.begin(), vals.end());
    double max = *std::max_element(vals.begin(), vals.end());
    double step = (max-min)/(vals.count()-1);
    ui->doubleSpinBox_start->setValue(min);
    ui->doubleSpinBox_start->setSingleStep(step);
    ui->doubleSpinBox_stop->setValue(max);
    ui->doubleSpinBox_stop->setSingleStep(step);
    ui->doubleSpinBox_step->setValue(step);
    ui->doubleSpinBox_step->setSingleStep(step*.1);

    if(random){
        ui->autogenerat->setEnabled(false);
        ui->pushButton_insert->setEnabled(false);
        ui->pushButton_delete->setEnabled(false);

        int r1 = ui->tableWidget_values->rowCount();
        ui->tableWidget_values->insertRow(r1);
        QDoubleSpinBox *spinBox1 = new QDoubleSpinBox(this);
        if(weightFactors){
            spinBox1->setMaximum(1);
            spinBox1->setMinimum(0);
        }else{
            spinBox1->setMaximum(10000);
            spinBox1->setMinimum(-10000);
        }
        spinBox1->setSingleStep(step);
        spinBox1->setValue(min);
        ui->tableWidget_values->setCellWidget(r1,0, spinBox1);

        int r2 = ui->tableWidget_values->rowCount();
        ui->tableWidget_values->insertRow(r2);
        QDoubleSpinBox *spinBox2 = new QDoubleSpinBox(this);
        if(weightFactors){
            spinBox2->setMaximum(1);
            spinBox2->setMinimum(0);
        }else{
            spinBox2->setMaximum(10000);
            spinBox2->setMinimum(-10000);
        }
        spinBox2->setSingleStep(step);
        spinBox2->setValue(max);
        ui->tableWidget_values->setCellWidget(r2,0, spinBox2);

        ui->tableWidget_values->setVerticalHeaderItem(0, new QTableWidgetItem("min"));
        ui->tableWidget_values->setVerticalHeaderItem(1, new QTableWidgetItem("max"));

    }else{
        for(double v : vals){
            int r = ui->tableWidget_values->rowCount();
            ui->tableWidget_values->insertRow(r);
            QDoubleSpinBox *spinBox = new QDoubleSpinBox(this);
            spinBox->setMaximum(10000);
            spinBox->setMinimum(-10000);
            spinBox->setSingleStep(step);
            spinBox->setValue(v);
            ui->tableWidget_values->setCellWidget(r,0, spinBox);
        }
    }

}

void multiSchedEditDialogDouble::on_doubleSpinBox_start_valueChanged(double arg1)
{
    if(arg1 > ui->doubleSpinBox_stop->value()){
        ui->doubleSpinBox_stop->setValue(arg1);
    }
}

void multiSchedEditDialogDouble::on_doubleSpinBox_stop_valueChanged(double arg1)
{
    if(arg1 < ui->doubleSpinBox_start->value()){
        ui->doubleSpinBox_start->setValue(arg1);
    }
}

void multiSchedEditDialogDouble::on_pushButton_generate_clicked()
{
    ui->tableWidget_values->setRowCount(0);
    QVector<double> values;
    for(double i = ui->doubleSpinBox_start->value(); i<= ui->doubleSpinBox_stop->value()+1e-5; i+=ui->doubleSpinBox_step->value()){
        values << i;
    }

    int n = values.size();
    for(int i = 0; i<n; ++i){
        ui->tableWidget_values->insertRow(i);
        QDoubleSpinBox *spinBox = new QDoubleSpinBox(this);
        spinBox->setSingleStep(.1);
        spinBox->setValue(values.at(i));
        ui->tableWidget_values->setCellWidget(i,0, spinBox);
    }
}

void multiSchedEditDialogDouble::on_pushButton_insert_clicked()
{
    ui->tableWidget_values->insertRow(0);
    QDoubleSpinBox *spinBox = new QDoubleSpinBox(this);
    spinBox->setSingleStep(.1);
    ui->tableWidget_values->setCellWidget(0,0, spinBox);
    spinBox->setFocus();
}

void multiSchedEditDialogDouble::on_pushButton_delete_clicked()
{
    auto sel = ui->tableWidget_values->selectionModel()->selectedRows(0);
    for(int i = sel.size()-1; i>=0 ; --i){
        int row = sel.at(0).row();
        ui->tableWidget_values->removeRow(row);
    }
}

void multiSchedEditDialogDouble::on_lineEdit_filter_textChanged(const QString &arg1)
{
    proxy->setFilterRegularExpression(arg1);
}

void multiSchedEditDialogDouble::on_buttonBox_accepted()
{
    if(ui->tableWidget_values->rowCount()>0){
        if(ui->groupBox_member->isHidden()){
            this->accept();
        }else{
            if(ui->listView_member->selectionModel()->selectedIndexes().count() == 1){
                this->accept();
            }else{
                QMessageBox ms;
                ms.warning(this,"Select member!","You forgot to select a member!");
            }
        }
    }else{
        this->reject();
    }
}
