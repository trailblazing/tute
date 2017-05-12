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

#include <cassert>
#include <memory>

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QException>
#include <QtCore/QMimeData>
#include <QtGui/QClipboard>
#include <QtGui/QDrag>
#include <QtGui/QMouseEvent>
#include <QtWebEngineWidgets/QWebEngineDownloadItem>
#include <QtWebEngineWidgets/QWebEngineProfile>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QListView>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStyle>
#include <QtWidgets/QToolButton>

//#include "models/tree/tree_item.dec"

#include "tabwidget.h"

#include "libraries/qt_single_application5/qtsingleapplication.h"

#include "browser.h"
#include "downloadmanager.h"
#include "fullscreennotification.h"
#include "history.h"
#include "urllineedit.h"
#include "webview.h"
//#include "views/browser/webview.h"
#include "libraries/fixed_parameters.h"
#include "libraries/global_parameters.h"
#include "models/record_table/linker.hxx"
#include "models/record_table/record.h"
#include "models/record_table/record_index.hxx"
#include "models/tree/tree_index.hxx"
#include "views/browser/history.h"
#include "views/record_table/record_screen.h"
// #include "models/record_table/record_index.hxx"
#include "docker.h"
#include "libraries/disk_helper.h"
#include "libraries/global_parameters.h"
#include "libraries/walk_history.h"
#include "libraries/window_switcher.h"
#include "libraries/wyedit/editor_text_area.h"
#include "main.h"
#include "models/record_table/items_flat.h"
#include "models/record_table/record_model.h"
#include "models/tree/binder.hxx"
#include "models/tree/tree_item.h"
#include "models/tree/tree_know_model.h"
#include "toolbar_search.h"
#include "views/find_in_base_screen/find_screen.h"
#include "views/main_window/main_window.h"
#include "views/record/editor_wrap.h"
#include "views/record_table/record_view.h"
#include "views/tree/tree_screen.h"
#include "views/tree/tree_view.h"
// #include "models/record_table/record_index.hxx"
#include "controllers/record_table/record_controller.h"
#include "libraries/qtm/blogger.h"
#include "models/record_table/record_model.h"
#include "models/record_table/record_model_proxy.h"

//extern const char* custom_hidable_tab_style;
extern const char* program_title;

//const char* custom_browser_tabbar_style =
//    // "QTabWidget::pane {"
//    // "border: 0 px;"
//    // "}"
//    // "QTabBar::tab {"
//    //// "left 3 px;"
//    //// "background: transparent;"
//    //// "background-color: transparent;"
//    // "border-color: white;"
//    // "border-width: 1 px;"
//    // "border-style: solid;"
//    //// "alignment: left;"  // bottom
//    // "max-height: 16px;"
//    // "width: 100 px;"
//    // "max-width: 200px;"
//    //// "qproperty-alignment: AlignLeft;"             //  AlignCenter |
//    ///AlignVCenter;*/
//    // "qproperty-wordWrap: false;"
//    // "padding-bottom: 5px;"
//    //// "padding: 0 px;"
//    // "margin-left: 2 px;"
//    // "margin-right: 0 px;"
//    // "}"

//    // "QTabWidget::tab-bar {"
//    //// "background: transparent;"
//    //// "border-color: transparent;"
//    // "border-width: 0 px;"
//    // "border-style: solid;"
//    // "background-color: transparent;"
//    //// "alignment: bottom;"
//    // "width: 100 px;"
//    // "max-width : 200 px;"
//    // "text-align: left;"
//    //// "qproperty-alignment: AlignBottom;"
//    // "qproperty-wordWrap: false;"
//    // "padding-bottom: 0px;"
//    // "margin-left : 0 px;"
//    // "padding : 0 px;"
//    // "margin-right : 0 px;"
//    // "}"
//    // "QTabWidget{"
//    // "background: transparent;"
//    // "border-color: transparent;"
//    // "}"
//    // "QTabBar::tab:selected, QTabBar::tab:hover {"
//    // "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #fafafa,
//    // stop: 0.4 #f4f4f4, stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);"
//    // "font-color: black;"
//    // "}"
//    // "QTabBar::tab:selected {"
//    // "border-color: #9B9B9B;"
//    // "border-bottom-color: white;"	// #C2C7CB         /* same as pane color
//    // */
//    // "background-color: white;"
//    // "}"
//    // "QTabBar::tab:!selected {"
//    // "margin-left: 0px;"		/* make non-selected tabs look smaller
//    // */
//    // "background-color: transparent;"
//    // "border-bottom-color: transparent;"
//    // "}"
//    // "QTabBar::close-button {"
//    //// "image: url(:/New_UI/tab_close.png);"
//    //// "subcontrol-origin: padding;"
//    //// "subcontrol-position: right;"
//    // "width: 13px;"
//    // "height: 13px;"
//    //// "image: url(:/tab-close.png);"
//    //// "padding-left: -13px;"

//    // "}"

//    // hidden from css
//    "QTabWidget::pane {"
//    "border: 0 px;"
//    "}"
//    "QTabBar::tab {"
//    // "left 3 px;"
//    // "background: transparent;"
//    // "background-color: transparent;"
//    "border-color: white;"
//    "border-width: 0 px;" // "border-width: 1 px;"
//    "border-style: solid;"
//    // "alignment: left;"  // bottom
//    "max-height: 0 px;" // "max-height: 16px;"
//    "width: 0 px;"      // "width: 100 px;"
//    "max-width: 200px;"
//    // "qproperty-alignment: AlignLeft;"             //  AlignCenter |
//    // AlignVCenter;*/
//    "qproperty-wordWrap: false;"
//    "padding-bottom: 0 px;" // "padding-bottom: 5px;"
//    // "padding: 0 px;"
//    "margin-left: 0 px;" // "margin-left: 2 px;"
//    "margin-right: 0 px;"
//    "}"

//    "QTabWidget::tab-bar {"
//    // "background: transparent;"
//    // "border-color: transparent;"
//    "border-width: 0 px;"
//    "border-style: solid;"
//    "background-color: transparent;"
//    // "alignment: bottom;"
//    "width: 0 px;"      // "width: 100 px;"
//    "max-width : 0 px;" // "max-width : 200 px;"
//    "text-align: left;"
//    // "qproperty-alignment: AlignBottom;"
//    "qproperty-wordWrap: false;"
//    "padding-bottom: 0px;"
//    "margin-left : 0 px;"
//    "padding : 0 px;"
//    "margin-right : 0 px;"
//    "}"
//    "QTabWidget{"
//    "background: transparent;"
//    "border-color: transparent;"
//    "}"
//    "QTabBar::tab:selected, QTabBar::tab:hover {"
//    "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 0, stop: 0 #fafafa, "
//    "stop: 0.4 #f4f4f4, stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);" // "background:
//    // qlineargradient(x1:
//    // 0, y1: 0, x2:
//    // 0, y2: 1,
//    // stop: 0
//    // #fafafa,
//    // stop: 0.4
//    // #f4f4f4,
//    // stop: 0.5
//    // #e7e7e7,
//    // stop: 1.0
//    // #fafafa);"
//    "font-color: black;"
//    "}"
//    "QTabBar::tab:selected {"
//    "border-color: #9B9B9B;"
//    "border-bottom-color: white;" // #C2C7CB         /* same as pane color */
//    "background-color: white;"
//    "}"
//    "QTabBar::tab:!selected {"
//    "margin-left: 0px;" /* make non-selected tabs look smaller */
//    "background-color: transparent;"
//    "border-bottom-color: transparent;"
//    "}"
//    "QTabBar::close-button {"
//    // "image: url(:/New_UI/tab_close.png);"
//    // "subcontrol-origin: padding;"
//    // "subcontrol-position: right;"
//    "width: 0 px;"  // "width: 13px;"
//    "height: 0 px;" // "height: 13px;"
//    // "image: url(:/tab-close.png);"
//    // "padding-left: -13px;"

//    "}";

extern std::shared_ptr<gl_para> gl_paras;
extern std::shared_ptr<AppConfig> appconfig;
extern QMap<Qt::CheckState, QString> char_from_check_state;
extern QMap<QString, Qt::CheckState> check_state_from_char;
class QException;

#include <QtCore/QDebug>

namespace web {
	namespace alter {
		// #include <QtGui>

		//// #if QT_VERSION >= 0x050000
		//// #include <QtWidgets>
		//// #endif

		//// #include "ExtendedTabWidget.h"
		// W_OBJECT_IMPL(TabBar)
		// TabBar::TabBar(TabWidget *parent) : QTabBar(parent){
		// buttonGroup = new QButtonGroup;

		// stackWidget = parent;	// new QStackedWidget;
		//// stackWidget->setFrameShape(QFrame::StyledPanel);

		// connect(buttonGroup,  SIGNAL(buttonClicked(int)), this,
		// SLOT(setCurrentIndex(int)));

		// buttonLayout = new QVBoxLayout();
		// buttonLayout->setSpacing(0);

		// QVBoxLayout *buttonStretchLayout = new QVBoxLayout();
		// buttonStretchLayout->setSpacing(0);
		// buttonStretchLayout->addLayout(buttonLayout);
		// buttonStretchLayout->addStretch();

		// layout = new QHBoxLayout;
		// layout->setSpacing(0);
		// layout->setContentsMargins(0, 0, 0, 0);
		// layout->addLayout(buttonStretchLayout);
		//// layout->addWidget(stackWidget);
		// setLayout(layout);

		// setContextMenuPolicy(Qt::CustomContextMenu);
		// setAcceptDrops(true);
		// connect(this, &TabBar::customContextMenuRequested, this,
		// &TabBar::contextMenuRequested);

		// QString ctrl = QLatin1String("Ctrl+%1");
		// for(int i = 1; i < 10; ++ i){
		// QShortcut *shortCut = new QShortcut(ctrl.arg(i), this);
		// _tabshortcuts.append(shortCut);
		// connect(shortCut, &QShortcut::activated, this, &TabBar::selectTabAction);
		// }
		// setTabsClosable(true);

		// connect(static_cast<QTabBar *const>(this), &QTabBar::tabCloseRequested, this,
		// &TabBar::closeTabSignal);
		// setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
		// setMovable(true);

		// QFont font;
		// font.setFamily("Courier");
		// font.setStyleHint(QFont::Monospace);
		// font.setFixedPitch(true);
		// font.setPointSize(10);

		// this->setFont(font);

		// setStyleSheet(custom_widget_style);
		// }
		// QSize TabBar:: sizeHint() const {
		// int xMax = 0, yMax = 0;
		// foreach(QAbstractButton * button, buttonGroup->buttons()){
		// xMax = qMax(xMax, button->sizeHint().width());
		// yMax = qMax(yMax, button->sizeHint().height());
		// }

		// return QSize(xMax, yMax);
		// }
		// void TabBar:: removePage(int index){
		//// QWidget *widget = stackWidget->widget(index);
		// stackWidget->removeTab(index);	// removeWidget(widget);

		// QPushButton *button = (QPushButton *)buttonGroup->button(index);
		// buttonLayout->removeWidget(button);
		// buttonGroup->removeButton(button);
		// delete button;

		// setCurrentIndex(0);
		// }
		// int TabBar:: count() const {
		// return stackWidget->count();
		// }
		// int TabBar:: currentIndex() const {
		// return stackWidget->currentIndex();
		// }
		// int TabBar:: addTab(QWidget *page, const QString &title){return addPage(page,
		// QIcon(), title);}

		// int TabBar:: addTab(QWidget *page, const QIcon &icon, const QString
		// &title){return addPage(page, icon, title);}

		// int TabBar:: addPage(QWidget *page, const QIcon &icon, const QString &title){
		// return insertPage(count(), page, icon, title);
		// }
		// int TabBar:: insertPage(int index, QWidget *page, const QIcon &icon, const
		// QString &title){
		// page->setParent(stackWidget);
		// stackWidget->insertTab(index, page, title);	// insertWidget(index, page);

		//// Set label
		// QString label = title;
		// if(label.isEmpty()){
		// label = QApplication::translate(((QObject
		// *)parent())->objectName().toLatin1().constData(),
		// titleList.value(index).toLatin1().constData());
		// if(label.isEmpty())label = tr("Page %1").arg(index);
		// }
		// page->setWindowTitle(label);

		//// Set icon
		// QIcon pix = icon;
		// if(pix.isNull()){
		// pix = QIcon(iconList.value(index));
		// if(pix.isNull()){
		// pix = QApplication::style()->standardIcon(QStyle::SP_ArrowUp);
		// page->setWindowIcon(pix);
		// }
		// }else page->setWindowIcon(pix);
		//// Add QPushButton
		// QPushButton *button = new QPushButton(pix, label);
		// button->setObjectName("__qt__passive_pushButton");	// required for
		// interaction within Designer
		// button->setCheckable(true);
		// if(count() == 1)button->setChecked(true);
		// buttonGroup->addButton(button, index);
		// buttonLayout->addWidget(button);

		// return index;
		// }
		// void TabBar:: setCurrentIndex(int index){
		// if(index < 0 || index >= count())index = 0;
		// if(index != currentIndex()){
		// stackWidget->setCurrentIndex(index);
		// buttonGroup->button(index)->setChecked(true);
		// emit currentIndexChanged(index);
		// }
		// }
		// QWidget *TabBar:: widget(int index){
		// return stackWidget->widget(index);
		// }
		// int TabBar:: indexOf(QWidget *widget){
		// for(int i = 0; i < stackWidget->count(); i ++){
		// if(stackWidget->widget(i) == widget)return i;
		// }
		// return - 1;
		// }
		// bool TabBar:: setVisible(QWidget *w, bool b){
		// int index = indexOf(w);
		// if(index == - 1)return false;
		// if(currentIndex() == index)setCurrentIndex(0);
		// buttonGroup->button(index)->setVisible(b);

		// return true;
		// }
		// bool TabBar:: setEnabled(QWidget *w, bool b){
		// int index = indexOf(w);
		// if(index == - 1)return false;
		// if(currentIndex() == index)setCurrentIndex(0);
		// buttonGroup->button(index)->setEnabled(b);

		// return true;
		// }
		// void TabBar:: setTabText(int index, const QString &title){setPageTitle(index,
		// title);}

		// QStringList TabBar:: pageTitleList() const {
		// QStringList titleList;
		// for(int i = 0; i < stackWidget->count(); i ++)titleList <<
		// stackWidget->widget(i)->windowTitle();
		// return titleList;
		// }
		// QString TabBar:: pageTitle() const {
		// if(const QWidget *currentWidget = stackWidget->currentWidget())return
		// currentWidget->windowTitle();
		// return QString();
		// }
		// QStringList TabBar:: pageIconList() const {
		// QStringList iconList;
		// for(int i = 0; i < stackWidget->count(); i ++)iconList <<
		// stackWidget->widget(i)->windowIcon().name();;

