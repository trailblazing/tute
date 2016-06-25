#ifndef __RECORDTABLEPROXYMODEL_H__
#define __RECORDTABLEPROXYMODEL_H__

#include <QSortFilterProxyModel>


class RecordProxyModel : public QSortFilterProxyModel {
public:
    RecordProxyModel(QObject *pobj = 0);
    ~RecordProxyModel();

};

#endif // __RECORDTABLEPROXYMODEL_H__
