#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H

#include <QItemDelegate>
#include <QString>
#include <QVector>
#include <QComboBox>
#include <QWidget>
#include <QModelIndex>
#include <QApplication>
#include <QStringListModel>

#include <iostream>


class ComboBoxDelegate : public QItemDelegate
{
Q_OBJECT
public:
    ComboBoxDelegate(const QVector<QString> &items, QObject *parent = 0);

    ComboBoxDelegate(QStringListModel *model, QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QStringListModel *items_;

};
#endif
