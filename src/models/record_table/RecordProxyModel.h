#ifndef __RECORDTABLEPROXYMODEL_H__
#define __RECORDTABLEPROXYMODEL_H__

#include <QSortFilterProxyModel>


class RecordProxyModel : public QSortFilterProxyModel {
public:
    RecordProxyModel(QString screen_name, QObject *pobj = 0);
    ~RecordProxyModel();

};

#endif // __RECORDTABLEPROXYMODEL_H__
