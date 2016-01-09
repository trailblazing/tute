#ifndef __MTCOMBOBOX_H__
#define __MTCOMBOBOX_H__

#include <QObject>
#include <QWidget>
#include <QComboBox>
#include <QStyledItemDelegate>
#include <QToolButton>
#include <QFontComboBox>

// Класс, исправляющий QComboBox, чтобы правильно применялись QSS-стили
// Проблема описана здесь: stackoverflow.com/questions/13308341/qcombobox-abstractitemviewitem


class MtComboBox : public QComboBox {
    Q_OBJECT

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
    Q_OBJECT

public:
    void (FlatComboBox::*currentindexchanged)(int index);
    FlatComboBox(QWidget *parent = 0);
    virtual ~FlatComboBox();
    bool flat() const { return flat_; }
    void setFlat(bool flat);

    Qt::Alignment arrowAlignment() const { return arrowAlignment_; }
    void setArrowAlignment(Qt::Alignment a);

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);

signals:
    void aboutToPullDown();

private:
    QStyledItemDelegate *itemDelegate;
    Qt::Alignment arrowAlignment_;
    bool flat_;

};

class FlatFontComboBox: public QFontComboBox {
    typedef QFontComboBox Super;
    Q_OBJECT
public:
    FlatFontComboBox(QWidget *parent = 0);  //: QFontComboBox(parent) {}

    void (FlatFontComboBox::*currentindexchanged)(int index);

    virtual ~FlatFontComboBox();
    bool flat() const { return flat_; }
    void setFlat(bool flat);

    Qt::Alignment arrowAlignment() const { return arrowAlignment_; }
    void setArrowAlignment(Qt::Alignment a);

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);

signals:
    void aboutToPullDown();

private:
    QStyledItemDelegate *itemDelegate;
    Qt::Alignment arrowAlignment_;
    bool flat_;

};

class FlatToolButton: public QToolButton {
    typedef QToolButton Super;
    Q_PROPERTY(bool autoRaise READ autoRaise WRITE setAutoRaise)
private:
    Q_OBJECT
public:
    FlatToolButton(QWidget *parent = nullptr);
    //    bool flat() const { return flat_; }
    //    void setFlat(bool flat);
    //    void setAutoRaise(bool yes) {QToolButton::setAutoRaise(yes);}
protected:
    virtual void paintEvent(QPaintEvent *e);
    //    virtual void mousePressEvent(QMouseEvent *e);
private:
    //    bool flat_;
};


#endif  /* __MTCOMBOBOX_H__ */

