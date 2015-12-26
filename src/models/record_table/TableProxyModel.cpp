#include <QSortFilterProxyModel>

#include "main.h"
#include "TableProxyModel.h"


// Конструктор модели
TableProxyModel::TableProxyModel(QString screen_name, QObject *pobj) : QSortFilterProxyModel(pobj)
{
    setObjectName(screen_name + "_proxymodel");
    // this->setSourceModel(this);

}

// Деструктор модели
TableProxyModel::~TableProxyModel()
{
    return;
}