		// return iconList;
		// }
		// QIcon TabBar:: pageIcon() const {
		// if(const QWidget *currentWidget = stackWidget->currentWidget())return
		// currentWidget->windowIcon();
		// return QIcon();
		// }
		// void TabBar:: setPageTitleList(QStringList const &newTitleList){
		// titleList = newTitleList;
		//// we have to force translation here
		// for(int i = 0; i < titleList.count(); ++ i)titleList[i] =
		// tr(titleList[i].toLatin1());
		// if(! count())return;
		// for(int i = 0; i < stackWidget->count() && i < titleList.count(); i ++){
		// buttonGroup->button(i)->setText(titleList.at(i));
		// stackWidget->widget(i)->setWindowTitle(titleList.at(i));
		// }
		// }
		// void TabBar:: setPageTitle(QString const &newTitle){
		// if(! count())return;
		// buttonGroup->button(currentIndex())->setText(newTitle);
		// if(QWidget *currentWidget =
		// stackWidget->currentWidget())currentWidget->setWindowTitle(newTitle);
		// emit pageTitleChanged(newTitle);
		// }
		// void TabBar:: setPageTitle(int index, QString const &newTitle){
		// if(index < 0 || index >= count())return;
		// buttonGroup->button(index)->setText(newTitle);
		// if(QWidget *currentWidget =
		// stackWidget->widget(index))currentWidget->setWindowTitle(newTitle);
		// emit pageTitleChanged(newTitle);
		// }
		// void TabBar:: setPageIconList(QStringList const &newIconList){
		// iconList = newIconList;
		// if(! count())return;
		// for(int i = 0; i < stackWidget->count() && i < newIconList.count(); i ++){
		// buttonGroup->button(i)->setIcon(QIcon(newIconList.at(i)));
		// stackWidget->widget(i)->setWindowIcon(QIcon(newIconList.at(i)));
		// }
		// }
		// void TabBar:: setPageIcon(QIcon const &newIcon){
		// buttonGroup->button(currentIndex())->setIcon(newIcon);
		// if(QWidget *currentWidget =
		// stackWidget->currentWidget())currentWidget->setWindowIcon(newIcon);
		// emit pageIconChanged(newIcon);
		// }
		// void TabBar:: selectTabAction(){
		// if(QShortcut *shortCut = qobject_cast<QShortcut *>(sender())){
		// int index = _tabshortcuts.indexOf(shortCut);
		// setCurrentIndex(index);
		// }
		// }
		// void TabBar:: cloneTab(){
		// if(QAction *action = qobject_cast<QAction *>(sender())){
		// int	index = action->data().toInt();
		// emit	cloneTabSignal(index);
		// }
		// }
		// void TabBar:: closeTab(){
		// if(QAction *action = qobject_cast<QAction *>(sender())){
		// int	index = action->data().toInt();
		// emit	closeTabSignal(index);
		// }
		// }
		// void TabBar:: closeOtherTabs(){
		// if(QAction *action = qobject_cast<QAction *>(sender())){
		// int	index = action->data().toInt();
		// emit	closeOtherTabsSignal(index);
		// }
		// }
		// void TabBar:: reloadTab(){
		// if(QAction *action = qobject_cast<QAction *>(sender())){
		// int	index = action->data().toInt();
		// emit	reloadTabSignal(index);
		// }
		// }
		// void TabBar:: contextMenuRequested(const QPoint &position){
		// QMenu menu;

		//// menu.addAction(tr("New &Tab"), this, &TabBarExtended::newTab,
		///QKeySequence::AddTab);

		// int index = tabAt(position);
		// if(- 1 != index){
		//// QAction *action = menu.addAction(tr("Clone Tab"), this,
		///&TabBarExtended::cloneTab);
		//// action->setData(index);

		//// menu.addSeparator();

		// QAction *action = menu.addAction(tr("&Close Tab"), this, &TabBar::closeTab,
		// QKeySequence::Close);
		// action->setData(index);

		// action = menu.addAction(tr("Close &Other Tabs"), this,
		// &TabBar::closeOtherTabs);
		// action->setData(index);

		// menu.addSeparator();

		// action = menu.addAction(tr("Reload Tab"), this, &TabBar::reloadTab,
		// QKeySequence::Refresh);
		// action->setData(index);
		// }else{
		// menu.addSeparator();
		// }
		// menu.addAction(tr("Reload All Tabs"), this, &TabBar::reloadAllTabs);
		// menu.exec(QCursor::pos());
		// }
		// void TabBar:: mousePressEvent(QMouseEvent *event){
		// if(event->button() == Qt::LeftButton)_dragstartpos = event->pos();
		// QTabBar::mousePressEvent(event);
		// }
		// void TabBar:: mouseMoveEvent(QMouseEvent *event){
		// if(event->buttons() == Qt::LeftButton){
		// int	diffX = event->pos().x() - _dragstartpos.x();
		// int	diffY = event->pos().y() - _dragstartpos.y();
		// if(  (event->pos() - _dragstartpos).manhattanLength() >
		// QApplication::startDragDistance()
		// && diffX < 3 && diffX > - 3
		// && diffY < - 10){
		// QDrag	*drag = new QDrag(this);
		// QMimeData	*mimeData = new QMimeData;
		// QList<QUrl> urls;
		// int		index = tabAt(event->pos());
		// QUrl	url = tabData(index).toUrl();
		// urls.append(url);
		// mimeData->setUrls(urls);
		// mimeData->setText(tabText(index));
		// mimeData->setData(QLatin1String("action"), "tab-reordering");
		// drag->setMimeData(mimeData);
		// drag->exec();
		// }
		// }
		// QTabBar::mouseMoveEvent(event);
		// }
	}

	namespace origin {
#if QT_VERSION == 0x050600
		W_OBJECT_IMPL(TabBar)
#endif

		TabBar::TabBar(QWidget* parent)
		    : QTabBar(parent)
		{
			setContextMenuPolicy(Qt::CustomContextMenu);
			setAcceptDrops(true);
			connect(this, &TabBar::customContextMenuRequested, this, &TabBar::contextMenuRequested);

			QString ctrl = QLatin1String("Ctrl+%1");
			for (int i = 1; i < 10; ++i) {
				QShortcut* shortCut = new QShortcut(ctrl.arg(i), this);
				_tabshortcuts.append(shortCut);
				connect(shortCut, &QShortcut::activated, this, &TabBar::selectTabAction);
			}
			setTabsClosable(true);

			connect(static_cast<QTabBar* const>(this), &QTabBar::tabCloseRequested, this, &TabBar::closeTabSignal);
			////	    connect(static_cast<QTabBar *const>(this), &QTabBar::lastTabClosed,
			///globalparameters.main_window()->vtab_record(),
			///&HidableTabWidget::tabCloseRequested);
			// setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
			setMovable(true);
			// setStyleSheet("QTabBar::tab { left: 1px; max-width: 200px; align: left;
			// text-align: left; margin-left: 2px; padding: 2px;}");  // ?

			// setStyleSheet("QTabBar::tab { max-width: 200px; padding: 2px; margin-left:
			// 2px; }");

			// setStyleSheet(
			// "QTabWidget::pane { border: 0 px; } "
			// "QTabBar::tab {"
			// "left 3px;"
			// "alignment: bottom; "
			// "height: 100px; "
			// "width: 16 px; "
			// "max-width: 20px; "
			// "qproperty-alignment: AlignBottom; "  // AlignCenter | AlignVCenter;
			// "qproperty-wordWrap: false; "
			// "padding-bottom: 0px; "
			// "padding: 0 px; "
			// "margin-left: 2 px; "
			// "margin-right: 0 px; "
			// "}"    //   margin: 0 px;
			// "QTabWidget::tab-bar {"
			// "alignment: bottom;"
			// " height: 100px;"
			// "width: 20px;"
			// "max-width: 18 px;"
			// "text-align: left;"
			// "qproperty-alignment: AlignBottom;"
			// "qproperty-wordWrap: false;"
			// "padding-bottom: 0px;"
			// "margin-left: 2 px;"
			// "padding: 0 px;"
			// "margin-right: 0 px;"
			// "}"   // margin: 0 px;
			// );

			// TabWidget* tabmanager = dynamic_cast<TabWidget*>(parent);
			// QString style_source = globalparameters.entrance()->style_source();

			// this->setFixedWidth(200);

			QFont font;
			font.setFamily("Courier");
			font.setStyleHint(QFont::Monospace);
			font.setFixedPitch(true);
			font.setPointSize(10);

			// QTextEdit* editor = new QTextEdit();
			// editor->setFont(font);
			this->setFont(font);

			this->setStyle(new CustomTabStyle()); // QStyle::CT_TabBarTab, new
							      // QStyleOption(QStyleOption::Version,
							      // QStyleOption::SO_Default),
							      // this->size(), this

			set_css_style(this, gl_paras->root_path(), gl_paras->target_os(), gl_para::_custom_browser_tabbar_style); //setStyleSheet(custom_browser_tabbar_style);
		}

		void TabBar::selectTabAction()
		{
			if (QShortcut* shortCut = qobject_cast<QShortcut*>(sender())) {
				int index = _tabshortcuts.indexOf(shortCut);
				setCurrentIndex(index);
			}
		}

		void TabBar::contextMenuRequested(const QPoint& position)
		{
			QMenu menu;

			// menu.addAction(tr("New &Tab"), this, &TabBar::newTab,
			// QKeySequence::AddTab);

			int index = tabAt(position);
			if (-1 != index) {
				// QAction *action = menu.addAction(tr("Clone Tab"), this,
				// &TabBar::cloneTab);
				// action->setData(index);

				// menu.addSeparator();

				QAction* action = menu.addAction(tr("&Close Tab"), this, &TabBar::closeTab, QKeySequence::Close);
				action->setData(index);

				action = menu.addAction(tr("Close &Other Tabs"), this, &TabBar::closeOtherTabs);
				action->setData(index);

				menu.addSeparator();

				action = menu.addAction(tr("Reload Tab"), this, &TabBar::reloadTab, QKeySequence::Refresh);
				action->setData(index);
			} else
				menu.addSeparator();
			menu.addAction(tr("Reload All Tabs"), this, &TabBar::reloadAllTabs);
			menu.exec(QCursor::pos());
		}

		void TabBar::cloneTab()
		{
			if (QAction* action = qobject_cast<QAction*>(sender())) {
				int index = action->data().toInt();
				emit cloneTabSignal(index);
			}
		}

		void TabBar::closeTab()
		{
			if (QAction* action = qobject_cast<QAction*>(sender())) {
				int index = action->data().toInt();
				emit closeTabSignal(index);
			}
		}

		void TabBar::closeOtherTabs()
		{
			if (QAction* action = qobject_cast<QAction*>(sender())) {
				int index = action->data().toInt();
				emit closeOtherTabsSignal(index);
			}
		}

		void TabBar::mousePressEvent(QMouseEvent* event)
		{
			if (event->button() == Qt::LeftButton) _dragstartpos = event->pos();
			QTabBar::mousePressEvent(event);
		}

		void TabBar::mouseMoveEvent(QMouseEvent* event)
		{
			if (event->buttons() == Qt::LeftButton) {
				int diffX = event->pos().x() - _dragstartpos.x();
				int diffY = event->pos().y() - _dragstartpos.y();
				if ((event->pos() - _dragstartpos).manhattanLength() > QApplication::startDragDistance() && diffX < 3 && diffX > -3 && diffY < -10) {
					QDrag* drag = new QDrag(this);
					QMimeData* mimeData = new QMimeData;
					QList<QUrl> urls;
					int index = tabAt(event->pos());
					QUrl url = tabData(index).toUrl();
					urls.append(url);
					mimeData->setUrls(urls);
					mimeData->setText(tabText(index));
					mimeData->setData(QLatin1String("action"), "tab-reordering");
					drag->setMimeData(mimeData);
					drag->exec();
				}
			}
			QTabBar::mouseMoveEvent(event);
		}

		void TabBar::reloadTab()
		{
			if (QAction* action = qobject_cast<QAction*>(sender())) {
				int index = action->data().toInt();
				emit reloadTabSignal(index);
			}
		}
	}
#if QT_VERSION == 0x050600
	W_OBJECT_IMPL(TabWidget)
#endif

	TabWidget::TabWidget(Blogger* blogger_, web::Browser* browser_, wn_t* main_window_ //, web::Profile* profile_
	    )
#ifdef USE_SIGNAL_CLOSE
	    : QTabWidget(), sd::renter() //sd::renter<TabWidget>()
#else
	    : QTabWidget(browser_)
#endif //USE_SIGNAL_CLOSE
	    , _tree_screen(gl_paras->tree_screen())
	    , _blogger(blogger_)
	    , _browser_docker(gl_paras->browser_docker())
	    , _browser(browser_)
	    , _main_window(main_window_)
	    , _recentlyclosedtabsaction(new QAction(tr("Recently Closed Tabs"), this))
	    , _newtabaction(new QAction(QIcon(QLatin1String(":addtab.png")), tr("New &Tab"), this))
	    , _closetabaction(new QAction(QIcon(QLatin1String(":closetab.png")), tr("&Close Tab"), this))
	    , _nexttabaction(new QAction(tr("Show Next Tab"), this))
	    , _previoustabaction(new QAction(tr("Show Previous Tab"), this))
	    , _recentlyclosedtabsmenu(new QMenu(this))
	    // , _completionModel(new HistoryCompletionModel(this))
	    , _lineeditcompleter(nullptr) // new QCompleter(_completionModel, this)
	    , _lineedit_stack([&] {
		    _lineedit_stack = nullptr;
		    auto ls = new QStackedWidget(this);
		    ls->setObjectName(gl_para::_browser_lineedit_stack_name);
		    return ls;
	    }())
	    , _tabbar(new TabBar(this))
	    , _profile(new web::Profile(_blogger->topic(), this)) //profile_storage_name,//(profile_) // globalparameters.profile()   // QWebEngineProfile::defaultProfile()
	    , _fullscreenview(nullptr)
	    , _fullscreennotification(nullptr)
	    , _record_screen(new rs_t(blogger_, browser_, this)
		  //#ifdef USE_SIGNAL_CLOSE
		  //		  sd::make_intrusive<rs_t>(blogger_, browser_, this)
		  //#else
		  //		  new rs_t(blogger_, browser_, this)
		  //#endif //USE_SIGNAL_CLOSE

		  )
	{
		//		_record_screen = sd::make_intrusive<rs_t>(blogger_, browser_, this);

		//		assert(_record_controller); // _record_controller = new
		//rctl_t(_editor_screen, this, _record_screen, _main_window);
		//		assert(_record_controller->tabmanager());
		setTabBar(_tabbar);
		setTabPosition(TabPosition::West); // South
						   // setTabShape(TabShape::Triangular);

#ifdef USE_QT_DELETE_ON_CLOSE
		setAttribute(Qt::WA_DeleteOnClose, true);
#endif // USE_QT_DELETE_ON_CLOSE
		// , _active_record(this)
		// , _active("", &active_record::operator(), &_active_record)
		// , _shadow_branch_root(_shadow_branch_root)
		// , _shadow_source_model(new TableModel(QString(table_screen_singleton_name)
		// + QString("_shadow"), _tree_item, this))
		// , _table_data(std::make_shared<TableData>(_tree_item))
		auto find_screen = gl_paras->find_screen();
		if (find_screen) {
			if (find_screen->lineedit_stack() != _lineedit_stack) find_screen->switch_stack(_lineedit_stack);
		}
		// _lineedits = globalparameters.find_screen()->toolbarsearch()->lineedits();
		// connect(parent, []() {}, globalparameters.find_screen()->toolbarsearch(),
		// [this]()
		// {globalparameters.find_screen()->toolbarsearch()->lineedits(_lineedits);});
		// move to     void web::activateWindow();

		// _tree_screen->tabmanager =
		// std::make_shared<sd::_interface<sd::meta_info<void >, TabWidget *>>("",
		// &TabWidget::tabmanager, this);
		// _find_screen->tabmanager =
		// std::make_shared<sd::_interface<sd::meta_info<void >, TabWidget *>>("",
		// &TabWidget::tabmanager, this);

		setElideMode(Qt::ElideRight);
		// _active_record = [this](Record * const record)-> WebView * {return
		// globalparameters.entrance()->active_record().first->tabWidget()->newTab(record);};

		//		auto _tree_screen->view() = _tree_screen->view();
		//		auto  _tree_screen->view()->current_item() =
		//tree_view->current_item();// maybe no current item yet!
		//		auto parent = _tree_screen->view()->current_item()->parent();
		//		if(!parent) throw std::runtime_error("! parent");
		connect(_tabbar, &TabBar::newTab, [&]() {
			auto it = real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(web::Browser::_defaulthome,
			    [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
				    return TreeIndex::url_activate(
					real_target_url_, //std::placeholders::_1,
					[&] { return _tree_screen->view()->source_model(); },
					_tree_screen->view()->current_item(),
					std::bind(&tv_t::move, _tree_screen->view(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
					[](boost::intrusive_ptr<const i_t> it_) -> bool { return url_equal(url_value(detail::to_qstring(it_->field<home_key>())), web::Browser::_defaulthome) || url_equal(it_->field<url_key>(), web::Browser::_defaulthome); },
					_browser);
			    });

			//				TreeIndex::url_activate(real_url_t<url_value>::instance(web::Browser::_defaulthome),
			//			    [&] { return _tree_screen->view()->source_model(); },
			//			    _tree_screen->view()->current_item(),
			//			    std::bind(&tv_t::move, _tree_screen->view(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
			//			    [](boost::intrusive_ptr<const i_t> it_) -> bool { return url_equal(url_value(detail::to_qstring(it_->field<home_key>())), web::Browser::_defaulthome) || url_equal(it_->field<url_key>(), web::Browser::_defaulthome); },
			//			    _browser);

			if (currentWebView() != it->page()->view()) setCurrentIndex(webViewIndex(it->page()->view()));
		});

		//		connect(_tabbar, &TabBar::closeTabSignal, this, &TabWidget::requestCloseTab);
		connect(_tabbar, &TabBar::closeTabSignal, this, &TabWidget::closeTab); // added by hughvonyoung@gmail.com
										       //		connect(this, &TabWidget::requestCloseTab, this, &TabWidget::closeTab); // this is not a signal
		// connect(_tabbar, &TabBar::cloneTabSignal, this, &TabWidget::cloneTab);//
		// removed by hughvonyoung@gmail.com
		connect(_tabbar, &TabBar::closeOtherTabsSignal, this, &TabWidget::closeOtherTabs);
		connect(_tabbar, &TabBar::reloadTabSignal, this, &TabWidget::reloadTab);
		connect(_tabbar, &TabBar::reloadAllTabs, this, &TabWidget::reloadAllTabs);

		connect(_tabbar, &TabBar::tabMoved, this, &TabWidget::moveTab);

		connect(this, &TabWidget::tabsChanged, this, &TabWidget::onTabsChanged);
		// connect(this, &TabWidget::lastTabClosed, [&] {
		// auto index = _main_window->vtab_record()->indexOf(_record_screen);
		// if(index != -1) emit _main_window->vtab_record()->tabCloseRequested(index);
		// });
		setDocumentMode(true);

		// _tabbar->setMaximumSize(0, 0);

		{
			// _tabbar->hide();// hidden from css for sometime I can't call count()
			// function if I hide tab bar at here
			//			_tabbar->show();
		}

		// Actions
		// _newtabaction = ;
		_newtabaction->setShortcuts(QKeySequence::AddTab //QList<QKeySequence>() << QKeySequence(Qt::ControlModifier + Qt::Key_T)
		    );                                           //QKeySequence::AddTab
		_newtabaction->setIconVisibleInMenu(false);
		connect(_newtabaction, &QAction::triggered, // , this
		    [&](bool make_current) {
			    Q_UNUSED(make_current)
			    auto it = real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(web::Browser::_defaulthome,
				[&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
					return TreeIndex::url_activate(
					    real_target_url_, //std::placeholders::_1,
					    [&] { return _tree_screen->view()->source_model(); },
					    _tree_screen->view()->current_item(),
					    std::bind(&tv_t::move, _tree_screen->view(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
					    [](boost::intrusive_ptr<const i_t> it_) -> bool { return url_equal(url_value(detail::to_qstring(it_->field<home_key>())), web::Browser::_defaulthome) || url_equal(url_value(detail::to_qstring(it_->field<url_key>())), web::Browser::_defaulthome); },
					    _browser);
				});

			    //				    TreeIndex::url_activate(real_url_t<url_value>::instance(web::Browser::_defaulthome),
			    //				[&] { return _tree_screen->view()->source_model(); },
			    //				_tree_screen->view()->current_item(),
			    //				std::bind(&tv_t::move, _tree_screen->view(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
			    //				[](boost::intrusive_ptr<const i_t> it_) -> bool { return url_equal(url_value(detail::to_qstring(it_->field<home_key>())), web::Browser::_defaulthome) || url_equal(url_value(detail::to_qstring(it_->field<url_key>())), web::Browser::_defaulthome); },
			    //				_browser);
			    if (currentWebView() != it->page()->view()) setCurrentIndex(webViewIndex(it->page()->view()));
		    });

		// _closetabaction = ;
		_closetabaction->setShortcuts(QKeySequence::Close);
		_closetabaction->setIconVisibleInMenu(false);
		connect(_closetabaction, &QAction::triggered, [&](bool) {
			closeTab(webViewIndex(currentWebView()));
		}); // , this, &TabWidget::closeTab);

		// _nexttabaction = ;
		QList<QKeySequence> shortcuts;
		shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BraceRight));
		shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_PageDown));
		shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BracketRight));
		shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Less));
		_nexttabaction->setShortcuts(shortcuts);
		connect(_nexttabaction, &QAction::triggered, this, &TabWidget::nextTab);

		// _previoustabaction = ;
		shortcuts.clear();
		shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BraceLeft));
		shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_PageUp));
		shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BracketLeft));
		shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Greater));
		_previoustabaction->setShortcuts(shortcuts);
		connect(_previoustabaction, &QAction::triggered, this, &TabWidget::previousTab);

		// _recentlyclosedtabsmenu = ;
		connect(_recentlyclosedtabsmenu, &QMenu::aboutToShow, this, &TabWidget::aboutToShowRecentTabsMenu);
		connect(_recentlyclosedtabsmenu, &QMenu::triggered, this, &TabWidget::aboutToShowRecentTriggeredAction);
		// _recentlyclosedtabsaction = ;
		_recentlyclosedtabsaction->setMenu(_recentlyclosedtabsmenu);
		_recentlyclosedtabsaction->setEnabled(false);

		connect(static_cast<QTabWidget*>(this), &QTabWidget::currentChanged, this, &TabWidget::currentChanged);

		// _lineedits = ;

		// _lineedits->setMaximumSize(0, 0);
		{       //?
			//			_lineedit_stack->hide();
		}
