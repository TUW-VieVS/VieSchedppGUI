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

#ifndef SETTIMES_H
#define SETTIMES_H

#include <QDialog>
#include <../VieSchedpp/XML/ParameterSettings.h>
#include <QMessageBox>
#include <QWhatsThis>
#include <QShortcut>
#include <secondaryGUIs/settingsloadwindow.h>
namespace Ui {
class setTimes;
}

class setTimes : public QDialog
{
    Q_OBJECT

public:
    explicit setTimes(QWidget *parent = 0, int fs=8, int preob=10);
    ~setTimes();
    QVector<int> getValues();
private slots:


private:
    Ui::setTimes *ui;

};

#endif // SETTIMES_H
