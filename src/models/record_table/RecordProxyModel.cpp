#include <QSortFilterProxyModel>

#include "main.h"
#include "RecordProxyModel.h"


// Конструктор модели
RecordProxyModel::RecordProxyModel(// QString screen_name,
                                   QObject *pobj) : QSortFilterProxyModel(pobj)
{
    // setObjectName(screen_name + "_proxymodel");
    // this->setSourceModel(this);

}

// Деструктор модели
RecordProxyModel::~RecordProxyModel()
{
    return;
}

