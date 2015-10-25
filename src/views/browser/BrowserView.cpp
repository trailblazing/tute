#include "main.h"
#include <QWebView> //#include <QtWebKitWidgets/QWebView>
#include <QWebElement>
#include <QWebPage>
#include <QWebFrame>
#include <QVBoxLayout>
#include <QNetworkDiskCache>
#include <QDesktopServices>
#include <QNetworkReply>
#include <QSslError>
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QMessageBox>
#include <QMap>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QAction>
#include <QApplication>
#include <QSettings>
#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QUrl>
#include <cassert>

#ifdef Q_WS_QWS
#include <QWSServer>
#endif

#include "BrowserView.h"
#include "browser_config.h"
#include "views/recordTable/RecordTableScreen.h"
#include "controllers/recordTable/RecordTableController.h"
#include "views/recordTable/RecordTableView.h"
#include "models/recordTable/RecordTableData.h"
#include "models/recordTable/RecordTableModel.h"
#include "libraries/WindowSwitcher.h"

struct BrowserViewPrivate {
    //Q_OBJECT
public:
    QWebView *view;
    QNetworkAccessManager *nam;
    QMap<QPair<QString, int>, QByteArray> exceptions;
    bool msgShown;
    //~BrowserViewPrivate() = default;
};


void BrowserView::loadUrl(const int pos)
{
    RecordTableController *recordTableController = globalParameters.getRecordTableScreen()->getRecordTableController();
    //int pos = recordTableController->getFirstSelectionPos();
    RecordTableModel *recordTableModel = recordTableController->getRecordTableModel();
    RecordTableData *recordTableData = recordTableModel->getRecordTableData();
    Record *record = recordTableData->getRecord(pos);

    QUrl _url;

    if(record) {
        //this->current_record = record;
        _url = record->getField("url");
    }

    loadUrl(_url);

}

//Record *BrowserView::findRecord()
//{
//    Record *_record = nullptr;
//    RecordTableController *recordTableController = globalParameters.getRecordTableScreen()->getRecordTableController();
//    int pos = recordTableController->getFirstSelectionPos();

//    if(pos != -1) {
//        //RecordTableView *view = recordTableController->getView();
//        //view->loadUrl();

//        RecordTableModel *recordTableModel = recordTableController->getRecordTableModel();
//        RecordTableData *recordTableData = recordTableModel->getRecordTableData();

//        Record *record = recordTableData->getRecord(pos);

//        //if(record->isLite())record->switchToFat();  // no such a value

//        //QVariant qVariant = recordTableModel->data(pos, Qt::DisplayRole);

//        //    if(current_record.isLite())current_record.switchToFat();

//        //    QString dir = globalParameters.getWorkDirectory();
//        //    QString name = this->d->view->title();

//        //    if(0 == name.size())name = "default";

//        //    this->current_record.setNaturalFieldSource("id", url.toString());

//        //    //this->current_record.setField("name", name);  //
//        //    this->current_record.setNaturalFieldSource("name", name);
//        //    //this->current_record.setField("crypt", "0");


//        //    QString author = "default";
//        //    QString tags = "default";

//        //    QString text = this->d->view->page()->mainFrame()->toHtml();

//        //    if(0 == text.size())text = "<html>no content</html>";

//        if(record) {
//            //this->current_record
//            _record = record;
//        }

//        //    this->current_record.setNaturalFieldSource("author", author);
//        //    this->current_record.setNaturalFieldSource("tags", tags);
//        //    this->current_record.setText(text);
//        //    this->current_record.setNaturalFieldSource("url", url.toString());
//        //    this->current_record.setNaturalFieldSource("dir", dir);

//    }

//    return _record;
//}

