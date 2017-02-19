#ifndef ADD_ACTION_H
#define ADD_ACTION_H

#include "libraries/flat_control.h"
#include <QIcon>
#include <QObject>
#include <QString>
#include <QToolBar>
#include <cassert>
#include <functional>

class QToolBar;
class FlatToolButton;
class EditingWindow;

template <typename bar_type_, typename return_type>
struct add_widget {
    add_widget(bar_type_* b, return_type* r)
    {
        b->addAction(r);
    }
};

template <typename return_type>
struct add_widget<QToolBar, return_type> { // return_type == FlatToolButton
    add_widget(QToolBar* b, return_type* r)
    {
        b->addWidget(r);
    }
};

template <>
struct add_widget<QToolBar, QAction> {
    add_widget(QToolBar* b, QAction* r)
    {
        b->addAction(r);
    }
};

template <typename return_type, typename receiver_type>
struct connect_action {
    connect_action(return_type* r, receiver_type* receiver = nullptr, void (receiver_type::*slot)(bool) = nullptr)
    {
        if (receiver)
            QObject::connect(r, &return_type::triggered, receiver, slot);
    }
    connect_action(return_type* r, const std::function<void(bool)>& func)
    {
        QObject::connect(r, &return_type::triggered, func);
    }
};

template <typename receiver_type>
struct connect_action<FlatToolButton, receiver_type> {
    typedef FlatToolButton return_type;
    connect_action(return_type* r, receiver_type* receiver = nullptr, void (receiver_type::*slot)(bool) = nullptr)
    {
        if (receiver)
            QObject::connect(r, &return_type::clicked, receiver, slot);
    }
    connect_action(return_type* r, const std::function<void(bool)>& func)
    {
        QObject::connect(r, &return_type::clicked, func);
    }
};

template <typename receiver_type>
struct connect_action<FlatFontComboBox, receiver_type> {
    typedef FlatFontComboBox return_type;
    connect_action(return_type* r, receiver_type* receiver = nullptr, void (receiver_type::*slot)(const QFont&) = nullptr)
    {
        if (receiver)
            QObject::connect(r, &return_type::currentFontChanged, receiver, slot);
    }
    connect_action(return_type* r, const std::function<void(bool)>& func)
    {
        QObject::connect(r, &return_type::currentFontChanged, func);
    }
};

template <typename receiver_type>
struct connect_action<FlatComboBox, receiver_type> {
    typedef FlatComboBox return_type;
    void (FlatComboBox::*_currentIndexChanged)(int index) = &FlatComboBox::currentIndexChanged;
    connect_action(return_type* r, receiver_type* receiver = nullptr, void (receiver_type::*slot)(int) = nullptr)
    {
        if (receiver)
            QObject::connect(r, _currentIndexChanged, receiver, slot);
    }
    connect_action(return_type* r, const std::function<void(bool)>& func)
    {
        QObject::connect(r, _currentIndexChanged, func);
    }
};

template <typename bar_type = QToolBar, typename return_type = FlatToolButton, typename parent_type = QWidget, typename receiver_type = EditingWindow>
return_type* add_action(bar_type* bar, parent_type* parent, QString title, QString toolTip, QString whatsThis // What's this help
    ,
    const QIcon& icon = QIcon(":/resource/pic/trace.svg") // QIcon()
    ,
    receiver_type* receiver = nullptr, void (receiver_type::*slot)(bool) = nullptr, const QKeySequence& key = QKeySequence(QKeySequence::UnknownKey))
{
    //    QAction *
    return_type* return_value = new return_type(title, parent); // new QAction(title, parent);	// parent = this by default
    if (return_value) {
        assert(bar);
        return_value->setVisible(true);
        return_value->setToolTip(toolTip);
        return_value->setStatusTip(toolTip);
        return_value->setWhatsThis(whatsThis);
        //		return_value->setAutoRaise(true);
        return_value->setIcon(icon);
        if (key != QKeySequence(QKeySequence::UnknownKey))
            return_value->setShortcut(key);
        add_widget<bar_type, return_type> s(bar, return_value); // bar->addAction(return_value);
        connect_action<return_type, receiver_type> c(return_value, receiver, slot); // if(receiver) connect(return_value, SIGNAL(triggered(bool)), receiver, slot);
    }
    return return_value;
}

