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

#include <functional>
#include <cassert>
#include "toolbarsearch.h"
#include "autosaver.h"

#include "libraries/qt_single_application5/qtsingleapplication.h"
// #include "views/find_in_base_screen/FindTableWidget.h"
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>

#include <QtWidgets/QCompleter>
#include <QtWidgets/QMenu>
#include <QtCore/QStringListModel>
#include <QtWidgets/QStackedWidget>
#include <QWebEngineSettings>

//#include "views/browser/chasewidget.h"
#include "models/tree/binder.hxx"
#include "models/record_table/linker.hxx"
#include "views/browser/tabwidget.h"
#include "views/browser/entrance.h"
#include "views/find_in_base_screen/FindScreen.h"
#include "main.h"
#include "views/tree/TreeScreen.h"
#include "views/tree/KnowView.h"
#include "models/tree/treeindex.hxx"
#include "models/tree/KnowModel.h"



namespace browser {



    /*
        ToolbarSearch is a very basic search widget that also contains a small history.
        Searches are turned into urls that use Google to perform search
     */
    ToolbarSearch::ToolbarSearch(QWidget *parent)   //QStackedWidget *lineedits, QLineEdit *findtext,
        : SearchLineEdit(parent)
        , _autosaver(new AutoSaver(this))
        , _maxsavedsearches(10)
        , _stringlistmodel(new QStringListModel(this))
        , _lineedits(nullptr)   // new QStackedWidget(this))  // , _lineedits(lineedits)
        , _findtext(new QLineEdit(this))        // , _findtext(findtext)
    {
        //        _lineedits->setVisible(false);
        _findtext->setVisible(false);

        QMenu *m = menu();
        connect(m, &QMenu::aboutToShow, this, &ToolbarSearch::aboutToShowMenu);
        connect(m, &QMenu::triggered, this, &ToolbarSearch::triggeredMenuAction);

        QCompleter *completer = new QCompleter(_stringlistmodel, this);
        completer->setCompletionMode(QCompleter::InlineCompletion);
        lineEdit()->setCompleter(completer);

        assert(lineEdit());
        connect(lineEdit(), &QLineEdit::returnPressed, this, &ToolbarSearch::searchNow);

        connect(this, &SearchLineEdit::textChanged, _findtext, [&](const QString & text) {_findtext->setText(text);});
        //connect(this, &ToolbarSearch::returnPressed, _tabmanager, &TabManager::lineEditReturnPressed);
        connect(this, &ToolbarSearch::returnPressed, _findtext, &QLineEdit::returnPressed);

        setInactiveText(tr("Google"));

        load();
    }

    ToolbarSearch::~ToolbarSearch()
    {
        _autosaver->saveIfNeccessary();
    }

    void ToolbarSearch::save()
    {
        QSettings settings;
        settings.beginGroup(QLatin1String("toolbarsearch"));
        settings.setValue(QLatin1String("recentSearches"), _stringlistmodel->stringList());
        settings.setValue(QLatin1String("maximumSaved"), _maxsavedsearches);
        settings.endGroup();
    }

    void ToolbarSearch::load()
    {
        QSettings settings;
        settings.beginGroup(QLatin1String("toolbarsearch"));
        QStringList list = settings.value(QLatin1String("recentSearches")).toStringList();
        _maxsavedsearches = settings.value(QLatin1String("maximumSaved"), _maxsavedsearches).toInt();
        _stringlistmodel->setStringList(list);
        settings.endGroup();
    }

