#include "hidabletabwidget.h"
#include <QTabBar>
#include <QToolButton>
#include <QLayout>
#include "libraries/GlobalParameters.h"
#include "views/browser/entrance.h"

extern GlobalParameters globalparameters;
const char *custom_hidabletabwidget_style =
    "QTabWidget::pane {"
//    "width: 250px;"	// content space, can not change easily if set here
    "border: 0 px;"
    "}"
    "QTabBar::tab {"
//    "left 3 px;"
//    "background: transparent;"
//    "background-color: transparent;"
    "border-color: white;"
    "border-width: 0 px;"
    "border-style: solid;"
    "alignment: bottom;"
    "max-height: 100px;"
    "width: 26 px;"	// 16
    "max-width: 28px;"
    "qproperty-alignment: AlignBottom;"			//  AlignCenter | AlignVCenter;*/
    "qproperty-wordWrap: false;"
    "padding-bottom: 5px;"
//    "padding: 0 px;"
    "margin-left: 2 px;"
    "margin-right: 0 px;"
    "}"

    "QTabWidget::tab-bar {"
//    "background: transparent;"
//    "border-color: transparent;"
    "border-width: 0 px;"
    "border-style: solid;"
    "background-color: transparent;"
    "alignment: bottom;"
    "width: 28px;"	// 16
    "max-width : 30 px;"
    "text-align: left;"
    "qproperty-alignment: AlignBottom;"
    "qproperty-wordWrap: false;"
    "padding : 0 px;"
    "padding-bottom: 0px;"
//    "padding-left: 0px;"
    "margin-left : 0 px;"	// 0
    "margin-right : 0 px;"	// 0
    "}"
    "QTabWidget{"
//    "width: 250px;" // nake fiexed size
    "background: transparent;"
    "border-color: transparent;"
    "}"
    "QTabBar::tab:selected, QTabBar::tab:hover {"
    "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #fafafa, stop: 0.4 #f4f4f4, stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);"
    "font-color: #black;"
    "}"
    "QTabBar::tab:selected {"
    "border-color: #9B9B9B;"
    "border-bottom-color: white;"	// #C2C7CB         /* same as pane color */
    "background-color: white;"
    "}"
    "QTabBar::tab:!selected {"
    "margin-left: 0px;"		/* make non-selected tabs look smaller */
    "background-color: transparent;"
    "border-bottom-color: transparent;"
    "}"
;

HidableTabWidget::HidableTabWidget(QString style_source, QWidget *parent) :
    QTabWidget(parent)
    , _hide_action(new QAction(tr("▾"), this))
    , _layout(new QStackedLayout(this))
    , _style(style_source){
//    , _delegate_tab(_delegate_tab)
    _hide_action->setCheckable(true);
    _hide_action->setToolTip("Hide Panels");
    QToolButton *hide_button = new QToolButton();
    hide_button->setDefaultAction(_hide_action);
    hide_button->setAutoRaise(true);
    this->setCornerWidget(hide_button);


    setWindowFlags(	// Qt::Window |
        Qt::FramelessWindowHint
        // |Qt::Popup
        | Qt::CustomizeWindowHint
//        | Qt::SplashScreen   // http://www.qtforum.org/article/20174/how-to-create-borderless-windows-with-no-title-bar.html?s=86e2c5a6509f28a482adbb7d9f3654bb2058a301#post75829
        // | Qt::DockWidgetArea::NoDockWidgetArea
        | Qt::MaximizeUsingFullscreenGeometryHint
        );

    setAutoFillBackground(true);
    adjustSize();


    setTabPosition(TabPosition::West);		// South
    setTabShape(TabShape::Triangular);
        //    setStyleSheet("QTabBar::tab { max-width: 200px; padding: 2px; margin-left: 2px; }");
//    setStyleSheet("QTabWidget::pane { border: 0 px; } QTabBar::tab { max-width: 200px; padding: 0 px; margin-left: 2 px; margin-right: 0 px;} QTabWidget::tab-bar { max-width: 200px; align: left; text-align: left; margin-left: 2 px; padding: 0 px; margin-right: 0 px;}");    // QWidget{border: 0px;}
//    setStyleSheet("QTabWidget::pane { border: 0 px; }");

//    QSettings settings;
//    settings.beginGroup(QLatin1String("websettings"));
//    QString style_source = settings.value(QLatin1String("userStyleSheet")).toString();
//    settings.endGroup();
//    QString style_source = globalparameters.entrance()->style_source();

    setStyleSheet(custom_hidabletabwidget_style);	// _style

    connect(_hide_action, &QAction::toggled, this, &HidableTabWidget::onHideAction);
    connect(this, &HidableTabWidget::tabBarClicked, this, &HidableTabWidget::onTabBarClicked);


    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(10);

//        QTextEdit* editor = new QTextEdit();
//        editor->setFont(font);
    this->setFont(font);


//    _layout->widget()->setContentsMargins(0, 0, 0, 0);
//    _layout->widget()->setStyleSheet("border:0px;");



    _layout->setMargin(0);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);
    setLayout(_layout);
}

void HidableTabWidget::onHideAction(bool checked){
    if(checked){
        if(this->tabPosition() == North || tabPosition() == South){	// , West, East
            this->setMaximumHeight(this->tabBar()->height());
        }else{
            this->setMaximumWidth(this->tabBar()->width());
        }
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    }else{
        if(this->tabPosition() == North || tabPosition() == South){	// , West, East
            this->setMaximumHeight(
                std::numeric_limits<int>::max()	// 100000
                );	// just a very big number
                        //            setContentsMargins(0, 0, 0, 0);
        }else{
            this->setMaximumWidth(
                std::numeric_limits<int>::max()	// 100000
                );	// just a very big number
                        //            setContentsMargins(0, 0, 0, 0);
        }
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
}

void HidableTabWidget::onTabBarClicked(){
    _hide_action->setChecked(false);
}

// HidableTabWidget *HidableTabWidget::delegate_tab(){
//    HidableTabWidget *r = nullptr;
//    if(_delegate_tab) r = _delegate_tab;
//    return r;
// }