// for FlatComboBox
template <typename bar_type = QToolBar, typename parent_type = QWidget, typename receiver_type = EditingWindow>
FlatComboBox* add_action(bar_type* bar // <bar_type, FlatFontComboBox, parent_type, receiver_type>
    ,
    parent_type* parent, QString title, QString toolTip, QString whatsThis // What's this help
    ,
    const QIcon& icon = QIcon(":/resource/pic/trace.svg") // QIcon()
    ,
    FlatComboBox* return_value_ = nullptr, receiver_type* receiver = nullptr, void (receiver_type::*slot)(int) = nullptr)
{
    (void)return_value_;
    typedef FlatComboBox return_type;
    //    QAction *
    return_type* return_value = new return_type(parent); // new QAction(title, parent);	// parent = this by default
    if (return_value) {
        assert(bar);
        return_value->setObjectName(title);
        return_value->setVisible(true);
        return_value->setToolTip(toolTip);
        return_value->setStatusTip(toolTip);
        return_value->setWhatsThis(whatsThis);
        //		return_value->setAutoRaise(true);
        return_value->addItem(" ", 0);
        return_value->setItemIcon(0, icon);
        //		if(key != QKeySequence(QKeySequence::UnknownKey)) return_value->setShortcut(key);
        add_widget<bar_type, return_type> s(bar, return_value); // bar->addAction(return_value);
        connect_action<return_type, receiver_type> c(return_value, receiver, slot); // if(receiver) connect(return_value, SIGNAL(triggered(bool)), receiver, slot);
    }
    return return_value;
}

// for FlatComboBox
template <typename bar_type = QToolBar, typename parent_type = QWidget, typename receiver_type = EditingWindow>
FlatComboBox* add_action(bar_type* bar // <bar_type, FlatFontComboBox, parent_type, receiver_type>
    ,
    parent_type* parent, QString title, QString toolTip, QString whatsThis // What's this help
    ,
    const QIcon& icon = QIcon(":/resource/pic/trace.svg") // QIcon()
    ,
    FlatComboBox* return_value_ = nullptr, const std::function<void(int)>& func = std::function<void(int)>())
{
    (void)return_value_;
    typedef FlatComboBox return_type;
    //    QAction *
    return_type* return_value = new return_type(parent); // new QAction(title, parent);	// parent = this by default
    if (return_value) {
        assert(bar);
        return_value->setObjectName(title);
        return_value->setVisible(true);
        return_value->setToolTip(toolTip);
        return_value->setStatusTip(toolTip);
        return_value->setWhatsThis(whatsThis);
        //		return_value->setAutoRaise(true);
        return_value->addItem(" ", 0);
        return_value->setItemIcon(0, icon);
        //		if(key != QKeySequence(QKeySequence::UnknownKey)) return_value->setShortcut(key);
        add_widget<bar_type, return_type> s(bar, return_value); // bar->addAction(return_value);
        connect_action<return_type, receiver_type> c(return_value, func); // if(receiver) connect(return_value, SIGNAL(triggered(bool)), receiver, slot);
    }
    return return_value;
}
// for FlatFontComboBox
template <typename bar_type = QToolBar, typename parent_type = QWidget, typename receiver_type = EditingWindow>
FlatFontComboBox* add_action(bar_type* bar // <bar_type, FlatFontComboBox, parent_type, receiver_type>
    ,
    parent_type* parent, QString title, QString toolTip, QString whatsThis // What's this help
    ,
    const QIcon& icon = QIcon(":/resource/pic/trace.svg") // QIcon()
    ,
    FlatFontComboBox* return_value_ = nullptr, receiver_type* receiver = nullptr, void (receiver_type::*slot)(const QFont&) = nullptr)
{
    (void)return_value_;
    typedef FlatFontComboBox return_type;
    //    QAction *
    return_type* return_value = new return_type(parent); // new QAction(title, parent);	// parent = this by default
    if (return_value) {
        assert(bar);
        return_value->setObjectName(title);
        return_value->setVisible(true);
        return_value->setToolTip(toolTip);
        return_value->setStatusTip(toolTip);
        return_value->setWhatsThis(whatsThis);
        //		return_value->setAutoRaise(true);
        return_value->addItem(" ");
        return_value->setItemIcon(return_value->count() - 1, icon);
        //		if(key != QKeySequence(QKeySequence::UnknownKey)) return_value->setShortcut(key);
        add_widget<bar_type, return_type> s(bar, return_value); // bar->addAction(return_value);
        connect_action<return_type, receiver_type> c(return_value, receiver, slot); // if(receiver) connect(return_value, SIGNAL(triggered(bool)), receiver, slot);
    }
    return return_value;
}