    void ToolbarSearch::searchNow()
    {
        QString search_text = lineEdit()->text();

        auto result_item = globalparameters.find_screen()->find_clicked();
        TreeScreen *_tree_screen = globalparameters.tree_screen();
        auto tree_view = _tree_screen->tree_view();

        if(!result_item) {  //  || 0 == result_item->count_direct()

            QUrl url = QUrl(search_text);


            // if(url.host().isSimpleText());

            //        bool url_isRelative = url.isRelative();
            //        bool url_isValid = url.isValid();
            //        bool host_not_null = !url.host().isNull();
            //        bool host_isDetached = url.host().isDetached();
            //        bool host_isEmpty = url.host().isEmpty();
            //        bool host_isSimpleText = url.host().isSimpleText();

            //        bool path_empty = url.path().isEmpty();
            //        bool path_null = url.path().isNull();
            //        QString path = url.path();

            //        if( // !url.host().isEmpty() &&
            //            url.isValid()
            //        ) {
            //            if(url.scheme().isEmpty()    //url.scheme().isNull()
            //               // && url.isRelative() //&& !url.host().isNull()
            //              ) {
            //                //url = QUrl("http://" + searchText);
            //                url.setScheme(QLatin1String("https"));
            //            }

            //            if(url.path().isEmpty()
            //              ) {
            //                url.setPath(QLatin1String("//"));
            //            }
            //        }

            //            auto tree_view = _tree_screen->tree_view();

            auto tree_view = _tree_screen->tree_view();

            boost::intrusive_ptr<TreeIndex> modelindex(nullptr);

            try {
                modelindex = new TreeIndex([&] {return tree_view->source_model();}, tree_view->current_item()->parent(), tree_view->current_item()->parent()->sibling_order([&](boost::intrusive_ptr<const Linker> il) {return il->host() == tree_view->current_item() && il == tree_view->current_item()->linker() && tree_view->current_item()->parent() == il->host_parent();}));
            } catch(std::exception &e) {}

            if(modelindex) {
                // example !url.isEmpty() && url.isValid() && !url.scheme().isEmpty()
                if(!url.isEmpty()
                   && !url.host().isNull()
                   && url.isValid()
                   && !url.scheme().isEmpty()
                   // && url != QUrl(DockedWindow::_defaulthome) //&& !url.host().isNull()
                  ) {
                    //QLineEdit *lineedit =

                    modelindex->item_bind(tree_view->current_item()
                                          , url
                                          , std::bind(&KnowView::view_paste_child, tree_view, modelindex, std::placeholders::_2, std::placeholders::_3)
                                          , [&](boost::intrusive_ptr<const TreeItem> it_)->bool {return it_->field("url") == url.toString();}
                                         )->activate(std::bind(&browser::Entrance::find, globalparameters.entrance(), std::placeholders::_1));

                    assert(_lineedits);

                    if(_lineedits) {
                        QLineEdit *line_edit = qobject_cast<QLineEdit *>(_lineedits->currentWidget());

                        if(line_edit)line_edit->setText(search_text);
                    }

                    //globalparameters.entrance()->activebrowser()->tabWidget()->currentLineEdit()->setText(searchText);

                    //globalparameters.entrance()->activebrowser()->tabWidget()->new_view(register_record(url));

                    //currentLineEdit();  // lineEditReturnPressed();
                    //assert(lineedit);
                    //lineedit->setText(searchText);
                    //lineedit->returnPressed();
                } else  {

                    QStringList newList = _stringlistmodel->stringList();

                    if(newList.contains(search_text))
                        newList.removeAt(newList.indexOf(search_text));

                    newList.prepend(search_text);

                    if(newList.size() >= _maxsavedsearches)
                        newList.removeLast();

                    if(!QtSingleApplication::instance()->privateBrowsing()) {
                        _stringlistmodel->setStringList(newList);
                        _autosaver->changeOccurred();
                    }

                    QUrl url(QLatin1String("https://www.google.com/search"));
                    QUrlQuery url_query;

                    //                url_query.addQueryItem(QLatin1String("q"), searchText);
                    url_query.addQueryItem(QLatin1String("ie"), QLatin1String("UTF-8"));
                    url_query.addQueryItem(QLatin1String("oe"), QLatin1String("UTF-8"));
                    url_query.addQueryItem(QLatin1String("client"), QLatin1String("mytetra"));
                    // urlQuery.addQueryItem();

                    url.setQuery(url_query);
                    url.setFragment("q=" + search_text);
                    // QString u = url.toString();

                    //                emit search(url, std::bind(&TreeScreen::view_paste_child, _tree_screen, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
                    modelindex->item_bind(tree_view->current_item(), url, std::bind(&KnowView::view_paste_child, tree_view, modelindex, std::placeholders::_2, std::placeholders::_3), [&](boost::intrusive_ptr<const TreeItem> it_)->bool {return it_->field("url") == url.toString();})->activate(std::bind(&browser::Entrance::find, globalparameters.entrance(), std::placeholders::_1));

                }
            }
        } else if(result_item != tree_view->current_item()) {
            //            globalparameters.entrance()->activiated_browser()->tabmanager()->setCurrentIndex(0);
            //            TreeScreen *_tree_screen = globalparameters.tree_screen();
            auto index_result = tree_view->source_model()->index(result_item);

            if(index_result.isValid()) {
                auto it = tree_view->source_model()->item(index_result);
                boost::intrusive_ptr<TreeIndex> tree_index;
                try {tree_index = new TreeIndex([&] {return tree_view->source_model();}, it->parent(), it->parent()->sibling_order([&](boost::intrusive_ptr<const Linker> il) {return il == it->linker() && il->host() == it && it->parent() == il->host_parent() && it->parent() == il->host_parent();}));} catch(std::exception &e) {throw e;}

                tree_view->select_as_current(tree_index);


                //                //            else {
                //                //                _tree_screen->view_paste_as_child(
                //                //                    TreeIndex([&]()->KnowModel* {return _tree_screen->tree_view()->source_model();}, _tree_screen->tree_view()->current_index())
                //                //                    , result_item
                //                //                    , std::bind([&](boost::intrusive_ptr<const Linker> target, boost::intrusive_ptr<const Linker> source)->bool{return target->host()->field("url") == source->host()->field("url");}, std::placeholders::_1, result_item->up_linker())
                //                //                );
                //                //            }

                //                //                //            _tree_screen->tree_view()->reset();
                //                //                _tree_screen->setup_model(result_item);
                //                auto _index = _tree_screen->tree_view()->select_as_current(result_item->item_direct(0)->host());
                //                //            _tree_screen->tree_view()->selectionModel()->select(_index, current_tree_selection_mode);   //QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);   // current_tree_selection_mode
                //                //            _tree_screen->tree_view()->selectionModel()->setCurrentIndex(_index, current_tree_current_index_mode); //QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);  //current_tree_selection_mode , QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current

                tree_view->index_invoke(index_result);
            }

        } else {
            tree_view->index_invoke(tree_view->source_model()->index(result_item));
        }
    }

    void ToolbarSearch::aboutToShowMenu()
    {
        lineEdit()->selectAll();
        QMenu *m = menu();
        m->clear();
        QStringList list = _stringlistmodel->stringList();

        if(list.isEmpty()) {
            m->addAction(tr("No Recent Searches"));
            return;
        }

        QAction *recent = m->addAction(tr("Recent Searches"));
        recent->setEnabled(false);

        for(int i = 0; i < list.count(); ++i) {
            QString text = list.at(i);
            m->addAction(text)->setData(text);
        }

        m->addSeparator();
        m->addAction(tr("Clear Recent Searches"), this, SLOT(clear()));
    }

    void ToolbarSearch::triggeredMenuAction(QAction *action)
    {
        QVariant v = action->data();

        if(v.canConvert<QString>()) {
            QString text = v.toString();
            lineEdit()->setText(text);
            searchNow();
        }
    }

    void ToolbarSearch::clear()
    {
        _stringlistmodel->setStringList(QStringList());
        _autosaver->changeOccurred();;
    }

    void ToolbarSearch::text(const QString &text) {_findtext->setText(text);}
    QString ToolbarSearch::text() const {return _findtext->text();}

}