void BrowserView::loadUrl(const QUrl &_url)
{
    QUrl url;
    // initialize

    QFileInfo settingsFile;

    // QString sb(QApplication::applicationDirPath());
    // sb.append(QDir::separator());
    // sb.append("browser_config.ini");

    QString configFileName = globalParameters.getWorkDirectory() + "/browser_config.ini";
    // check to see if we have a settings file where we started from
    // if not fall back to system hard coded path
    QFileInfo file(configFileName.toLatin1()); //file(sb.toLatin1());

    if(file.exists()) {
        qDebug() << "using local settings file";
        settingsFile.setFile(file.filePath());
    } else {
        qDebug() << "using system defined settings file";
        settingsFile.setFile(SETTINGS_FILE);
    }

    QSettings settings(settingsFile.filePath(), QSettings::NativeFormat);

    settings.beginGroup(SETTINGS_SECTION);

    if(_url.isEmpty()) {
        url = (settings.value("browser_view").toString());  //QUrl url(settings.value("browser_view").toString());
    } else {
        url = _url;
    }

    this->setUrl(url);

    qDebug() << "loading url: " << settings.value("main_view").toString();

    if(settings.value("full_screen", false).toBool()) {
        this->showFullScreen();
    }

    if(settings.value("hide_cursor", false).toBool()) {
        this->setCursor(QCursor(Qt::BlankCursor));
#ifdef Q_WS_QWS
        QWSServer::setCursorVisible(false);
#endif
    }

    this->setScrollbars(settings.value("hide_scrollbars", false).toBool());
    this->setCache(settings.value("enable_cache", false).toBool(), settings.value("cache_size_megabytes", 5).toInt());

    settings.endGroup();



    this->show();

    //    RecordTableController *recordTableController = globalParameters.getRecordTableScreen()->getRecordTableController();
    //    int pos = recordTableController->getFirstSelectionPos();

    //    if(pos != -1) {
    //        //RecordTableView *view = recordTableController->getView();
    //        //view->loadUrl();

    //        RecordTableModel *recordTableModel = recordTableController->getRecordTableModel();
    //        RecordTableData *recordTableData = recordTableModel->getRecordTableData();

    //        Record *record = recordTableData->getRecord(pos);

    //        //if(record->isLite())record->switchToFat();  // no such a value

    //        //QVariant qVariant = recordTableModel->data(pos, Qt::DisplayRole);

    //        //    if(current_record.isLite())current_record.switchToFat();

    //        //    QString dir = globalParameters.getWorkDirectory();
    //        //    QString name = this->d->view->title();

    //        //    if(0 == name.size())name = "default";

    //        //    this->current_record.setNaturalFieldSource("id", url.toString());

    //        //    //this->current_record.setField("name", name);  //
    //        //    this->current_record.setNaturalFieldSource("name", name);
    //        //    //this->current_record.setField("crypt", "0");


    //        //    QString author = "default";
    //        //    QString tags = "default";

    //        //    QString text = this->d->view->page()->mainFrame()->toHtml();

    //        //    if(0 == text.size())text = "<html>no content</html>";

    //        if(record) this->current_record = record;

    //        //    this->current_record.setNaturalFieldSource("author", author);
    //        //    this->current_record.setNaturalFieldSource("tags", tags);
    //        //    this->current_record.setText(text);
    //        //    this->current_record.setNaturalFieldSource("url", url.toString());
    //        //    this->current_record.setNaturalFieldSource("dir", dir);

    //    }

    //this->current_record = findRecord();
}

BrowserView::BrowserView(QWidget *parent)
    : QWidget(parent)
    , d(new BrowserViewPrivate)
      //, current_record(nullptr)
{
    d->view = new QWebView(this);
    d->view->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    d->view->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    d->view->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    d->view->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);

    d->msgShown = false;

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(d->view);

    setupActions();

    setupUI();
    assembly();

    loadUrl();

    setupSignals();

    d->nam = d->view->page()->networkAccessManager();

    connect(d->nam, SIGNAL(finished(QNetworkReply *)), this, SLOT(finished(QNetworkReply *)));

    connect(d->nam, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this, SLOT(sslErrors(QNetworkReply *, const QList<QSslError> &)));


}

BrowserView::~BrowserView()
{
    //if(isselfcreated())delete current_record;   // no, do not apply memory by this class for record, from the original source
    delete actionFreeze;
    delete d;
}


