#include <QSortFilterProxyModel>

#include "main.h"
#include "RecordProxyModel.h"


// Конструктор модели
RecordProxyModel::RecordProxyModel(QObject *pobj) : QSortFilterProxyModel(pobj){// QString screen_name,
        // setObjectName(screen_name + "_proxymodel");
        // this->setSourceModel(this);
}

// Деструктор модели
RecordProxyModel::~RecordProxyModel(){
    return;
}