// setStyleSheet("QTabWidget::tab-bar { max-width: 200px; align: left;
// text-align: left; margin-left: 2px; padding: 2px;}");         // left: 1px;
// alignment: left; // does not work
// Qt::Alignment tabAlignment =
// Qt::Alignment(q->style()->styleHint(QStyle::SH_TabBar_Alignment, 0, q));
#ifdef USE_SIGNAL_CLOSE


		destroy_transfer([&](sd::renter* const r) {
			(void)r;
			if (r != this) { //&& !this->_destroyed_request_sent
				close_trigger_from_others()(r);
				//				this->_destroyed_request_sent =
				this->close();
				//				delete this;// do it by browser
			}
		}); //std::make_shared<sd::method<sd::meta_info<void>>>("", &web::TabWidget::close, &_closed, this) //close_requested.connect(std::bind(&web::TabWidget::close, this));


#endif //USE_SIGNAL_CLOSE
	}

	// When index is -1 index chooses the current tab
	void TabWidget::reloadTab(int index)
	{
		if (index < 0) index = currentIndex();
		if (index < 0 || index >= count()) return;
		QWidget* widget = this->widget(index);
		if (WebView* tab = qobject_cast<WebView*>(widget)) tab->reload();
	}

	TabWidget::~TabWidget()
	{
		//		this->_destroyed_request_triggered = true;
		////        delete _profile;
		// delete _tabbar;
		// delete _lineedits;
		// delete _lineeditcompleter;
		//// delete _completionModel;
		// delete _recentlyclosedtabsmenu;
		// delete _previoustabaction;
		// delete _nexttabaction;
		// delete _closetabaction;
		// delete _newtabaction;
		// delete _recentlyclosedtabsaction;
		if (_fullscreennotification) {
			delete _fullscreennotification;
			_fullscreennotification = nullptr;
		}
		if (_fullscreenview) {
			delete _fullscreenview;
			_fullscreenview = nullptr;
		}
// delete _record_controller;
//		close_requested(this);
//		delete _record_screen;
#ifdef USE_SIGNAL_CLOSE
		if (_browser) {
			if (!_browser->close_request_sent() && !_browser->destroy_request_sent())

				//			_browser->close_connect([&](renter* const r) {if(r != this && !this->_closed) this->_closed = this->close(); });
				//			destroy_transfer(
				_browser->destroy_trigger_from_others()(this);
			//				    ); //std::bind(&Browser::close_requested_from_others, _browser, static_cast<sd::renter* const>(this))
		}


		// let Browser::~Browser() do it
		if (_record_screen) {                                                                           // close_connect(std::make_shared<sd::method<sd::meta_info<void>>>("", &rs_t::on_close_requested, _record_screen, static_cast<sd::renter* const>(this))); //close_requested.connect(_record_screen->close_requested);               //(std::bind(&rs_t::self_close_request, _record_screen));
			if (!_record_screen->close_request_sent() && !_record_screen->destroy_request_sent()) { //			destroy_transfer(
														//				_record_screen->destroy_trigger_from_others()(this);
				//				    ); // std::bind(&rs_t::close_requested_from_others, _record_screen, static_cast<sd::renter* const>(this))
				delete static_cast<rs_t*>(_record_screen);
			}
		}

//		_blogger->destroy_trigger_from_others()(nullptr);
//		while (this->count() > 0) this->closeTab(this->count() - 1);


#endif // USE_SIGNAL_CLOSE
		auto find_screen = gl_paras->find_screen();
		if (find_screen) {
			if (find_screen->lineedit_stack() == _lineedit_stack) find_screen->switch_stack();
		}
	}

	void TabWidget::clear()
	{
		// clear the recently closed tabs
		_recentlyclosedtabs.clear();
		// clear the line edit history
		for (int i = 0; i < _lineedit_stack->count(); ++i) {
			QLineEdit* qLineEdit = lineEdit(i);
			qLineEdit->setText(qLineEdit->text());
		}
	}

	void TabWidget::moveTab(int fromIndex, int toIndex)
	{
		auto pre_view = webView(_previous_index);
		QWidget* lineEdit = _lineedit_stack->widget(fromIndex);
		auto _rctrl = _record_screen->record_ctrl();
		if (_rctrl) {
			_lineedit_stack->removeWidget(lineEdit);
			_lineedit_stack->insertWidget(toIndex, lineEdit);

			auto source_index = static_cast<QModelIndex>(
			    _rctrl->index<index_source>(pos_source(fromIndex)));
			auto target_index = static_cast<QModelIndex>(
			    _rctrl->index<index_source>(pos_source(toIndex)));
			_rctrl->source_model()->moveRow(source_index.parent(), source_index.row(), target_index.parent(), target_index.row());
		}
		if (_previous_index == fromIndex || _previous_index == toIndex) _previous_index = webViewIndex(pre_view);
	}

	// void TabWidget::setCurrentIndex(int index){
	// QTabWidget::setCurrentIndex(index);
	// WebView *current_view = this->currentWebView();
	////        if(current_view != nullptr && current_view != webView(index)){
	// current_view->setFocus();
	// boost::intrusive_ptr<TreeItem> current_item = current_view->page()->host();
	// if(current_item){
	// assert((current_item->page_valid() && current_item->page()) ||
	// (current_item->field<url_type>() == web::Browser::_defaulthome));
	////                        // QModelIndex proxyindex =
	///view->recordtablecontroller()->convertIdToProxyIndex(record->getField("id"));
	////                        // int position =
	///view->recordtablecontroller()->convertProxyIndexToPos(proxyindex);
	////                        // RecordTableView *recordtableview =
	///view->recordtablecontroller()->getView();
	////                        //
	///if(recordtableview)recordtableview->setSelectionToPos(position); // work
	////                        current_view->setFocus();
	////                        // globalparameters.mainwindow()
	// if(_record_controller->view()->current_item() !=
	// current_item)_record_controller->cursor_to_index(_record_controller->index<PosProxy>(current_item));
	// // IdType(item->field("id"))
	////                                selection_first<IdType>() !=
	///current_item->field<id_type>()
	// if(_tree_screen->view()->current_item() !=
	// current_item)_tree_screen->view()->select_as_current(TreeIndex::instance([&]
	// {return _tree_screen->view()->source_model(); }, current_item->parent(),
	// current_item));
	// }
	////        }
	// }

	void TabWidget::addWebAction(QAction* action, QWebEnginePage::WebAction webAction)
	{
		if (!action) return;
		_actions.append(new WebActionMapper(action, webAction, this));
	}

	WebView* TabWidget::select_as_current(WebView* v)
	{
		auto index = webViewIndex(v);
		if (currentIndex() != index) {
			setCurrentIndex(index);
			auto tree_screen = gl_paras->main_window()->tree_screen();
			auto page = v->page();
			//		if(!v->isActiveWindow()) v->activateWindow();
			if (page) {
				auto it = v->page()->host();
				if (it) {
					if (static_cast<QModelIndex>(tree_screen->view()->source_model()->index(it)).isValid()) {
						if (tree_screen->view()->current_item() != it) tree_screen->view()->select_as_current(TreeIndex::item_require_treeindex([&] { return tree_screen->view()->source_model(); }, it));
						auto _rctrl = _record_screen->record_ctrl();
						if (_rctrl) {

#ifdef USE_HAS_SELECTION
							auto _record_view = _rctrl->view();
							QItemSelectionModel* item_selection_model = _record_view->selectionModel();
							bool has_selection = item_selection_model->hasSelection();
#endif // USE_HAS_SELECTION
							if (_rctrl->view()->current_item() != it
#ifdef USE_HAS_SELECTION
							    || !has_selection
#endif // USE_HAS_SELECTION

							    )
								_rctrl->select_as_current(_rctrl->index<pos_proxy>(pos_source(index)));
						}
						if (page->title() != it->field<name_key>()) page->record_info_update(page->url(), page->title());
						_main_window->synchronize_title(it->field<name_key>());

						it->activate(std::bind(&wn_t::find, gl_paras->main_window(), std::placeholders::_1));
					} else
						closeTab(index);
				}
			}
		}
		return currentWebView();
	}

	boost::intrusive_ptr<i_t> TabWidget::current_item()
	{
		boost::intrusive_ptr<i_t> result(nullptr);
		auto view = currentWebView();
		//		assert(view);
		if (view) {
			auto page = view->page();
			if (page) {
				auto host = page->host();
				if (host) result = host;
			}
		} else {
			if (count() > 0) {
				view = webView(0);
				if (view) {
					select_as_current(view);
					auto page = view->page();
					if (page) {
						auto host = page->host();
						if (host) result = host;
					}
				}
			}
		}
		//		assert(result);
		return result;
	}

	WebView* TabWidget::currentWebView() const
	{
		WebView* v = nullptr;
		if (currentIndex() == -1) {
			//			auto tree_view = _tree_screen->view();
			//			auto current_item = tree_view->current_item();

			//			v = _browser->bind(RecordIndex::instance([&] {
			//									 if(!_rctrl) const_cast<TabWidget *>(this)->_rctrl =
			//_browser->record_screen()->record_ctrl();
			//									 return
			//_rctrl->source_model();
			//								 }, nullptr,
			//current_item))->activate(std::bind(&wn_t::find, gl_paras->main_window(),
			//std::placeholders::_1));
		} else
			v = webView(currentIndex());
		return // webView(currentIndex()); //
		    v;
	}

	void TabWidget::currentChanged(int index_)
	{ // switch tab

		const int index = index_;
		auto synchronize = [&](WebView* view_current) {
			view_current->setFocus();

			// auto controller = webView->recordtablecontroller();
			auto _target_item = view_current->page()->host();
			if (_target_item) { // controller != nullptr &&
				// QModelIndex proxyindex =
				// controller->convertIdToProxyIndex(record->getField("id"));
				// int position = controller->convertProxyIndexToPos(proxyindex);
				// RecordTableView *recordtableview = controller->getView();
				// if(recordtableview)recordtableview->setSelectionToPos(position); //
				// work
				view_current->setFocus();
				_target_item->add_rating();
				auto _tree_view = _tree_screen->view();
				QModelIndex index_on_tree = _tree_view->source_model()->index(_target_item);
				if (!index_on_tree.isValid()) {
					if (!static_cast<QModelIndex>(_tree_view->know_model_board()->index(_target_item)).isValid()) {
						auto _url = QUrl(_target_item->field<url_key>()); //
						_target_item = real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(_target_item->field<url_key>(),
						    [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
							    return TreeIndex::url_require_item_from_tree(real_target_url_, //real_url_t<url_value>::instance(_target_item->field<url_key>()),
								std::bind(&tv_t::move, _tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), [&](boost::intrusive_ptr<const i_t> it_) -> bool {
									return url_equal(detail::to_string(it_->field<home_key>()), _url.toString().toStdString()) || url_equal(detail::to_string(it_->field<url_key>()), _url.toString().toStdString());
								});
						    });
					}
					_tree_view->cursor_focus(_target_item);
					index_on_tree = _tree_view->source_model()->index(_target_item);
					assert(index_on_tree.isValid());
				}
				if (index_on_tree.isValid()) {
					if (_target_item != _tree_view->current_item()) _tree_view->select_as_current(TreeIndex::item_require_treeindex([&] { return _tree_view->source_model(); }, _target_item));
					auto _rctrl = _record_screen->record_ctrl();
					if (_rctrl) {
						auto _record_view = _rctrl->view();
#ifdef USE_HAS_SELECTION
						QItemSelectionModel* item_selection_model = _record_view->selectionModel();
						bool has_selection = item_selection_model->hasSelection();
#endif // USE_HAS_SELECTION
						if ((_record_view->current_item() != _target_item) || (currentWebView() != _target_item->page()->view())
#ifdef USE_HAS_SELECTION
						    || !has_selection
#endif // USE_HAS_SELECTION
						    )
							_rctrl->select_as_current(_rctrl->index<pos_proxy>(_target_item));
					}
				}
				// else{
				// auto _tree_it_current = _tree_view->current_item();
				// auto _index_tree_it_on_record_table =
				// _record_controller->source_model()->index(_tree_it_current);
				// if(((QModelIndex)_index_tree_it_on_record_table).isValid()){
				// auto pos_source =
				// _record_controller->index<PosSource>(_index_tree_it_on_record_table);
				// currentChanged(pos_source);
				// }else{
				// auto index_tree_current = _tree_view->current_index();
				// if(index_tree_current.isValid()){
				// _tree_view->select_as_current(TreeIndex::instance([&] {return
				// _tree_view->source_model(); }, _tree_it_current->parent(),
				// _tree_it_current));	//
				// }else{
				// auto tree_previous = _tree_view->previous_index();
				// auto _tree_it_previous =
				// _tree_view->source_model()->item(tree_previous);
				// if(tree_previous.isValid()){
				// _tree_view->select_as_current(TreeIndex::instance([&] {return
				// _tree_view->source_model(); }, _tree_it_previous->parent(),
				// _tree_it_previous));	// _tree_view->index_invoke(tree_previous);
				// }
				// }
				// }
				// }
				auto _main_nwindow = gl_paras->main_window();
				if (!_main_nwindow->windowTitle().contains(
					view_current->page()->title())) {
					auto page = view_current->page();
					auto it = page->host();
					if (page && it) {
						if (page->title() != it->field<name_key>()) page->record_info_update(page->url(), page->title());
						_main_window->setWindowTitle(tr(QString(QString("%1 - ") + program_title).toStdString().c_str()).arg(it ? detail::to_string(it->field<name_key>()).c_str() : ""));
					}
				}
				assert(_blogger);
#ifdef USE_EDITOR_WRAP
				if (_blogger->item() != _target_item)

#endif                                                                                                             // USE_EDITOR_WRAP
					emit view_current->page()->become_current(view_current->page()->binder()); // metaeditor->bind(record);
				//
				if (currentWebView() != view_current) select_as_current(view_current);
			}
		};

		WebView* view_current = this->webView(index);
		if (view_current) { // return;
			// int lc = _lineedits->count();
			// int c = count();
			// assert(lc == c);
			Q_ASSERT(_lineedit_stack->count() == count());

			qDebug() << "_lineedits->count()\t" << _lineedit_stack->count() << "\tcount()\n";

			WebView* view_previous = this->webView(_lineedit_stack->currentIndex());
			if (view_previous && view_current != view_previous) {
#if defined(QWEBENGINEVIEW_STATUSBARMESSAGE)
				disconnect(oldWebView, &PageView::statusBarMessage, this, &TabManager::showStatusBarMessage);
#endif
				disconnect(static_cast<QWebEnginePage*>(view_previous->page()), &QWebEnginePage::linkHovered, this, &TabWidget::linkHovered);
				disconnect(view_previous, &WebView::loadProgress, this, &TabWidget::loadProgress);
				disconnect(view_previous->page()->profile(), &Profile::downloadRequested, this, &TabWidget::downloadRequested);
				disconnect(static_cast<QWebEnginePage*>(view_previous->page()), &QWebEnginePage::fullScreenRequested, this, &TabWidget::fullScreenRequested);

				// auto it_current = view_current->page()->host();

				// if(it_current) {

				// auto _tree_view = _tree_screen->tree_view();
				// QModelIndex _i = _tree_view->source_model()->index(it_current);

				// if(_i.isValid() && _tree_view->current_index() != _i) {
				////
				///_tree_view->selectionModel()->setCurrentIndex(_i,
				///QItemSelectionModel::SelectionFlag::Current);
				////                        _tree_view->selectionModel()->select(_i,
				///current_tree_selection_mode);  //
				// auto parent = it_current->parent();
				////                        boost::intrusive_ptr<TreeIndex> tree_index;
				////                        try {tree_index = new TreeIndex([&] {return
				///_tree_view->source_model(); }, parent, parent->sibling_order([&]
				///(boost::intrusive_ptr<const Linker> il) {
				////                                return il == it_current->linker() &&
				///il->host() == it_current && it_current->parent() == il->host_parent();
				////                            })); } catch(std::exception &e) {throw e;
				///}

				// _tree_view->select_as_current(TreeIndex::instance([&] {return
				// _tree_view->source_model(); }, parent, it_current));

				////                    // globalparameters.mainwindow()
				////
				///if(_record_controller->view()->selection_first_id() !=
				///_record->field("id")) {
				//// _record_controller->select_id(_record->field("id"));
				////                    }
				// }
				// }

				synchronize(view_current);
			}
#if defined(QWEBENGINEVIEW_STATUSBARMESSAGE)
			connect(webView, &WebView::statusBarMessage, this, &TabWidget::showStatusBarMessage);
#endif
			connect(static_cast<QWebEnginePage*>(view_current->page()), &QWebEnginePage::linkHovered, this, &TabWidget::linkHovered);
			connect(view_current, &WebView::loadProgress, this, &TabWidget::loadProgress);
			connect(view_current->page()->profile(), &Profile::downloadRequested, this, &TabWidget::downloadRequested);
			connect(static_cast<QWebEnginePage*>(view_current->page()), &QWebEnginePage::fullScreenRequested, this, &TabWidget::fullScreenRequested);
			for (int i = 0; i < _actions.count(); ++i) {
				WebActionMapper* mapper = _actions[i];
				mapper->updateCurrent(view_current->page());
			}
			emit setCurrentTitle(view_current->title()); // "test"//

			_lineedit_stack->setCurrentIndex(index);

			emit loadProgress(view_current->progress());
			emit showStatusBarMessage(view_current->lastStatusBarText());
			if (view_current->page()->url().isEmpty()) _lineedit_stack->currentWidget()->setFocus();
			// else {}
		}
	}

	QAction* TabWidget::recentlyClosedTabsAction() const
	{
		return _recentlyclosedtabsaction;
	}

	QAction* TabWidget::nextTabAction() const
	{
		return _nexttabaction;
	}

	QAction* TabWidget::previousTabAction() const
	{
		return _previoustabaction;
	}

	// QWidget *
	QStackedWidget* TabWidget::lineEditStack() const
	{
		return _lineedit_stack;
	}

	QLineEdit* TabWidget::currentLineEdit() const
	{
		return lineEdit(_lineedit_stack->currentIndex());
	}

	// void TabWidget::lineEditStack(QStackedWidget *lineedits)
	// {
	// _lineedits = lineedits;
	// }

	QLineEdit* TabWidget::lineEdit(int index) const
	{
		UrlLineEdit* urlLineEdit = qobject_cast<UrlLineEdit*>(_lineedit_stack->widget(index));
		if (urlLineEdit) return urlLineEdit->lineEdit();
		return 0;
	}

	WebView* TabWidget::webView(int index) const
	{
		WebView* webView = nullptr;
		if (-1 != index) {
			QWidget* widget = nullptr;
			widget = this->widget(index);
			if (widget) {
				webView = qobject_cast<WebView*>(widget);

				// if(webView == nullptr) {
				////return webView;
				////} else {
				// if(count() == 0) {

				// BrowserTabManager *that = const_cast<BrowserTabManager *>(this);
				// that->setUpdatesEnabled(false);
				// that->new_dummy();
				////that->newTab();
				////that->closeTab(0);
				// that->setUpdatesEnabled(true);
				// return currentWebView();
				// }

				//// optimization to delay creating the first webview
				// if(count() == 1) {  // current is a empty_dummy, look newTabFull
				// BrowserTabManager *that = const_cast<BrowserTabManager *>(this);
				// that->setUpdatesEnabled(false);
				// that->newTab();
				// that->closeTab(0);
				// that->setUpdatesEnabled(true);
				// return currentWebView();
				// }
				// }
			}
		}
		return webView;
	}

	int TabWidget::webViewIndex(WebView* view) const
	{
		int index = -1;

		try {
			index = indexOf(view);
		} catch (QException& e) {
			qDebug() << e.what();
		}

		return index;
	}

	// void TabManager::new_view(bool make_current)
	// {
	// new_view(register_record(QUrl(DockedWindow::_defaulthome)),
	// globalparameters.getRecordTableScreen()->getRecordTableController(),
	// make_current);
	// }

	// BrowserView *BrowserTabManager::new_dummy()
	// {
	// QWidget *empty_dummy = new QWidget;
	// QPalette p = empty_dummy->palette();
	// p.setColor(QPalette::Window, palette().color(QPalette::Base));
	// empty_dummy->setPalette(p);
	// empty_dummy->setAutoFillBackground(true);
	// disconnect(this, &BrowserTabManager::currentChanged, this,
	// &BrowserTabManager::currentChanged);
	// addTab(empty_dummy, tr("(Untitled)"));
	// connect(this, &BrowserTabManager::currentChanged, this,
	// &BrowserTabManager::currentChanged);
	// return nullptr;
	// }

	WebView* TabWidget::view_no_pinned()
	{
		int found = 0;
		WebView* r = nullptr;
		for (int i = 0; i < count(); i++) {
			WebView* v = webView(i);
			WebPage* p = v->page();
			if (p) {
				auto it = p->host();
				if (it && it->field<pin_key>() == bool_from_check_state[Qt::Unchecked]) {
					found++;
					if (found == 1)
						r = webView(i);
					else
						closeTab(i);
				}
			}
		}
		return r;
	}

	/// *!
	// \fn int QTabWidget::insertTab(int index, QWidget *page, const QIcon& icon,
	// const QString &label)
	// \overload

	// Inserts a tab with the given \a label, \a page, and \a icon into
	// the tab widget at the specified \a index, and returns the index of the
	// inserted tab in the tab bar.

	// This function is the same as insertTab(), but with an additional
	// \a icon.
	// */
	// int TabWidget::insertTab(int index, QWidget *w, const QIcon& icon, const
	// QString &label){
	// Q_D(QTabWidget);
	// QTabWidget *const _this = static_cast<QTabWidget*const>(this);
	// if(! w) return - 1;
	// index = _this->stack//d->stack->insertWidget(index, w);
	// _this->d->tabs->insertTab(index, icon, label);
	// _this->setUpLayout();
	// _this->tabInserted(index);

	// return index;
	// }

	QAction* TabWidget::newTabAction() const
	{
		return _newtabaction;
	}

	int TabWidget::shrink(const size_t bar)
	{
		int result = static_cast<int>(bar);
		auto _rctrl = _record_screen->record_ctrl();
		if (_rctrl) {
			auto _record_view = _rctrl->view();
			if (_record_view) {
				auto current = _record_view->current_item();
				////	    auto	_proxy_model	=
				///_record_controller->proxy_model();
				////	    auto _source_model = _record_controller->source_model();
				////	    _view->setSortingEnabled(true);
				////	    _proxy_model->setSortRole(SORT_ROLE);
				// std::vector<web::WebView *> v_list;
				// for(int index_ = 0; index_ < count(); index_ ++)
				// v_list.push_back(webView(index_));
				// std::sort(v_list.begin(), v_list.end(), [&](web::WebView *v0,
				// web::WebView *v1){
				// auto p0 = v0 != *v_list.end() ? v0->page() : nullptr;
				// auto p1 = v1 != *v_list.end() ? v1->page() : nullptr;
				// return p0 ? p1 ? p0->host()->field<rating_type>().toULongLong() >
				// p1->host()->field<rating_type>().toULongLong() ||
				// TreeIndex::is_ancestor_of(current, p0->host()) : true : false;
				// });
				////	    int t = 0;
				////	    for(auto v : v_list){
				////		_source_model->move(pos_source(webViewIndex(v)),
				///pos_source(t));
				////		t ++;
				////	    }
				////	    _view->reset();
				////	    _proxy_model->setSourceModel(_source_model);
				////	    _view->setModel(_proxy_model);
				////	    _view->setSortingEnabled(false);
				////	    _proxy_model->setSortRole(Qt::InitialSortOrderRole);
				////	    _proxy_model->invalidate();
				////
				///_record_controller->select_as_current(_record_controller->index<pos_proxy>(_source_model->index(current)));
				////		// Сохранение дерева веток
				////		// find_object<TreeScreen>(tree_screen_singleton_name)
				////	    globalparameters.tree_screen()->view()->know_model_save();
				////	    _record_screen->tools_update();
				//
				//
				//
				// for(int i = 0; i < count(); i ++){	// for(std::vector<web::WebView
				// *>::size_type i = 20; i < static_cast<std::vector<web::WebView
				// *>::size_type>(count()); i ++){
				// auto v = webView(i);	// v_list[i];
				// if(v){
				// auto p = v->page();
				// if(p){
				// auto h = p->host();
				// if(  h
				////		      && ! TreeIndex::is_ancestor_of(current, p->host())
				// && (! h->is_ancestor_of(current) || h->distance(current) > 1)
				// && count() > 30
				////			  && h->field<pin_type>() !=
				///_string_from_check_state[Qt::Checked]
				// && v != currentWebView()
				// && v != webView(0)
				// ){
				////		    v_list[i]->page()->deleteLater();
				////		    v_list[i]->page(nullptr);
				////
				///v_list[i]->page()->setUrl(QUrl(web::Browser::_defaulthome));
				////			auto h = p->host();
				// closeTab(webViewIndex(v));
				////			_browser->bind(RecordIndex::instance(record_index->current_model(),
				///h, sibling(v)->page()->host()), false);// horrible
				////			v = nullptr;
				// }
				// }
				// }
				// }
				//
				if (static_cast<size_t>(count()) > bar) {
					// std::vector<boost::intrusive_ptr<TreeItem> >	list;
					std::vector<WebView*> list_v;
					for (int i = 0; i < count();
					     i++) { // for(std::vector<web::WebView *>::size_type i = 20; i <
						// static_cast<std::vector<web::WebView
						// *>::size_type>(count()); i ++){
						auto v = webView(i); // v_list[i];
						if (v) {
							auto p = v->page();
							if (p) {
								auto h = p->host();
								if (h
								    // && (! h->is_ancestor_of(current) || h->distance(current) > 1)
								    // && count() > 30
								    ////				      && h->field<pin_type>() !=
								    ///_string_from_check_state[Qt::Checked]
								    // && v != currentWebView()
								    // && v != webView(0)
								    ) {
									//
									bool found = false;
									// [&] {for(auto &j : list) if(j->id() == h->id()){found =
									// true;break;}return;} ();
									[&] {
										for (auto& j : list_v)
											if (j == v) {
												found = true;
												break;
											}
										return;
									}();
									if (!found) {
										// list.push_back(h);
										list_v.push_back(v);
									}
									// closeTab(webViewIndex(v));
								}
							}
						}
					}
					// std::sort(list.begin(), list.end(), [&](boost::intrusive_ptr<TreeItem>
					// t0, boost::intrusive_ptr<TreeItem> t1){
					////			auto _e = list.end();
					////			auto _re = _e.base();
					////			auto _se = _re->get();
					// return	// &t0 != _re ? &t1 != _re ?
					// t0	// && t0 != *list.end()
					// ? t1	// && t1 != *list.end()
					// ?    (current->distance(t0) != - 1 && current->distance(t0) <
					// current->distance(t1))
					// || (- 1 == current->distance(t1))
					// || (t0->field<rating_type>().toInt() >
					// t1->field<rating_type>().toInt())
					// : true
					// : false	//: true : false
					// ;
					// });

					qSort(list_v.begin(), list_v.end(), [&](WebView* v0, WebView* v1) -> int {
						// auto _e = list.end();
						// auto _re = _e.base();
						// auto _se = _re->get();
						return // &t0 != _re ? &t1 != _re ?
						    v0 // && t0 != *list.end()
						    ?
						    v1 // && t1 != *list.end()
							?
						    (current->distance(v0->page()->host()) != -1 && current->distance(v0->page()->host()) < current->distance(v1->page()->host())) || (-1 == current->distance(v1->page()->host())) || (v0->page()->host()->field<rating_key>() > v1->page()->host()->field<rating_key>()) :
						    true :
						    false // : true : false
						    ;
					});
					// while(list.size() > 30) list.pop_back();
					while (list_v.size() > bar)
						list_v.pop_back();
					// std::set<boost::intrusive_ptr<TreeItem> > list_set;
					std::set<WebView*> list_v_set;
					// for(auto i : list) list_set.insert(i);
					for (auto i : list_v) list_v_set.insert(i);
					for (int i = 0; i < count();
					     i++) { // for(std::vector<web::WebView *>::size_type i = 20; i <
						// static_cast<std::vector<web::WebView
						// *>::size_type>(count()); i ++){
						auto v = webView(i); // v_list[i];
						if (v) {
							auto p = v->page();
							if (p) {
								auto h = p->host();
								if (h && detail::to_qstring(h->field<pin_key>()) != char_from_check_state[Qt::Checked] && v != currentWebView() && v != webView(0)) {
									//
									// if(list_set.find(h) == list_set.end())
									// closeTab(webViewIndex(v));
									if (list_v_set.find(v) == list_v_set.end()) closeTab(webViewIndex(v));
								}
							}
						}
					}
					result = count();
				}
			}
		}
		return result;
	}

	WebView* TabWidget::newTab(boost::intrusive_ptr<RecordIndex> record_index, bool make_current)
	{
		WebView* view = nullptr;
		// boost::intrusive_ptr<TreeItem>
		// tab_brother,
		// boost::intrusive_ptr<TreeItem> target
		auto _rctrl = _record_screen->record_ctrl();
		if (_rctrl) {
			if (count() > 30) {
				shrink(20);
			}
			//
			// , bool openinnewtab   // , const TreeScreen::paste_strategy
			// &_view_paste_strategy // , equal_t _equal

			// auto _record_model = modelindex.current_model();
			// boost::intrusive_ptr<TreeItem> tab_brother =
			// record_index->target_sibling();
			boost::intrusive_ptr<i_t> result_item = record_index->host();
			// boost::intrusive_ptr<TreeItem>	result	= result;
			assert(result_item);
			// assert(tab_brother != target);
			// assert(!target->is_lite());
			// if(target->is_lite())target->to_fat();
			// if(record == nullptr) {
			// record = register_record(QUrl(DockedWindow::_defaulthome));
			// } else {
			// record = register_record(QUrl(record->getNaturalFieldSource("url")));
			// }

			view = find([&](boost::intrusive_ptr<const ::Binder> b) {
				return b->host()->id() == result_item->id();
			}); // view_no_pinned();   // nullptr;   // find(record);
			if (view == nullptr) {
				view = find([&](boost::intrusive_ptr<const ::Binder> b) {
					return url_equal(b->host()->field<url_key>(), result_item->field<url_key>());
				});
				if (view == nullptr) {
					// if(view == nullptr) {

					// {
					////		    line edit, move after new WebView ! ! !
					// UrlLineEdit *urlLineEdit = new UrlLineEdit;
					// QLineEdit	*lineEdit = urlLineEdit->lineEdit();
					// if(! _lineeditcompleter && count() > 0){
					// HistoryCompletionModel *completionModel = new
					// HistoryCompletionModel(this);
					// completionModel->setSourceModel(QtSingleApplication::historyManager()->historyFilterModel());
					// _lineeditcompleter = new QCompleter(completionModel, this);
					////			Should this be in Qt by default?
					// QAbstractItemView	*popup		= _lineeditcompleter->popup();
					// QListView		*listView	= qobject_cast<QListView
					// *>(popup);
					// if(listView)listView->setUniformItemSizes(true);
					// }
					// lineEdit->setCompleter(_lineeditcompleter);
					// connect(lineEdit, &QLineEdit::returnPressed, this,
					// &TabWidget::lineEditReturnPressed);
					// _lineedits->addWidget(urlLineEdit);
					// _lineedits->setSizePolicy(lineEdit->sizePolicy());
					// }

					// optimization to delay creating the more expensive WebView, history, etc
					// if(count() == 0) {return new_dummy();}

					//// webview
					// if(!record->page_valid()
					////           && !record->unique_page()
					// ) {

					view = new WebView(result_item, _profile, // use record for return
					    _tree_screen, _blogger, _browser_docker, _browser, this, _record_screen->record_ctrl());
					// } else {
					// view = record->unique_page()->view();
					// }

					// record->view(webView);  // inside PageView initialization
					// webView->setPage(new WebPage(_profile, webView));

					// assert(item->page_valid() && item->unique_page());
					assert(view);
					assert(view->page()->binder()->integrity_internal());
					assert(view->page()->binder()->integrity_external(result_item, view->page()));

					//					{
					auto find_screen = gl_paras->find_screen();
					assert(find_screen);
					// line edit
					auto toolbar_search = view->toolbarsearch(); // new ToolbarSearch(find_screen, view); //UrlLineEdit(view);

					QLineEdit* lineEdit = toolbar_search->lineEdit();
					if (!_lineeditcompleter && count() > 0) {
						HistoryCompletionModel* completionModel = new HistoryCompletionModel(this);
						completionModel->setSourceModel(::sapp_t::historyManager()->historyFilterModel());
						_lineeditcompleter = new QCompleter(completionModel, this);
						// Should this be in Qt by default?
						QAbstractItemView* popup = _lineeditcompleter->popup();
						QListView* listView = qobject_cast<QListView*>(popup);
						if (listView) listView->setUniformItemSizes(true);
					}
					lineEdit->setCompleter(_lineeditcompleter);
					//					connect(lineEdit, &QLineEdit::textChanged, [view, urlLineEdit](const QString& text) {

					//						auto real_url_ = to_be_url(text);
					//						if (real_url_ != QUrl() //&& real_url_ != detail::to_qstring(web::Browser::_defaulthome)
					//						    ) {
					//							//								auto iconlabel = urlLineEdit->iconlabel();
					//							//								assert(iconlabel);
					//							//								urlLineEdit->left_widget(iconlabel);
					//							//								auto left_widget = urlLineEdit->left_widget();
					//							//								assert(left_widget);
					//							//								left_widget->show();
					//							//								urlLineEdit->searchbutton()->hide();


					//							//							urlLineEdit->searchbutton()->setIcon(view->icon());


					//						} else {
					//							//							auto searchbutton = urlLineEdit->searchbutton();
					//							//							assert(searchbutton);
					//							//							urlLineEdit->left_widget(searchbutton);
					//							//							auto left_widget = urlLineEdit->left_widget();
					//							//							assert(left_widget);
					//							//							left_widget->show();
					//							//							urlLineEdit->iconlabel()->hide();
					//						}
					//					});
					connect(toolbar_search, &ToolbarSearch::return_pressed, this, &TabWidget::lineEditReturnPressed); //lineEdit, &QLineEdit::returnPressed,

					_lineedit_stack->addWidget(toolbar_search);
					_lineedit_stack->setSizePolicy(lineEdit->sizePolicy());
					//					}

					// urlLineEdit->setWebView(view);    // move inside
					// UrlLineEdit::UrlLineEdit

					connect(view, &WebView::loadStarted, this, &TabWidget::webViewLoadStarted);
					connect(view, &WebView::iconChanged, this, &TabWidget::webViewIconChanged);
					connect(view, &WebView::titleChanged, this, &TabWidget::webViewTitleChanged);
					connect(view, &WebView::urlChanged, this, &TabWidget::webViewUrlChanged);
					connect(static_cast<QWebEnginePage*>(view->page()), &QWebEnginePage::windowCloseRequested, this, &TabWidget::windowCloseRequested);
					connect(static_cast<QWebEnginePage*>(view->page()), &QWebEnginePage::geometryChangeRequested, this, &TabWidget::geometryChangeRequested);

#if defined(QWEBENGINEPAGE_PRINTREQUESTED)
					connect(view->page(), &WebPage::printRequested, this, &TabWidget::printRequested);
#endif

#if defined(QWEBENGINEPAGE_MENUBARVISIBILITYCHANGEREQUESTED)
					connect(view->page(), &WebPage::menuBarVisibilityChangeRequested, this, &TabWidget::menuBarVisibilityChangeRequested);
#endif

#if defined(QWEBENGINEPAGE_STATUSBARVISIBILITYCHANGEREQUESTED)
					connect(view->page(), &WebPage::statusBarVisibilityChangeRequested, this, &TabWidget::statusBarVisibilityChangeRequested);
#endif

#if defined(QWEBENGINEPAGE_TOOLBARVISIBILITYCHANGEREQUESTED)
					connect(view->page(), &WebPage::toolBarVisibilityChangeRequested, this, &TabWidget::toolBarVisibilityChangeRequested);
#endif

					// int index = addTab(view, target->field("name").leftJustified(8, '.',
					// true)); //, tr("(Untitled)")

					//
					QString short_title = detail::to_qstring(result_item->field<name_key>()).leftJustified(8, '.', true);
					_previous_index = static_cast<QModelIndex>(record_index->sibling_index()).row(); // tab_brother ? tab_brother->binder() ?
															 // webViewIndex(tab_brother->binder()->page()->view()) + 1
															 // : 0 : 0;
					QIcon icon;
					//
					int index = insertTab(_previous_index + 1, view, icon, short_title); // index =
					// _tabbar->insertPage(pre_index,
					// view, icon, title);  //
					// _tabbar->setTabButton(index, QTabBar::RightSide, new
					// FlatToolButton(_tabbar));
					record_index->sibling_index(record_index->current_model()()->createIndex(index, 0, static_cast<void*>(result_item.get())));
					_rctrl->addnew_item(record_index, make_current);
					//

					setTabToolTip(index, result_item->field<name_key>());
					// record->page()->load(record);
					// globalparameters.entrance()->invoke_view(record);
					// assert(item->page_valid() && item->unique_page());
					// int lc = _lineedits->count();
					// int c = count();
					// assert(lc == c);
					// Q_ASSERT(_lineedits->count() == count());

					// webview actions
					for (int i = 0; i < _actions.count(); ++i) {
						WebActionMapper* mapper = _actions[i];
						mapper->addChild(view->page()->action(mapper->webAction()));
					}
					result_item = view->page()->host();
					// else {
					assert(
					    result_item->binder()->integrity_external(result_item, view->page()));
				} else { // url is equal, id is not equal
					 // auto _record_binder = view->page()->binder();

					// if(_record_binder && _record_binder->item() != target) {
					// view->page()->binder_reset();
					////                    _record_binder->item() = target;
					////                    _record_binder->page() = view->page();
					////                    result = target;

					// result = view->page()->item_bind(target);
					// assert(result->binder()->integrity_external(result, view->page()));
					// }

					// assert(result->binder()->integrity_external(result, view->page()));

					auto item_with_same_url = view->page()->host();
					assert(static_cast<QModelIndex>(_tree_screen->view()->source_model()->index(result_item)).isValid());
					if (item_with_same_url != result_item)
						result_item = TreeLevel::instance(
								  TreeIndex::item_require_treeindex(
								      [&]() { return _tree_screen->view()->source_model(); }, result_item),
								  item_with_same_url)
								  ->merge();
					// TreeIndex::instance([&](){return
					// _tree_screen->view()->source_model();}, page_item,
					// page_item->parent()), target);
					//
					// if(!target->binder())target->binder(std::forward<boost::intrusive_ptr<TreeItem::coupler>>(view->page()->binder()));

					// result = page_item;	// target;
					if (!result_item->binder() || !result_item->binder()->page()) view->page()->bind(result_item);
					assert(
					    result_item->binder()->integrity_external(result_item, view->page()));
				}
				assert(result_item->binder());
				assert(
				    result_item->binder()->integrity_external(result_item, view->page()));
			} else { // id is equal
				auto item_with_same_id = view->page()->host();
				if (item_with_same_id != result_item)
					result_item = TreeLevel::instance(
							  TreeIndex::item_require_treeindex(
							      [&]() { return _tree_screen->view()->source_model(); }, item_with_same_id),
							  result_item)
							  ->merge(); // TreeIndex::instance([&](){return
								     // _tree_screen->view()->source_model();}, page_item,
								     // page_item->parent()), target);
				// if(!target->binder())target->binder(std::forward<boost::intrusive_ptr<TreeItem::coupler>>(view->page()->binder()));

				// result = page_item;	// target;
				if (!result_item->binder() || !result_item->binder()->page()) view->page()->bind(result_item);
				assert(
				    result_item->binder()->integrity_external(result_item, view->page()));
			}
			assert(result_item == view->page()->binder()->host()); // old one choosed
			assert(result_item == result_item || result_item->field<url_key>() == result_item->field<url_key>());
			// assert(result->record_binder()->bounded_page());
			if (!result_item->binder()->page()) result_item->binder()->page(
			    view->page());                                       // result->bind();   // this is recursively call
			assert(result_item->binder()->host());                   // 0
			assert(result_item->binder()->page());                   // 1
			assert(view->page()->binder());                          // 8
			assert(result_item->binder() == view->page()->binder()); // 10
			assert(result_item->binder()->integrity_external(result_item, view->page()));
			// result->record_binder()->binder(result);
			// }
			// result->record_binder()->binder();   //recursive calling

			if (make_current || (1 == count())) {

				setCurrentWidget(view);
				// auto _tree_view = _tree_screen->view();
				// QModelIndex _i =
				// _tree_view->source_model()->index(result->binder()->host());
				// if(_i != _tree_view->current_index() &&
				// result->binder()->integrity_external(result,
				// view->page()))_tree_view->selectionModel()->select(_i,
				// QItemSelectionModel::SelectionFlag::Select);	//
				// _tree_view->selectionModel()->select(_i,
				// QItemSelectionModel::SelectionFlag::Deselect);
				// if(_record_controller->view()->current_item() != result)
				// _record_controller->cursor_to_index(_record_controller->index<PosProxy>(result));
				// _tree_view->select_and_current(_record_binder->bounded_item());

				// if(!_record_controller->source_model()->find(item))
				RecordIndex::synchronize(result_item); // _record_controller->addnew_item_fat(result);

				// item->activate(); // activate after initialization of browser
				//				if (count() == 1)
				if (currentWebView() != view)
					emit static_cast<QTabWidget*>(this)->currentChanged(webViewIndex(view)); //currentIndex() // default this is input the new index
			}

			emit tabsChanged();
			// webView->openLinkInNewTab(QUrl());    // don't need, initialized
			// }

			assert(view);
			// assert(item->page_valid() && item->unique_page());
			assert(!result_item->is_lite());
			if (result_item->field<name_key>() == name_value("")) {
				// result->field<name_type>(result->field<url_type>());
				result_item->binder()->page()->record_info_update(QUrl(detail::to_qstring(result_item->field<url_key>())), detail::to_qstring(result_item->field<url_key>()));
			}
			// view_no_pinned();
		}
		return view; // TabWidget::newTabFull::WebView *
	}

	void TabWidget::reloadAllTabs()
	{
		for (int i = 0; i < count(); ++i) {
			QWidget* tabWidget = widget(i);
			if (WebView* tab = qobject_cast<WebView*>(tabWidget)) tab->reload();
		}
	}

	void TabWidget::lineEditReturnPressed()
	{
		if (ToolbarSearch* search_edit = qobject_cast<ToolbarSearch*>(sender())) { //if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender())) {
			//			auto parent = lineEdit->parent();
			//			if (parent) {
			//				auto lineEdit = dynamic_cast<UrlLineEdit*>(parent);
			//				if (lineEdit) {
			auto view = search_edit->webview();
			auto page = view->page();
			auto it = page->host();
			auto old_url = page->url();
			auto text = search_edit->lineEdit()->text();
			auto real_url_ = to_be_url(text);
			if (real_url_ != QUrl() && real_url_ != detail::to_qstring(web::Browser::_defaulthome)) {
				if (old_url == QUrl(detail::to_qstring(web::Browser::_defaulthome))) { // for Ctrl+T and QKeySequence::AddTab
					it->field<url_key>(detail::from_qstring<url_key>(real_url_.toString()));
					page->load(it, true, true); // force reload
				} else {
					emit loadPageNewTab(text);
				}
				if (_lineedit_stack->currentWidget() == search_edit) {
					auto v = currentWebView();
					if (v) v->setFocus();
				}
			}
			//				}
			//			}
		}
	}

	void TabWidget::windowCloseRequested()
	{
		WebPage* webPage = qobject_cast<WebPage*>(sender());
		WebView* webView = qobject_cast<WebView*>(webPage->view());
		int index = webViewIndex(webView);
		if (index >= 0) {
			if (count() == 1) {
				auto br = webView->page()->browser();
				if (br) {
					//					br->self_close_request();
					br->close();
				}
			} else
				closeTab(index);
		}
	}

	void TabWidget::closeOtherTabs(int index)
	{
		if (-1 == index) return;
		for (int i = count() - 1; i > index; --i) closeTab(i);
		for (int i = index - 1; i >= 0; --i) closeTab(i);
	}

	//// When index is -1 index chooses the current tab
	// void TabWidget::cloneTab(int index)
	// {
	// if(index < 0)
	// index = currentIndex();

	// if(index < 0 || index >= count())
	// return;

	// assert(webView(index)->page()->host()->page_valid() &&
	// webView(index)->page()->host()->page());
	// assert(webView(index)->page()->host()->is_registered_to_browser() ||
	// webView(index)->page()->host()->field("url") == web::Browser::_defaulthome);

	// newTab(webView(index)->page()->host(), webView(index)->page()->host(), true);

	// }

	// When index is -1 index chooses the current tab
	void TabWidget::requestCloseTab(int index)
	{
		// if(index < 0)index = currentIndex();  // why?
		if (index < 0 || index >= count()) return;
		WebView* view_ = webView(index);
		if (!view_) return;
		view_->page()->triggerAction(QWebEnginePage::RequestClose);
	}

	QAction* TabWidget::closeTabAction() const
	{
		return _closetabaction;
	}

	// When index is -1 index chooses the current tab
	void TabWidget::closeTab(int index_to_close)
	{ // , bool sychronize_to_record_view
		// int current_index = currentIndex();
		// if(index != current_index)index = current_index;
		// if(index < 0)index = currentIndex();
		auto _rctrl = _record_screen->record_ctrl();
		if (_rctrl) {
			if (index_to_close >= 0 && index_to_close < count()) { // if(index < 0 || index >= count())
				if (count() > 0) {
					// return;
					// bool hasFocus = false;
					if (WebView* _view_to_close = webView(index_to_close)) {
						assert(widget(index_to_close) == _view_to_close); // debug
#if defined(QWEBENGINEPAGE_ISMODIFIED)
						if (tab->isModified()) {
							QMessageBox closeConfirmation(view_);
							closeConfirmation.setWindowFlags(Qt::Sheet);
							closeConfirmation.setWindowTitle(
							    tr("Do you really want to close this page?"));
							closeConfirmation.setInformativeText(
							    tr("You have modified this page and when closing it you would lose "
							       "the modification.\n"
							       "Do you really want to close this page?\n"));
							closeConfirmation.setIcon(QMessageBox::Question);
							closeConfirmation.addButton(QMessageBox::Yes);
							closeConfirmation.addButton(QMessageBox::No);
							closeConfirmation.setEscapeButton(QMessageBox::No);
							if (closeConfirmation.exec() == QMessageBox::No) return;
						}
#endif
						// hasFocus = _view_to_close->hasFocus();
						if (_profile == gl_paras->profile()) { // QWebEngineProfile::defaultProfile()
							_recentlyclosedtabsaction->setEnabled(true);
							_recentlyclosedtabs.prepend(_view_to_close->page()->url());
							if (_recentlyclosedtabs.size() >= TabWidget::_recentlyclosedtabssize) _recentlyclosedtabs.removeLast();
						}
						QWidget* lineEdit = _lineedit_stack->widget(index_to_close);
						if (lineEdit) {
							_lineedit_stack->removeWidget(lineEdit);
							lineEdit->deleteLater(); // delete lineEdit;
						}
						auto it = _view_to_close->page()->host();
						assert(it->page() == _view_to_close->page());

						// auto _tree_screen = _tree_screen;
						_tree_screen->view()->selectionModel()->select(
						    _tree_screen->view()->source_model()->index(it), QItemSelectionModel::SelectionFlag::Deselect);
						_rctrl->view()->selectionModel()->select(_rctrl->source_model()->index(it), QItemSelectionModel::SelectionFlag::Deselect);
						// _tree_screen->tree_view()->select_and_current(it, [](KnowView * v,
						// const QModelIndex & _i)->QModelIndex {
						// v->selectionModel()->select(_i,
						// QItemSelectionModel::SelectionFlag::Deselect);
						// assert(_i != v->selectionModel()->selectedIndexes().last());
						// return v->selectionModel()->selectedIndexes().last();
						// });

						// if(!_record_controller->source_model()->is_item_exists(to_be_closed_view->page()->current_item()->id()))
						// {
						// assert(_record_controller->source_model()->size() == _tabbar->count());
						// } else {
						// _record_controller->source_model()->remove_child(to_be_closed_view->page()->current_item());
						// to_be_closed_view->page()->break_records(); // same as
						// _record_controller->source_model()->remove_child()
						// }

						removeTab(index_to_close);
						// if(sychronize_to_record_view)
						_view_to_close->on_close_requested();
						_view_to_close->deleteLater();
						_rctrl->remove(it->id());
						//				{// lead double deconstruct
						//					emit
						//_view_to_close->close_requested(); //
						//_record_controller->remove_child(to_be_closed_view->page()->current_item()->id());
						//				}
						// delete to_be_closed_view;
						{       //?
							//				_view_to_close->deleteLater();
						}
						// move to WebView::on_close_requested
						// int tab_widget_count = count();
						// int tab_bar_count = _tabbar->count();
						// int source_model_size = _record_controller->source_model()->size();

						// if(source_model_size > tab_widget_count) {
						// bool found = false;

						// for(int i = 0; i < source_model_size; i++) {
						// if(_record_controller->source_model()->child(i)->unique_page() ==
						// to_be_closed_view->page()) {
						// _record_controller->remove_child(_record_controller->source_model()->child(i)->id());
						// found = true;
						// break;
						// }
						// }

						// assert(found == true);
						// }
						//				WebView *view_privious = nullptr;
						//				view_privious =
						//find([&](boost::intrusive_ptr<const Binder> b){return b->host() ==
						//it->linker()->host_parent();});
						//				if(!view_privious) view_privious =
						//count() > 0 ? webView(-1 == _previous_index ? 0 : _previous_index) :
						//nullptr;
						//				if(view_privious)
						//select_as_current(view_privious);
					}
					// move forward before removeTab(index);
					// QWidget *lineEdit = _lineedits->widget(index);

					// if(lineEdit) {
					// _lineedits->removeWidget(lineEdit);
					////delete lineEdit;    //
					// lineEdit->deleteLater();
					// }

					emit tabsChanged();
					// if(count() > 0){	// hasFocus &&
					// auto behavior = _tabbar->selectionBehaviorOnRemove();
					////		enum SelectionBehavior {
					////		    SelectLeftTab,
					////		    SelectRightTab,
					////		    SelectPreviousTab
					////		};
					// WebView *current_view = nullptr;
					// current_view = currentWebView();
					// assert(current_view != webView(index));
					// if(current_view != nullptr){
					// current_view->setFocus();
					// boost::intrusive_ptr<TreeItem> current_item =
					// current_view->page()->host();
					// if(current_item){
					// assert((current_item->page_valid() && current_item->page()) ||
					// (current_item->field<url_type>() == web::Browser::_defaulthome));
					////                        // QModelIndex proxyindex =
					///view->recordtablecontroller()->convertIdToProxyIndex(record->getField("id"));
					////                        // int position =
					///view->recordtablecontroller()->convertProxyIndexToPos(proxyindex);
					////                        // RecordTableView *recordtableview =
					///view->recordtablecontroller()->getView();
					////                        //
					///if(recordtableview)recordtableview->setSelectionToPos(position); // work
					////                        current_view->setFocus();
					////                        // globalparameters.mainwindow()
					// if(_record_controller->view()->current_item() !=
					// current_item)_record_controller->cursor_to_index(_record_controller->index<PosProxy>(current_item));
					// // IdType(item->field("id"))
					////                                selection_first<IdType>() !=
					///current_item->field<id_type>()
					// }
					// }
					// }
					// int tab_widget_count = count();
					// int tab_bar_count = _tabbar->count();
					// int source_model_size = _record_controller->source_model()->size();

					assert(count() == _tabbar->count());
					// auto source_model_ = _record_controller->source_model();
					// assert(_record_controller->source_model()->size() == _tabbar->count());
					// if(sychronize_to_record_view)
					_rctrl->on_recordtable_configchange();
					// if(count() == 0)
					if (0 == _tabbar->count()) emit lastTabClosed();
				} else {
					if (_browser) _browser->destroy_trigger_from_others()(_blogger);
				}
			}
		}
	}

	// deprecated, profile should be assigned at the construction of BrowserPage
	void TabWidget::setProfile(Profile* profile)
	{ // QWebEngineProfile *profile
		_profile = profile;

		// for(int i = 0; i < count(); ++i) {
		// QWidget *tabWidget = widget(i);

		// if(BrowserView *tab = qobject_cast<BrowserView *>(tabWidget)) {
		// BrowserPage *webPage = tab->webPage();  // ->setProfile(profile); //
		// BrowserPage *webPage = new BrowserPage(_profile, tab);
		// webPage->load(tab->page()->url());      // this semantic is reload
		// tab->setPage(webPage);
		// }
		// }
	}

	void TabWidget::webViewLoadStarted()
	{
		WebView* webView = qobject_cast<WebView*>(sender());
		int index = webViewIndex(webView);
		if (-1 != index) {
			QIcon icon(QLatin1String(":loading.gif"));
			setTabIcon(index, icon);
		}
	}

	void TabWidget::webViewIconChanged()
	{
		WebView* webView = qobject_cast<WebView*>(sender());
		int index = webViewIndex(webView);
		if (-1 != index) {
			QIcon icon = webView->icon();
			setTabIcon(index, icon);
		}
	}

	void TabWidget::webViewTitleChanged(const QString& title)
	{
		WebView* webView = qobject_cast<WebView*>(sender());
		int index = webViewIndex(webView);
		auto _real_title = title.leftJustified(8, '.', true);
		if (-1 != index) setTabText(index, _real_title);
		if (currentIndex() == index) emit setCurrentTitle(title); // _real_title// "test"
		//
		// if(_real_title != "")
		if (title != webView->page()->title()) webView->page()->record_info_update(webView->page()->url(), title);
		::sapp_t::historyManager()->updateHistoryItem(webView->page()->url(), title);
	}

	void TabWidget::webViewUrlChanged(const QUrl& url)
	{
		WebView* webView = qobject_cast<WebView*>(sender());
		int index = webViewIndex(webView);
		if (-1 != index) _tabbar->setTabData(index, url);
		emit tabsChanged();
	}

	void TabWidget::aboutToShowRecentTabsMenu()
	{
		_recentlyclosedtabsmenu->clear();
		for (int i = 0; i < _recentlyclosedtabs.count(); ++i) {
			QAction* action = new QAction(_recentlyclosedtabsmenu);
			action->setData(_recentlyclosedtabs.at(i));
			QIcon icon = ::sapp_t::instance()->icon(_recentlyclosedtabs.at(i));
			action->setIcon(icon);
			action->setText(_recentlyclosedtabs.at(i).toString());
			_recentlyclosedtabsmenu->addAction(action);
		}
	}

	void TabWidget::aboutToShowRecentTriggeredAction(QAction* action)
	{
		QUrl url = action->data().toUrl();

		loadUrlInCurrentTab(url);
	}

	void TabWidget::mouseDoubleClickEvent(QMouseEvent* event)
	{
		if (!childAt(event->pos())
		    // Remove the line below when QTabWidget does not have a one pixel frame
		    && event->pos().y() < (tabBar()->y() + tabBar()->height())) {
			auto tree_view = _tree_screen->view();

			auto current_item = tree_view->current_item();
			auto parent = current_item->parent();
			if (!parent)
				throw std::runtime_error(
				    formatter()
				    << std::string(
					   typeid(decltype(&TabWidget::mouseDoubleClickEvent)).name())
				    << " ! parent");
			// boost::intrusive_ptr<TreeIndex> modelindex(nullptr);

			// try {
			// modelindex = new TreeIndex([&] {return tree_view->source_model(); },
			// parent, parent->sibling_order([&] (boost::intrusive_ptr<const Linker> il)
			// {
			// return il->host() == current_item && il == current_item->linker() &&
			// parent == il->host_parent();
			// }));
			// } catch(std::exception &e) {}

			// if(modelindex) {
			real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(web::Browser::_defaulthome,
			    [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
				    return TreeIndex::url_activate(real_target_url_,
					[&] { return tree_view->source_model(); },
					current_item,
					std::bind(&tv_t::move, tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
					[](boost::intrusive_ptr<const i_t> it_) -> bool { return url_equal(url_value(detail::to_qstring(it_->field<home_key>())), web::Browser::_defaulthome) || url_equal(it_->field<url_key>(), web::Browser::_defaulthome); },
					_browser);
				    // }
			    });
			return;
		}
		QTabWidget::mouseDoubleClickEvent(event);
	}

	void TabWidget::contextMenuEvent(QContextMenuEvent* event)
	{
		if (!childAt(event->pos())) {
			_tabbar->contextMenuRequested(event->pos());

			return;
		}
		QTabWidget::contextMenuEvent(event);
	}

	void TabWidget::resizeEvent(QResizeEvent* e)
	{
		for (int i = 0; i < count(); i++) {
			WebView* view = webView(i);
			if (view) view->resizeEvent(e);
		}
		QTabWidget::resizeEvent(e);
	}

	void TabWidget::mouseReleaseEvent(QMouseEvent* event)
	{
		if (event->button() == Qt::MiddleButton && !childAt(event->pos())
		    // Remove the line below when QTabWidget does not have a one pixel frame
		    && event->pos().y() < (tabBar()->y() + tabBar()->height())) {
			QUrl url(QApplication::clipboard()->text(QClipboard::Selection));
			url_value real_url = url_value(url.toString());
			auto tree_view = _tree_screen->view();

			auto current_item = tree_view->current_item();
			auto parent = current_item->parent();
			if (!parent)
				throw std::runtime_error(
				    formatter() << typeid(decltype(&TabWidget::mouseReleaseEvent)).name()
						<< "! parent");
			if (!url.isEmpty() && url.isValid() && !url.scheme().isEmpty()) {
				real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(real_url,
				    [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
					    return TreeIndex::url_activate(real_target_url_,
						[&] { return tree_view->source_model(); },
						current_item,
						std::bind(&tv_t::move, tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
						[&](boost::intrusive_ptr<const i_t> it_) -> bool { return url_equal(url_value(detail::to_qstring(it_->field<home_key>())), real_url) || url_equal(it_->field<url_key>(), real_url); },
						_browser);
				    });
			}
		}
	}

	void TabWidget::loadUrlInCurrentTab(const QUrl& _url)
	{
		url_value real_url = url_value(_url.toString());
		WebView* webView = currentWebView();
		if (webView) {
			// Record *record;
			boost::intrusive_ptr<i_t> _item = webView->page()->host();
			if (_item->page()->url() != _url) { // _item->page_valid() &&
				auto tree_view = _tree_screen->view();
				auto current_item = tree_view->current_item();
				auto parent = current_item->parent();
				if (!parent)
					throw std::runtime_error(
					    formatter()
					    << typeid(decltype(&TabWidget::loadUrlInCurrentTab)).name()
					    << "! parent");
				auto it = real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(real_url,
				    [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
					    return TreeIndex::url_require_item_from_tree(
						real_target_url_, std::bind(&tv_t::move, tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), [&](boost::intrusive_ptr<const i_t> it_) -> bool {
							return url_equal(url_value(detail::to_qstring(it_->field<home_key>())), real_url) || url_equal(it_->field<url_key>(), real_url);
						}); // instance([&] {return tree_view->source_model();},
						    // current_item)->
				    });
				auto r = webView->page()->bind(it); // _record_controller
				r->activate(std::bind(&TabWidget::find, this, std::placeholders::_1));
			}
		}
	}

	void TabWidget::nextTab()
	{
		int next = currentIndex() + 1;
		if (next == count()) next = 0;
		setCurrentIndex(next);
		auto _rctrl = _record_screen->record_ctrl();
		if (_rctrl) _rctrl->select_as_current(_rctrl->index<pos_proxy>(pos_source(next)));
	}

	WebView* TabWidget::sibling(WebView* v) const
	{
		int index = webViewIndex(v) - 1;
		if (index < 0) index = count() - 1;
		return webView(index);
	}

	void TabWidget::previousTab()
	{
		int next = currentIndex() - 1;
		if (next < 0) next = count() - 1;
		setCurrentIndex(next);
		auto _rctrl = _record_screen->record_ctrl();
		if (_rctrl) _rctrl->select_as_current(_rctrl->index<pos_proxy>(pos_source(next)));
	}


	void TabWidget::topic_replace_traverse(const QString& new_topic_, const QString& original_topic_)
	{
		for (int idx = 0; idx < this->count(); idx++) {
			auto v = this->webView(idx);
			if (v) {
				auto p = v->page();
				if (p) {
					auto h = p->host();
					if (h) {
						h->topic_replace(new_topic_, original_topic_);
					}
				}
			}
		}
	}


	QByteArray TabWidget::saveState() const
	{
		int version = 1;
		QByteArray data;
		QDataStream stream(&data, QIODevice::WriteOnly);

		stream << qint32(TabWidget::TabWidgetMagic);
		stream << qint32(version);

		QStringList tabs_url;
		//		//		auto _rctrl = nullptr;
		//		auto _rctrl = _record_screen->record_ctrl();
		//		if (_rctrl) {
		//			auto _count = _rctrl->row_count(); // count();
		//			for (int i = 0; i < _count; ++i) {
		//				// if(WebView *view = qobject_cast<WebView *>(widget(i))){
		//				// auto url = view->page()->url().toString();
		//				// if(  url != "" && url != web::Browser::_defaulthome
		//				// && (  view->page()->host()->field<pin_type>() !=
		//				// _string_from_check_state[Qt::Unchecked]
		//				// || view == currentWebView())
		//				// ) tabs_url.append(url);
		//				// }
		//				//// else tabs_url.append(QString::null);
		//				auto it = _rctrl->index<boost::intrusive_ptr<i_t>>(pos_source(i));
		//				if (it) {
		//					auto url = it->field<url_key>();
		//					if (url != url_value("") && url != web::Browser::_defaulthome && (it->field<pin_key>() != pin_value(bool_from_check_state[Qt::Unchecked]) || (it->page() && it->page()->view() == currentWebView())))
		//						tabs_url.append(detail::to_qstring(url));
		//				}
		//			}
		//		}
		for (int i = 0; i < count(); i++) {
			auto it = webView(i)->page()->host();
			if (it) {
				auto url = it->field<url_key>();
				if ((url != url_value("")                    //
					&& url != web::Browser::_defaulthome //
									     //					&& it->field<pin_key>() != pin_value(bool_from_check_state[Qt::Unchecked]) //
					) ||
				    (it->page() && it->page()->view() == currentWebView()) //
				    ) tabs_url.append(detail::to_qstring(url));
			}
		}
		stream << tabs_url;
		auto v = currentWebView();
		if (v) {
			auto p = v->page();
			if (p) {
				auto h = p->host();
				if (h) stream << h->field<id_key>(); // currentIndex();
			}
		}
		return data;
	}

	std::tuple<qint32, qint32, QStringList, QString> TabWidget::state(const QByteArray& state_)
	{
		//
		std::tuple<qint32, qint32, QStringList, QString> result;
		int version = 1;
		QByteArray sd = state_;
		QDataStream stream(&sd, QIODevice::ReadOnly);
		//	    if(stream.atEnd()) return false;
		if (!stream.atEnd()) {
			qint32 marker;
			qint32 v;
			stream >> marker;
			std::get<0>(result) = marker;
			stream >> v;
			std::get<1>(result) = v;
			//			if(marker != TabWidgetMagic || v != version) return
			//false;
			if (marker == TabWidget::TabWidgetMagic && v == version) {
				QStringList open_tabs;
				stream >> open_tabs;
				std::get<2>(result) = open_tabs;
				QString current_tab_id;
				stream >> current_tab_id;
				std::get<3>(result) = current_tab_id;
			}
		}
		return result;
	}

	bool TabWidget::restoreState(const QByteArray& state)
	{
		int version = 1;
		QByteArray sd = state;
		QDataStream stream(&sd, QIODevice::ReadOnly);
		if (stream.atEnd()) return false;
		qint32 marker;
		qint32 v;
		stream >> marker;
		stream >> v;
		if (marker != TabWidget::TabWidgetMagic || v != version) return false;
		QStringList open_tabs;
		stream >> open_tabs;
		QString current_tab_id;
		stream >> current_tab_id;
		// setCurrentIndex(current_tab_id);
		if (open_tabs.count() == 0) {
			if (current_tab_id != "") {
				auto tree_screen = gl_paras->tree_screen();
				if (tree_screen) {
					auto tree_view = tree_screen->view();
					if (tree_view) {
						auto model = tree_view->source_model();
						if (model) {
							auto it = model->item([&](boost::intrusive_ptr<const i_t> ti) { return ti->id() == current_tab_id; });
							if (it) open_tabs << detail::to_qstring(it->field<url_key>());
						}
					}
				}
			} else
				open_tabs << detail::to_qstring(Browser::_defaulthome); //_find_screen need it
		}
		for (int i = 0; i < open_tabs.count(); ++i) {
			auto _url = open_tabs.at(i);
			url_value real_url = url_value(_url);
			// Record *_record = request_record(_url);

			auto tree_view = _tree_screen->view();

			boost::intrusive_ptr<TreeIndex> tree_index = TreeIndex::item_require_treeindex([&] { return tree_view->source_model(); }, tree_view->current_item());
			if (tree_index) {
				// if(i != 0){
				auto ti = real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(real_url,
				    [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
					    return tree_index->url_bind_browser(real_target_url_,
						std::bind(&tv_t::move, tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
						[&](boost::intrusive_ptr<const i_t> it_) -> bool {
							return url_equal(url_value(detail::to_qstring(it_->field<home_key>())), real_url) || url_equal(it_->field<url_key>(), real_url);
						},
						static_cast<web::Browser*>(_browser));
				    });
				if (ti) {
					if (_blogger->topic() != "" && ti->field<url_key>() == Browser::_defaulthome) {
						auto page = ti->page();
						if (page) {
							auto view = page->view();
							if (view) {
								auto toolbar_search = view->toolbarsearch();
								if (toolbar_search) {
									ti->field<url_key>(toolbar_search->query_internet_decomposed(_blogger->topic()));
									ti->field<home_key>(detail::from_qstring<home_key>(detail::to_qstring(ti->field<url_key>())));
								}
							}
						}
					}
					if (ti->field<id_key>() == id_value(current_tab_id)) {
						auto v = ti->activate(std::bind(&TabWidget::find, this, std::placeholders::_1)); // std::bind(&HidableTabWidget::find, globalparameters.main_window()->vtab_record(), std::placeholders::_1)
						if (v != currentWebView()) select_as_current(v);
					}
				}
				// }else{
				// if(webView(0)->page()->url() != _url){
				//// webView(0)->load(_record);    //loadUrl(_url);
				//// auto ar =
				///boost::make_shared<WebPage::ActiveRecordBinder>(webView(0)->page());
				// auto it =	// tree_index->
				// TreeIndex::create_treeitem_from_url(QUrl(_url), std::bind(&tv_t::move,
				// tree_view, std::placeholders::_1, std::placeholders::_2,
				// std::placeholders::_3, std::placeholders::_4),
				// [&](boost::intrusive_ptr<const TreeItem> it_) -> bool {return
				// url_equal(it_->field<home_type>().toStdString(), _url.toStdString()) ||
				// url_equal(it_->field<url_type>().toStdString(), _url.toStdString());});
				//// boost::intrusive_ptr<RecordModel::ModelIndex> record_index(nullptr);

				//// try {
				//// record_index = new RecordModel::ModelIndex([&] {return
				///_record_controller->source_model();},
				///_record_controller->source_model()->sibling(it), it);
				//// } catch(std::exception &e) {}

				// webView(0)->page()->bind(it)->activate(std::bind(&HidableTabWidget::find,
				// globalparameters.main_window()->vtab_record(), std::placeholders::_1));
				// }
				// }
			}
		}

		return true;
	}

	// void TabWidget::current_download_acceptance(std::pair<QUrl, bool> policy){
	// _current_download_acceptance = policy;
	// }

	// std::pair<QUrl, bool> TabWidget::current_download_acceptance() const {
	// return _current_download_acceptance;
	// }

	void TabWidget::downloadRequested(QWebEngineDownloadItem* download)
	{
		int ret = !QMessageBox::Ok;
		auto url_ = download->url();
		if (_current_download_acceptance //.first
		    != url_) {                   //!
			// _current_download_acceptance.second &&
			QMessageBox message_box;

			message_box.setText(tr("Do you want to download the file?"));
			message_box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
			message_box.setDefaultButton(QMessageBox::Cancel);
			ret = message_box.exec();
			auto state = download->state();
			if (ret == QMessageBox::Ok) {
				//				_current_download_acceptance = {download->url(), true};
				// if(_current_download_acceptance.second){
				if (state == QWebEngineDownloadItem::DownloadRequested || state == QWebEngineDownloadItem::DownloadInProgress) {
					// download->accept();  // default in construction
					if (state == QWebEngineDownloadItem::DownloadRequested) {
						gl_paras->request_download_manager()->download(this, download);
					}
				}
				// else																																			// if(state ==
				// QWebEngineDownloadItem::DownloadCompleted || state ==
				// QWebEngineDownloadItem::DownloadCancelled || state ==
				// QWebEngineDownloadItem::DownloadInterrupted) {
				// _current_download_acceptance.second = false;
			} else {
				download->cancel();
				////	    fileNameLabel->setText(QString("Download canceled: ") +
				///download->path());
				//				_current_download_acceptance.first = download->url();
			}
			_current_download_acceptance //.first
			    = url_;
			// auto	_main_window	= globalparameters.mainwindow();
			// auto	_vtab_tree	= _main_window->vtab_tree();
			// auto	_vtab_record	= _main_window->vtab_record();
			// if(_vtab_record->currentWidget()->objectName() !=
			// download_manager_singleton_name)
			// for(auto tree_viewer : _main_window->tree_viewers())
			// if(tree_viewer->widget_right()->objectName() ==
			// download_manager_singleton_name)_vtab_tree->setCurrentWidget(tree_viewer);
			//
			// auto	_h_tree_splitter	= _main_window->h_tree_splitter();
			// auto	sizes			= _h_tree_splitter->sizes();
			//// auto ww = _h_left_splitter->widget(0)->width(); // 100 != 0 when sizes[0]
			///== 0
			// if(0 == sizes[0]){	// _h_left_splitter->widget(0)->width()
			// auto vtab_tree_min_width = _vtab_tree->minimumSizeHint().width();	//
			// _tree_screen->minimumSizeHint().width();                 //
			// globalparameters.entrance()->activated_browser()->record_screen()->minimumSizeHint().width();
			// // 6xx   // h_right_splitter->widget(0)->width();    // 0    //
			// sizeHint().width();    // 23
			//// auto h = h_right_splitter->handle(1);
			//// h->move(lr + shw, h->rect().top());

			// auto	size_memory	= appconfig->h_tree_splitter_sizelist();
			// auto	sum		= size_memory[0] + size_memory[1];
			// sizes[0]	= size_memory[0] > vtab_tree_min_width ? size_memory[0] < sum ?
			// size_memory[0] : sum * 15 / 100 : vtab_tree_min_width;
			// sizes[1]	= sum - sizes[0] > 0 ? sum - sizes[0] : sum * 85 / 100;		//
			// sizes[1] > size_memory[1] ? size_memory[1] : sizes[1];
			//// h_left_splitter->moveSplitter(sizes[0], 1);   // protected member
			// _h_tree_splitter->setSizes(sizes);	// emit
			// _h_left_splitter->splitterMoved(sizes[0], 1);

			//// auto s_0 = _vtab_tree->minimumSizeHint();   // (146, 146)
			//// auto s_1 = _vtab_record->minimumSizeHint(); // (25, 146)
			//// auto s_2 = _entrance->minimumSizeHint();
			//// auto s_3 = _h_right_splitter->minimumSizeHint();    // (241,146)
			//// auto s_4 = _h_right_splitter->maximumWidth();    // (241,146)
			//// auto sizes_check = _h_left_splitter->sizes();
			//// h_right_splitter->resize(h_right_splitter->sizeHint().width(),
			///h_right_splitter->height());
			// }
		}
	}

	void TabWidget::onTabsChanged()
	{
		// if(count() > 0) {
		// WebView *tab = nullptr;
		// tab = currentWebView();

		// if(tab != nullptr) {
		// tab->setFocus();
		// auto controller = tab->recordtablecontroller();
		// auto record = tab->record();

		// if(controller != nullptr && record != nullptr) {
		// QModelIndex proxyindex =
		// controller->convertIdToProxyIndex(record->getField("id"));
		// int position = controller->convertProxyIndexToPos(proxyindex);
		// RecordTableView *recordtableview = controller->getView();

		// if(recordtableview)recordtableview->setSelectionToPos(position); // work
		// }
		// }
		// }

		// auto _tree_view = _tree_screen->tree_view();
		// QModelIndex _current = _tree_view->current_index();

		// for(int i = 0; i < count(); i++) {
		// auto vi = webView(i);

		// if(vi != nullptr) {
		// auto it = vi->page()->bounded_item();

		// if(it) {

		//// _tree_view->select_and_current(it);
		// QModelIndex _i = _tree_view->source_model()->index(it);
		// _tree_view->selectionModel()->select(_i,
		// QItemSelectionModel::SelectionFlag::Select);
		// }
		// }
		// }

		// _tree_view->select_and_current(_current);
	}

	// will fail if count()==0 or it not inside current tabmanager
	boost::intrusive_ptr<i_t> TabWidget::sibling(boost::intrusive_ptr<i_t> it) const
	{
		boost::intrusive_ptr<i_t> r(nullptr);
		if (count() > 0 && it) {
			auto b = it->binder();
			if (b) {
				auto p = b->page();
				if (p) {
					auto v = p->view();
					if (v) {
						auto index = webViewIndex(v) - 1;
						if (index < 0) index = 0; // count() - 1;		// if(index != - 1 && index > 0)
						r = webView(index)->page()->binder()->host();
					}
				}
			}
		}
		return r;
	}

	web::Browser* TabWidget::browser() const
	{
		return static_cast<web::Browser*>(_browser);
	}

	TabBar* TabWidget::tabbar()
	{
		return _tabbar;
	}

	RecordModel* TabWidget::source_model()
	{
		RecordModel* rm = nullptr;
		auto _rctrl = _record_screen->record_ctrl();
		if (_rctrl) rm = _rctrl->source_model();
		return rm;
	}

	rv_t* TabWidget::view()
	{
		rv_t* v = nullptr;
		auto _rctrl = _record_screen->record_ctrl();
		if (_rctrl) v = _rctrl->view();
		return v;
	}


	// template<>
	// WebView *TabWidget::find<url_full>(const QUrl &find_url)const
	// {
	// WebView *view = nullptr;

	// for(int i = 0; i < count(); i++) {
	// auto current_view = webView(i);

	// if(current_view != nullptr) {
	// auto it = current_view->page()->bounded_item();

	// if(it) {

	// if(it->url() == find_url) {
	// view = current_view;
	// break;
	// }

	// }
	// }
	// }

	// return view;
	// }

	// template<>
	// WebView *TabWidget::find<url_fragment>(const QUrl &find_url)const
	// {
	// WebView *view = nullptr;
	////        boost::intrusive_ptr<TreeItem> re;
	////if(record) {

	// for(int i = 0; i < count(); i++) {
	// auto current_view = webView(i);

	// if(current_view != nullptr) {
	// auto it = current_view->page()->bounded_item();

	////                //                Record *checkrecord =
	///vi->page()->record();
	////                QString checkurl = vi->page()->url().toString();    // debug
	////                QString checktitle = vi->page()->title();           // debug
	// if(it) {

	// QString s_item_url = it->field("url");
	// assert(s_item_url != "");
	//// QString _url = find_url.toString();
	//// QUrl find_url = QUrl(_url);
	// QUrl item_url = QUrl(s_item_url);
	//// QString s_find_url = find_url.toString();
	//// QString s_item_url = item_url.toString();
	//// QUrl page_url = vi->page()->url();
	//// QString s_page_url = page_url.toString();
	//// int i = 0;

	// if(item_url.fragment() == find_url.fragment()  //it->field("url") == _url
	////.toString()
	// ) {   // record->getNaturalFieldSource("url")
	// view = current_view;
	////                        re = it;
	// break;
	// }

	// }
	// }
	// }

	////}

	// return view;
	// }

	WebView* TabWidget::find_nopin() const
	{
		WebView* bv = nullptr;
		for (int i = 0; i < count(); i++) {
			auto vi = webView(i);
			if (vi != nullptr) {
				if (vi->page()->host()) {
					if (vi->page()->host()->field<pin_key>() == pin_value(bool_from_check_state[Qt::Unchecked])) {
						bv = vi;
						break;
					}
				}
			}
		}
		return bv;
	}

	WebView* TabWidget::find(const std::function<bool(boost::intrusive_ptr<const ::Binder>)>& _equal) const
	{
		WebView* view = nullptr;
		for (int i = 0; i < count(); i++) {
			auto current_view = webView(i);
			if (current_view != nullptr) {
				auto it = current_view->page()->host();
				if (it) {
					if (_equal(
						it->binder())) { // it->url<url_type>() == url_type()(find_url)
						view = current_view;
						break;
					}
				}
			}
		}
		return view;
	}

	// WebView *TabWidget::find(boost::intrusive_ptr<const TreeItem> it_find)const
	// {
	//// boost::intrusive_ptr<TreeItem> re;
	// WebView *bv = nullptr;

	// for(int i = 0; i < count(); i++) {
	// if(webView(i)->page()->bounded_item() == it_find) {
	// bv = webView(i); //re = webView(i)->page()->bounded_item();
	// break;
	// }
	// }

	// return bv;
	// }

	void TabWidget::fullScreenRequested(QWebEngineFullScreenRequest request)
	{
		WebPage* webPage = qobject_cast<WebPage*>(sender());
		if (request.toggleOn()) {
			if (!_fullscreenview) {
				_fullscreenview = new QWebEngineView();
				_fullscreennotification = new FullScreenNotification(_fullscreenview);

				QAction* exitFullScreenAction = new QAction(_fullscreenview);
				exitFullScreenAction->setShortcut(Qt::Key_Escape);
				connect(exitFullScreenAction, &QAction::triggered, [webPage] {
					webPage->triggerAction(QWebEnginePage::ExitFullScreen);
				});
				_fullscreenview->addAction(exitFullScreenAction);
			}
			webPage->setView(_fullscreenview);
			request.accept();
			_fullscreenview->showFullScreen();
			_fullscreenview->raise();
			_fullscreennotification->show();
		} else {
			if (!_fullscreenview) return;
			WebView* oldWebView = this->webView(_lineedit_stack->currentIndex());
			webPage->setView(oldWebView);
			request.accept();
			raise();

			// {
			//// Change the delete and window hide/show back to a simple
			///m_fullScreenView->hide()
			//// once QTBUG-46701 gets fixed.
			// delete _fullscreenview;
			// _fullscreenview = nullptr;
			// window()->hide();
			// window()->show();
			// }
			{
				_fullscreenview->hide();
				_fullscreennotification->hide();
			}
		}
	}

#if QT_VERSION == 0x050600
	W_OBJECT_IMPL(WebActionMapper)
#endif

	WebActionMapper::WebActionMapper(QAction* root, QWebEnginePage::WebAction webAction, QObject* parent)
	    : QObject(parent)
	    , _currentparent(0)
	    , _root(root)
	    , _webaction(webAction)
	{
		if (!_root) return;
		connect(_root, &QAction::triggered, this, &WebActionMapper::rootTriggered);
		connect(_root, &QAction::destroyed, this, &WebActionMapper::rootDestroyed);
		_root->setEnabled(false);
	}

	void WebActionMapper::rootDestroyed()
	{
		_root = 0;
	}

	void WebActionMapper::currentDestroyed()
	{
		updateCurrent(0);
	}

	void WebActionMapper::addChild(QAction* action)
	{
		if (!action) return;
		connect(action, &QAction::changed, this, &WebActionMapper::childChanged);
	}

	QWebEnginePage::WebAction WebActionMapper::webAction() const
	{
		return _webaction;
	}

	void WebActionMapper::rootTriggered()
	{
		if (_currentparent) {
			QAction* gotoAction = _currentparent->action(_webaction);
			gotoAction->trigger();
		}
	}

	void WebActionMapper::childChanged()
	{
		if (QAction* source = qobject_cast<QAction*>(sender())) {
			if (_root && _currentparent && source->parent() == _currentparent) {
				_root->setChecked(source->isChecked());
				_root->setEnabled(source->isEnabled());
			}
		}
	}

	void WebActionMapper::updateCurrent(QWebEnginePage* currentParent)
	{
		if (_currentparent) disconnect(static_cast<QObject*>(_currentparent), &QObject::destroyed, this, &WebActionMapper::currentDestroyed);
		_currentparent = currentParent;
		if (!_root) return;
		if (!_currentparent) {
			_root->setEnabled(false);
			_root->setChecked(false);

			return;
		}
		QAction* source = _currentparent->action(_webaction);
		_root->setChecked(source->isChecked());
		_root->setEnabled(source->isEnabled());
		connect(static_cast<QObject*>(_currentparent), &QObject::destroyed, this, &WebActionMapper::currentDestroyed);
	}

	CustomTabStyle::CustomTabStyle()
	{
	}

	QSize CustomTabStyle::SizeFromContents(QStyle::ContentsType type, const QStyleOption* option, const QSize& size, const QWidget* widget) const
	{
		QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
		if (type == QStyle::CT_TabBarTab) {
			s.transpose();
			s.rwidth() = 100;
			s.rheight() = 16;
		}
		return s;
	}

	void CustomTabStyle::drawControl(QStyle::ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
	{
		if (element == CE_TabBarTabLabel) {
			if (const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab*>(option)) {
				QStyleOptionTab opt(*tab);

				opt.shape = QTabBar::RoundedNorth; // TriangularWest

				// opt.text = tr ("Hello");
				QIcon icon("/Resources/icon2.ico");
				opt.icon = icon;

				opt.palette.setCurrentColorGroup(QPalette::Disabled);
				opt.state |= QStyle::State_Sunken;
				QProxyStyle::drawControl(element, &opt, painter, widget);

				return;
			}
		}
		if (element == CE_TabBarTab) {
			auto _size = widget->geometry();
			painter->drawText(_size.top(), _size.left(), "Hello");

			// painter->setBackground (QBrush (Qt:: red, Qt:: SolidPattern));
		}
		QProxyStyle::drawControl(element, option, painter, widget);
	}

	// PopupWindow::PopupWindow(TabWidget *tabmanager
	// , QWebEngineProfile *profile
	// , QUrl const &url
	// , TableController *_record_controller
	// , TableController *_page_controller
	////                             , Browser *parent
	// ) :
	////        QWidget(nullptr)
	////        TabWidget(_record_controller, _page_controller, parent)
	// Browser(url         // Record *const record
	// , _record_controller
	// , _page_controller
	// , nullptr   //_entrance   // nullptr can not work!
	////, QDockWidget *parent
	// , QString()
	// )
	// , _addressbar(new QLineEdit(this))
	// , _view(nullptr
	////              new WebView(record, profile
	////                          // , false
	////                          , this
	////                          , _record_ontroller    //
	///globalparameters.getRecordTableScreen()->getRecordTableController()
	////                         )

	////            [tabmanager, this, url, profile, _record_controller,
	///_page_controller]
	////    {
	////        boost::shared_ptr<ActiveRecordBinder> wvh =
	///boost::make_shared<ActiveRecordBinder>(
	////                                                        tabmanager
	////                                                        , this
	////                                                        , profile
	////                                                        , _record_controller
	////                                                        , _page_controller
	////                                                    );
	////        boost::intrusive_ptr<Record> record
	////            = _record_controller->request_record(
	////                  url
	////                  , std::make_shared
	///<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, WebView *,
	///boost::intrusive_ptr<Record>>> (
	////                      ""
	////                      , &ActiveRecordBinder::binder
	////                      , wvh
	////                  )
	////                  , std::make_shared
	///<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, WebView *,
	///boost::intrusive_ptr<Record>>> (
	////                      ""
	////                      , &ActiveRecordBinder::activator
	////                      , wvh
	////                  )
	////              );  // ->binded_only_page()->view();
	////        record->active();
	////        return record->unique_page()->view();
	////    }()

	// )
	// {
	////        assert(record);
	////        assert(record->binded_only_page());

	////        _view->page(new WebPage(profile, url
	////                                // , false
	////                                , _record_ontroller
	////                                , _view));
	// _view = [tabmanager, this, url, profile, _record_controller,
	// _page_controller] {
	// boost::shared_ptr<ActiveRecordBinder> wvh =
	// boost::make_shared<ActiveRecordBinder>(
	// tabmanager
	// , this
	// , profile
	// , _record_controller
	// , _page_controller
	// );
	// boost::intrusive_ptr<Record> record
	// = _record_controller->request_record(
	// url
	// , std::make_shared <sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
	// WebView *, boost::intrusive_ptr<Record>>> (
	// ""
	// , &ActiveRecordBinder::binder
	// , wvh
	// )
	// , std::make_shared <sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
	// WebView *, boost::intrusive_ptr<Record>>> (
	// ""
	// , &ActiveRecordBinder::activator
	// , wvh
	// )
	// );  // ->binded_only_page()->view();
	// record->active();
	// return record->unique_page()->view();
	// }();

	////        _addressbar->hide();
	// QVBoxLayout *layout = new QVBoxLayout(this);
	// layout->setMargin(0);

	// layout->addWidget(_addressbar);
	// layout->addWidget(_view);
	// setLayout(layout);

	// _view->setFocus();

	// connect(_view, &WebView::titleChanged, static_cast<QWidget *const>(this),
	// &QWidget::setWindowTitle);
	// connect(_view, &WebView::urlChanged, this, &PopupWindow::setUrl);
	// connect(static_cast<QWebEnginePage *>(page()),
	// &QWebEnginePage::geometryChangeRequested, this,
	// &PopupWindow::adjustGeometry);
	// connect(static_cast<QWebEnginePage *>(page()),
	// &QWebEnginePage::windowCloseRequested, static_cast<QWidget *const>(this),
	// &QWidget::close);
	// }

	////    QWebEnginePage
	// WebPage *PopupWindow::page() const { return _view->page(); }

	// void PopupWindow::setUrl(const QUrl &url)
	// {
	// _addressbar->setText(url.toString());
	// }

	// void PopupWindow::adjustGeometry(const QRect &newGeometry)
	// {
	// const int x1 = frameGeometry().left() - geometry().left();
	// const int y1 = frameGeometry().top() - geometry().top();
	// const int x2 = frameGeometry().right() - geometry().right();
	// const int y2 = frameGeometry().bottom() - geometry().bottom();

	// setGeometry(newGeometry.adjusted(x1, y1 - _addressbar->height(), x2, y2));
	// }

	//// Слот для добавления новой записи в конец таблицы
	// void TabWidget::addnew_to_end(void)
	// {
	// _record_controller->addnew_to_end();    //
	// addnew_blank(ADD_NEW_RECORD_TO_END);
	// }
	//// Слот для добавления новой записи перед выделенной строкой
	// void TabWidget::addnew_before(void)
	// {
	// _record_controller->addnew_before();
	// }

	//// Слот для добавления новой записи после выделенной строки
	//// Slot to add a new record after the highlighted row
	// void TabWidget::addnew_after(void)
	// {
	// _record_controller->addnew_after();
	// }
	// void TabWidget::on_edit_fieldcontext(void)
	// {
	// _record_controller->on_edit_fieldcontext();
	// }

	//// Обработка клика по удалению записи в контекстном меню и по кнопке на панели
	// void TabWidget::close_context(void)
	// {
	// _record_controller->close_context();

	// }

	//// Копирование отмеченных записей в буфер обмена с удалением
	//// из таблицы конечных записей
	// void TabWidget::cut(void)
	// {
	// _record_controller->cut();
	// }

	//// Копирование отмеченных записей в буфер обмена
	// void TabWidget::copy(void)
	// {
	// _record_controller->copy();
	// }

	//// Insert records from the clipboard    // Вставка записей из буфера обмена
	// void TabWidget::paste(void)
	// {
	// _record_controller->paste();
	// }
	//// Слот, срабатывающий при вызове настроек
	// void TabWidget::settings(void)
	// {
	// _record_controller->settings();
	// }

	//// Перемещение записи вверх
	// void TabWidget::move_up(void)
	// {
	// _record_controller->move_up();
	// }

	//// Перемещение записи вниз
	// void TabWidget::move_dn(void)
	// {
	// _record_controller->move_dn();
	// }

	//// Клик по пункту "Сортировка" в контекстном меню
	// void TabWidget::on_sort_click(void)
	// {
	// _record_controller->on_sort_click();
	// }

	// void TabWidget::on_print_click(void)
	// {
	// _record_controller->on_print_click();
	// }

	// TabWidget::coupler::coupler(
	// TabWidget                           *_tab_widget
	// , boost::intrusive_ptr<TreeItem>    item_link_
	// , bool                              make_current_
	// )
	// : // std::enable_shared_from_this<Coupler>()
	// _tab_widget(_tab_widget)
	// , _item_link(item_link_)
	// , _page_link(nullptr)
	// , _make_current(make_current_)
	// {
	////        //
	///_bounded_item->record_binder(boost::intrusive_ptr<CouplerDelegation>(new
	///CouplerDelegation(shared_from_this())));
	////        _bounded_page->record_binder(new
	///TreeItem::coupler_delegation(std::const_pointer_cast<Coupler>(shared_from_this())));
	////        _bounded_item->record_binder(_bounded_page->record_binder());
	// }

	// WebView *TabWidget::coupler::binder()   // boost::intrusive_ptr<TreeItem>
	// item
	// {
	// WebView *view = nullptr;
	//// assert(item);

	////        boost::intrusive_ptr<TreeItem> result =
	///_tab_widget->_record_controller->source_model()->find(QUrl(web::Browser::_defaulthome));
	////        WebView *view = nullptr;

	////        if(!result) {
	//// assert(!item->is_registered_to_browser());
	// if(!_page_link) {
	// view = _tab_widget->newTab(_item_link, _make_current); //

	////            if(!_page_link) _page_link = view->page();
	// }

	////        view->page()->record_binder()->binder(item);  // included in
	///newTab()

	////        (item.get()->*_bind)(view->page());
	////        //        } else {
	////        //            //                    //
	///assert(item->is_registered_to_browser());
	////        //            //                    view = item->bind();
	/////result->unique_page()->view();
	////        //            assert(result->unique_page());
	////        //            assert(result->unique_page()->view());
	////        //            view = result->unique_page()->view();
	////        //            auto it = (item.get()->*_bind)(result->unique_page());
	////        //            assert(it->is_registered_to_browser());
	////        //        }
	////        _bounded_item = item;

	////        assert(_bounded_page == view->page());    // we can give it up now
	////        assert(_bounded_page->record_binder());
	// assert(_item_link->binder());
	////        _bounded_page->record_binder(_bounded_item->record_binder());

	// assert(_item_link->is_registered_to_browser() || (_item_link->field("url") ==
	// web::Browser::_defaulthome));

	// return view;
	// }

	// WebView *TabWidget::coupler::activator(
	//// boost::intrusive_ptr<TreeItem> item
	// )
	// {
	// assert(_item_link->page_link() == _page_link);
	// assert(_item_link == _page_link->item_link());

	// assert(_item_link);
	// assert(_item_link->page_valid());
	// return _item_link->page_link()->activate();
	// }
	rs_t* TabWidget::record_screen() const
	{
		return static_cast<rs_t*>(_record_screen);
	}
}