// for FlatFontComboBox
template <typename bar_type = QToolBar, typename parent_type = QWidget, typename receiver_type = EditingWindow>
FlatFontComboBox* add_action(bar_type* bar // <bar_type, FlatFontComboBox, parent_type, receiver_type>
    ,
    parent_type* parent, QString title, QString toolTip, QString whatsThis // What's this help
    ,
    const QIcon& icon = QIcon(":/resource/pic/trace.svg") // QIcon()
    ,
    FlatFontComboBox* return_value_ = nullptr, const std::function<void(bool)>& func = std::function<void(bool)>())
{
    (void)return_value_;
    typedef FlatFontComboBox return_type;
    //    QAction *
    return_type* return_value = new return_type(parent); // new QAction(title, parent);	// parent = this by default
    if (return_value) {
        assert(bar);
        return_value->setObjectName(title);
        return_value->setVisible(true);
        return_value->setToolTip(toolTip);
        return_value->setStatusTip(toolTip);
        return_value->setWhatsThis(whatsThis);
        //		return_value->setAutoRaise(true);
        return_value->addItem(" ");
        return_value->setItemIcon(return_value->count() - 1, icon);
        //		if(key != QKeySequence(QKeySequence::UnknownKey)) return_value->setShortcut(key);
        add_widget<bar_type, return_type> s(bar, return_value); // bar->addAction(return_value);
        connect_action<return_type, receiver_type> c(return_value, func); // if(receiver) connect(return_value, SIGNAL(triggered(bool)), receiver, slot);
    }
    return return_value;
}

template <typename bar_type = QToolBar, typename return_type = FlatToolButton, typename parent_type = QWidget, typename receiver_type = EditingWindow>
return_type* add_action(bar_type* bar, parent_type* parent, QString title, QString toolTip, QString whatsThis // What's this help
    ,
    const QIcon& icon = QIcon(":/resource/pic/trace.svg") // QIcon()
    ,
    const std::function<void(bool)>& func = std::function<void(bool)>(), const QKeySequence& key = QKeySequence(QKeySequence::UnknownKey))
{
    //    QAction *
    return_type* return_value = new return_type("editor_tb_" + title, parent); // ("editor_tb_" + title, parent) // new QAction(title, parent);	// parent = this by default
    if (return_value) {
        assert(bar);
        return_value->setVisible(true);
        return_value->setToolTip(toolTip);
        return_value->setWhatsThis(whatsThis);
        //		return_value->setAutoRaise(true);
        return_value->setIcon(icon);
        if (key != QKeySequence(QKeySequence::UnknownKey))
            return_value->setShortcut(key);
        add_widget<bar_type, return_type> s(bar, return_value); // bar->addAction(return_value);
        connect_action<return_type, receiver_type> c(return_value, func); // if(receiver) connect(return_value, SIGNAL(triggered(bool)), receiver, slot);
    }
    return return_value;
}

template <typename tool_button>
void add_action(QToolBar* tools_line, QAction* action)
{ // void    insert_action_as_button(QToolBar *tools_line, QAction *action);
    tools_line->addAction(action);
    qobject_cast<tool_button*>(tools_line->widgetForAction(action))->setAutoRaise(true); // false
}

template <typename tool_button>
void add_action(QToolBar* tools_line, QAction* before, QAction* action)
{ // void    insert_action_as_button(QToolBar *tools_line, QAction *action);
    tools_line->insertAction(before, action);
    qobject_cast<tool_button*>(tools_line->widgetForAction(action))->setAutoRaise(true); // false
}

#endif // ADD_ACTION_H
