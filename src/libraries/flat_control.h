#ifndef __MTCOMBOBOX_H__
#define __MTCOMBOBOX_H__

#include <QCheckBox>
#include <QComboBox>
#include <QFontComboBox>
#include <QIcon>
#include <QLineEdit>
#include <QStyledItemDelegate>
#include <QToolButton>
#include <QWidget>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

//extern const QIcon default_icon;
// Класс, исправляющий QComboBox, чтобы правильно применялись QSS-стили
// Проблема описана здесь: stackoverflow.com/questions/13308341/qcombobox-abstractitemviewitem

class MtComboBox : public QComboBox {
#if QT_VERSION == 0x050600
    W_OBJECT(MtComboBox)
#else
    Q_OBJECT
#endif
public:
    void (MtComboBox::*currentindexchanged)(int index);
    MtComboBox(QWidget* parent = 0);
    virtual ~MtComboBox();

private:
    QStyledItemDelegate* itemDelegate;
};

class FlatComboBox : public QComboBox {
    typedef QComboBox Super;

private:
#if QT_VERSION == 0x050600
    W_OBJECT(FlatComboBox)
#else
    Q_OBJECT
#endif
public:
    void (FlatComboBox::*currentindexchanged)(int index);
    FlatComboBox(QWidget* parent = 0);
    virtual ~FlatComboBox();
    bool flat() const { return flat_; }

    void flat(bool flat);

    Qt::Alignment arrowAlignment() const { return arrowAlignment_; }

    void setArrowAlignment(Qt::Alignment a);

protected:
    virtual void paintEvent(QPaintEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);

signals:
    void aboutToPullDown()
#if QT_VERSION == 0x050600
        W_SIGNAL(aboutToPullDown) //
#else
        ;
#endif

        private : QStyledItemDelegate* itemDelegate;
    Qt::Alignment arrowAlignment_;
    bool flat_ = true;
};

class FlatFontComboBox : public QFontComboBox {
    typedef QFontComboBox Super;
#if QT_VERSION == 0x050600
    W_OBJECT(FlatFontComboBox)
#else
    Q_OBJECT
#endif
public:
    FlatFontComboBox(QWidget* parent = 0); // : QFontComboBox(parent) {}

    void (FlatFontComboBox::*currentindexchanged)(int index);

    virtual ~FlatFontComboBox();
    bool flat() const { return flat_; }

    void flat(bool flat);

    Qt::Alignment arrowAlignment() const { return arrowAlignment_; }

    void setArrowAlignment(Qt::Alignment a);

protected:
    virtual void paintEvent(QPaintEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);

signals:
    void aboutToPullDown()
#if QT_VERSION == 0x050600
        W_SIGNAL(aboutToPullDown) //
#else
        ;
#endif

        private : QStyledItemDelegate* itemDelegate;
    Qt::Alignment arrowAlignment_;
    bool flat_ = true;
};

class FlatToolButton : public QToolButton {
#if QT_VERSION == 0x050600
    W_OBJECT(FlatToolButton)
    W_PROPERTY(bool, autoRaise, &FlatToolButton::autoRaise, &FlatToolButton::setAutoRaise) //
#else
    Q_OBJECT Q_PROPERTY(bool autoRaise READ autoRaise WRITE setAutoRaise)
#endif
    typedef QToolButton Super;

private:
public:
    FlatToolButton(const QString& name = "", QWidget* parent = nullptr, const QIcon& icon = QIcon(":/resource/pic/trace.svg"));
    FlatToolButton(QWidget* parent = nullptr, const QString& name = "");
    // bool flat() const { return flat_; }
    // void setFlat(bool flat);
    // void setAutoRaise(bool yes) {QToolButton::setAutoRaise(yes);}
protected:
    virtual void paintEvent(QPaintEvent* e);
    // virtual void mousePressEvent(QMouseEvent *e);
private:
    // bool flat_;
};

class FlatCheckBox : public QCheckBox {
    typedef QCheckBox Super;

public:
    FlatCheckBox(const QString& name);
};

class FlatLineEdit : public QLineEdit {
    typedef QLineEdit Super;
    FlatLineEdit();
};

#endif /* __MTCOMBOBOX_H__ */
