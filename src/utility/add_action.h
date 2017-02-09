#ifndef ADD_ACTION_H
#define ADD_ACTION_H


#include <functional>
#include <QObject>
#include <QString>
#include <QIcon>
#include <QToolBar>
#include "libraries/flat_control.h"

class QToolBar;
class FlatToolButton;
class EditingWindow;

template<typename bar_type_, typename return_type>struct add_widget {
	add_widget(bar_type_ *b, return_type *r){
		b->addAction(r);
	}
};

template<typename return_type>struct add_widget<QToolBar, return_type>{
	add_widget(QToolBar *b, return_type *r){
		b->addWidget(r);
	}
};

template<typename return_type, typename receiver_type>struct connect_action {
	connect_action(return_type *r, receiver_type *receiver = nullptr, void(receiver_type::*slot)(bool) = nullptr){
		if(receiver) QObject::connect(r, &return_type::triggered, receiver, slot);
	}
	connect_action(return_type *r, const std::function<void (bool)> &func){
		QObject::connect(r, &return_type::triggered, func);
	}
};

template<typename receiver_type>struct connect_action<FlatToolButton, receiver_type>{
	connect_action(FlatToolButton *r, receiver_type *receiver = nullptr, void(receiver_type::*slot)(bool) = nullptr){
		if(receiver) QObject::connect(r, &FlatToolButton::clicked, receiver, slot);
	}
	connect_action(FlatToolButton *r, const std::function<void (bool)> &func){
		QObject::connect(r, &FlatToolButton::clicked, func);
	}
};

template<typename bar_type = QToolBar, typename return_type = FlatToolButton, typename parent_type = QWidget, typename receiver_type = EditingWindow>
return_type *add_action(bar_type *bar
					   , parent_type *parent
					   , QString title
					   , QString toolTip
					   , QString whatsThis	// What's this help
					   , QIcon *icon
					   , receiver_type *receiver = nullptr
					   , void (receiver_type::*slot)(bool) = nullptr){
	//    QAction *
	return_type *return_value = new return_type(title, parent);			// new QAction(title, parent);	// parent = this by default
	if(return_value){
		assert(bar);
		return_value->setVisible(true);
		return_value->setToolTip(toolTip);
		return_value->setStatusTip(toolTip);
		return_value->setWhatsThis(whatsThis);
		if(icon) return_value->setIcon(*icon);
		add_widget<bar_type, return_type>					s(bar, return_value);			// bar->addAction(return_value);
		connect_action<return_type, receiver_type>			c(return_value, receiver, slot);// if(receiver) connect(return_value, SIGNAL(triggered(bool)), receiver, slot);
	}
	return return_value;
}

template<typename bar_type = QToolBar, typename return_type = FlatToolButton, typename parent_type = QWidget, typename receiver_type = EditingWindow>
return_type *add_action(bar_type *bar
					   , parent_type *parent
					   , QString title
					   , QString toolTip
					   , QString whatsThis	// What's this help
					   , QIcon *icon
					   , const std::function<void (bool)> &func){
	//    QAction *
	return_type *return_value = new return_type("editor_tb_" + title, parent);			// new QAction(title, parent);	// parent = this by default
	if(return_value){
		assert(bar);
		return_value->setVisible(true);
		return_value->setToolTip(toolTip);
		return_value->setWhatsThis(whatsThis);
		if(icon) return_value->setIcon(*icon);
		add_widget<bar_type, return_type>					s(bar, return_value);			// bar->addAction(return_value);
		connect_action<return_type, receiver_type>			c(return_value, func);	// if(receiver) connect(return_value, SIGNAL(triggered(bool)), receiver, slot);
	}
	return return_value;
}

#endif	// ADD_ACTION_H
