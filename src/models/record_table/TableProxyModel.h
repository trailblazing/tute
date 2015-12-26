#ifndef __RECORDTABLEPROXYMODEL_H__
#define __RECORDTABLEPROXYMODEL_H__

#include <QSortFilterProxyModel>


class TableProxyModel : public QSortFilterProxyModel {
public:
    TableProxyModel(QString screen_name, QObject *pobj=0);
    ~TableProxyModel();

};

#endif // __RECORDTABLEPROXYMODEL_H__
