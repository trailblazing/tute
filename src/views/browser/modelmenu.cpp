/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include "modelmenu.h"

#include <QtCore/QAbstractItemModel>
#include <qdebug.h>

Q_DECLARE_METATYPE(QModelIndex)

namespace browser {
#if QT_VERSION == 0x050600
W_OBJECT_IMPL(ModelMenu)
#endif

ModelMenu::ModelMenu(QWidget* parent)
    : QMenu(parent)
    , _maxrows(7)
    , _firstseparator(-1)
    , _maxwidth(-1)
    , _hoverrole(0)
    , _separatorrole(0)
    , _model(0)
{
    connect(this, &ModelMenu::aboutToShow, this, &ModelMenu::aboutToShow);
}

bool ModelMenu::prePopulated()
{
    return false;
}

void ModelMenu::postPopulated()
{
}

void ModelMenu::setModel(QAbstractItemModel* model)
{
    _model = model;
}

QAbstractItemModel* ModelMenu::model() const
{
    return _model;
}

void ModelMenu::setMaxRows(int max)
{
    _maxrows = max;
}

int ModelMenu::maxRows() const
{
    return _maxrows;
}

void ModelMenu::setFirstSeparator(int offset)
{
    _firstseparator = offset;
}

int ModelMenu::firstSeparator() const
{
    return _firstseparator;
}

void ModelMenu::setRootIndex(const QModelIndex& index)
{
    _root = index;
}

QModelIndex ModelMenu::rootIndex() const
{
    return _root;
}

void ModelMenu::setHoverRole(int role)
{
    _hoverrole = role;
}

int ModelMenu::hoverRole() const
{
    return _hoverrole;
}

void ModelMenu::setSeparatorRole(int role)
{
    _separatorrole = role;
}

int ModelMenu::separatorRole() const
{
    return _separatorrole;
}

//    Q_DECLARE_METATYPE(QModelIndex)

void ModelMenu::aboutToShow()
{
    if (QMenu* menu = qobject_cast<QMenu*>(sender())) {
        QVariant v = menu->menuAction()->data();
        if (v.canConvert<QModelIndex>()) {
            QModelIndex idx = qvariant_cast<QModelIndex>(v);
            createMenu(idx, -1, menu, menu);
            disconnect(menu, &QMenu::aboutToShow, this, &ModelMenu::aboutToShow);

            return;
        }
    }
    clear();
    if (prePopulated())
        addSeparator();
    int max = _maxrows;
    if (max != -1)
        max += _firstseparator;
    createMenu(_root, max, this, this);
    postPopulated();
}

void ModelMenu::createMenu(const QModelIndex& parent, int max, QMenu* parentMenu, QMenu* menu)
{
    if (!menu) {
        QString title = parent.data().toString();
        menu = new QMenu(title, this);
        QIcon icon = qvariant_cast<QIcon>(parent.data(Qt::DecorationRole));
        menu->setIcon(icon);
        parentMenu->addMenu(menu);
        QVariant v;
        v.setValue(parent);
        menu->menuAction()->setData(v);
        connect(menu, &QMenu::aboutToShow, this, &ModelMenu::aboutToShow);

        return;
    }
    int end = _model->rowCount(parent);
    if (max != -1)
        end = qMin(max, end);
    connect(menu, &QMenu::triggered, this, &ModelMenu::triggered);
    connect(menu, &QMenu::hovered, this, &ModelMenu::hovered);
    for (int i = 0; i < end; ++i) {
        QModelIndex idx = _model->index(i, 0, parent);
        if (_model->hasChildren(idx))
            createMenu(idx, -1, menu);
        else {
            if (_separatorrole != 0
                && idx.data(_separatorrole).toBool())
                addSeparator();
            else
                menu->addAction(makeAction(idx));
        }
        if (menu == this && i == _firstseparator - 1)
            addSeparator();
    }
}

QAction* ModelMenu::makeAction(const QModelIndex& index)
{
    QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    QAction* action = makeAction(icon, index.data().toString(), this);
    QVariant v;
    v.setValue(index);
    action->setData(v);

    return action;
}

QAction* ModelMenu::makeAction(const QIcon& icon, const QString& text, QObject* parent)
{
    QFontMetrics fm(font());
    if (-1 == _maxwidth)
        _maxwidth = fm.width(QLatin1Char('m')) * 30;
    QString smallText = fm.elidedText(text, Qt::ElideMiddle, _maxwidth);

    return new QAction(icon, smallText, parent);
}

void ModelMenu::triggered(QAction* action)
{
    QVariant v = action->data();
    if (v.canConvert<QModelIndex>()) {
        QModelIndex idx = qvariant_cast<QModelIndex>(v);
        emit activated(idx);
    }
}

void ModelMenu::hovered(QAction* action)
{
    QVariant v = action->data();
    if (v.canConvert<QModelIndex>()) {
        QModelIndex idx = qvariant_cast<QModelIndex>(v);
        QString hoveredString = idx.data(_hoverrole).toString();
        if (!hoveredString.isEmpty())
            emit hovered_signal(hoveredString);
    }
}
}
