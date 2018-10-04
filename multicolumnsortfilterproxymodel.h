#ifndef MULTICOLUMNSORTFILTERPROXYMODEL_H
#define MULTICOLUMNSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class MultiColumnSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit MultiColumnSortFilterProxyModel(QObject *parent = 0);
    void setFilterKeyColumns(const QList<qint32> &filterColumns);
    void addFilterFixedString(const QString &pattern);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    QList<qint32> columns_;
    QString pattern_;
};

#endif // MULTICOLUMNSORTFILTERPROXYMODEL_H
