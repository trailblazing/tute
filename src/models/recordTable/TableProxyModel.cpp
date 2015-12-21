#include <QSortFilterProxyModel>

#include "main.h"
#include "TableProxyModel.h"


// Конструктор модели
TableProxyModel::TableProxyModel(QObject *pobj) : QSortFilterProxyModel(pobj)
{
    // this->setSourceModel(this);

}

// Деструктор модели
TableProxyModel::~TableProxyModel()
{
    return;
}

