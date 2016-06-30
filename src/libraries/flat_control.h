#ifndef __MTCOMBOBOX_H__
#define __MTCOMBOBOX_H__


#include <wobjectdefs.h>
#include <QObject>


#include <QWidget>
#include <QComboBox>
#include <QStyledItemDelegate>
#include <QToolButton>
#include <QFontComboBox>

// Класс, исправляющий QComboBox, чтобы правильно применялись QSS-стили
// Проблема описана здесь: stackoverflow.com/questions/13308341/qcombobox-abstractitemviewitem


class MtComboBox : public QComboBox {
    W_OBJECT(MtComboBox)

    public:
	void (MtComboBox::*currentindexchanged)(int index);
	MtComboBox(QWidget *parent = 0);
	virtual ~MtComboBox();

    private:

	QStyledItemDelegate *itemDelegate;
};

class FlatComboBox : public QComboBox {
    typedef QComboBox Super;

    private:
	W_OBJECT(FlatComboBox)

    public:
	void (FlatComboBox::*currentindexchanged)(int index);
	FlatComboBox(QWidget *parent = 0);
	virtual ~FlatComboBox();
	bool	flat() const {return flat_;}
	void	setFlat(bool flat);

	Qt::Alignment	arrowAlignment() const {return arrowAlignment_;}
	void		setArrowAlignment(Qt::Alignment a);

    protected:
	virtual void	paintEvent(QPaintEvent *e);
	virtual void	mousePressEvent(QMouseEvent *e);

    signals:
	void aboutToPullDown() W_SIGNAL(aboutToPullDown)// ;

    private:
	QStyledItemDelegate * itemDelegate;
	Qt::Alignment	arrowAlignment_;
	bool		flat_;
};

class FlatFontComboBox : public QFontComboBox {
    typedef QFontComboBox Super;
    W_OBJECT(FlatFontComboBox)
    public:
	FlatFontComboBox(QWidget *parent = 0);	//: QFontComboBox(parent) {}

	void (FlatFontComboBox::*currentindexchanged)(int index);

	virtual ~FlatFontComboBox();
	bool	flat() const {return flat_;}
	void	setFlat(bool flat);

	Qt::Alignment	arrowAlignment() const {return arrowAlignment_;}
	void		setArrowAlignment(Qt::Alignment a);

    protected:
	virtual void	paintEvent(QPaintEvent *e);
	virtual void	mousePressEvent(QMouseEvent *e);

    signals:
	void aboutToPullDown() W_SIGNAL(aboutToPullDown)// ;

    private:
	QStyledItemDelegate * itemDelegate;
	Qt::Alignment	arrowAlignment_;
	bool		flat_;
};

class FlatToolButton : public QToolButton {
    W_OBJECT(FlatToolButton)
    typedef QToolButton Super;
    W_PROPERTY(bool, autoRaise, &FlatToolButton::autoRaise, &FlatToolButton::setAutoRaise)	// Q_PROPERTY(bool autoRaise READ autoRaise WRITE setAutoRaise)
    private:

    public:
	FlatToolButton(QWidget *parent = nullptr);
	// bool flat() const { return flat_; }
	// void setFlat(bool flat);
	// void setAutoRaise(bool yes) {QToolButton::setAutoRaise(yes);}
    protected:
	virtual void paintEvent(QPaintEvent *e);
	// virtual void mousePressEvent(QMouseEvent *e);
    private:
	// bool flat_;
};


#endif /* __MTCOMBOBOX_H__ */