void BrowserView::setupActions()
{
    actionFreeze = new QAction(tr("Freeze browser view"), this);
    actionFreeze->setStatusTip(tr("Freeze browser view"));
    actionFreeze->setIcon(QIcon(":/resource/pic/freeze_browser_view.png"));
}

void BrowserView::setupUI(void)
{

}

void BrowserView::setupSignals(void)
{
    connect(this->actionFreeze, SIGNAL(triggered()), globalParameters.getWindowSwitcher(), SLOT(findInBaseClick()));

    //    //Connect a signal to a pointer to qobject member function
    //    template <typename Func1, typename Func2>
    //    static inline QMetaObject::Connection connect(
    //        const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
    //        const typename QtPrivate::FunctionPointer<Func2>::Object *receiver, Func2 slot,
    //        Qt::ConnectionType type = Qt::AutoConnection);

    QMetaObject::Connection _connection;    // for disconnect
    //…
    //    _connection = QObject::connect(…);
    //…
    //    QObject::disconnect(_connection);

    //_connection =  //QWebView::
    QWebView::connect
    //                   <
    //                   void (QWebView::*)(bool)
    //                   , void (RecordTableController::*)(void)
    //                   >
    (
        d->view
        //, &BrowserView::urlChanged//, &QWebView::urlChanged  //
        , &QWebView::loadFinished
        , this//globalParameters.getRecordTableScreen()->getRecordTableController()
        //, &BrowserView::onUrlChanged//&RecordTableController::autoAddNewAfterContext
        , &BrowserView::onLoadFinished//
    );

    //    _connection = connect(this
    //                          , &BrowserView::loadChanged
    //                          , globalParameters.getRecordTableScreen()->getRecordTableController()
    //                          , &RecordTableController::autoAddNewAfterContext);

    //void (BrowserView::*lU)(const QUrl &) = &BrowserView::loadUrl;
    _connection = QWebView::connect <
                  void (QWebView::*)(const QUrl &)
                  , void (BrowserView::*)(const QUrl &)
                  > (d->view, &QWebView::urlChanged, this
                     //, &BrowserView::loadUrl //
                     , &BrowserView::onUrlChanged
                    );
}

void BrowserView::assembly(void)
{

}

void BrowserView::setUrl(const QUrl &_url)
{
    d->view->load(_url);
}

void BrowserView::setScrollbars(bool hide)
{
    if(!hide) {
        d->view->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
        d->view->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAsNeeded);
    }
}

void BrowserView::setCache(bool cache, int cacheSize)
{
    if(cache) {
        QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
        QString location = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        diskCache->setCacheDirectory(location);
        diskCache->setMaximumCacheSize(cacheSize * 1024 * 1024); //in MB's
        d->nam->setCache(diskCache);
        qDebug() << QString("Cache location: %1").arg(location);
        qDebug() << QString("Cache maximum size: %1MB").arg(cacheSize);
    }
}

void BrowserView::onLoadFinished(bool finished)
{
    if(finished) {

        QWebFrame *frame = d->view->page()->mainFrame();
        QWebElement dom_title(frame->findFirstElement("title"));
        QString title = dom_title.evaluateJavaScript("this.text").toString();

        RecordTableController *recordTableController = globalParameters.getRecordTableScreen()->getRecordTableController();

        //RecordTableData *recordTableData = recordTableController->getRecordTableModel()->getRecordTableData();

        int pos = recordTableController->getFirstSelectionPos();
        Record *previous_record = recordTableController->getRecordTableModel()->getRecordTableData()->getRecord(pos);
        previous_record->setNaturalFieldSource("name", title);
    }
}


