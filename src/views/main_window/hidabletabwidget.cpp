#include "hidabletabwidget.h"
#include <QTabBar>
#include <QToolButton>

HidableTabWidget::HidableTabWidget(QWidget *parent) :
    QTabWidget(parent)
    , _hide_action(new QAction(tr("▾"), this))
//    , _delegate_tab(_delegate_tab)
{
    _hide_action->setCheckable(true);
    _hide_action->setToolTip("Hide Panels");
    QToolButton *hide_button = new QToolButton();
    hide_button->setDefaultAction(_hide_action);
    hide_button->setAutoRaise(true);
    this->setCornerWidget(hide_button);


    setWindowFlags( //Qt::Window |
        Qt::FramelessWindowHint
        // |Qt::Popup
        | Qt::CustomizeWindowHint
        // | Qt::SplashScreen  // http://www.qtforum.org/article/20174/how-to-create-borderless-windows-with-no-title-bar.html?s=86e2c5a6509f28a482adbb7d9f3654bb2058a301#post75829
        // | Qt::DockWidgetArea::NoDockWidgetArea
        | Qt::MaximizeUsingFullscreenGeometryHint
        );

    setAutoFillBackground(true);
    adjustSize();


    setTabPosition(TabPosition::West); // South
    setTabShape(TabShape::Triangular);
    //    setStyleSheet("QTabBar::tab { max-width: 200px; padding: 2px; margin-left: 2px; }");
    setStyleSheet("QTabBar::tab { max-width: 200px; padding: 0 px; margin-left: 2 px; margin-right: 0 px;} QTabWidget::tab-bar { max-width: 200px; align: left; text-align: left; margin-left: 2 px; padding: 0 px; margin-right: 0 px;}");    // border: 0 px;

    connect(_hide_action, &QAction::toggled, this, &HidableTabWidget::onHideAction);
    connect(this, &HidableTabWidget::tabBarClicked, this, &HidableTabWidget::onTabBarClicked);
}

void HidableTabWidget::onHideAction(bool checked)
{
    if(checked) {
        if(this->tabPosition() == North || tabPosition() == South) { // , West, East
            this->setMaximumHeight(this->tabBar()->height());
        } else {
            this->setMaximumWidth(this->tabBar()->width());
        }

        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    } else {
        if(this->tabPosition() == North || tabPosition() == South) { // , West, East
            this->setMaximumHeight(
                std::numeric_limits<int>::max() // 100000
                ); // just a very big number
                   //            setContentsMargins(0, 0, 0, 0);
        } else {
            this->setMaximumWidth(
                std::numeric_limits<int>::max() // 100000
                ); // just a very big number
                   //            setContentsMargins(0, 0, 0, 0);
        }

        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
}

void HidableTabWidget::onTabBarClicked()
{
    _hide_action->setChecked(false);
}

//HidableTabWidget *HidableTabWidget::delegate_tab(){
//    HidableTabWidget *r = nullptr;
//    if(_delegate_tab) r = _delegate_tab;
//    return r;
//}
