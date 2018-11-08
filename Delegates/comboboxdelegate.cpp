#include "comboboxdelegate.h"


ComboBoxDelegate::ComboBoxDelegate(const QVector<QString> &items, QObject *parent)
    :QItemDelegate(parent)
{
    items_ = new QStringListModel(this);
    QStringList tmp;
    for(const auto &any : items){
        tmp << any;
    }
    items_->setStringList(tmp);
}

ComboBoxDelegate::ComboBoxDelegate(QStringListModel *model, QObject *parent)
    :QItemDelegate(parent), items_{model}
{

}


QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
    QComboBox* editor = new QComboBox(parent);
    editor->setModel(items_);
    return editor;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    auto tmp = index.model()->data(index).toString();
    comboBox->setCurrentIndex(items_->stringList().indexOf(tmp));
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentIndex(), Qt::EditRole);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

