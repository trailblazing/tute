#include <wobjectimpl.h>
#include "main.h"

#include "flat_control.h"

W_OBJECT_IMPL(MtComboBox)
MtComboBox::MtComboBox(QWidget *parent) : QComboBox(parent){
    Q_UNUSED(parent);
    currentindexchanged = &QComboBox::currentIndexChanged;
    itemDelegate = new QStyledItemDelegate();
    this->setItemDelegate(itemDelegate);
}
MtComboBox::~MtComboBox(){
    delete itemDelegate;
}
// http://engineer-works.blogspot.com/2010/02/flat-qcombobox.html

#include <QStylePainter>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPalette>

W_OBJECT_IMPL(FlatComboBox)

FlatComboBox::FlatComboBox(QWidget *parent)
    : Super(parent)
      , itemDelegate(new QStyledItemDelegate())
      , arrowAlignment_(Qt::AlignRight)
      , flat_(true){
    setFlat(true);
    setAutoFillBackground(true);
	//    QPalette plt(palette());
	//    plt.setColor(QPalette::Background, Qt::white);
	//    setPalette(plt);

    currentindexchanged = &QComboBox::currentIndexChanged;
	//    itemDelegate = new QStyledItemDelegate();
    this->setItemDelegate(itemDelegate);
}
FlatComboBox::~FlatComboBox(){delete itemDelegate;}

void FlatComboBox:: paintEvent(QPaintEvent *e){
    if(flat()){
	QStylePainter painter(this);
	painter.setPen(palette().color(QPalette::Text));
	QStyleOptionComboBox opt;
	initStyleOption(&opt);
	QString displayText(opt.currentText);
	opt.currentText = "";
	painter.drawItemText(rect(), Qt::AlignCenter, palette(), true, displayText);
	const QRect rcOld(opt.rect);
	opt.rect = QStyle::alignedRect(Qt::LeftToRight, arrowAlignment(), QSize(16, rcOld.height()), rcOld);
	painter.drawPrimitive(QStyle::PE_IndicatorArrowDown, opt);
	opt.rect = rcOld;
	painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
    }else{
	Super::paintEvent(e);
    }
}
void FlatComboBox:: mousePressEvent(QMouseEvent *e){
    if(! isEditable() || ! lineEdit()->rect().contains(e->pos())){
	emit aboutToPullDown();
    }
    Super::mousePressEvent(e);
}
void FlatComboBox:: setFlat(bool flat){
    flat_ = flat;
}
void FlatComboBox:: setArrowAlignment(Qt::Alignment a){
    arrowAlignment_ = a;
}
//

W_OBJECT_IMPL(FlatFontComboBox)
FlatFontComboBox::FlatFontComboBox(QWidget *parent)
    : Super(parent)
      , itemDelegate(new QStyledItemDelegate())
      , arrowAlignment_(Qt::AlignRight)
      , flat_(true){
    setFlat(true);
    setAutoFillBackground(true);

    QPalette plt(palette());
    plt.setColor(QPalette::Background, Qt::darkGray
	// white
	);
    setPalette(plt);

    currentindexchanged = &Super::currentIndexChanged;
	//    itemDelegate = new QStyledItemDelegate();
    this->setItemDelegate(itemDelegate);
}
FlatFontComboBox::~FlatFontComboBox(){delete itemDelegate;}

void FlatFontComboBox:: paintEvent(QPaintEvent *e){
    if(flat()){
	QStylePainter painter(this);
	painter.setPen(palette().color(QPalette::Text));
	QStyleOptionComboBox opt;
	initStyleOption(&opt);
	QString displayText(opt.currentText);
	opt.currentText = "";
	painter.drawItemText(rect(), Qt::AlignCenter, palette(), true, displayText);
	const QRect rcOld(opt.rect);
	opt.rect = QStyle::alignedRect(Qt::LeftToRight, arrowAlignment(), QSize(16, rcOld.height()), rcOld);
	painter.drawPrimitive(QStyle::PE_IndicatorArrowDown, opt);
	opt.rect = rcOld;
	painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
    }else{
	Super::paintEvent(e);
    }
}
void FlatFontComboBox:: mousePressEvent(QMouseEvent *e){
    if(! isEditable() || ! lineEdit()->rect().contains(e->pos())){
	emit aboutToPullDown();
    }
    Super::mousePressEvent(e);
}
void FlatFontComboBox:: setFlat(bool flat){
    flat_ = flat;
}
void FlatFontComboBox:: setArrowAlignment(Qt::Alignment a){
    arrowAlignment_ = a;
}


W_OBJECT_IMPL(FlatToolButton)

FlatToolButton::FlatToolButton(QWidget *parent)
    : QToolButton(parent){
	//    , flat_(true)
	//    setFlat(true);
    setAutoFillBackground(true);
    setAutoRaise(true);
}
// void FlatToolButton::setFlat(bool flat)
// {
//    flat_ = flat;
//    setAutoRaise(true);
// }

void FlatToolButton:: paintEvent(QPaintEvent *e){
	//    if(flat()) {
	//        QStylePainter painter(this);
	//        painter.setPen(palette().color(QPalette::Text));
	//        QStyleOptionToolButton opt; //        QStyleOptionComboBox opt;
	//        initStyleOption(&opt);
	//        //QString displayText(opt.currentText);
	//        //opt.currentText = "";
	//        //        painter.drawItemText(rect(), Qt::AlignCenter, palette(), true, "");
	//        //        const QRect rcOld(opt.rect);
	//        //        opt.rect = QStyle::alignedRect(Qt::LeftToRight
	//        //                                       , Qt::AlignCenter    //arrowAlignment()
	//        //                                       , QSize(16
	//        //                                               , 16 //rcOld.height()
	//        //                                               )
	//        //                                       , rcOld);
	//        //        painter.drawPrimitive(QStyle::PE_IndicatorButtonDropDown
	//        //                              // PE_IndicatorArrowDown
	//        //                              , opt);
	//        //        opt.rect = rcOld;
	//        painter.drawControl(QStyle::CE_ToolButtonLabel
	//                            // CE_ComboBoxLabel
	//                            , opt);
	//    } else {
    Super::paintEvent(e);
	//    }
}