void BrowserView::onUrlChanged(const QUrl &_url)
{
    //QString url(QString(""));

    RecordTableController *recordTableController = globalParameters.getRecordTableScreen()->getRecordTableController();

    RecordTableData *recordTableData = recordTableController->getRecordTableModel()->getRecordTableData();

    if(!recordTableData->isRecordExists(_url)) {

        int pos = recordTableController->getFirstSelectionPos();
        Record *previous_record = recordTableController->getRecordTableModel()->getRecordTableData()->getRecord(pos);

        Record record;

        if(record.isLite())record.switchToFat();

        //QString title = d->view->title(); // not ready yet
        record.setNaturalFieldSource("id",   previous_record->getNaturalFieldSource("id"));
        record.setNaturalFieldSource("name",   previous_record->getNaturalFieldSource("name"));
        record.setNaturalFieldSource("author", previous_record->getNaturalFieldSource("author"));
        record.setNaturalFieldSource("url",    _url.toString());    // only changed
        record.setNaturalFieldSource("tags",   previous_record->getNaturalFieldSource("tags"));

        recordTableController->addNew(ADD_NEW_RECORD_AFTER, record);   //recordTableController->autoAddNewAfterContext();
    }

    //    Record *previous_record = recordTableController->getRecordTableModel()->getRecordTableData()->getRecord(pos);
    //    //if(current_record)
    //    url = previous_record->getNaturalFieldSource("url");

    //    std::string url_compare_stored = _url.toString().toStdString();
    //    std::string url_compare_get = url.toStdString();
    //    std::string compare = getDifference(url_compare_stored, url_compare_get);
    //    //QUrl qurl(url);

    //    //if(qurl != _url) {
    //    if(compare.size() != 0 && compare != "/") { // really changed!

    //        //if(this->current_record->getNaturalFieldSource("url") != _url.toString()) {



    //        //this->current_record = new Record();    // should from framework

    //        Record record;

    //        if(record.isLite())record.switchToFat();

    //        record.setNaturalFieldSource("id",   previous_record->getNaturalFieldSource("id"));
    //        record.setNaturalFieldSource("name",   previous_record->getNaturalFieldSource("name"));
    //        record.setNaturalFieldSource("author", previous_record->getNaturalFieldSource("author"));
    //        record.setNaturalFieldSource("url",    _url.toString());    // only changed
    //        record.setNaturalFieldSource("tags",   previous_record->getNaturalFieldSource("tags"));

    //        //record.setText(browser_view->getText());
    //        if(!recordTableData->isRecordExists(_url)) {
    //            //this->current_record->setNaturalFieldSource("url", _url.toString());
    //            recordTableController->addNew(ADD_NEW_RECORD_AFTER, record);   //recordTableController->autoAddNewAfterContext();

    //            //loadUrl(_url);

    //            //this->current_record = findRecord();
    //            //assert(this->current_record->getNaturalFieldSource("url") == _url.toString());
    //        }

    //    }
}


void BrowserView::finished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError) {
        qDebug() <<  QString("Network Error: %1").arg(reply->errorString());
    }

    if(reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute).toBool() == true) {
        QVariant contentVar = reply->header(QNetworkRequest::ContentTypeHeader);
        qDebug() <<  QString("Cache Used: %1").arg(contentVar.toString());
    }

    //    QUrl url = reply->url();
    //    QString _url = current_record->getNaturalFieldSource("url");
    //    std::string url_compare_stored = _url.toStdString();
    //    std::string url_compare_get = url.toString().toStdString();
    //    std::string compare = "";

    //    if(url_compare_stored.size() >= url_compare_get.size()) {
    //        url_compare_stored.erase(url_compare_get.begin(), url_compare_get.end());
    //        compare = url_compare_stored;
    //    } else {
    //        url_compare_get.erase(url_compare_stored.begin(), url_compare_stored.end());
    //        compare = url_compare_get;
    //    }

    //    std::string::size_type pos;

    //    while((pos = compare.find_first_of(" ")) != compare.npos)compare.erase(pos, 1);

    //    if(compare == "" || compare == "/") {
    //        //if(this->current_record->getNaturalFieldSource("url") != url.toString()) {
    //        this->current_record->setNaturalFieldSource("url", url.toString());
    //        this->loadChanged();
    //    }

}

void BrowserView::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    foreach(const QSslError &error, errors) {
        qDebug() << QString("SSL Error: %1").arg(error.errorString());
    }

    reply->ignoreSslErrors(errors);
}

