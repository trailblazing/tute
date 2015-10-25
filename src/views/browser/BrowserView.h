#ifndef _BROWSERVIEW_H_
#define _BROWSERVIEW_H_

#include <QWidget>
#include <QList>
#include <QUrl>
#include "models/recordTable/Record.h"
#include "libraries/GlobalParameters.h"

class QNetworkReply;
class QSslError;
extern GlobalParameters globalParameters;


class BrowserView : public QWidget {
    Q_OBJECT
public:
    BrowserView(QWidget *parent = 0);
    ~BrowserView();

public slots:
    void setUrl(const QUrl &url);
    void loadUrl(const QUrl &_url = QUrl());
    void loadUrl(const int pos);
    void setScrollbars(bool hide);
    void setCache(bool cache, int cacheSize);
    void finished(QNetworkReply *reply);
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
    QAction *getactionFreeze() { return actionFreeze; }
    void onUrlChanged(const QUrl &_url);
    void onLoadFinished(bool);
    void setupActions(void);

private:
    void setupUI(void);
    void setupSignals(void);
    void assembly(void);
    //void urlChanged(const QUrl &_url){onUrlChanged(_url);}
    QAction *actionFreeze;
    struct BrowserViewPrivate *d;

};

#endif // _BROWSERVIEW_H_
