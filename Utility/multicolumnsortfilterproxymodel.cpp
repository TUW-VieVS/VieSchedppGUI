#include "multicolumnsortfilterproxymodel.h"

MultiColumnSortFilterProxyModel::MultiColumnSortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent){
}

void MultiColumnSortFilterProxyModel::setFilterKeyColumns(const QList<qint32> &filterColumns) {
    columns_ = filterColumns;
}

void MultiColumnSortFilterProxyModel::addFilterFixedString(const QString &pattern) {
    pattern_ = pattern;
    invalidateFilter();
}

bool MultiColumnSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {

    for(auto column : columns_){
        QModelIndex index = sourceModel()->index(sourceRow, column, sourceParent);
        if(pattern_.isEmpty()){
            return true;
        }else{
            QString tmp = index.data().toString();
            if(tmp.contains(pattern_, Qt::CaseInsensitive)){
                return true;
            }
        }
    }

    return false;
}
