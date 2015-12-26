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
#include "toolbarsearch.h"
#include "autosaver.h"

#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "views/find_in_base_screen/FindTableWidget.h"
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>

#include <QtWidgets/QCompleter>
#include <QtWidgets/QMenu>
#include <QtCore/QStringListModel>
#include <QtWidgets/QStackedWidget>
#include <QWebEngineSettings>
//#include "views/browser/chasewidget.h"
#include "views/browser/tabwidget.h"
#include "views/browser/entrance.h"
#include "views/find_in_base_screen/FindScreen.h"
#include "main.h"



namespace browser {



    /*
        ToolbarSearch is a very basic search widget that also contains a small history.
        Searches are turned into urls that use Google to perform search
     */
    ToolbarSearch::ToolbarSearch(//QStackedWidget *lineedits, QLineEdit *findtext,
        QWidget *parent)
        : SearchLineEdit(parent)
        , _autosaver(new AutoSaver(this))
        , _maxsavedsearches(10)
        , _stringlistmodel(new QStringListModel(this))
        , _lineedits(new QStackedWidget(this))  // , _lineedits(lineedits)
        , _findtext(new QLineEdit(this))        // , _findtext(findtext)
    {
        _lineedits->setVisible(false);
        _findtext->setVisible(false);

        QMenu *m = menu();
        connect(m, SIGNAL(aboutToShow()), this, SLOT(aboutToShowMenu()));
        connect(m, SIGNAL(triggered(QAction *)), this, SLOT(triggeredMenuAction(QAction *)));

        QCompleter *completer = new QCompleter(_stringlistmodel, this);
        completer->setCompletionMode(QCompleter::InlineCompletion);
        lineEdit()->setCompleter(completer);

        connect(lineEdit(), SIGNAL(returnPressed()), SLOT(searchNow()));
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
        QString searchText = lineEdit()->text();

        std::shared_ptr<TableData> recordtabledata = globalparameters.find_screen()->findClicked();

        if(!recordtabledata || 0 == recordtabledata->size()) {

            QUrl url = QUrl(searchText);


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

            // example !url.isEmpty() && url.isValid() && !url.scheme().isEmpty()
            if(!url.isEmpty()
               // && !url.host().isNull()
               && url.isValid()
               && !url.scheme().isEmpty()
               // && url != QUrl(DockedWindow::_defaulthome) //&& !url.host().isNull()
              ) {
                //QLineEdit *lineedit =

                //            globalparameters.entrance()->active_record(request_record(url));
                auto ara = boost::make_shared<browser::Entrance::ActiveRecordBinder>(globalparameters.entrance());
                auto r = globalparameters.table_screen()->table_controller()->request_record(
                             url
                             , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, std::shared_ptr<Record>>>(
                                 ""
                                 , &browser::Entrance::ActiveRecordBinder::binder
                                 , ara
                             )
                             , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, std::shared_ptr<Record>>>(
                                 ""
                                 , &browser::Entrance::ActiveRecordBinder::activator
                                 , ara
                             )
                         );

                r->active();

                QLineEdit *line_edit = qobject_cast<QLineEdit *>(_lineedits->currentWidget());

                if(line_edit)line_edit->setText(searchText);

                //globalparameters.entrance()->activebrowser()->tabWidget()->currentLineEdit()->setText(searchText);

                //globalparameters.entrance()->activebrowser()->tabWidget()->new_view(register_record(url));

                //currentLineEdit();  // lineEditReturnPressed();
                //assert(lineedit);
                //lineedit->setText(searchText);
                //lineedit->returnPressed();
            } else  {

                QStringList newList = _stringlistmodel->stringList();

                if(newList.contains(searchText))
                    newList.removeAt(newList.indexOf(searchText));

                newList.prepend(searchText);

                if(newList.size() >= _maxsavedsearches)
                    newList.removeLast();

                if(!QtSingleApplication::instance()->privateBrowsing()) {
                    _stringlistmodel->setStringList(newList);
                    _autosaver->changeOccurred();
                }

                QUrl url(QLatin1String("https://www.google.com/search"));
                QUrlQuery urlQuery;
                urlQuery.addQueryItem(QLatin1String("q"), searchText);
                urlQuery.addQueryItem(QLatin1String("ie"), QLatin1String("UTF-8"));
                urlQuery.addQueryItem(QLatin1String("oe"), QLatin1String("UTF-8"));
                urlQuery.addQueryItem(QLatin1String("client"), QLatin1String("mytetra"));
                // urlQuery.addQueryItem();
                url.setQuery(urlQuery);
                // QString u = url.toString();
                emit search(url);
            }
        } else {
            globalparameters.table_screen()->table_controller()->select_pos(0);
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

    void ToolbarSearch::setText(const QString &text) {_findtext->setText(text);}
    QString ToolbarSearch::text() const {return _findtext->text();}

}



